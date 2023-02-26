/*
 * sector_reader.c
 *
 *  Created on: 07.09.2020
 *      Author: VBKesha
 */

#include <stdint.h>
#include <stdio.h>
#include <sdcard.h>
#include <sys/alt_cache.h>
//#define DEBUG
#include <dbg_printf.h>

#define CD_SECTOR_SIZE (2364)
#define SD_SECTOR_SIZE (512)


static sdcard_inst * sdcard;
static uint32_t cur_sector_in_buff = 0xFFFFFFFF;
static uint8_t sdbuff[SD_SECTOR_SIZE] __attribute__((section(".sgdma")));

static  volatile uint8_t sectors[2][CD_SECTOR_SIZE]  __attribute__((section(".sgdma")));

static uint8_t cur_buff_t;
static uint8_t cur_Buff_d;

void sector_reader_init(sdcard_inst * inst){
	sdcard = inst;

	cur_buff_t = 0;
	cur_Buff_d = 0;
}

void sector_reader_swap(){
	cur_buff_t = (cur_buff_t == 0) ? 1 : 0;
	cur_Buff_d = 0;
}

uint8_t sector_reader_is_done(uint32_t ** buff_addr){
	*buff_addr = (uint32_t *)&sectors[cur_buff_t][0];
	return cur_Buff_d;
}

void sector_reader_read(uint32_t sector, uint32_t ** buff_addr){
	uint32_t need_sector;
	uint16_t offset;
	uint16_t byte_readed;
	uint16_t len;

	need_sector = (sector * CD_SECTOR_SIZE) / SD_SECTOR_SIZE;
	offset = (sector * CD_SECTOR_SIZE) % SD_SECTOR_SIZE;
	dbg_printf("RD[%08u] %08u:%03u\t\n", sector, need_sector, offset);
	byte_readed = 0;
	while(byte_readed < CD_SECTOR_SIZE){
		if(cur_sector_in_buff != need_sector){
			sdcard_read_sector(sdcard, need_sector, sdbuff);
			cur_sector_in_buff = need_sector;
		}
		need_sector++;
		len = (SD_SECTOR_SIZE - offset) < (CD_SECTOR_SIZE - byte_readed) ?
				(SD_SECTOR_SIZE - offset) : (CD_SECTOR_SIZE - byte_readed);
		dbg_printf("Len: %03u\r\n", len);
		for(uint16_t i=0; i<len; i++){
			sectors[cur_buff_t][i+byte_readed] = sdbuff[i+offset];
		}

		byte_readed += len;
		offset = 0;
	}
	*buff_addr = (uint32_t *)&sectors[cur_buff_t][0];
	cur_Buff_d = 1;
}
