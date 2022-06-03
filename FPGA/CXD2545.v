//Legal Notice: (C)2006 Altera Corporation. All rights reserved. Your
//use of Altera Corporation's design tools, logic functions and other
//software and tools, and its AMPP partner logic functions, and any
//output files any of the foregoing (including device programming or
//simulation files), and any associated documentation or information are
//expressly subject to the terms and conditions of the Altera Program
//License Subscription Agreement or other applicable license agreement,
//including, without limitation, that your use is for the sole purpose
//of programming logic devices manufactured by Altera and sold by Altera
//or its authorized distributors.  Please refer to the applicable
//agreement for further details.


module CXD2545	(
	////////////////////	Clock Input	 	////////////////////	 
	input	 [1:0]	CLOCK_24,				//	24 MHz
	input	 [1:0]	CLOCK_27,				//	27 MHz
	input			CLOCK_50,				//	50 MHz
	input			EXT_CLOCK,				//	External Clock

	////////////////////	Push Button		////////////////////
	input	 [3:0]	KEY,					//	Pushbutton[3:0]

	////////////////////	DPDT Switch		////////////////////
	input	 [9:0]	SW,						//	Toggle Switch[9:0]

	////////////////////	7-SEG Dispaly	////////////////////
	output 	 [6:0]	HEX0,					//	Seven Segment Digit 0
	output 	 [6:0]	HEX1,					//	Seven Segment Digit 1
	output 	 [6:0]	HEX2,					//	Seven Segment Digit 2
	output 	 [6:0]	HEX3,					//	Seven Segment Digit 3

	////////////////////////	LED		////////////////////////
	output 	 [7:0]	LEDG,				//	LED Green[7:0]
	output 	 [9:0]	LEDR,				//	LED Red[9:0]

	////////////////////////	UART	////////////////////////
	output 			UART_TXD,			//	UART Transmitter
	input 			UART_RXD,			//	UART Receiver

	///////////////////// SDRAM Interface	////////////////
	inout	  [15:0]	DRAM_DQ,				//	SDRAM Data bus 16 Bits
	output  [11:0]	DRAM_ADDR,				//	SDRAM Address bus 12 Bits
	output 			DRAM_LDQM,				//	SDRAM Low-byte Data Mask 
	output			DRAM_UDQM,				//	SDRAM High-byte Data Mask
	output			DRAM_WE_N,				//	SDRAM Write Enable
	output			DRAM_CAS_N,				//	SDRAM Column Address Strobe
	output			DRAM_RAS_N,				//	SDRAM Row Address Strobe
	output			DRAM_CS_N,				//	SDRAM Chip Select
	output			DRAM_BA_0,				//	SDRAM Bank Address 0
	output			DRAM_BA_1,				//	SDRAM Bank Address 0
	output			DRAM_CLK,				//	SDRAM Clock
	output			DRAM_CKE,				//	SDRAM Clock Enable

	////////////////////	Flash Interface	////////////////
	inout  [7:0]	FL_DQ,				//	FLASH Data bus 8 Bits
	output [21:0]  FL_ADDR,				//	FLASH Address bus 22 Bits
	output			FL_WE_N,				//	FLASH Write Enable
	output			FL_RST_N,			//	FLASH Reset
	output			FL_OE_N,				//	FLASH Output Enable
	output			FL_CE_N,				//	FLASH Chip Enable
	
	////////////////////	SRAM Interface		////////////////
	inout	 [15:0]	SRAM_DQ,					//	SRAM Data bus 16 Bits
	output  [17:0]	SRAM_ADDR,					//	SRAM Address bus 18 Bits
	output			SRAM_UB_N,					//	SRAM High-byte Data Mask 
	output			SRAM_LB_N,					//	SRAM Low-byte Data Mask 
	output			SRAM_WE_N,					//	SRAM Write Enable
	output			SRAM_CE_N,					//	SRAM Chip Enable
	output			SRAM_OE_N,					//	SRAM Output Enable
	
	////////////////////	SD_Card Interface	////////////////
	inout			SD_DAT,						//	SD Card Data
	inout			SD_DAT3,						//	SD Card Data 3
	inout			SD_CMD,						//	SD Card Command Signal
	output		SD_CLK,						//	SD Card Clock
	
	////////////////////	USB JTAG link	////////////////////
	input			TDI,  					// CPLD -> FPGA (data in)
	input			TCK,  					// CPLD -> FPGA (clk)
	input			TCS,  					// CPLD -> FPGA (CS)
	output		TDO,						// FPGA -> CPLD (data out)
	
	////////////////////	I2C	////////////////////////////
	inout			I2C_SDAT,	//	I2C Data
	output 		I2C_SCLK,	//	I2C Clock
	
	////////////////////	PS2	////////////////////////////
	input 		PS2_DAT,	//	PS2 Data
	input			PS2_CLK,	//	PS2 Clock
	
	////////////////////	VGA		////////////////////////////
	output 			VGA_HS,			//	VGA H_SYNC
	output 			VGA_VS,			//	VGA V_SYNC
	output 	[3:0]	VGA_R,   		//	VGA Red[3:0]
	output 	[3:0]	VGA_G,	 		//	VGA Green[3:0]
	output 	[3:0]	VGA_B,  			//	VGA Blue[3:0]
	
	////////////////	Audio CODEC		////////////////////////
	inout			AUD_ADCLRCK,		//	Audio CODEC ADC LR Clock
	input			AUD_ADCDAT,			//	Audio CODEC ADC Data
	inout			AUD_DACLRCK,		//	Audio CODEC DAC LR Clock
	output		AUD_DACDAT,			//	Audio CODEC DAC Data
	inout			AUD_BCLK,			//	Audio CODEC Bit-Stream Clock
	output		AUD_XCK,				//	Audio CODEC Chip Clock
	
	////////////////////	GPIO	////////////////////////////
	inout	[35:0]	GPIO_0,			//	GPIO Connection 0
	inout	[35:0]	GPIO_1			//	GPIO Connection 1
);


//	Turn on all display
assign	HEX0		=	7'h7f;
assign	HEX1		=	7'h7f;
assign	HEX2		=	7'h7f;
assign	HEX3		=	7'h7f;
//assign	LEDG		=	8'h00;
//assign	LEDR		=	10'h00;


//	All inout port turn to tri-state
//assign	DRAM_DQ		=	16'hzzzz;
assign	FL_DQ			=	8'hzz;
assign	SRAM_DQ		=	16'hzzzz;
assign	SD_DAT		=	1'bz;
assign	I2C_SDAT		=	1'bz;
assign	AUD_ADCLRCK	=	1'bz;
assign	AUD_DACLRCK	=	1'bz;
assign	AUD_BCLK		=	1'bz;
//assign	GPIO_0		=	36'bz;
//assign	GPIO_1		=	36'bz;


// CDX2545 Controll pins
localparam PIN_CLK 	= 18;
localparam PIN_DATA 	= 19;
localparam PIN_XLAT 	= 17;
localparam PIN_SENS 	= 16;
localparam PIN_SCLK 	= 15;

// CDX2545 SUBQ Pins
localparam PIN_SQCK = 14;
localparam PIN_SUBQ = 13;
localparam PIN_SCOR = 12;

// CDX2545 Data Pins
localparam PIN_EMPH	 = 0;
localparam PIN_C2PO	 = 1;
localparam PIN_CDCLK  = 4;
localparam PIN_CDLRCK = 2;
localparam PIN_CDDATA = 3;

// CXD2545 Clocks
localparam PIN_MCLK   = 5;  // Clock SUBCPU
localparam PIN_SYSCLK = 11; // System Clock 16.9xxx MHz

// CXD2545 Status pin
localparam PIN_LMTSW  = 10;
localparam PIN_DOOR  = 7;

reg w;

//assign LEDG[3] = w;

wire CPU_CLK = CLOCK_50;
wire ext_clk = GPIO_0[PIN_SYSCLK];

reg [2:0] clk_div;
reg [7:0] clk_div2;

wire clk_44100;
wire cdr_dat_clk;
wire lbit;
wire cdr_data;
wire scor, sqck, sqso;

reg CLK_DOUBLE_SPEED, CLK_SINGLE_SPEED;
wire  CD_CLK;
//wire cd_clk = ext_clk;

reg SUB_CPU_CLK;
always @(negedge ext_clk) begin
	clk_div <= clk_div + 1'b1;
	
	SUB_CPU_CLK 		<= clk_div[1];
	CLK_SINGLE_SPEED 	<= clk_div[1];
	CLK_DOUBLE_SPEED 	<= clk_div[0];
end

assign CD_CLK = (DSPB == 0) ? CLK_SINGLE_SPEED : CLK_DOUBLE_SPEED;


assign GPIO_0[PIN_EMPH] 	= SW[0]; 		// EMPH
assign GPIO_0[PIN_C2PO] 	= c2po; 			// C2PO
assign GPIO_0[PIN_CDLRCK] 	= clk_44100; 	// LRCK (44100 / 88200)
assign GPIO_0[PIN_CDDATA] 	= cdr_data; 	// DATA
assign GPIO_0[PIN_CDCLK] 	= cdr_dat_clk; // BCLK / D15 (2.12M / 4.23)
assign GPIO_0[PIN_MCLK] 	= SUB_CPU_CLK;	// SUBCPU CLK

assign GPIO_0[PIN_SCOR] = scor;
assign sqck 				= GPIO_0[PIN_SQCK];

assign GPIO_0[PIN_SUBQ] = (SOCT_SEL == 1'b0) ? sqso : SOCT_OUT;

reg [35:0] GPIO_01;

always @(posedge CPU_CLK) begin
	GPIO_01 <= GPIO_0;
end

assign GPIO_0[35:0] = {30'bz, /*clk_div[1]*/1'bz, 5'bz};

wire c2po;


assign GPIO_1[0] = GPIO_01[PIN_CLK]; // CLK
assign GPIO_1[1] = GPIO_01[PIN_DATA]; // DATA
assign GPIO_1[2] = GPIO_01[PIN_XLAT]; // LATCH
assign GPIO_1[3] = GPIO_01[PIN_SENS]; // SENS

/*
assign GPIO_1[0] = SD_DAT3; // CS
assign GPIO_1[1] = SD_CLK; // CLK
assign GPIO_1[2] = SD_CMD; // MOSI
assign GPIO_1[3] = SD_DAT; // MISO
*/
assign GPIO_1[4] = c2po;//time_check;//SD_CLK;//cd_clk;//GPIO_01[15]; // SCLK
assign GPIO_1[5] = GPIO_01[PIN_SQCK]; // SQCK 
assign GPIO_1[6] = GPIO_01[PIN_SUBQ]; // SUBQ 
assign GPIO_1[7] = scor;// GPIO_01[12]; // SQCR

assign GPIO_1[8] = cdr_dat_clk;
assign GPIO_1[9] = clk_44100;
assign GPIO_1[10] = cdr_data;



assign AUD_XCK = CLOCK_24[0];

wire I2C_CLK;
wire I2C_DAT;


assign I2C_SCLK = (I2C_CLK == 1) ? 1'bz : 1'b0;
assign I2C_SDAT = (I2C_DAT == 1) ? 1'bz : 1'b0;
wire TRK0, DSPB;

assign GPIO_0[PIN_LMTSW] = (TRK0 == 1) ? 1'b0 : 1'b1;


wire door;
assign LEDG[0] = door;
assign LEDR[0] = TRK0;
assign LEDR[1] = DSPB;
assign GPIO_0[PIN_DOOR] = ((door == 1) || (SW[1] == 1)) ? 1'b0 : 1'bz;
assign LEDG[1] = SW[1];

wire sens_out;

wire trc_toggle;
wire [31:0] track_count;

wire [15:0] trc_div;
CXD2545_TRACK_COUNTER trc_cnt_inst(
	.clk(CPU_CLK),
	.div(trc_div),
	.toggle_cnt(256),
	.toggle_clk(trc_toggle),
	
	.trigger(trc_cnt_en),
	.track_count(track_count)
);



wire [15:0] SENS_PIN;


reg REG_CLK1;
reg REG_DATA1;
reg REG_XLAT1;
reg REG_SQCK1;

reg REG_CLK2;
reg REG_DATA2;
reg REG_XLAT2;
reg REG_SQCK2;

reg REG_CLK;
reg REG_DATA;
reg REG_XLAT;
reg REG_SQCK;


always @(posedge CPU_CLK) begin
	REG_CLK1 <= GPIO_0[PIN_CLK];
	REG_DATA1 <= GPIO_0[PIN_DATA];
	REG_XLAT1 <= GPIO_0[PIN_XLAT];
	REG_SQCK1 <= GPIO_0[PIN_SQCK];

	
	REG_CLK2  <= REG_CLK1;
	REG_DATA2 <= REG_DATA1;
	REG_XLAT2 <= REG_XLAT1;
	REG_SQCK2 <= REG_SQCK1;
	
	REG_CLK  <= REG_CLK2;
	REG_DATA <= REG_DATA2;
	REG_XLAT <= REG_XLAT2;
	REG_SQCK <= REG_SQCK2;
	
end


CXD2545_SENS sens_inst(
	.sclk(CPU_CLK),
	.clk(REG_CLK),
	.data(REG_DATA),
	.xlat(REG_XLAT),
	.sens_data({SENS_PIN[15:14], c2po, trc_toggle, SENS_PIN[11], ~c2po, SENS_PIN[9:0]}),
	.sens(sens_out)
);


wire SOCT_OUT, SOCT_SEL;

CXD2545_SOCT soct_inst(
	.sclk(CPU_CLK),
	.xlat(GPIO_0[PIN_XLAT]),
	
	.PER(8'h01),
	.C1(3'b000),
	.C2(3'b000),
	
	.FOK(SENS_PIN[5]),
	.GFS(SENS_PIN[10]),
	.LOCK(SENS_PIN[10]),
	.EMPH(1'b0),
	
	.clk(sqck),
	.out(SOCT_OUT),
);

assign GPIO_0[PIN_SENS] = sens_out;

assign DRAM_CLK = CPU_CLK;

wire DRAIN_DMA;

wire time_check;

CXD2545_EMU CXD2545_TS(
     .sys_clk                       (CPU_CLK),
     .reset_n                       (SW[9]),
      
	  .cd_clk(CD_CLK),
	  
     .rxd_to_the_UART_DEBUG         (GPIO_1[29]),
     .txd_from_the_UART_DEBUG       (GPIO_1[31]),

	  .in_port_to_the_TRACK_COUNTER			   (track_count),
     .out_port_from_the_CXD2545_CTRL         ({SOCT_SEL, time_check, trc_cnt_en, door, DSPB, TRK0, I2C_CLK, DRAIN_DMA}),
	  .out_port_from_the_SENS_OUT			   	(SENS_PIN),
		
     .CLOK_to_the_CXD2545_CPU_0               (GPIO_0[PIN_CLK]),
     .DATA_to_the_CXD2545_CPU_0               (GPIO_0[PIN_DATA]),
     .XLAT_to_the_CXD2545_CPU_0               (GPIO_0[PIN_XLAT]),

	  .MISO_to_the_SDCARD_SPI                  (SD_DAT),
     .MOSI_from_the_SDCARD_SPI                (SD_CMD),
     .SCLK_from_the_SDCARD_SPI                (SD_CLK),
     .SS_n_from_the_SDCARD_SPI                (SD_DAT3),
		
	  .drain_to_the_CXD2545_DRAIN			   	 (DRAIN_DMA),
	  
	  .cd_c2po_from_the_CXD2545S			   	 (c2po),
     .cd_clk_from_the_CXD2545S                (cdr_dat_clk),
     .cd_lr_from_the_CXD2545S                 (clk_44100),
	  .cd_data_from_the_CXD2545S			   	 (cdr_data),
      
     .in_sqck_to_the_CXD2545S_SUBQ_OUT        (REG_SQCK),
	  .out_scor_from_the_CXD2545S_SUBQ_OUT     (scor),
     .out_sqso_from_the_CXD2545S_SUBQ_OUT     (sqso),
	
	  .out_port_from_the_CXD_TRACK_CTRL			 (trc_div)
	);

endmodule