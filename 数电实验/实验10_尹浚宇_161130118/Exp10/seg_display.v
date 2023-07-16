module seg_display(pitch, HEX1, HEX0);

input [7:0] pitch;

output reg [6:0] HEX1, HEX0;

always @(pitch)
begin
	case(pitch)
		8'hC5: begin HEX1 = 7'b1000110; HEX0 = 7'b0010010; end
		8'hD5: begin HEX1 = 7'b0100001; HEX0 = 7'b0010010; end
		8'hE5: begin HEX1 = 7'b0000110; HEX0 = 7'b0010010; end
		8'hF5: begin HEX1 = 7'b0001110; HEX0 = 7'b0010010; end
		8'h05: begin HEX1 = 7'b0000000; HEX0 = 7'b0010010; end // G5
		8'hA5: begin HEX1 = 7'b0001000; HEX0 = 7'b0010010; end
		8'hB5: begin HEX1 = 7'b0000011; HEX0 = 7'b0010010; end
		8'hC6: begin HEX1 = 7'b1000110; HEX0 = 7'b0000010; end
		8'h00: begin HEX1 = 7'b1111111; HEX0 = 7'b1111111; end // not press or invalid key
		default: begin HEX1 = HEX1; HEX0 = HEX0; end
	endcase
end

endmodule