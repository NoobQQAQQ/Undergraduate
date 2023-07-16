module my_timer(clk, en, s_p, clear, reset, seg1, seg2, over_flag, sp_flag);
	input clk, en, s_p, clear, reset;
	output reg [6:0] seg1, seg2;
	output reg over_flag;
	output reg sp_flag;
	
	reg [6:0] result;
	reg clk_1s;
	reg [31:0] count_clk;
	reg over;

	initial
	begin
		seg1 = 0; seg2 = 0; sp_flag = 0; clk_1s = 0; count_clk = 0; result = 0;
	end
	
	always @ (posedge clk) // here is a frequency divider
	begin
		if (count_clk == 25000000)
		begin
			count_clk <= 0;
			clk_1s <= ~clk_1s;
		end
		else
			count_clk <= count_clk + 1;
	end
	
	always @ (posedge clk_1s) 
	begin
		if (!clear)
		begin
			if (en)
				result <= 0;
			else
				result <= result;
		end
		
		else if (!s_p)
		begin
			if (en)
				sp_flag <= ~sp_flag;
			else
				result <= result;
		end
		
		else if (!reset)
		begin
			if (en)
			begin
				over <= 0;
				result <= 0;
				over_flag <= 0;
				sp_flag <= 0;
			end
			else
				result <= result;
		end
		
		else
		begin
			if (en)
			begin
				if (sp_flag && !over)
					result <= result + 1;
					
				else if (!sp_flag || over)
					result <= result;
					
				if (over) // make LED flash
					over_flag <= ~over_flag;
				
				if (result == 99)
				begin
					over <= 1;
					over_flag <= 1;
					result <= 0;
				end
			end
			else
				result <= result;
		end
	end
	
	always @ (result) // change the 7 segments 
	begin
		case (result / 10)
			0: seg1 = 7'b1000000;
			1: seg1 = 7'b1111001;
			2: seg1 = 7'b0100100;
			3: seg1 = 7'b0110000;
			4: seg1 = 7'b0011001;
			5: seg1 = 7'b0010010;
			6: seg1 = 7'b0000010;
			7: seg1 = 7'b1111000;
			8: seg1 = 7'b0000000;
			9: seg1 = 7'b0010000;
			default : seg1 = seg1;
		endcase
		
		case (result % 10)
			0: seg2 = 7'b1000000;
			1: seg2 = 7'b1111001;
			2: seg2 = 7'b0100100;
			3: seg2 = 7'b0110000;
			4: seg2 = 7'b0011001;
			5: seg2 = 7'b0010010;
			6: seg2 = 7'b0000010;
			7: seg2 = 7'b1111000;
			8: seg2 = 7'b0000000;
			9: seg2 = 7'b0010000;
			default : seg2 = seg2;
		endcase
	end

endmodule