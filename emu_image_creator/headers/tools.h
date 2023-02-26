#ifndef TOOLS_H_INCLUDED
#define TOOLS_H_INCLUDED

#define IMAGE_SECTOR_SIZE (2352)
#define IMAGE_SUBQ_SIZE (12)

void lba_to_msf(const uint32_t lba, uint8_t * min, uint8_t * sec, uint8_t * frame);
uint8_t bcd_to_hex(uint8_t num);
void swap_bytes(uint8_t in_sector[IMAGE_SECTOR_SIZE]);
void l2b(uint8_t in_sector[IMAGE_SECTOR_SIZE]);
void gen_subq(uint8_t track_mode, uint8_t track_num, uint32_t abs_sect, uint32_t rel_sect, uint8_t subq[IMAGE_SUBQ_SIZE]);
void print_subq(uint8_t subq[IMAGE_SUBQ_SIZE]);
void scramb(uint8_t in_sector[IMAGE_SECTOR_SIZE]);
void msb_to_lsb(uint8_t * inout, uint8_t len);


#endif // TOOLS_H_INCLUDED
