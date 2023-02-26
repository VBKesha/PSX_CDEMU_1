/*
 * sdcard_spi.h
 *
 *  Created on: 04.09.2020
 *      Author: VBKesha
 */

#ifndef SDCARD_SPI_H_
#define SDCARD_SPI_H_

#define SDCARD_CMD_START 0x40
#define SDCARD_CMD_MSK 0x3F
#define SDCARD_CMD_STOP 0x01

enum SDCARD_STATE {
	SDCARD_IDLE,
	SDCARD_READ_SECTOR,
};

static void sdcard_warmup(sdcard_inst * inst);
static void sdcard_send_cmd(sdcard_inst * inst, uint8_t cmd, uint32_t args, uint8_t crc, uint8_t resp_bytes, uint8_t hold_cs);
static void sdcard_start_read(sdcard_inst * inst, uint32_t sector);
static void sdcard_stop_read(sdcard_inst * inst);
static void sdcard_read_data(sdcard_inst * inst, uint8_t * buff);

#endif /* SDCARD_SPI_H_ */
