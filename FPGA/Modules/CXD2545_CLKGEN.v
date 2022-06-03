module CXD2545_CLKGEN(
		input clk,
		output reg cdr_dat,
		output reg cdr_lr,
		output reg cdr_lbit
);

reg [7:0] cnt;

always @(negedge clk) begin
	
	if(cnt < 95) begin
		cnt <= cnt + 1'b1;
	end else begin
		cnt <= 0;
	end
	
	cdr_lr <= (cnt < 48) ? 1'b1 : 1'b0;
	cdr_lbit <= (cnt == 95);
	cdr_dat  <= cnt[0];	
end

endmodule