/*
 * sdcard_spi.c
 *
 *  Created on: 04.09.2020
 *      Author: VBKesha
 */


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sdcard.h>
#include "sdcard_spi.h"
#include <nios_spi_command.h>
#include "display.h"

//#define DEBUG
#include <dbg_printf.h>

static uint8_t stub[8];
static uint8_t wait = 0xFF;
static uint8_t wait_ret[4];

int nios_spi32_wr(const uint32_t base, const uint32_t slave, const uint32_t write_length,	const uint8_t * write_data, const uint32_t read_length, uint8_t * read_data, const uint32_t flags);

#define spi_wr nios_spi32_wr

uint8_t sdcard_init(sdcard_inst * inst, uint32_t spi_addr){
	uint16_t count;
	inst->spi_base_addr = spi_addr;
	dbg_printf("Start sdcard init\r\n");

	count = 0;
	do{
		dbg_printf("X\r\n");
		sdcard_warmup(inst);
		dbg_printf("Y\r\n");
		sdcard_send_cmd(inst, 0, 0, 0x95, 1, 0);
		dbg_printf("RET: %02X\r\n", inst->ret_buff[0]);
		count++;
		if(count > 254){
			diaplay_sd_err(1);
			while(1){}
		}
	} while(inst->ret_buff[0] != 0x01);

	sdcard_send_cmd(inst, 8, 0x1AA, 0x87, 5, 0);

	count = 0;
	do{
		sdcard_send_cmd(inst, 55, 0x0, 0x0, 1, 0);
		dbg_printf("RET55: %02X\r\n", inst->ret_buff[0]);
		sdcard_send_cmd(inst, 41, 0x40000000, 0x77, 1, 0);
		dbg_printf("RETA41: %02X\r\n", inst->ret_buff[0]);
		count++;
		if(count > 8192){
			diaplay_sd_err(2);
			while(1){}
		}
	}while(inst->ret_buff[0] != 0);

	dbg_printf("Sdcard Inited\r\n");
	inst->state = SDCARD_IDLE;

	return 0;
};


uint8_t sdcard_read_sector(sdcard_inst * inst, uint32_t sector, uint8_t * buff){
	dbg_printf("START READ SECTOR\r\n");
	if((inst->state == SDCARD_READ_SECTOR) && (inst->cur_sector != sector)){
		dbg_printf("Stop previous read\r\n");
		sdcard_stop_read(inst);
	}

	if(inst->state == SDCARD_IDLE){
		dbg_printf("Send read command\r\n");
		sdcard_start_read(inst, sector);
	}
	dbg_printf("Get data from card\r\n");
	sdcard_read_data(inst, buff);
	dbg_printf("STOP READ SECTOR\r\n");
	return 0;
}

static void sdcard_warmup(sdcard_inst * inst){
	memset(inst->cmd_buff, 0xFF, 8);
	spi_wr(inst->spi_base_addr, 1 /* Dirty hack for CS been up*/,
			8, inst->cmd_buff,
			0, stub, 0);
	spi_wr(inst->spi_base_addr, 1 /* Dirty hack for CS been up*/,
			8, inst->cmd_buff,
			0, stub, 0);

}

static void sdcard_send_cmd(sdcard_inst * inst, uint8_t cmd, uint32_t args, uint8_t crc, uint8_t resp_bytes, uint8_t hold_cs){
	uint8_t dp, count;
	/*memset(inst->cmd_buff, 0xFF, 4);
	nios_spi_rw(inst->spi_base_addr, 0,
				1, inst->cmd_buff, stub, ((hold_cs == 0) ? 0 : ALT_AVALON_SPI_COMMAND_MERGE));*/
	inst->cmd_buff[0] = 0xFF;
	inst->cmd_buff[1] = 0xFF;

	inst->cmd_buff[2] = (SDCARD_CMD_START | (cmd & SDCARD_CMD_MSK));
	inst->cmd_buff[3] = (args >> 24);
	inst->cmd_buff[4] = (args >> 16) & 0xFF;
	inst->cmd_buff[5] = (args >> 8) & 0xFF;
	inst->cmd_buff[6] = args  & 0xFF;
	inst->cmd_buff[7] = crc | 1;

	spi_wr(inst->spi_base_addr, 0,
			8, inst->cmd_buff,
			0, stub, ALT_AVALON_SPI_COMMAND_MERGE);

	count = 0;
	do{
		spi_wr(inst->spi_base_addr, 0,
				0, &wait,
				1, wait_ret, ALT_AVALON_SPI_COMMAND_MERGE);
		count++;
		if(count > 254){
			diaplay_sd_err(3);
			while(1){}
		}
	} while(wait_ret[0] == 0xFF);

	dp = 0;
	inst->ret_buff[dp] = wait_ret[0];
	dp++;
	resp_bytes--;
	while(resp_bytes > 0) {
		spi_wr(inst->spi_base_addr, 0,
					0, &wait,
					1, wait_ret, ALT_AVALON_SPI_COMMAND_MERGE);
		inst->ret_buff[dp] = wait_ret[0];
		dp++;
		resp_bytes--;
	};

   	IOWR_ALTERA_AVALON_SPI_CONTROL(inst->spi_base_addr, 0);
}


static void sdcard_start_read(sdcard_inst * inst, uint32_t sector){
	sdcard_send_cmd(inst, 18, sector, 0xFF, 1, 1);
	inst->cur_sector = sector;
	inst->state = SDCARD_READ_SECTOR;
	inst->data_pos = 4;
}

static void sdcard_stop_read(sdcard_inst * inst){
	inst->cmd_buff[0] = 0xFF;
	inst->cmd_buff[1] = 0xFF;
	inst->cmd_buff[2] = 0x40 | 12;
	inst->cmd_buff[3] = 0;
	inst->cmd_buff[4] = 0;
	inst->cmd_buff[5] = 0;
	inst->cmd_buff[6] = 0;
	inst->cmd_buff[7] = 1;

	spi_wr(inst->spi_base_addr, 0,
			8, inst->cmd_buff,
			1, inst->ret_buff, 0);
	inst->state = SDCARD_IDLE;
}

static void sdcard_read_data(sdcard_inst * inst, uint8_t * buff){
	uint16_t i;
	dbg_printf("SDCARD_WAIT_DATA\r\n");
	//printf("AI[%d]: %02X %02X %02X %02X\r\n", inst->data_pos, inst->ret_buff[0], inst->ret_buff[1], inst->ret_buff[2],inst->ret_buff[3]);

	do{
		spi_wr(inst->spi_base_addr, 0,
			0, inst->cmd_buff,
			1, inst->ret_buff, ALT_AVALON_SPI_COMMAND_MERGE);
	} while(inst->ret_buff[0] != 0xFE);

	dbg_printf("SDCARD_READ_DATA\r\n");
	i=0;
	//printf("OFFSET I: %d\r\n", i);
	spi_wr(inst->spi_base_addr, 0,
			0, inst->cmd_buff,
			512, buff,
			ALT_AVALON_SPI_COMMAND_MERGE);

	//dbg_printf("SDCARD_READ_CRC[%d]\r\n", offset);
	dbg_printf("OFFDATA: %02X %02X %02X %02X\r\n", inst->ret_buff[0], inst->ret_buff[1], inst->ret_buff[2], inst->ret_buff[3]);
//	printf("AW[%d]: %02X %02X %02X %02X --- ", inst->data_pos, inst->ret_buff[0], inst->ret_buff[1], inst->ret_buff[2],inst->ret_buff[3]);

	spi_wr(inst->spi_base_addr, 0,
		0, inst->cmd_buff,
		2, inst->ret_buff, ALT_AVALON_SPI_COMMAND_MERGE);

	/*for(i=0; i<4; i++){
		printf("%02X ",inst->ret_buff[i]);
	}
	printf("\r\n Offset: %d, DP: %d\r\n", offset, inst->data_pos);*/
	/*nios_spi_command(inst->spi_base_addr, 0,0, inst->cmd_buff,
			2, inst->ret_buff, ALT_AVALON_SPI_COMMAND_MERGE);*/
	inst->cur_sector++;
}
