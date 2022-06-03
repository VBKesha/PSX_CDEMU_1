module CXD2545_SOCT(
	input sclk,
	input xlat,
	
	input [7:0] PER,
	input [2:0] C1,
	input [2:0] C2,
	input FOK,
	input GFS,
	input LOCK,
	input EMPH,
	
	input clk,
	output reg out
);

reg [17:0] soct;
reg prev_xlat;
reg prev_clk;

/*
always @(posedge sclk or negedge xlat) begin
	if(xlat == 1'b0) begin
		soct <= {PER[0], PER[1], PER[2], PER[3], PER[4], PER[5], PER[6], PER[7], 
					 C1[2:0], C2[2:0], FOK, GFS, LOCK, EMPH};
	end else begin
		if((prev_clk == 1'b1) && (clk == 1'b0)) begin
//			out <= soct[16];
			soct <= {soct[16:0], 1'b0};
		end
	
		prev_clk <= clk;
		out <= soct[17];
	end
end
*/

always @(posedge sclk) begin
	if((prev_xlat == 1'b1) && (xlat == 1'b0)) begin
		soct <= {PER[0], PER[1], PER[2], PER[3], PER[4], PER[5], PER[6], PER[7], 
					 C1[2:0], C2[2:0], FOK, GFS, LOCK, EMPH};
	//end 
	end else begin
		if((prev_clk == 1'b1) && (clk == 1'b0)) begin
			soct <= {soct[16:0], 1'b0};
		end
	end
	
	out <= soct[17];	
	prev_clk <= clk;
	prev_xlat <= xlat;

end

endmodule
