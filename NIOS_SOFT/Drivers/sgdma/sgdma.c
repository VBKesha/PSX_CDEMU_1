/*
 * sgdma.c
 *
 *  Created on: 04.09.2020
 *      Author: VBKesha
 */
#include <stdio.h>
#include <stdint.h>
#include <system.h>
#include <io.h>

#include <altera_avalon_sgdma_descriptor.h>
#include <altera_avalon_sgdma_regs.h>
#include <sys/alt_irq.h>

#include "sdcard.h"
#include "sector_reader.h"
#include "cxd2545.h"

static volatile alt_sgdma_descriptor dma_data_desc __attribute__((section(".sgdma")));
static volatile alt_sgdma_descriptor dma_end_desc __attribute__((section(".sgdma")));
static uint8_t is_dma_work;

//#define printf(fmt, ...)

// static volatile uint8_t sector[2364]  __attribute__((section(".sdram")));
uint8_t sgdma_is_working(){
	return is_dma_work;
}

uint8_t sgdma_send_buffer(uint8_t * buffer, uint16_t length){

	if(is_dma_work != 0){
		return 1;
	}

	dma_data_desc.bytes_to_transfer = length;
	dma_data_desc.read_addr = (uint32_t *) buffer;
	dma_data_desc.next = (uint32_t *) &dma_end_desc;
	dma_data_desc.control = 0x85;
	dma_end_desc.control = 0x00;

	IOWR(SGDMA_CXD2545_DATA_BASE, 4, ALTERA_AVALON_SGDMA_CONTROL_IE_GLOBAL_MSK | ALTERA_AVALON_SGDMA_CONTROL_IE_CHAIN_COMPLETED_MSK | ALTERA_AVALON_SGDMA_CONTROL_CLEAR_INTERRUPT_MSK);
	IOWR(SGDMA_CXD2545_DATA_BASE, 8, (int)&dma_data_desc);
	IOWR(SGDMA_CXD2545_DATA_BASE, 4, ALTERA_AVALON_SGDMA_CONTROL_IE_GLOBAL_MSK | ALTERA_AVALON_SGDMA_CONTROL_IE_CHAIN_COMPLETED_MSK | ALTERA_AVALON_SGDMA_CONTROL_RUN_MSK);
	/*
	alt_avalon_sgdma_construct_mem_to_stream_desc((alt_sgdma_descriptor *)&dma_data_desc, (alt_sgdma_descriptor *)&dma_end_desc, (uint32_t *)buffer, length, 0, 1, 1 ,0);
	if(alt_avalon_sgdma_do_async_transfer(dma, (alt_sgdma_descriptor *)&dma_data_desc) != 0){
		printf("SGDAM ASYNC: ERRR!\r\n");
	}
	*/

	is_dma_work = 1;

	return 0;
}

static void sgdma_irq(){
	//IOWR(CXD2545_CTRL_BASE, 5, CTRL_DRAIN_DMA);
	uint32_t * data;
	IOWR(SGDMA_CXD2545_DATA_BASE, 4, ALTERA_AVALON_SGDMA_CONTROL_CLEAR_INTERRUPT_MSK);

	is_dma_work = 0;
	if(cxd2545_get_state() == CXD_2545_OUTPUT){
		if(sector_reader_is_done(&data) != 0){
			sgdma_send_buffer((uint8_t *)data, 2364);
			cxd2545_next_sector();
			sector_reader_swap();
			printf("C");
		} else {
			cxd2545_set_state(CXD_2545_WAITDATA);
			printf("M");
		}
	} else {
		printf("P");
	}

}

void sgdma_init(){
	//dma = alt_avalon_sgdma_open("/dev/SGDMA_CXD2545_DATA");
	is_dma_work = 0;
	IOWR(SGDMA_CXD2545_DATA_BASE, 4, ALTERA_AVALON_SGDMA_CONTROL_IE_GLOBAL_MSK | ALTERA_AVALON_SGDMA_CONTROL_IE_CHAIN_COMPLETED_MSK);


	alt_irq_register(SGDMA_CXD2545_DATA_IRQ, NULL, sgdma_irq);
	alt_irq_enable(SGDMA_CXD2545_DATA_IRQ);
	//alt_avalon_sgdma_init(dma, 0, 0);
	//alt_avalon_sgdma_register_callback(dma, sgdma_irq, (ALTERA_AVALON_SGDMA_CONTROL_IE_GLOBAL_MSK | ALTERA_AVALON_SGDMA_CONTROL_IE_CHAIN_COMPLETED_MSK), /*inst*/0);
}
