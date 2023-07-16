module h_addr_trans(h_addr, x_70, x_9);

input [9:0] h_addr;
output reg [6:0] x_70;
output reg [4:0] x_9;

reg [11:0] x_data [640:0];

initial 
begin
	$readmemh("D:/JunYuYin/DigitalExp/ExpCode/exp11/h_addr.txt", x_data, 0, 629);
end

always @(h_addr)
begin
	x_70 = x_data[h_addr][11:5];
	x_9 = x_data[h_addr][4:0];
end

endmodule