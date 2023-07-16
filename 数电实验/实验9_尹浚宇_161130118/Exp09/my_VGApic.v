module my_VGApic(clk, rst_vga, rst_clkgen, en_clkgen, hsync, vsync, valid, vga_r, vga_g, vga_b, pclk, sync_n, ctrl);
	input clk, rst_vga, rst_clkgen, en_clkgen;
	input ctrl;
	
	output hsync, vsync, valid;
	output [7:0] vga_r, vga_g, vga_b;
	
	output pclk, sync_n;
	assign sync_n = 0;
	clkgen #(25000000) my_clkgen(clk, rst_clkgen, en_clkgen, pclk);
	
	
	wire [11:0] vga_data;
	wire [9:0] h_addr, v_addr;
	
	
	
	vd_mem get_vga_data({h_addr, v_addr[8:0]}, pclk, vga_data);
	
	reg [11:0] vga_data2;
	
	always @(h_addr)
	begin
		if (ctrl)
		begin
			if (h_addr < 160)
				vga_data2 = 12'hF00;
			else if (h_addr < 320)
				vga_data2 = 12'h0F0;
			else if (h_addr < 480)
				vga_data2 = 12'h00F;
			else
				vga_data2 = 12'h0FF;
		end
		else
			vga_data2 = vga_data;
	end
	
	wire [11:0] vga_data3;
	
	assign vga_data3 = vga_data2;
	
	vga_ctrl my_vgactrl(pclk, rst_vga, vga_data3, h_addr, v_addr, hsync, vsync, valid, vga_r, vga_g, vga_b);
	
endmodule