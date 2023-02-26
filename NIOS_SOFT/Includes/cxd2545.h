/*
 * cxd2545.h
 *
 *  Created on: 14.02.2021
 *      Author: VBKesha
 */

#ifndef CXD2545_H_
#define CXD2545_H_

enum CXD2545_COMMAND {
    CXD2545_CMD_FOCUS_CONTROL = 0x0,
    CXD2545_CMD_TRACKING_CONTROL = 0x1,
    CXD2545_CMD_TRACKING_MODE = 0x2,
    CXD2545_CMD_SELECT = 0x3,
    CXD2545_CMD_AUTO_SEQUENCE = 0x4,
    CXD2545_CMD_BLIND_BRAKE_OVERFLOW = 0x5,
    CXD2545_CMD_SLED_KICK = 0x6,
    CXD2545_CMD_ASEQ_TRACK_COUNT = 0x7,
    CXD2545_CMD_MODE_SPECIFICATION = 0x8,
    CXD2545_CMD_FUNCTION_SPECIFICATION = 0x9,
    CXD2545_CMD_AUDIO_CONTROL = 0xA,
    CXD2545_CMD_TRAVERS_MONITOR_COUNTER = 0xB,
    CXD2545_CMD_SPINDLE_SERVO_SETTING = 0xC,
    CXD2545_CMD_CLV_CTRL = 0xD,
    CXD2545_CMD_CLV_MODE = 0xE,
};

enum TRACKING_MODE {
    SLED_FORWARD = 0x2,
    SLED_REVERSE = 0x3,
};

enum ASEQ_CMD {
    ASEQ_CMD_CANCEL     = 0x0,
    ASEQ_CMD_FINE_SEARCH = 0x2,
    ASEQ_CMD_FOCUS_ON    = 0x3,
    ASEQ_CMD_1TRK_JUMP  = 0x4,
    ASEQ_CMD_10TRK_JUMP = 0x5,
    ASEQ_CMD_2NTRK_JUMP = 0x6,
    ASEQ_CMD_MTRK_JUMP  = 0x7,
};

enum CLV_MODE {
    CLV_MODE_STOP  = 0x0,
    CLV_MODE_KICK  = 0x8,
    CLV_MODE_BRAKE = 0xA,
    CLV_MODE_CLVS  = 0xE,
    CLV_MODE_CLVH  = 0xC,
    CLV_MODE_CLVP  = 0xF,
    CLV_MODE_CLVA  = 0x6,
};

typedef union cxd2545_cmd_t {
    struct {
        uint32_t :20;
        uint32_t id:4;
        uint32_t :8;
    } cmd;

    struct {
        uint32_t :16;
        uint32_t sled:2;
        uint32_t tracking:2;
        uint32_t :12;
    } tracking_mode;

    struct {
        uint32_t :11;
        uint32_t LSSL:1;
        uint32_t MT:4;
        uint32_t dir:1;
        uint32_t cmd:3;
        uint32_t :12;
    } aseq_cmd;

    struct {
        uint32_t :4;
        uint32_t count:16;
        uint32_t :12;
    } aseq_track_count;

    struct {
        uint32_t :13;
        uint32_t SOCT:1;
        uint32_t ASHS:1;
        uint32_t VCOSEL:1;
        uint32_t WSEL:1;
        uint32_t DOUT_MuteF:1;
        uint32_t DOUT_Mute:1;
        uint32_t CDROM:1;
        uint32_t :12;
    } mode_specification;

    struct {
        uint32_t :13;
        uint32_t FLFC:1;
        uint32_t BiliGL_SUB:1;
        uint32_t BiliGL_Main:1;
        uint32_t DPLL:1;
        uint32_t ASEQ:1;
        uint32_t DSPB:1;
        uint32_t DCLV:1;
        uint32_t :12;
    } function_specification;

    struct {
        uint32_t :16;
        uint32_t mode:4;
        uint32_t :12;
    } clv_mode;

    uint32_t raw;
} cxd2545_cmd;

enum CXD2545_STATES {
	CXD_2545_IDLE,
	CXD_2545_WAITDATA,
	CXD_2545_OUTPUT,
};


void cxd2545_init(uint32_t offset, uint32_t lead_out, uint32_t max_sector);
uint32_t cxd2545_cur_sector();
void cxd2545_set_state(enum CXD2545_STATES state);
uint32_t cxd2545_get_state();
uint32_t cxd2545_get_offset();
uint32_t cxd2545_get_sector();
void cxd2545_next_sector();
void cxd2545_change_sector(uint32_t step, uint8_t rev);
void cxd2545_set_asq_jump(uint16_t jump);
uint16_t cxd2545_get_asq_jump();
uint8_t cxd2545_servo_valid();


void cxd2545_cpu_init();

#endif /* CXD2545_H_ */
