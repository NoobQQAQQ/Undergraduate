module get_data(keycode, freq, pitch);

input [7:0] keycode;

output reg [15:0] freq;
output reg [7:0] pitch;

always @(keycode)
begin
	case(keycode)
	
	// C5
		8'h16: begin freq = 16'd714; pitch = 8'hC5; end
		// D5
		8'h1E: begin freq = 16'd802; pitch = 8'hD5; end
		// E5
		8'h26: begin freq = 16'd900; pitch = 8'hE5; end
		// F5
		8'h25: begin freq = 16'd954; pitch = 8'hF5; end
		// G5
		8'h2E: begin freq = 16'd1070; pitch = 8'h05; end
		// A5
		8'h36: begin freq = 16'd1201; pitch = 8'hA5; end
		// B5
		8'h3D: begin freq = 16'd1349; pitch = 8'hB5; end
		// C6
		8'h3E: begin freq = 16'd1429; pitch = 8'hC6; end
	/*
		// C5
		8'h16: begin freq = 16'h02CA; pitch = 8'hC5; end
		// D5
		8'h1E: begin freq = 16'h0227; pitch = 8'hD5; end
		// E5
		8'h26: begin freq = 16'h028A; pitch = 8'hE5; end
		// F5
		8'h25: begin freq = 16'h02BF; pitch = 8'hF5; end
		// G5
		8'h2E: begin freq = 16'h042E; pitch = 8'h05; end
		// A5
		8'h36: begin freq = 16'h04B1; pitch = 8'hA5; end
		// B5
		8'h3D: begin freq = 16'h0544; pitch = 8'hB5; end
		// C6
		8'h3E: begin freq = 16'h05A8; pitch = 8'hC6; end
	*/
		default: begin freq = 16'h0000; pitch = 8'h00; end
	endcase
end

endmodule