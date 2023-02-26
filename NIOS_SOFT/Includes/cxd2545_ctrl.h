/*
 * cxd2545_ctrl.h
 *
 *  Created on: 15.02.2023
 *      Author: VBKesha
 */

#ifndef CXD2545_CTRL_H_
#define CXD2545_CTRL_H_
#include <io.h>
#include <system.h>

#define CTRL_DRAIN_DMA  0x0001
#define CTRL_TRK0	    0x0004
#define CTRL_DBL_SPEED  0x0008
#define CTRL_DOOR  		0x0010
#define CTRL_TRC_CNT	0x0020
#define CTRL_TST		0x0040
#define CTRL_SOCT		0x0080

static inline void cxd2545_dma_drain_on(){
	IOWR(CXD2545_CTRL_BASE, 4, CTRL_DRAIN_DMA);
}

static inline void cxd2545_dma_drain_off(){
	IOWR(CXD2545_CTRL_BASE, 5, CTRL_DRAIN_DMA);
}

static inline void cxd2545_set_cd_speed(uint32_t speed){
	if(speed){
		IOWR(CXD2545_CTRL_BASE, 4, CTRL_DBL_SPEED);
	} else {
		IOWR(CXD2545_CTRL_BASE, 5, CTRL_DBL_SPEED);
	}
}

static inline void cxd2545_set_trk0(uint32_t sector){
	if(sector){
		IOWR(CXD2545_CTRL_BASE, 5, CTRL_TRK0);
	} else {
		IOWR(CXD2545_CTRL_BASE, 4, CTRL_TRK0);
	}
}

static inline void cxd2545_set_door(uint32_t door){
	if(door){
		IOWR(CXD2545_CTRL_BASE, 4, CTRL_DOOR);
	} else {
		IOWR(CXD2545_CTRL_BASE, 5, CTRL_DOOR);
	}
}

static inline void cxd2545_track_counter_start(){
	IOWR(CXD2545_CTRL_BASE, 4, CTRL_TRC_CNT);
}

static inline void cxd2545_track_counter_stop(){
	IOWR(CXD2545_CTRL_BASE, 5, CTRL_TRC_CNT);
}


static inline void cxd2545_tst_start(){
	IOWR(CXD2545_CTRL_BASE, 4, CTRL_TST);
}

static inline void cxd2545_tst_stop(){
	IOWR(CXD2545_CTRL_BASE, 5, CTRL_TST);
}

static inline void cxd2545_set_soct(uint32_t soct){
	if(soct){
		IOWR(CXD2545_CTRL_BASE, 4, CTRL_SOCT);
	} else {
		IOWR(CXD2545_CTRL_BASE, 5, CTRL_SOCT);
	}
}

#undef CTRL_DRAIN_DMA
#undef CTRL_TRK0
#undef CTRL_DBL_SPEED
#undef CTRL_DOOR
#undef CTRL_TRC_CNT
#undef CTRL_TST
#undef CTRL_SOCT

#endif /* CXD2545_CTRL_H_ */
