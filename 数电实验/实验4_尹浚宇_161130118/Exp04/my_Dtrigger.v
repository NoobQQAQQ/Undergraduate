module my_Dtrigger(clk, in_d, en, reset, out_q);
	input clk;
	input [1:0] in_d;
	input [1:0] en;
	input reset;
	output [1:0] out_q;
	
	syn my_syn(.clk(clk), .in_d(in_d[0]), .en(en[0]), .reset(reset), .out_q(out_q[0]));
	asyn my_asyn(.clk(clk), .in_d(in_d[1]), .en(en[1]), .reset(reset), .out_q(out_q[1]));
	
endmodule