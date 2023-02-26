#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "tools.h"
#include "nrg5_parser.h"

typedef struct toc_struct_t {
    uint8_t items;
    uint8_t subq[101][IMAGE_SUBQ_SIZE];
} toc_struct;

typedef struct image_item_t {
    uint32_t offset;
    uint32_t lead_out;
    uint32_t disk_size;
    char name[20];
} image_item;

typedef struct image_game_list_t {
    uint32_t magic;
    image_item item[73];
} image_game_list;

typedef union toc_sector_t {
    image_game_list games;
    uint8_t raw[2364];
} toc_sector;

#define SUBQ_CTRL_ADDR  (0)
#define SUBQ_TNO        (1)
#define SUBQ_INDEX      (2)
#define SUBQ_MIN        (3)
#define SUBQ_SEC        (4)
#define SUBQ_FRAME      (5)
#define SUBQ_ZERO       (6)
#define SUBQ_AMIN       (7)
#define SUBQ_ASEC       (8)
#define SUBQ_AFRAME     (9)
#define SUBQ_CRC_L     (10)
#define SUBQ_CRC_H     (11)

uint8_t sector_header[12] = {0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00};

nrg_daox disk_daox;
toc_struct st_toc;

uint8_t generate_toc(toc_struct * toc, nrg_daox * disk_daox);
uint8_t write_leadin(FILE * out_file, toc_struct * toc);
uint8_t write_tracks(nrg_daox * disk_daox, FILE * read_data, FILE * out_file, uint32_t * abs_pos);
uint8_t write_track_part(uint8_t track, nrg_daox_item * item, uint8_t type, FILE * read_data, FILE * out_file,  uint32_t * abs_pos);
uint8_t write_leadout(FILE * out_image,  uint32_t * abs_pos);



int main(int argc, char *argv[]) {

    FILE * nrg;
    FILE * out_image;
    uint32_t abs_pos;
    uint32_t img_num;
    uint32_t img_offset;
    char img_name[21];

    toc_sector image_toc;
    printf("Sizeof: %u\r\n",  sizeof(image_toc));
    memset(image_toc.raw, 0, sizeof(image_toc));
    image_toc.games.magic=0xAB031337;

    if(argc < 2){
        printf("No images select\r\n");
        return 0;
    }


    out_image = fopen("sd_image.img", "wb");
    fwrite(image_toc.raw, sizeof(image_toc), 1, out_image);

    img_offset = 1;
    img_num = 0;

    for(uint8_t i=1; i<argc; i+=2){
        printf("%d %s\r\n", i, argv[i]);

        nrg = fopen(argv[i], "rb"); //695808624
        memset(img_name, 0, sizeof(img_name));
        snprintf(img_name, sizeof(img_name), "%s", argv[i+1]);

        if(nrg == 0) {
            printf("Cant open file - %s\r\n", "zzt");
            return 1;
        }


        memcpy(image_toc.games.item[img_num].name, img_name, sizeof(image_toc.games.item[img_num].name));
        image_toc.games.item[img_num].offset = img_offset;
        parse_nrg(nrg, &disk_daox);
        generate_toc(&st_toc, &disk_daox);
        printf("Tok Items %d\r\n", st_toc.items);


        abs_pos = 0;
        write_leadin(out_image, &st_toc);
        img_offset += 4500;
        write_tracks(&disk_daox, nrg, out_image, &abs_pos);

        image_toc.games.item[img_num].lead_out = 4501 + abs_pos;
        write_leadout(out_image, &abs_pos);

        image_toc.games.item[img_num].disk_size = 4501 + abs_pos;
        img_offset += abs_pos;
        img_num++;
        fclose(nrg);
    }
    fseek(out_image, 0, SEEK_SET);
    fwrite(image_toc.raw, sizeof(image_toc), 1, out_image);
    fclose(out_image);

    return 0;
}

uint8_t write_tracks(nrg_daox * disk_daox, FILE * read_data, FILE * out_file,  uint32_t * abs_pos) {
    for(uint8_t trk=0; trk<disk_daox->Last_Track; trk++) {
        printf("Write track: %d\r\n", trk);
        if((disk_daox->items[trk].Sector_Size != 2352) && (disk_daox->items[trk].Sector_Size != 2336)) {
            printf("Unknow sector size: %u\r\n", disk_daox->items[trk].Sector_Size);
            return 1;
        }
        write_track_part(trk+1, &disk_daox->items[trk], 0, read_data, out_file, abs_pos);
        write_track_part(trk+1, &disk_daox->items[trk], 1, read_data, out_file, abs_pos);
    }
    printf("ABS: %u\r\n", abs_pos);
    return 0;
}

uint8_t write_track_part(uint8_t track, nrg_daox_item * item, uint8_t type, FILE * read_data, FILE * out_file,  uint32_t * abs_pos) {
    uint64_t start_offset;
    uint64_t end_offset;
    uint64_t pregap_size;
    uint64_t sector_count;

    uint8_t rm, rs, rf;
    uint8_t am, as, af;
    uint8_t subq[IMAGE_SUBQ_SIZE];
    uint8_t sector[IMAGE_SECTOR_SIZE];

    start_offset = (type == 0) ? item->PreGap_Offset : item->Track_Offset;
    end_offset   = (type == 0) ? item->Track_Offset : item->Next_Track_Offset;
    pregap_size = (item->Track_Offset - item->PreGap_Offset) / item->Sector_Size;

    sector_count = (end_offset - start_offset) / item->Sector_Size;

    fseek(read_data, start_offset, SEEK_SET);
    printf("Sector count: %llu\r\n", start_offset);

    memset(sector, 0, sizeof(sector));

    for(uint32_t i=0; i<sector_count; i++) {
        fread(&sector[2352 - item->Sector_Size], item->Sector_Size, 1, read_data);

        switch(item->Mode) {
            case 0x0300:
                memcpy(sector, sector_header, sizeof(sector_header));
                lba_to_msf(((type == 0) ? i : i + pregap_size), &rm, &rs, &rf);

                sector[12] = bcd_to_hex(rm);
                sector[13] = bcd_to_hex(rs);
                sector[14] = bcd_to_hex(rf);
                sector[15] = 0x02;

            case 0x0600: // Mode 2 Form 1
                scramb(sector);
                subq[SUBQ_CTRL_ADDR] = 0x41;
            break;

            case 0x0700: // audio
                subq[SUBQ_CTRL_ADDR] = 0x01;
            break;

            default:
                printf("ERROR[%04X]: No PS1 Image file????\r\n", item->Mode);
                return 1;
            break;
        }

        l2b(sector);

        subq[SUBQ_TNO]       = bcd_to_hex(track);
        subq[SUBQ_INDEX]     = (type == 0) ? 0 : 1;

        lba_to_msf(((type == 0) ? sector_count - i : i), &rm, &rs, &rf);

        subq[SUBQ_MIN]       = bcd_to_hex(rm);
        subq[SUBQ_SEC]       = bcd_to_hex(rs);
        subq[SUBQ_FRAME]     = bcd_to_hex(rf);

        subq[SUBQ_ZERO]      = 0x00;

        lba_to_msf(*abs_pos, &am, &as, &af);
        subq[SUBQ_AMIN]      = bcd_to_hex(am);
        subq[SUBQ_ASEC]      = bcd_to_hex(as);
        subq[SUBQ_AFRAME]    = bcd_to_hex(af);

        subq[SUBQ_CRC_L]    = 0xFF;
        subq[SUBQ_CRC_H]    = 0xFF;

        msb_to_lsb(subq, sizeof(subq));

        fwrite(sector, sizeof(sector), 1, out_file);
        fwrite(subq, sizeof(subq), 1, out_file);
        (*abs_pos)++;
        //fread(&sector[2352 - item->Sector_Size], item->Sector_Size, 1, read_data);
    }

    return 0;
}

uint8_t write_leadout(FILE * out_file,  uint32_t * abs_pos) {
    uint8_t rm,rs,rf;
    uint8_t am,as,af;
    uint8_t subq[IMAGE_SUBQ_SIZE];
    uint8_t sector[IMAGE_SECTOR_SIZE];

    uint16_t i;
    for(i=0; i<6750; i++){
        memset(sector, 0, sizeof(sector));
        subq[SUBQ_CTRL_ADDR] = 0x01;
        subq[SUBQ_TNO]       = 0xAA;
        subq[SUBQ_INDEX]     = 0x00;

        lba_to_msf(i, &rm, &rs, &rf);
        subq[SUBQ_MIN]       = bcd_to_hex(rm);
        subq[SUBQ_SEC]       = bcd_to_hex(rs);
        subq[SUBQ_FRAME]     = bcd_to_hex(rf);

        subq[SUBQ_ZERO]      = 0x00;

        lba_to_msf(*abs_pos, &am, &as, &af);
        subq[SUBQ_AMIN]      = bcd_to_hex(am);
        subq[SUBQ_ASEC]      = bcd_to_hex(as);
        subq[SUBQ_AFRAME]    = bcd_to_hex(af);

        subq[SUBQ_CRC_L]    = 0xFF;
        subq[SUBQ_CRC_H]    = 0xFF;

        msb_to_lsb(subq, sizeof(subq));

        fwrite(sector, sizeof(sector), 1, out_file);
        fwrite(subq, sizeof(subq), 1, out_file);
        (*abs_pos)++;
    }

    return 0;
}

uint8_t write_leadin(FILE * out_file, toc_struct * toc) {
    uint8_t sector[IMAGE_SECTOR_SIZE];
    uint8_t subq[IMAGE_SUBQ_SIZE];
    uint8_t tok_item;
    uint8_t m, s, f;
    tok_item = st_toc.items;
    memset(sector, 0, sizeof(sector));
    for(uint16_t i=0; i<4500; i++) {
        if((i % 4) == 0) {
            tok_item++;
            if(tok_item > st_toc.items) {
                tok_item = 0;
            }
        }

        memcpy(subq, st_toc.subq[tok_item], sizeof(subq));
        lba_to_msf(i, &m, &s, &f);
        subq[SUBQ_MIN]   = bcd_to_hex(m);
        subq[SUBQ_SEC]   = bcd_to_hex(s);
        subq[SUBQ_FRAME] = bcd_to_hex(f);
        subq[SUBQ_CRC_L]    = 0xFF;
        subq[SUBQ_CRC_H]    = 0xFF;
        fwrite(sector, sizeof(sector), 1, out_file);
        msb_to_lsb(subq, sizeof(subq));
        fwrite(subq, sizeof(subq), 1, out_file);
    }
    return 0;
}

uint8_t generate_toc(toc_struct * toc, nrg_daox * disk_daox) {
    uint32_t cur_pos;
    uint8_t m, s, f;
    uint8_t i;

    toc->items = 0;

    /* A0 DISK MODE + First Track */
    toc->subq[toc->items][SUBQ_CTRL_ADDR] = 0x41;
    toc->subq[toc->items][SUBQ_TNO]       = 0x00;
    toc->subq[toc->items][SUBQ_INDEX]     = 0xA0;
    toc->subq[toc->items][SUBQ_ZERO]      = 0x00;
    toc->subq[toc->items][SUBQ_AMIN]      = bcd_to_hex(disk_daox->First_Track);
    toc->subq[toc->items][SUBQ_ASEC]      = (disk_daox->TOC_Type >> 8) & 0xFF;
    toc->subq[toc->items][SUBQ_AFRAME]    = 0x00;
    toc->items++;

    /* A! Last Track */
    toc->subq[toc->items][SUBQ_CTRL_ADDR] = 0x41;
    toc->subq[toc->items][SUBQ_TNO]       = 0x00;
    toc->subq[toc->items][SUBQ_INDEX]     = 0xA1;
    toc->subq[toc->items][SUBQ_ZERO]      = 0x00;
    toc->subq[toc->items][SUBQ_AMIN]      = bcd_to_hex(disk_daox->Last_Track);
    toc->subq[toc->items][SUBQ_ASEC]      = 0x00;
    toc->subq[toc->items][SUBQ_AFRAME]    = 0x00;
    toc->items++;

    /* TOC Items */
    cur_pos = 0;
    for(i=0; i<(disk_daox->Last_Track); i++) {
        uint32_t track_start = (disk_daox->items[i].Track_Offset - disk_daox->items[i].PreGap_Offset) / disk_daox->items[i].Sector_Size;
        cur_pos += track_start;
        lba_to_msf(cur_pos, &m, &s, &f);
        printf("Track Start[%04X][%02X:%02X:%02X]: %d \r\n", disk_daox->items[i].Mode, bcd_to_hex(m), bcd_to_hex(s), bcd_to_hex(f), cur_pos);

        switch(disk_daox->items[i].Mode) {
            case 0x0300:
            case 0x0600: // Mode 2 Form 1
                toc->subq[toc->items][SUBQ_CTRL_ADDR] = 0x41;
            break;

            case 0x0700: // audio
                toc->subq[toc->items][SUBQ_CTRL_ADDR] = 0x01;
            break;

            default:
                printf("ERROR[%04X]: No PS1 Image file????\r\n", disk_daox->items[i].Mode);
                return 1;
            break;
        }
        toc->subq[toc->items][SUBQ_TNO]       = 0x00;
        toc->subq[toc->items][SUBQ_INDEX]     = bcd_to_hex(i+1);
        toc->subq[toc->items][SUBQ_ZERO]      = 0x00;
        toc->subq[toc->items][SUBQ_AMIN]      = bcd_to_hex(m);
        toc->subq[toc->items][SUBQ_ASEC]      = bcd_to_hex(s);
        toc->subq[toc->items][SUBQ_AFRAME]    = bcd_to_hex(f);


        uint32_t track_size = (disk_daox->items[i].Next_Track_Offset - disk_daox->items[i].Track_Offset) / disk_daox->items[i].Sector_Size;
        cur_pos += track_size;
        toc->items++;
    }
    lba_to_msf(cur_pos, &m, &s, &f);

    printf("Track Start[%04X][%02X:%02X:%02X]: %d \r\n", 0x0600, bcd_to_hex(m), bcd_to_hex(s), bcd_to_hex(f), cur_pos);

    /* A2 Lead out time*/
    toc->subq[toc->items][SUBQ_CTRL_ADDR] = 0x41;
    toc->subq[toc->items][SUBQ_TNO]       = 0x00;
    toc->subq[toc->items][SUBQ_INDEX]     = 0xA2;
    toc->subq[toc->items][SUBQ_ZERO]      = 0x00;
    toc->subq[toc->items][SUBQ_AMIN]      = bcd_to_hex(m);
    toc->subq[toc->items][SUBQ_ASEC]      = bcd_to_hex(s);
    toc->subq[toc->items][SUBQ_AFRAME]    = bcd_to_hex(f);


    return 0;
}
