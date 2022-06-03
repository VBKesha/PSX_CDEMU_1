// CXD2545_CPU.v

// This file was auto-generated as a prototype implementation of a module
// created in component editor.  It ties off all outputs to ground and
// ignores all inputs.  It needs to be edited to make it do something
// useful.
// 
// This file will not be automatically regenerated.  You should check it in
// to your version control system if you want to keep it.

`timescale 1 ps / 1 ps
module CXD2545_CPU (
		input  wire        clk,         //            clock.clk
		input  wire        rst_n,       //                 .reset
		input  wire [3:0]  m_addr,      //     avalon_slave.address
		output wire [31:0] m_read_data, //                 .readdata
		input  wire [31:0] m_write_data,//                 .writedata
		input  wire        m_read,      //                 .read_n
		input  wire        m_write,     //                 .write_n
		output wire        m_irq,       // interrupt_sender.irq_n
	
		input  wire        CLOK,        //          cxd_cpu.export
		input  wire        DATA,        //                 .export
		input  wire        XLAT         //                 .export
	);

	reg [23:0] cxd_cmd;
	reg [23:0] cxd_cur_cmd;
	

	reg reg_cur_clok;
	reg reg_prev_clok;
	reg reg_data_cur;
	reg reg_cur_xlat;
	reg reg_prev_xlat;
	
	reg irq;
	
	assign m_read_data[23:0] = cxd_cmd;
	assign m_irq = irq;
	
	always @(posedge clk) begin
		if(rst_n == 1) begin
			irq <= 1'b1;
		end else begin
			reg_prev_clok <= reg_cur_clok;
			reg_cur_clok <= CLOK;
			
			reg_prev_xlat <= reg_cur_xlat;
			reg_cur_xlat <= XLAT;
			
			reg_data_cur <= DATA;
			
			if((reg_prev_clok == 1'b0) && (reg_cur_clok == 1'b1)) begin
				cxd_cur_cmd[23:0] <= {reg_data_cur, cxd_cur_cmd[23:1]};
			end
			
			if((reg_prev_xlat == 1'b1) && (reg_cur_xlat == 1'b0)) begin
				cxd_cmd <= cxd_cur_cmd;
				irq <= 1'b0;
			end
			
			if((m_read == 1'b0) && (m_addr == 4'h1)) begin
				irq <= 1'b1;
			end
			
		end
	
	end
	
endmodule
