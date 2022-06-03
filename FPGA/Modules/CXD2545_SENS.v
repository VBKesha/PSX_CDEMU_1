module CXD2545_SENS(
		input sclk,
		
		input clk,
		input data,
		input xlat,
		
		input [15:0] sens_data,
		
		output reg sens
);

reg [3:0] cnt;
reg [7:0] shift_reg;
reg [3:0] select_reg;

reg prev_clk;
reg prev_xlat;
/*
always @(posedge sclk or negedge xlat) begin

	if(xlat == 1'b0) begin
		cnt <= 0;
		shift_reg[7:0] <= 0;
	end else begin
		if((prev_clk == 1'b0) && (clk == 1'b1)) begin
			shift_reg[7:0] <= {data, shift_reg[7:1]};
			if(cnt < 7) begin
				cnt <= cnt + 1'b1;
			end else begin
				select_reg <= {data, shift_reg[7:5]};
				cnt <= 0;
			end
		end
		
		prev_clk <= clk;
		sens <= sens_data[select_reg];
	end

end
*/
/*
always @(posedge clk or negedge xlat) begin
	if(xlat == 1'b0) begin
		cnt <= 0;
		shift_reg[7:0] <= 0;
	end else begin
		shift_reg[7:0] <= {data, shift_reg[7:1]};
		if(cnt < 7) begin
				cnt <= cnt + 1'b1;
		end else begin
				select_reg <= {data, shift_reg[7:5]};
				cnt <= 0;
		end
	end
end

assign sens = sens_data[select_reg];
*/

always @(posedge sclk) begin

	if((prev_xlat == 1'b1) && (xlat == 1'b0)) begin
		cnt <= 0;
		shift_reg[7:0] <= 0;
	end else begin
		if((prev_clk == 1'b0) && (clk == 1'b1)) begin
			shift_reg[7:0] <= {data, shift_reg[7:1]};
			if(cnt < 7) begin
				cnt <= cnt + 1'b1;
			end else begin
				select_reg <= {data, shift_reg[7:5]};
				cnt <= 0;
			end
		end
		
	end

	prev_xlat <= xlat;
	prev_clk <= clk;
	sens <= sens_data[select_reg];

end

endmodule