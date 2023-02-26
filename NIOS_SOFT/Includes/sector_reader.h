/*
 * sector_reader.h
 *
 *  Created on: 21.03.2021
 *      Author: VBKesha
 */

#ifndef SECTOR_READER_H_
#define SECTOR_READER_H_

void sector_reader_init(sdcard_inst * inst);
void sector_reader_read(uint32_t sector, uint32_t ** buff_addr);
uint8_t sector_reader_is_done(uint32_t ** buff_addr);
void sector_reader_swap();

#endif /* SECTOR_READER_H_ */
