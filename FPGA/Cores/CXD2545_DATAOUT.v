// CXD2545_DATAOUT.v

// This file was auto-generated as a prototype implementation of a module
// created in component editor.  It ties off all outputs to ground and
// ignores all inputs.  It needs to be edited to make it do something
// useful.
// 
// This file will not be automatically regenerated.  You should check it in
// to your version control system if you want to keep it.

`timescale 1 ps / 1 ps
module CXD2545_DATAOUT (
		input  wire        clk,        //   clock_sink.clk
		input  wire        rst_n,      //             .reset_n
		input  wire [3:0]  addr,       // avalon_slave.address
		output wire [31:0] read_data,  //             .readdata
		input  wire [31:0] write_data, //             .writedata
		input  wire        read_n,     //             .read_n
		input  wire        write_n,    //             .write_n
		output wire        emph,       //     CXD_DATA.export
		output wire        c2po,       //             .export
		input  wire        lrck,       //             .export
		input  wire        cdclk,      //             .export
		input  wire        cdlbit,     //             .export
		output wire        cddat,      //             .export
		output wire        scor,       //     CXD_SUBQ.export
		input  wire        sqck,       //             .export
		output wire        sqso,       //             .export
		output wire        irq_n       //          irq.irq_n
	);

	// TODO: Auto-generated HDL template

	parameter DIV_REG = 4'h0;
	parameter FIFO_REG = 4'h1;
	parameter FIFO_READ = 4'h2;
	parameter FIFO_RESET = 4'h3;
	
	reg [31:0] out;
	assign read_data = out;
	reg cddat_reg;
	//assign cdclk = 1'b0;
	//assign lrck = 1'b0;
	assign cddat = cddat_reg;
	
	assign c2po = 1'b0;
	assign emph = 1'b0;

	assign sqso = sqso_reg;
	assign scor = scor_reg;

	reg [31:0] clk_divisor;
	reg [31:0] clk_counter;
	reg [6:0] lrclk_div;
	
	reg [31:0] cd_data;
	
	reg clk_bit;
	reg clk_lr;
	
	/*assign cdclk = clk_bit;
	assign lrck = clk_lr;*/

	reg wrreq_sig, rdreq_sig;
	wire empty_sig, full_sig;
	wire [6:0] wrusedw_sig;
	wire [31:0] q_sig;
	wire [6:0] usedw_sig;
	reg fifo_reset;
	
	reg err_seq, err_out;
	
	assign irq_n = ~err_out;
	
	fifo	fifo_inst (
	.clock ( clk ),
	.data ( write_data ),
	.rdreq ( rdreq_sig ),
	.sclr ( fifo_reset ),
	.wrreq ( wrreq_sig ),
	.empty ( empty_sig ),
	.full ( full_sig ),
	//.usedw_sig(usedw_sig),
	.q ( q_sig )
	);
	
	always @(negedge clk) begin
		wrreq_sig <= 1'b0;
		fifo_reset <= 1'b0;
		if(err_seq == 1'b1) begin
			err_out <= 1'b1;
		end
		
		if(write_n == 1'b0) begin
			case(addr)
				DIV_REG: clk_divisor <= write_data;
				FIFO_REG: begin
					wrreq_sig <= 1'b1;
				end
				2: begin
					//cd_data <= write_data;
				end
				FIFO_RESET: begin
					fifo_reset <= 1'b1;
				end
			endcase
		end
		
		if(read_n == 1'b0) begin
			case(addr)
				5: begin
						err_out <= 0;
				end
				
				FIFO_REG: begin
					out <= {29'h0000000, err_out, empty_sig, full_sig};
				end

				3: begin
					out <= cddata[31:0];
				end

				4: begin
					out <= cddata[47:41];
				end
				
				
				
				default: begin
					out <= {32'hdeadbeef};
				end
			endcase
		end
	end
	
	
	reg [47:0] cddata;
	reg [31:0] ff_read;
	reg [7:0] state;
	reg [97:0] q_subchan;
	reg [97:0] q_subchan_buff;
	reg [6:0] q_cnt_read;
	reg [6:0] q_cnt;
	reg [4:0] byte_cnt;
	
	reg prev_cdlbit, cur_cdlbit;
	reg prev_cdclk, cur_cdclk;
	reg [5:0] cddatbit_cnt;
	
	reg cddata_ready;
	reg scor_reg;
	reg sqso_reg;
	reg sub_q_prep;
	reg sqck_reg, sqck_reg_prev;
	reg fifo_ready;
	
	always @(negedge clk) begin
		//scor_reg <= (q_cnt == 7'h61) ? 1 : 0;
		err_seq <= 1'h0;
		
		if((full_sig == 1'b1) && (fifo_reset == 1'b0)) begin
			fifo_ready = 1'b1;
		end
		
		if(fifo_reset == 1'b0) begin
			if((cddata_ready == 1'b0) && (fifo_ready == 1'b1))begin
				rdreq_sig <= 1'b1;		
			end
			
			if(rdreq_sig == 1'b1) begin
				if(byte_cnt == 0) begin
					q_subchan[q_cnt] <= q_sig[6];
					err_seq <= ((q_sig & 32'hFFFFFFBF) != 0);

					if(q_cnt > 0) begin
						q_cnt <= q_cnt - 1'b1;
					end else begin
						q_cnt <= 7'h61;
						scor_reg <= 1'b1;
						
						if(sub_q_prep == 1'b1) begin
							sqso_reg <= 1'b1;
							q_subchan_buff <= q_subchan;
							q_cnt_read <= 8'h60;
						end else begin
							sqso_reg <= 1'b0;
						end
					end
						
				end else begin
					cd_data <= q_sig;
					cddata_ready <= 1'b1;
				end

				if(byte_cnt < 6) begin
					byte_cnt <= byte_cnt + 1'b1;
				end else begin
					byte_cnt <= 5'h00;
					sub_q_prep <= 1'b1;
				end
				rdreq_sig <= 1'b0;				
			end


			cur_cdlbit <= cdlbit;
			prev_cdlbit <= cur_cdlbit;
			if((prev_cdlbit == 1'b0) && (cur_cdlbit == 1'b1)) begin
				if(cddata_ready == 1'b1) begin
					cddata <= {	cd_data[31], cd_data[31], cd_data[31], cd_data[31], cd_data[31], cd_data[31], cd_data[31], cd_data[31], cd_data[31:16],
									cd_data[15], cd_data[15], cd_data[15], cd_data[15], cd_data[15], cd_data[15], cd_data[15], cd_data[15], cd_data[15:0]
								 };
					cddata_ready <= 1'b0;
				end else begin
					fifo_ready <= 1'b0;
					q_cnt <= 8'h61;
					q_cnt_read <= 8'h60;
					byte_cnt <= 1'b0;
					sub_q_prep <= 1'b0;
					//cddata <= 48'h001BAD00B001;
					cddata <= 48'h000000000000;
				end;
				cddatbit_cnt <= 47;
			end


			
			cur_cdclk <= cdclk;
			prev_cdclk <= cur_cdclk;
			
			if((prev_cdclk == 1'b1) && (cur_cdclk == 1'b0) && (scor_reg == 1'b1)) begin
				scor_reg <= 1'b0;
			end
			
			if((prev_cdclk == 1'b1) && (cur_cdclk == 1'b0)) begin
				
				cddat_reg <= cddata[cddatbit_cnt];
				if(cddatbit_cnt > 0) begin
					cddatbit_cnt <= cddatbit_cnt - 1'b1;
				end
			end
			
			sqck_reg <= sqck;
			sqck_reg_prev <= sqck_reg;
			if((sqck_reg_prev == 1'b1) && (sqck_reg == 1'b0)) begin
				if(q_cnt_read > 0) begin
					sqso_reg <= q_subchan_buff[q_cnt_read - 1];
					q_cnt_read <= q_cnt_read - 1'b1;
				end else begin
					sqso_reg <= 1'b0;
					sub_q_prep <= 1'b0;
				end
			end
		
		end else begin
			q_cnt <= 8'h61;
			q_cnt_read <= 8'h60;
			byte_cnt <= 1'b0;
			sub_q_prep <= 1'b0;
			cddata <= 48'h001BAD00B001;
			sqso_reg <= 1'b0;
			q_subchan_buff <= 0;
			cddata_ready <= 1'b0;
			fifo_ready <= 1'b0;
		end
	
	end
		
	
endmodule
