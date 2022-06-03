module CXD2545_TRACK_COUNTER(
	input clk,
	input [15:0] div,
	input [15:0] toggle_cnt,
	input trigger,
	
	output reg toggle_clk,
	output reg [31:0] track_count
);

reg [32:0] cnt_50;
reg [32:0] cnt_div_50;
reg prev_trigger;
reg [30:0] trc_cnt;


reg trk_clk;

always @(posedge clk) begin
	if(cnt_50 < div) begin
		cnt_50 <= cnt_50 + 1'b1;
	end else begin
		cnt_50 <= 0;
		trk_clk <= ~trk_clk;
	
		if(cnt_div_50 < 256) begin
			cnt_div_50 <= cnt_div_50 + 1'b1;
		end else begin
			cnt_div_50 <= 0;
			toggle_clk <= ~toggle_clk;
		end
	
		if((prev_trigger == 1'b1) && (trigger == 1'b0)) begin
			track_count <= {1'b1, trc_cnt};
		end else if((prev_trigger == 1'b0) && (trigger == 1'b1)) begin
			trc_cnt <= 0;
			track_count <= 0;
		end else begin
			trc_cnt <= trc_cnt + 1'b1;
		end

	
	prev_trigger <= trigger;
	end
end



/*
always @(posedge trk_clk) begin
	if(cnt_div_50 < 256) begin
		cnt_div_50 <= cnt_div_50 + 1'b1;
	end else begin
		cnt_div_50 <= 0;
		toggle_clk <= ~toggle_clk;
	end
	
	if((prev_trigger == 1'b1) && (trigger == 1'b0)) begin
		track_count <= trc_cnt;
	end else if((prev_trigger == 1'b0) && (trigger == 1'b1)) begin
		trc_cnt <= 0;
	end else begin
		trc_cnt <= trc_cnt + 1'b1;
	end

	
	prev_trigger <= trigger;
end*/

endmodule