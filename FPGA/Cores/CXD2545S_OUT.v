// CXD2545S_OUT.v

// This file was auto-generated as a prototype implementation of a module
// created in component editor.  It ties off all outputs to ground and
// ignores all inputs.  It needs to be edited to make it do something
// useful.
// 
// This file will not be automatically regenerated.  You should check it in
// to your version control system if you want to keep it.

`timescale 1 ps / 1 ps

module CXD2545S_OUT (
		input  wire        clk,      //   clock.clk
		input  wire        rst,      //        .reset_n
		output wire        in_ready, //   	   .ready
		input  wire        in_valid, //        .valid
		input  wire [31:0] in_data,  //        .data
		input  wire        in_sof,   //        .startofpacket
		input  wire        in_eof,   //        .endofpacket
		input  wire [1:0]  in_empty, //        .empty
		

		
		output reg			cd_c2po,
		output wire			cd_clk,
		output reg			cd_lr,
		output reg			cd_data
	);

	assign in_ready = wait_data;	
	
	reg [47:0] cddata;
	reg [47:0] cddata_new;

	reg wait_data;
	
	
	reg [9:0] word_cnt;

	reg new_c2po;
	
	always @(negedge clk) begin
		if(rst == 0) begin
			wait_data <= 1'b1;
			word_cnt <= 10'h000;
			new_c2po <= 1'b0;
		end else begin
			
			if((wait_data == 1) && (in_valid == 1)) begin
				if(in_sof == 1'b1) begin
					word_cnt <= 10'h000;
				end else begin
					word_cnt <= word_cnt + 1'b1;
				end
					
				if((word_cnt < 587) || (in_sof == 1'b1)) begin
					cddata_new <= { in_data[31], in_data[31], in_data[31], in_data[31], in_data[31], in_data[31], in_data[31], in_data[31], in_data[31:16],
										 in_data[15], in_data[15], in_data[15], in_data[15], in_data[15], in_data[15], in_data[15], in_data[15], in_data[15:0]
									  };
					wait_data  <= 1'b0;
				end
			end

			if((cd_cnt == 0) && (clk_out == 1)) begin
				if(wait_data == 0) begin
					wait_data <= 1'b1;
					cddata 	 <= cddata_new;
					new_c2po  <= 1'b0;
				end else begin
					cddata 	<= 48'h000000000000;
					new_c2po <= 1'b1;
				end
			end

		end
	end

	reg [5:0] cd_cnt;
	reg clk_out;
	reg [47:0] cdout_data;
	reg c2po_delay;
	always @(negedge clk) begin
		clk_out <= ~clk_out;
		if(clk_out == 1'b1) begin
			cd_c2po <= c2po_delay;
			if(cd_cnt > 0) begin
				cd_cnt <= cd_cnt - 1'b1;
			end else begin
				cd_cnt <= 47;
				cdout_data <= cddata;
				c2po_delay <= new_c2po;
			end
			cd_lr   <= (cd_cnt < 24) ? 1'b0 : 1'b1;
			cd_data <= cdout_data[cd_cnt];
		end;
	end

	assign cd_clk = clk_out;
	
endmodule

