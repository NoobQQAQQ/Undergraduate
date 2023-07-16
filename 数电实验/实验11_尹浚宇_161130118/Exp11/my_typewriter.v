module my_typewriter(clk, clrn, ps2_clk, ps2_data, 
hsync, vsync, valid, vga_r, vga_g, vga_b, sync_n, pclk, seg2, seg3);

input clk, clrn, ps2_clk, ps2_data;
wire [7:0] ascii_write;
wire pressing;
keyboard_top my_keyboard(clk, clrn, ps2_clk, ps2_data, ascii_write, pressing);


output pclk, sync_n;
assign sync_n = 0;
clkgen #(25000000) my_clkgen(clk, 1'b0, 1'b1, pclk);


output hsync, vsync, valid;
output [7:0] vga_r, vga_g, vga_b;
wire [23:0] vga_data;
wire [9:0] h_addr, v_addr;
vga_ctrl my_vga_ctrl(pclk, 1'b0, vga_data, h_addr, v_addr, hsync, vsync, valid, vga_r, vga_g, vga_b);


wire [11:0] vgamem_addr; // addr in vgamem [0, 30 x 70 - 1]
wire [4:0] y_16, x_9;
addr_translate get_addr(h_addr, v_addr, vgamem_addr, y_16, x_9);


wire [7:0] ascii_read;
vga_mem my_vga_mem(clk, vgamem_addr, pressing, ascii_write, ascii_read);


get_vga_data get_data(clk, ascii_read, y_16, x_9, vga_data);


output reg [6:0] seg2, seg3;

always @(ascii_write) // display asciicode
	begin
		if (!pressing)
		begin
			seg3 = 7'b1111111;
			seg2 = 7'b1111111;
		end
		
		else
		begin
				case (ascii_write[7:4])
				0: seg3 = 7'b1000000;
				1: seg3 = 7'b1111001;
				2: seg3 = 7'b0100100;
				3: seg3 = 7'b0110000;
				4: seg3 = 7'b0011001;
				5: seg3 = 7'b0010010;
				6: seg3 = 7'b0000010;
				7: seg3 = 7'b1111000;
				8: seg3 = 7'b0000000;
				9: seg3 = 7'b0010000;
				10: seg3 = 7'b0001000;
				11: seg3 = 7'b0000011;
				12: seg3 = 7'b1000110;
				13: seg3 = 7'b0100001;
				14: seg3 = 7'b0000110;
				15: seg3 = 7'b0001110;
				default : seg3 = seg3;
			endcase
		
			case (ascii_write[3:0])
				0: seg2 = 7'b1000000;
				1: seg2 = 7'b1111001;
				2: seg2 = 7'b0100100;
				3: seg2 = 7'b0110000;
				4: seg2 = 7'b0011001;
				5: seg2 = 7'b0010010;
				6: seg2 = 7'b0000010;
				7: seg2 = 7'b1111000;
				8: seg2 = 7'b0000000;
				9: seg2 = 7'b0010000;
				10: seg2 = 7'b0001000;
				11: seg2 = 7'b0000011;
				12: seg2 = 7'b1000110;
				13: seg2 = 7'b0100001;
				14: seg2 = 7'b0000110;
				15: seg2 = 7'b0001110;
				default : seg2 = seg2;
			endcase
			
		end
	end



endmodule