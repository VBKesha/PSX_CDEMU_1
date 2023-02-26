/*
 * sdcard.h
 *
 *  Created on: 04.09.2020
 *      Author: VBKesha
 */

#ifndef SDCARD_H_
#define SDCARD_H_

typedef struct sdcard_inst_t{
	uint32_t spi_base_addr;
	uint8_t cmd_buff[8];
	uint8_t ret_buff[8];
	uint32_t cur_sector;
	uint8_t state;

//	uint8_t i;
//	uint8_t data_ready;
	uint8_t data_pos;
} sdcard_inst;

uint8_t sdcard_init(sdcard_inst * inst, uint32_t spi_addr);
uint8_t sdcard_read_sector(sdcard_inst * inst, uint32_t sector, uint8_t * buff);

#endif /* SDCARD_H_ */
