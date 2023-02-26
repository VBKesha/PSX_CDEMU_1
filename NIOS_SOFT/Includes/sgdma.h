/*
 * sgdma.h
 *
 *  Created on: 04.09.2020
 *      Author: VBKesha
 */

#ifndef SGDMA_H_
#define SGDMA_H_

void sgdma_init();
uint8_t sgdma_send_buffer(uint8_t * buffer, uint16_t length);
uint8_t sgdma_is_working();

#endif /* SGDMA_H_ */
