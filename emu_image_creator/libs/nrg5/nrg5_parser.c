#include <stdio.h>
#include <stdint.h>
#include "nrg5_parser.h"

uint8_t NER5_HDR[4] = {'N', 'E', 'R', '5'};
uint8_t CUEX_HDR[4] = {'C', 'U', 'E', 'X'};
uint8_t DAOX_HDR[4] = {'D', 'A', 'O', 'X'};
uint8_t END_HDR[4]  = {'E', 'N', 'D', '!'};

static uint8_t check_header(const uint8_t * hdr1,  const uint8_t * hdr2);
static void read_be_uint(void * value, uint8_t size, FILE * file);
static void parse_nrg_daox(FILE * file, nrg_daox * daox);


uint8_t parse_nrg(FILE* nrg, nrg_daox * disk_daox) {
    uint8_t buff[4];
    uint64_t chunks_offset;
    uint32_t chunk_size;
    uint8_t ret = 3;

    int32_t fsize;
    fseek(nrg, 0, SEEK_END);
    fsize = ftell(nrg);
    printf("File size: %d\r\n", fsize);

    // READ HEADER

    fseek(nrg, fsize - 12, SEEK_SET);
    fread(buff, 4, 1, nrg);
    if(check_header(buff, NER5_HDR) != 1) {
        printf("UNKNOW FILE FORMAT\r\n");
        fclose(nrg);
        return 2;
    }

    read_be_uint(&chunks_offset, sizeof(chunks_offset), nrg);
    printf("First chunk: %ld\r\n", chunks_offset);

    fseek(nrg, chunks_offset, SEEK_SET);


    do {
        fread(buff, 4, 1, nrg);
        read_be_uint(&chunk_size, sizeof(chunk_size), nrg);

        printf("Found chunk: ");
        for(uint8_t i=0; i<4; i++) {
            printf("%c", buff[i]);
        }
        printf(" size: %d\r\n", chunk_size);

        if(check_header(buff, DAOX_HDR) == 1){
            parse_nrg_daox(nrg, disk_daox);
            ret = 0;
        } else {
            fseek(nrg, chunk_size, SEEK_CUR);
        };

    } while(check_header(buff, END_HDR) != 1);

    return ret;
}

void print_nrg_daox_item(nrg_daox_item * item){
    printf("\t Sector size: %u\r\n", item->Sector_Size);
    printf("\t Mode: %04X\r\n", item->Mode);
    printf("\t Unknow: %04X\r\n", item->Unknow);
    printf("\t PreGap Offset: %llu Size: %llu Sectors: %llu\r\n", item->PreGap_Offset, item->Track_Offset - item->PreGap_Offset, (item->Track_Offset - item->PreGap_Offset) / item->Sector_Size);
    printf("\t Track Offset: %llu Size: %llu Sectors: %llu\r\n", item->Track_Offset, item->Next_Track_Offset - item->Track_Offset, (item->Next_Track_Offset - item->Track_Offset) / item->Sector_Size);
}

void print_nrg_daox(nrg_daox * daox){
    printf("TOC Type: %04X\r\n", daox->TOC_Type);
    printf("FirstTrack: %u\r\n", daox->First_Track);
    printf("LastTrack: %u\r\n", daox->Last_Track);
    for(uint8_t i=0; i<daox->Last_Track; i++){
        printf("Track: %u\r\n", i+1);
        print_nrg_daox_item(&daox->items[i]);
    }
}

void parse_nrg_daox_item(FILE * file, nrg_daox_item * daox_item){
    fread(daox_item->ISRC, sizeof(daox_item->ISRC), 1, file);
    read_be_uint(&daox_item->Sector_Size, sizeof(daox_item->Sector_Size), file);
    read_be_uint(&daox_item->Mode, sizeof(daox_item->Mode), file);
    read_be_uint(&daox_item->Unknow, sizeof(daox_item->Unknow), file);

    read_be_uint(&daox_item->PreGap_Offset, sizeof(daox_item->PreGap_Offset), file);
    read_be_uint(&daox_item->Track_Offset, sizeof(daox_item->Track_Offset), file);
    read_be_uint(&daox_item->Next_Track_Offset, sizeof(daox_item->Next_Track_Offset), file);
}

void parse_nrg_daox(FILE * file, nrg_daox * daox){
    uint32_t tmp;
    fread(&tmp, 4, 1, file);
    fread(daox->UPC, sizeof(daox->UPC), 1, file);
    read_be_uint(&daox->TOC_Type, sizeof(daox->TOC_Type), file);
    read_be_uint(&daox->First_Track, sizeof(daox->First_Track), file);
    read_be_uint(&daox->Last_Track, sizeof(daox->Last_Track), file);


    for(uint8_t i=0; i<daox->Last_Track; i++){
        parse_nrg_daox_item(file, &daox->items[i]);
    }
    print_nrg_daox(daox);
}

static void read_be_uint(void * value, uint8_t size, FILE * file){
    uint8_t buff[32];
    uint8_t * val = value;
    fread(buff, size, 1, file);
    for(uint8_t i=0; i<size; i++){
        val[size - 1 - i] = buff[i];
    }
}

static uint8_t check_header(const uint8_t * hdr1,  const uint8_t * hdr2) {
    for(uint8_t i=0; i<4; i++) {
        if(hdr1[i] != hdr2[i]) {
            return 0;
        }
    }
    return 1;
}
