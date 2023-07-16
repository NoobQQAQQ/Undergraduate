module my_register(clk, mode, in, out, single);
	input clk;
	input [2:0] mode;
	input [7:0] in;
	input single;
	
	output reg [7:0] out;
	
	always @ (negedge clk)
		case (mode)
			0: begin out <= 0; end
			1: begin out <= in; end
			2: begin out <= {1'b0, out[7:1]}; end
			3: begin out <= {out[6:0], 1'b0}; end
			4: begin out <= {out[7], out[7:1]}; end
			5: begin out <= {single, out[7:1]}; end
			6: begin out <= {out[0], out[7:1]}; end
			7: begin out <= {out[6:0], out[7]}; end
			default: out <= out;
		endcase
	
	
endmodule
	