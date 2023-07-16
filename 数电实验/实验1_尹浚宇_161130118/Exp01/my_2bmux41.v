module my_2bmux41(x, y, f);
	input [7:0] x;
	input [1:0] y;
	output reg [1:0] f;
	
	always @ (y or x)
		case (y)
			0: begin f[0] = x[0]; f[1] = x[1]; end
			1: begin f[0] = x[2]; f[1] = x[3]; end
			2: begin f[0] = x[4]; f[1] = x[5]; end
			3: begin f[0] = x[6]; f[1] = x[7]; end
			default: f = 2'b00;
		endcase
		
endmodule