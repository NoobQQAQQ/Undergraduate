module addr_translate(h_addr, v_addr, vgamem_addr, y_16, x_9);

input [9:0] h_addr, v_addr;

output [11:0] vgamem_addr; // addr in vgamem [0, 30 x 70 - 1]
output reg [4:0] y_16;//, x_9;

reg [6:0] y_30;//, x_70;

always @(v_addr)
begin
	y_30 = v_addr >> 4;
	y_16 = v_addr[3:0];
end

//always @(h_addr)
//begin
	//x_70 = (h_addr >> 4) + (h_addr >> 5) + (h_addr >> 6);
	//x_70 = h_addr/9;
	//x_9 = h_addr - (x_70 << 3) - x_70;
	//x_70 = h_addr%9;
//end


wire [6:0] x_70;
output wire [4:0] x_9;
h_addr_trans get_x(h_addr, x_70, x_9);

assign vgamem_addr = (y_30 << 6) + (y_30 << 2) + (y_30 << 1) + x_70;

endmodule