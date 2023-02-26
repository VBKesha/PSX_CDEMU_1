/*
 * cxd2545.c
 *
 *  Created on: 20.03.2021
 *      Author: VBKesha
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/alt_irq.h>
#include <io.h>

#include "cxd2545.h"
#include "cxd2545_ctrl.h"

uint32_t zone[] = 			{13500, 27000, 45000, 63000, 85500, 103500, 130500, 153000, 175500, 207000, 234000, 265500, 297000, 999999};
uint32_t sect_pet_track[] = { 	10,	   11,    12,    13,    14,     15,     16,     17,     18,     19,     20,     21,     22,     23};

typedef struct cxd2545_t {
	uint8_t state;
	uint32_t sector;
	uint16_t asq_jump;
	uint8_t cur_zone;

	uint32_t offset;
	uint32_t lead_out;
	uint32_t max_sector;
} cxd2545;

static cxd2545 cxd_inst;
static alt_irq_context ictx;

void cxd2545_init(uint32_t offset, uint32_t lead_out, uint32_t max_sector){
	cxd_inst.state = 0;
	cxd_inst.sector = 0;
	cxd_inst.asq_jump = 0;
	cxd_inst.cur_zone = 0;

	cxd_inst.offset = offset;
	cxd_inst.lead_out = lead_out;
	cxd_inst.max_sector = max_sector;
	cxd2545_set_trk0(cxd_inst.sector);
	printf("Offset %lu, Out %lu, max_sector %lu\r\n", offset, lead_out, max_sector);
}

uint32_t cxd2545_cur_sector(){
	return cxd_inst.sector;
}

uint32_t cxd2545_get_offset(){
	return cxd_inst.offset;
}

void cxd2545_set_state(enum CXD2545_STATES state){
	cxd_inst.state = state;
}

uint32_t cxd2545_get_state(){
	return cxd_inst.state;
}

void cxd2545_set_asq_jump(uint16_t jump){
	cxd_inst.asq_jump = jump;
}


uint16_t cxd2545_get_asq_jump(){
	return cxd_inst.asq_jump;
}

uint32_t cxd2545_get_sector(){
	return cxd_inst.sector;
}

void cxd2545_next_sector(){
	if(cxd_inst.sector < cxd_inst.max_sector){
		cxd_inst.sector++;
	} else {
		cxd2545_dma_drain_on();
		cxd2545_set_state(CXD_2545_IDLE);
	}
}

uint8_t cxd2545_servo_valid(){
	return cxd_inst.sector < cxd_inst.max_sector;

}

void cxd2545_change_sector(uint32_t step, uint8_t rev){
	ictx = alt_irq_disable_all();

	cxd2545_set_state(CXD_2545_IDLE);
	cxd2545_dma_drain_on();

	while(step != 0){
        if(rev == 0){
            uint32_t sbze = zone[cxd_inst.cur_zone] - cxd_inst.sector;
            uint32_t sbreq = step*sect_pet_track[cxd_inst.cur_zone];
            uint32_t do_steps = ((sbze > sbreq) ? sbreq : sbze) / sect_pet_track[cxd_inst.cur_zone];
            cxd_inst.sector += do_steps*sect_pet_track[cxd_inst.cur_zone];
            if(cxd_inst.sector > cxd_inst.max_sector){
                cxd_inst.sector = cxd_inst.lead_out;
                break;
            }
            step -= do_steps;
            if(step > 0){
                cxd_inst.cur_zone++;
            }
        } else {
            uint32_t sect_before_ze;
            uint32_t do_steps;
            uint32_t req_sect_step = step*sect_pet_track[cxd_inst.cur_zone];

            if(cxd_inst.cur_zone > 0){
                sect_before_ze = cxd_inst.sector - zone[cxd_inst.cur_zone-1];
            } else {
                sect_before_ze = req_sect_step;
            }

            do_steps = ((sect_before_ze > req_sect_step) ? req_sect_step : sect_before_ze) / sect_pet_track[cxd_inst.cur_zone];
            if(cxd_inst.sector > (do_steps*sect_pet_track[cxd_inst.cur_zone])){
                cxd_inst.sector -= do_steps*sect_pet_track[cxd_inst.cur_zone];
            } else {
                cxd_inst.sector = 0;
                break;
            }
            step -= do_steps;
            if((cxd_inst.cur_zone > 0) && (step > 0)){
                cxd_inst.cur_zone--;
            }
        }
	}

	cxd2545_set_trk0(cxd_inst.sector);

	alt_irq_enable_all(ictx);
}
