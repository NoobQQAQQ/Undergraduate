module my_rand(clk, reset, load, seed, seg1, seg2);
	input clk;
	input reset;
	input load;
	input [7:0] seed;
	
	output reg[6:0] seg1;
	output reg [6:0] seg2;
	
	reg [7:0] result;

	always @ (negedge clk or negedge reset)
	begin
		if (!reset)
			result <= 8'b00000000;
		else if (load)
			result <= seed;
		else
		begin
			if (result == 0)
				result <= 8'b10101010;
			else
			begin
				result[7] <= result[0];
				result[6] <= result[7];
				result[5] <= result[6];
				result[4] <= result[5];
				result[3] <= result[4] ^ result[0];
				result[2] <= result[3] ^ result[0];
				result[1] <= result[2] ^ result[0];
				result[0] <= result[1];
			end
		end
	end
	
	always @ (result) // change the 7 segments 
	begin
		case (result[7:4])
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
			10: seg1 <= 7'b0001000;
			11: seg1 <= 7'b0000011;
			12: seg1 <= 7'b1000110;
			13: seg1 <= 7'b0100001;
			14: seg1 <= 7'b0000110;
			15: seg1 <= 7'b0001110;
			default : seg1 = seg1;
		endcase
		
		case (result[3:0])
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
			10: seg2 <= 7'b0001000;
			11: seg2 <= 7'b0000011;
			12: seg2 <= 7'b1000110;
			13: seg2 <= 7'b0100001;
			14: seg2 <= 7'b0000110;
			15: seg2 <= 7'b0001110;
			default : seg2 = seg2;
		endcase
	end

endmodule 