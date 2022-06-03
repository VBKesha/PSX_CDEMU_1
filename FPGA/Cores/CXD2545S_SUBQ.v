// CXD2545S_SUBQ.v

// This file was auto-generated as a prototype implementation of a module
// created in component editor.  It ties off all outputs to ground and
// ignores all inputs.  It needs to be edited to make it do something
// useful.
// 
// This file will not be automatically regenerated.  You should check it in
// to your version control system if you want to keep it.

`timescale 1 ps / 1 ps
module CXD2545S_SUBQ (
	input  wire        clk,       //     Clock.clk
	input  wire        reset,     //     Reset.reset

	input  wire        in_eof,    // SUBQ_Sink.endofpacket
	input  wire        in_sof,    //          .startofpacket
	output wire        out_ready, //          .ready
	input  wire        in_valid,  //          .valid
	input  wire [31:0] in_data,   //          .data
	input  wire [1:0]  in_empty,  //          .empty

	input  wire        in_sqck,   //  SUBQ_OUT.export
	output reg         out_sqso,  //          .export
	output reg         out_scor,  //          .export		
	output wire        out_emph   //          .export		
);

assign out_ready = 1'b1;

reg [95:0] sub_q;
reg [31:0] sub_q_2;
reg [31:0] sub_q_1;

reg sqck_cur;
reg sqck_prev;
reg [3:0] scor_latch;

assign out_emph = 1'b0;

always @(negedge clk) begin

	if(in_valid == 1'b1) begin
		sub_q_1 <= in_data;
		sub_q_2 <= sub_q_1;
		
		if(in_eof == 1'b1) begin
			sub_q <= {sub_q_2, sub_q_1, in_data};
			out_sqso <= 1'b1;
		end
	end
	
	if((in_valid == 1'b1) && (in_eof == 1'b1)) begin
			scor_latch <= 4'b1111;
			out_scor <= 1'b1;
	end else begin

		if(scor_latch > 0) begin
			scor_latch <= scor_latch - 1'b1;
		end else begin
			out_scor <= 1'b0;
		end
	
	end

	if((sqck_cur == 1'b0) && (sqck_prev == 1'b1)) begin
		out_sqso <= sub_q[95];
		sub_q <= {sub_q[94:0], 1'b0};
	end

	sqck_cur  <= in_sqck; 
	sqck_prev <= sqck_cur;
end

endmodule
