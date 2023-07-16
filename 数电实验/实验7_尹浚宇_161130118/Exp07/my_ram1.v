module my_ram1(clk, we, inaddr, outaddr, din, dout);
	input clk;
	input we;
	input [3:0] inaddr;
	input [3:0] outaddr;
	input [1:0] din;
	
	output reg [7:0] dout;
	
	reg [7:0] ram [15:0];
	reg [7:0] data;
	
	
initial
begin
	$readmemh("D:/JunYuYin/DigitalExp/ExpCode/exp07/mem1.txt", ram, 0, 15);
end

always @ (posedge clk)
begin
	if (we)
		ram[inaddr][1:0] <= din;
	else //read enable
		data <= ram[outaddr];
		dout <= data;
end

endmodule