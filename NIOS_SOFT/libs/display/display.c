/*
 * display.c
 *
 *  Created on: 26.02.2023
 *      Author: VBKesha
 */

#include <stdint.h>
#include <system.h>
#include <io.h>

#include "display.h"

#define HEX_CLOSE 0x46FF0000
#define HEX_OPEN  0x40FF0000
#define HEX_SDE	  0x12210600
static uint8_t hexseg[] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
static uint32_t isOpen = 1;
static uint8_t save_num = 0;

void display_set_open(){
	isOpen = 1;
	display_num_game(save_num);
}

void display_set_close(){
	isOpen = 0;
	display_num_game(save_num);
}


void display_num_game(uint8_t num){
	uint8_t r,l;

	save_num = num;

	l = num % 10;
	r = num / 10;

	IOWR(HEX4_BASE, 0, ((isOpen) ? HEX_OPEN : HEX_CLOSE) | (hexseg[r] << 8) | hexseg[l]);
}

void diaplay_sd_err(uint8_t num){
	IOWR(HEX4_BASE, 0, HEX_SDE | hexseg[num]);

}
