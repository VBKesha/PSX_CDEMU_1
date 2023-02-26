/*
 * nios_spi_command.h
 *
 *  Created on: 03.04.2019
 *      Author: VBKesha
 */

#ifndef NIOS_SPI_COMMAND_H_
#define NIOS_SPI_COMMAND_H_

#include <altera_avalon_spi_regs.h>

int nios_spi_command2(uint32_t base, uint32_t slave,
					uint32_t write_length, const uint8_t * write_data,
					uint32_t read_length, uint8_t * read_data,
					uint32_t flags);

void nios_spi_rw(uint32_t base, uint32_t slave,
		uint32_t w_length, const uint32_t * write_data,
		uint32_t r_length, uint32_t * read_data,
		uint32_t flags);

void nios_spi_r(uint32_t base, uint32_t slave,
		uint32_t r_length, uint8_t * read_data,
		uint32_t * data_end, uint8_t * data_pos,
		uint32_t flags);

#define ALT_AVALON_SPI_COMMAND_MERGE (0x01)

#endif /* NIOS_SPI_COMMAND_H_ */
