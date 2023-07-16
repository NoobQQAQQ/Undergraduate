module my_fsmKeyboard(clk, clrn, ps2_clk, ps2_data, nextdata_n, seg0, seg1, seg2, seg3, seg4, seg5);
	input clk, clrn, ps2_clk, ps2_data, nextdata_n;
	
	output [6:0] seg0, seg1, seg2, seg3, seg4, seg5;
	
	wire [7:0] data;
	wire ready;
	wire overflow;
	
	ps2_keyboard get_keycode(clk, clrn, ps2_clk, ps2_data, data, ready, nextdata_n, overflow);
	
	reg [7:0] counter;
	wire [7:0] ascii_code;
	
	get_ascii key2ascii(dout, ascii_code);
	
	reg [7:0] old;
	reg pressing;
	
	seg_display display(counter, ascii_code, dout, pressing, seg0, seg1, seg2, seg3, seg4, seg5);
	
	reg state;
	parameter SR = 1'b0, SP = 1'b1; // release and press
	
	initial begin counter = 8'b00000000; state = SR; end
	
	reg [7:0] dout;
	
	
	always @(data) // skip break code
	begin
		if (data != 8'hF0 && old != 8'hF0 && ready)
			dout = data;
		else
			dout = dout;
	end
	
	always @(posedge clk)
	begin
		if (ready)
			old <= data;
	end
	
	always @(posedge clk) // change output
	begin
		case (state)
			SR: 
			begin
				if (ready)
				begin
					if (data != 8'hF0 && old != 8'hF0)
					begin
						pressing <= 1; counter <= counter + 8'b1;
					end
					else
						pressing <= 0;
				end
				else
					pressing <= 0;
			end
			
			SP:
			begin
				if (ready)
				begin
					if (data != 8'hF0 && old != 8'hF0 && data == old) // keep pressing
						pressing <= 1;
					else
						pressing <= 0;
				end
				else
					pressing <= 1;
			end
			
		endcase
	end
	
	always @(state or data) // change state
	begin
		case (state)
			SR:
			begin
				if (pressing)
					state = SP;
				else
					state = SR;
			end
			SP:
			begin
				if (pressing)
					state = SP;
				else
					state = SR;
			end
		endcase
	end
	
endmodule						
						
					