module vga_mem(clk, vgamem_addr, pressing, ascii_write, ascii_read);

input clk;
input pressing;
input [11:0] vgamem_addr;
input [7:0] ascii_write;

output reg [7:0] ascii_read;

reg [11:0] ptr;

reg [7:0] vgamem [2099:0];

initial begin ptr = 12'b0; end

always @(posedge clk)
begin
	ascii_read <= vgamem[vgamem_addr];
end

reg [31:0] cnt;

always @(negedge clk)
begin
	cnt <= cnt + 1;
	if (pressing && cnt >= 6500000)
	begin
		if (ascii_write == 8'h0D)
			ptr <= ptr + (70 - ptr%70);
		else if (ascii_write == 8'h08)
		begin ptr <= ptr - 1; vgamem[ptr] <= 8'h00; end
		else
		begin vgamem[ptr] <= ascii_write; ptr <= ptr + 1; end
		cnt <= 0;
	end
	if(ptr > 2099)
		ptr <= 0;
end

endmodule
