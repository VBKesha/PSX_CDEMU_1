/*
 * dbg_printf.h
 *
 *  Created on: 04.09.2020
 *      Author: VBKesha
 */

#ifndef DBG_PRINTF_H_
#define DBG_PRINTF_H_

#ifndef DEBUG
	#define dbg_printf(fmt, ...)
#else
	#define dbg_printf(fmt, ...) printf("%s:%d:" fmt, __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#endif /* DBG_PRINTF_H_ */
