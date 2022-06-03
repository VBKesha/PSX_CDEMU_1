// avalon_st_drainer.v

// This file was auto-generated as a prototype implementation of a module
// created in component editor.  It ties off all outputs to ground and
// ignores all inputs.  It needs to be edited to make it do something
// useful.
// 
// This file will not be automatically regenerated.  You should check it in
// to your version control system if you want to keep it.

`timescale 1 ps / 1 ps
module avalon_st_drainer (
		input  wire        clk,       //       clock.clk
		input  wire        reset,     //  reset_sink.reset_n
		
		output wire        out_valid, //         out.valid
		output wire        out_sof,   //            .startofpacket
		output wire        out_eof,   //            .endofpacket
		output wire [31:0] out_data,  //            .data
		output wire [1:0]  out_empty, //            .empty
		input  wire        out_ready, //            .ready
		
		input  wire        in_valid,  //          in.valid
		input  wire        in_sof,    //            .startofpacket
		input  wire        in_eof,    //            .endofpacket
		input  wire [31:0] in_data,   //            .data
		input  wire [1:0]  in_empty,  //            .empty
		output wire        in_ready,  //            .ready
		input  wire        drain      // conduit_end.export
);


assign out_eof		= (lock_drain == 1'b0) ? in_eof	  : 1'b0;
assign out_valid	= (lock_drain == 1'b0) ? in_valid  : 1'b0;
assign in_ready	= ((drain == 1'b0)  || (in_sof == 1'b1)) ? out_ready	: 1'b1;
assign out_sof		= in_sof;
assign out_empty 	= in_empty;
assign out_data	= in_data;


reg lock_drain;

always @(negedge clk) begin
	if(drain == 1'b1) begin 
		lock_drain <= 1'b1;
	end else begin
		if(in_sof == 1'b1) begin
			lock_drain <= 1'b0;
		end
	end
end
	
endmodule
