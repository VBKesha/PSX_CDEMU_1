#include <stdio.h>
#include <stdint.h>

#include <system.h>
#include <io.h>

#define SENS_FCZ     	0x0001
#define SENS_AS     	0x0002
#define SENS_TZC    	0x0004
#define SENS_SSTP     	0x0008
#define SENS_XBUSY    	0x0010
#define SENS_FOK     	0x0020
#define SENS_GFS     	0x0400
#define SENS_COMP     	0x0800
#define SENS_COUT     	0x1000
#define SENS_XOV64     	0x2000
#define SENS_ZERO     	0xC3C0


void sens_init(){
	IOWR(SENS_OUT_BASE, 4, 0xFFFF);
	IOWR(SENS_OUT_BASE, 5, SENS_COMP);
	IOWR(SENS_OUT_BASE, 5, SENS_COUT);
	IOWR(SENS_OUT_BASE, 5, SENS_ZERO);
	IOWR(SENS_OUT_BASE, 5, SENS_GFS);
	IOWR(SENS_OUT_BASE, 5, SENS_FOK);
	IOWR(SENS_OUT_BASE, 5, SENS_XOV64);
}

void sens_FOK_on(){
	IOWR(SENS_OUT_BASE, 4, SENS_FOK);
}

void sens_FOK_off(){
	IOWR(SENS_OUT_BASE, 5, SENS_FOK);
}

void sens_XBUSY_on(){
	IOWR(SENS_OUT_BASE, 4, SENS_XBUSY);
}

void sens_XBUSY_off(){
	IOWR(SENS_OUT_BASE, 5, SENS_XBUSY);
}

