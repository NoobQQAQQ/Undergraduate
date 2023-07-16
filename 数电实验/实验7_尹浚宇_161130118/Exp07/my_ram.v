module my_ram(clk, we, addr1, addr2, din, seg1, seg2, seg3, seg4);
	input clk;
	input we;
	input [3:0] addr1;
	input [3:0] addr2;
	
	input [1:0] din;
	output reg[6:0] seg1;
	output reg [6:0] seg2;
	output reg [6:0] seg3;
	output reg [6:0] seg4;
	
	wire [7:0] dout1;
	wire [7:0] dout2;

	my_ram1 ram1(clk, we, addr1, addr1, din, dout1); // 0 read 1 write	
	ram2port ram2(clk, {6'b111100, din}, addr2, addr2, we, dout2); // 0 read 1 write
	
always @ (dout1) // change the 7 segments 
	begin
		case (dout1[7:4])
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
		
		case (dout1[3:0])
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

	always @ (dout2) // change the 7 segments 
	begin
		case (dout2[7:4])
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
		
		case (dout2[3:0])
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

endmodule