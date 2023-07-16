module seg_display(counter, ascii_code, key_code, pressing, seg0, seg1, seg2, seg3, seg4, seg5);
	input [7:0] counter;
	input [7:0] ascii_code;
	input [7:0] key_code;
	input pressing;
	
	output reg [6:0] seg0;
	output reg [6:0] seg1;
	output reg [6:0] seg2;
	output reg [6:0] seg3;
	output reg [6:0] seg4;
	output reg [6:0] seg5;
	
	always @(counter)
	begin
		case (counter[7:4])
			0: seg5 = 7'b1000000;
			1: seg5 = 7'b1111001;
			2: seg5 = 7'b0100100;
			3: seg5 = 7'b0110000;
			4: seg5 = 7'b0011001;
			5: seg5 = 7'b0010010;
			6: seg5 = 7'b0000010;
			7: seg5 = 7'b1111000;
			8: seg5 = 7'b0000000;
			9: seg5 = 7'b0010000;
			10: seg5 = 7'b0001000;
			11: seg5 = 7'b0000011;
			12: seg5 = 7'b1000110;
			13: seg5 = 7'b0100001;
			14: seg5 = 7'b0000110;
			15: seg5 = 7'b0001110;
			default : seg5 = seg5;
		endcase
		
		case (counter[3:0])
			0: seg4 = 7'b1000000;
			1: seg4 = 7'b1111001;
			2: seg4 = 7'b0100100;
			3: seg4 = 7'b0110000;
			4: seg4 = 7'b0011001;
			5: seg4 = 7'b0010010;
			6: seg4 = 7'b0000010;
			7: seg4 = 7'b1111000;
			8: seg4 = 7'b0000000;
			9: seg4 = 7'b0010000;
			10: seg4 = 7'b0001000;
			11: seg4 = 7'b0000011;
			12: seg4 = 7'b1000110;
			13: seg4 = 7'b0100001;
			14: seg4 = 7'b0000110;
			15: seg4 = 7'b0001110;
			default : seg4 = seg4;
		endcase
	end
	
	

	always @(ascii_code)
	begin
		if (!pressing)
		begin
			seg3 = 7'b1111111;
			seg2 = 7'b1111111;
		end
		
		else
		begin
			/*if (caps)
			begin
			case (ascii_code[7:4] - 4'h2)
				0: seg3 = 7'b1000000;
				1: seg3 = 7'b1111001;
				2: seg3 = 7'b0100100;
				3: seg3 = 7'b0110000;
				4: seg3 = 7'b0011001;
				5: seg3 = 7'b0010010;
				6: seg3 = 7'b0000010;
				7: seg3 = 7'b1111000;
				8: seg3 = 7'b0000000;
				9: seg3 = 7'b0010000;
				10: seg3 = 7'b0001000;
				11: seg3 = 7'b0000011;
				12: seg3 = 7'b1000110;
				13: seg3 = 7'b0100001;
				14: seg3 = 7'b0000110;
				15: seg3 = 7'b0001110;
				default : seg3 = seg3;
			endcase
		
			case (ascii_code[3:0])
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
				10: seg2 = 7'b0001000;
				11: seg2 = 7'b0000011;
				12: seg2 = 7'b1000110;
				13: seg2 = 7'b0100001;
				14: seg2 = 7'b0000110;
				15: seg2 = 7'b0001110;
				default : seg2 = seg2;
			endcase
			end*/
			
			//else
			//begin
				case (ascii_code[7:4])
				0: seg3 = 7'b1000000;
				1: seg3 = 7'b1111001;
				2: seg3 = 7'b0100100;
				3: seg3 = 7'b0110000;
				4: seg3 = 7'b0011001;
				5: seg3 = 7'b0010010;
				6: seg3 = 7'b0000010;
				7: seg3 = 7'b1111000;
				8: seg3 = 7'b0000000;
				9: seg3 = 7'b0010000;
				10: seg3 = 7'b0001000;
				11: seg3 = 7'b0000011;
				12: seg3 = 7'b1000110;
				13: seg3 = 7'b0100001;
				14: seg3 = 7'b0000110;
				15: seg3 = 7'b0001110;
				default : seg3 = seg3;
			endcase
		
			case (ascii_code[3:0])
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
				10: seg2 = 7'b0001000;
				11: seg2 = 7'b0000011;
				12: seg2 = 7'b1000110;
				13: seg2 = 7'b0100001;
				14: seg2 = 7'b0000110;
				15: seg2 = 7'b0001110;
				default : seg2 = seg2;
			endcase
			
		end
	end
	
	
	always @(key_code)
	begin
		if (!pressing)
		begin
			seg1 = 7'b1111111;
			seg0 = 7'b1111111;
		end
		
		else
		begin
		case (key_code[7:4])
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
			10: seg1 = 7'b0001000;
			11: seg1 = 7'b0000011;
			12: seg1 = 7'b1000110;
			13: seg1 = 7'b0100001;
			14: seg1 = 7'b0000110;
			15: seg1 = 7'b0001110;
			default : seg1 = seg1;
		endcase
		
		case (key_code[3:0])
			0: seg0 = 7'b1000000;
			1: seg0 = 7'b1111001;
			2: seg0 = 7'b0100100;
			3: seg0 = 7'b0110000;
			4: seg0 = 7'b0011001;
			5: seg0 = 7'b0010010;
			6: seg0 = 7'b0000010;
			7: seg0 = 7'b1111000;
			8: seg0 = 7'b0000000;
			9: seg0 = 7'b0010000;
			10: seg0 = 7'b0001000;
			11: seg0 = 7'b0000011;
			12: seg0 = 7'b1000110;
			13: seg0 = 7'b0100001;
			14: seg0 = 7'b0000110;
			15: seg0 = 7'b0001110;
			default : seg0 = seg0;
		endcase
		end
	end
endmodule