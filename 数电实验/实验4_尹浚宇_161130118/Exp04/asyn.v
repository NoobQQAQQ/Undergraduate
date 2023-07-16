module asyn(clk, in_d, en, reset, out_q);
	input clk;
	input in_d;
	input en;
	input reset;
	output reg out_q;
	
	always @ (posedge clk or negedge reset)
	if (!reset)
	begin
		if (en)
			out_q <= 0;
		else
			out_q <= out_q;
	end
	else
		out_q <= in_d;
endmodule