module state_machine(clk, data, ready, dout);
input clk, ready;
input [7:0] data;

output reg [7:0] dout;

reg state;
parameter SR = 1'b0, SP = 1'b1; // release and press

reg [7:0] old;

initial begin state = SR; end

always @(posedge clk)
begin
	if (ready)
		old <= data;
end

always @(posedge clk) // change output and state
begin
	case (state)
		SR: 
		begin
			if (ready)
			begin
				if (data != 8'hF0 && old != 8'hF0) // press a key
				begin
					dout <= data; state <= SP;
				end
				
				else
				begin
					state <= SR; dout <= 8'h00;
				end
			end
			
			else
			begin
				dout <= dout; state <= state;
			end
		end
			
		SP:
		begin
			if (ready)
			begin
				if (data != 8'hF0 && old != 8'hF0 && data == old) // keep pressing
				begin
					dout <= dout; state <= SP;
				end
				
				else
				begin
					dout <= 8'h00; state <= SR;
				end
			end
			else
			begin
				dout <= dout; state <= state;
			end
		end
			
	endcase
end

endmodule
