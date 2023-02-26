/*
 * main.c
 *
 *  Created on: 13.05.2022
 *      Author: VBKesha
 */
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>

#include <system.h>
#include <altera_avalon_uart_regs.h>
#include <io.h>

#include <dbg_printf.h>

#include "sdcard.h"
#include "sector_reader.h"
#include "cxd2545.h"
#include "cxd2545_ctrl.h"
#include "sens_ctrl.h"
#include "sgdma.h"
#include "display.h"



#define SDCARD_SPI_ADDR SDCARD_SPI_BASE

#define DOOR_OPEN 0x01
#define DOOR_CLOSE 0x02
#define PREV_GAME 0x08
#define NEXT_GAME 0x04

sdcard_inst sdcard;

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

toc_sector * game_list;

static void alt_putchar(char c){
	while((IORD(UART_DEBUG_BASE, 2) & ALTERA_AVALON_UART_STATUS_TRDY_MSK) == 0){}
	IOWR(UART_DEBUG_BASE, 1, c);
}

char inkey(){

	return ((IORD(UART_DEBUG_BASE, 2) & ALTERA_AVALON_UART_STATUS_RRDY_MSK) != 0) ? IORD(UART_DEBUG_BASE, 0) : 0x00 ;
}

int write(int file, const void *ptr, size_t len){
	const char * cptr = ptr;
	int i;
	for(i=0; i<len; i++){
		alt_putchar(cptr[i]);
	}
	return len;
}

void print_sector(uint8_t * data){
	uint16_t i;
	for(i=0; i<2364; i++){
		if((i % 32) == 0){
			printf("\r\n%04u:", i);
		}
		printf(" %02X", data[i]);
	}
	printf("\r\n");
}

char game_name[21];
void get_gname(const char * name_in, char * name_out){
	memset(name_out, 0, 21);
	memcpy(name_out, name_in, 20);
}

int main(){
	uint8_t sel = 0;
	uint16_t trc_cnt = 1400;
	uint32_t * data;
	uint8_t key, uart_key;
	uint8_t old_sel;

	printf("Start!\r\n");
	cxd2545_set_door(0);
	sdcard_init(&sdcard, SDCARD_SPI_ADDR);
	printf("SDCard Done!\r\n");

	sector_reader_init(&sdcard);

	sgdma_init();
	cxd2545_cpu_init();
	sens_init();
	cxd2545_track_counter_stop();

	IOWR(CXD_TRACK_CTRL_BASE, 0, trc_cnt);

	sector_reader_read(0, &data);
	game_list = (toc_sector *) (uint32_t *)data;
	printf("Magic: %08lX\r\n", game_list->games.magic);
	if(game_list->games.magic != 0xAB031337){
		diaplay_sd_err(9);
		while(1){};
	}

	uint8_t max_games;
	while(game_list->games.item[sel].offset != 0){
		get_gname(game_list->games.item[sel].name, game_name);
		printf("%u - %s[%lu:%lu:%lu]\r\n", sel, game_name, game_list->games.item[sel].offset, game_list->games.item[sel].lead_out, game_list->games.item[sel].disk_size);
		sel++;
	}
	max_games = sel;

	usleep(100000);

	sel=0;
	old_sel = sel;

	while(1){
		cxd2545_set_state(CXD_2545_IDLE);
		cxd2545_dma_drain_off();
		sector_reader_read(0, &data);
		game_list = (toc_sector *) (uint32_t *)data;
		display_set_open();
		cxd2545_set_door(0);
		while(1){
			key = IORD(BUTTONS_BASE, 3);
			IOWR(BUTTONS_BASE, 3, key);
			uart_key = inkey();
			if((key & NEXT_GAME) || (uart_key == '/')){
				sel++;
				if(game_list->games.item[sel].offset == 0){
					sel = 0;
				}
			}

			if((key & PREV_GAME) || (uart_key == '.')){
				if(sel == 0){
					sel = max_games - 1;
				} else {
					sel--;
				}

			}
			if(old_sel != sel){
				get_gname(game_list->games.item[sel].name, game_name);
				printf("%u - %s[%lu:%lu:%lu]\r\n", sel, game_name, game_list->games.item[sel].offset, game_list->games.item[sel].lead_out, game_list->games.item[sel].disk_size);
				old_sel=sel;
			}

			if((key & DOOR_CLOSE) || (uart_key == ']')){
				display_set_close();
				get_gname(game_list->games.item[sel].name, game_name);
				printf("%u - %s[%lu:%lu:%lu]\r\n", sel, game_name, game_list->games.item[sel].offset, game_list->games.item[sel].lead_out, game_list->games.item[sel].disk_size);
				cxd2545_init(game_list->games.item[sel].offset, game_list->games.item[sel].lead_out, game_list->games.item[sel].disk_size);
				cxd2545_set_door(1);
				break;
			}

			display_num_game(sel);
		}

		printf("Start emulation!\r\n");
		while(1){

			if(cxd2545_get_state() == CXD_2545_WAITDATA){
				cxd2545_dma_drain_off();
				if(sgdma_is_working() != 1){
					cxd2545_tst_start();

					sector_reader_read(cxd2545_cur_sector() + cxd2545_get_offset(), &data);
					sgdma_send_buffer((uint8_t *)data, 2364);
					cxd2545_set_state(CXD_2545_OUTPUT);
					cxd2545_next_sector();
					sector_reader_swap();

					cxd2545_tst_stop();
				}
			}

			if(cxd2545_get_state() == CXD_2545_OUTPUT){
				if(sector_reader_is_done(&data) == 0){
					sector_reader_read(cxd2545_cur_sector() + cxd2545_get_offset(), &data);
				}
			}

			key = IORD(BUTTONS_BASE, 3);
			IOWR(BUTTONS_BASE, 3, key);
			uart_key = inkey();
			if((key & DOOR_OPEN) || (uart_key == '[')){
				printf("Stop emulation!\r\n");
				break;
			}
		}
	}
	return 0;
}
