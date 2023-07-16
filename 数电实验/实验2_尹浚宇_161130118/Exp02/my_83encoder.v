module my_83encoder(x, en, valid, seg, y);
	input [7:0] x;
	input en;
	output reg valid;
	output reg [6:0] seg;
	output reg [2:0] y;
	
	always @(x or en)
		if (en)
		begin
			valid = 1;
			casex(x)
				8'b1??????? : begin y = 3'b111; seg = 7'b1111000; end
				8'b01?????? : begin y = 3'b110; seg = 7'b0000010; end
				8'b001????? : begin y = 3'b101; seg = 7'b0010010; end
				8'b0001???? : begin y = 3'b100; seg = 7'b0011001; end
				8'b00001??? : begin y = 3'b011; seg = 7'b0110000; end
				8'b000001?? : begin y = 3'b010; seg = 7'b0100100; end
				8'b0000001? : begin y = 3'b001; seg = 7'b1111001; end
				8'b00000001 : begin y = 3'b000; seg = 7'b1000000; end
				8'b00000000 : begin y = 3'bzzz; seg = 7'b0000000; valid = 0; end
				default : begin y = 3'bzzz; seg = 7'b0000000; valid = 0; end
			endcase
		end
		
		else begin y = 3'bzzz; seg = 7'b1111111; valid = 0; end
		
endmodule	