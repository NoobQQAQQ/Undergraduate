module get_vga_data(pclk, ascii_read, y_16, x_9, vga_data);

input pclk;
input [7:0] ascii_read;
input [4:0] y_16, x_9;

output reg [23:0] vga_data;

reg [11:0] vga_font [4095:0];

initial 
begin
	$readmemh("D:/JunYuYin/DigitalExp/ExpCode/exp11/vga_font.txt", vga_font, 0, 4095);
end

reg [11:0] line;
always @(ascii_read or y_16)
begin
	line = vga_font[{ascii_read, y_16[3:0]}];
end


always @(posedge pclk)
begin
	if (line[x_9] == 1'b1)
		vga_data <= 24'hFFFFFF;
	else
		vga_data <= 24'h000000;
end

endmodule