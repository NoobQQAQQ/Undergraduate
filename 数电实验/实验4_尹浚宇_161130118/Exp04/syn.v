module syn(clk, in_d, en, reset, out_q);
	input clk;
	input in_d;
	input en;
	input reset;
	output reg out_q;
	
	always @ (posedge clk)
	if (en)
	begin
		if (reset)
			out_q <= 0;
		else
			out_q <= in_d;
	end
	else
		out_q <= out_q;
endmodule