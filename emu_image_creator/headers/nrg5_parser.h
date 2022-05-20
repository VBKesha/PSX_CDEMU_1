#ifndef NRG5_PARSER_H_INCLUDED
#define NRG5_PARSER_H_INCLUDED

/*
12	Text	ISRC (or NULLs)
2	16bit	Sector size in the image file (bytes)
2	16bit	Mode of the data in the image file (values already found: 0x0700 for audio; 0x1000 for audio with sub-channel; 0x0000 for data; 0x0500 for raw data; 0x0f00 for raw data with sub-channel; 0x0300 for Mode 2 Form 1 data; 0x0600 for raw Mode 2/form 1 data; 0x1100 for raw Mode 2/form 1 data with sub-channel)
2	16bit	Unknown (always 0x0001 found)
8	64bit	Index0 (Pre-gap) (bytes)
8	64bit	Index1 (Start of track) (bytes)
8	64bit	End of track + 1 (bytes)
*/
typedef struct nrg_daox_item_t{
    char ISRC[12];
    uint16_t Sector_Size;
    uint16_t Mode;
    uint16_t Unknow;
    uint64_t PreGap_Offset;
    uint64_t Track_Offset;
    uint64_t Next_Track_Offset;
} nrg_daox_item;

/*
13	Text	UPC (or NULLs)
1	8-bit	Padding? (always NULL found)(Zero termination??)
2	16-bit	Toc type (values already found: 0x0000 for audio; 0x0001 for data; 0x2001 for Mode 2/form 1 data)
1	8bit	First track in the session
1	8bit	Last track in the session
*/

typedef struct nrg_daox_t {
    char UPC[14];
    uint16_t TOC_Type;
    uint8_t First_Track;
    uint8_t Last_Track;
    nrg_daox_item items[99];
} nrg_daox;

uint8_t parse_nrg(FILE * nrg, nrg_daox * disk_daox);


#endif // NRG5_PARSER_H_INCLUDED
