module get_ascii(key_code, ascii_code);
	input [7:0] key_code;
	
	output reg [7:0] ascii_code;
	
	(* ram_init_file = "D:/JunYuYin/DigitalExp/ExpCode/exp08/scancode.mif" *) reg [7:0] myrom[255:0];
	
	always @(key_code)
	begin
		ascii_code = myrom[key_code];
	end

endmodule