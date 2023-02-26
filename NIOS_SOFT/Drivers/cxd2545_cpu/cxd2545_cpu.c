/*
 * cxd2545_cpu.c
 *
 *  Created on: 07.09.2020
 *      Author: VBKesha
 */
#include <stdio.h>
#include <stdint.h>
#include <sys/alt_irq.h>
#include <io.h>
#include <system.h>

#include "cxd2545.h"
#include "cxd2545_ctrl.h"
#include "sens_ctrl.h"

static const uint32_t cxd2545_cpu_addr = CXD2545_CPU_0_BASE;
static const uint32_t cxd2545_cpu_int_num = CXD2545_CPU_0_IRQ;

#define CXD2545_CPU_CMD_REG (0x00)
#define CXD2545_CPU_IRQ_CLR (0x04)


static uint8_t dir = 0;
static uint8_t sled_work = 0;
static uint8_t prev_dir = 0;
static uint8_t sled_break = 0;

static enum CXD2545_STATES prev_state;

static void cxd2545_cpu_isr(void * in, uint32_t id){
	cxd2545_cmd command;
	uint32_t scount;

	command.raw = IORD_32DIRECT(cxd2545_cpu_addr, CXD2545_CPU_CMD_REG);
	IORD_32DIRECT(cxd2545_cpu_addr, CXD2545_CPU_IRQ_CLR);

	switch(command.cmd.id){
		case CXD2545_CMD_TRACKING_MODE:
			cxd2545_dma_drain_on();

			if(sled_work == 1){
				cxd2545_track_counter_stop();
				printf("%c", (dir == 0) ? '+' : '-');
				do{
					scount = IORD(TRACK_COUNTER_BASE, 0);
				} while((scount & 0x80000000) == 0);

				scount &= 0x7FFFFFFF;

				if((sled_break == 0) || (prev_dir == dir)){
					cxd2545_change_sector(scount & 0x7FFFFFFF, dir);
				}
				sled_work = 0;
				sled_break = 1;
				prev_dir = dir;
			}

			switch(command.tracking_mode.sled){
				case SLED_FORWARD:
					cxd2545_set_state(CXD_2545_IDLE);
					cxd2545_track_counter_start();
					dir = 0;
					sled_work = 1;
					prev_state = cxd2545_get_state();
					cxd2545_set_state(CXD_2545_IDLE);
				break;

				case SLED_REVERSE:
					cxd2545_set_state(CXD_2545_IDLE);
					cxd2545_track_counter_start();
					dir = 1;
					sled_work = 1;
					prev_state = cxd2545_get_state();
					cxd2545_set_state(CXD_2545_IDLE);
	        	break;
			}

		break;


		case CXD2545_CMD_AUTO_SEQUENCE:
			cxd2545_dma_drain_on();
			cxd2545_set_state(CXD_2545_IDLE);

	        switch(command.aseq_cmd.cmd){
	            case ASEQ_CMD_CANCEL:
	            	sens_XBUSY_on();
	            break;

	            case ASEQ_CMD_FINE_SEARCH:
	            break;


	            case ASEQ_CMD_FOCUS_ON:
	            	sens_FOK_on();
	            	sens_XBUSY_on();
	            break;


	            case ASEQ_CMD_1TRK_JUMP:
	            	cxd2545_change_sector(1, command.aseq_cmd.dir);
	            break;

	            case ASEQ_CMD_10TRK_JUMP:
	            	cxd2545_change_sector(10, command.aseq_cmd.dir);
	            break;


	            case ASEQ_CMD_2NTRK_JUMP:
	            	cxd2545_change_sector(cxd2545_get_asq_jump() * 2, command.aseq_cmd.dir);
	            break;

	            case ASEQ_CMD_MTRK_JUMP:
	            	cxd2545_change_sector(cxd2545_get_asq_jump(), command.aseq_cmd.dir);
	            break;

	        }
		break;

		case CXD2545_CMD_ASEQ_TRACK_COUNT:
			cxd2545_set_asq_jump(command.aseq_track_count.count);
		break;

		case CXD2545_CMD_MODE_SPECIFICATION:
			cxd2545_set_soct(command.mode_specification.SOCT);
		break;

		case CXD2545_CMD_FUNCTION_SPECIFICATION:
			cxd2545_set_cd_speed(command.function_specification.DSPB);
		break;

		case CXD2545_CMD_CLV_MODE:
			sled_break = 0;
			switch(command.clv_mode.mode){
				case CLV_MODE_STOP:
				case CLV_MODE_BRAKE:
					cxd2545_dma_drain_on();
					cxd2545_set_state(CXD_2545_IDLE);
					printf("T");
				break;

				case CLV_MODE_KICK:
					printf("K");
				break;


				case CLV_MODE_CLVS:
				case CLV_MODE_CLVH:
				case CLV_MODE_CLVP:
				case CLV_MODE_CLVA:
					if(cxd2545_servo_valid() != 0){
						cxd2545_set_state(CXD_2545_WAITDATA);
						printf("S");
					} else {
						printf("E");
					}
				break;
			}
		break;
	}
}

void cxd2545_cpu_init(){
	alt_irq_register(cxd2545_cpu_int_num, NULL, cxd2545_cpu_isr);
	alt_irq_enable(cxd2545_cpu_int_num);

	sled_work = 0;
}
