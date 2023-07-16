module my_alu(x, y, sel, res, OF, CF, ZF);
	input [3:0] x;
	input [3:0] y;
	input [2:0] sel;
	output reg [3:0] res;
	output reg OF, CF, ZF;
	reg [3:0] complement; // 补码用于减法
	
	always @ (*)
	case(sel)
		3'b000: 
		begin
			{CF, res} = x + y;
			OF = (x[3] == y[3]) && (res[3] != x[3]);
			ZF = ~(|res);
		end
		3'b001: 
		begin
			complement = ~y + 1;
			{CF, res} = x + complement;
			OF = (x[3] != complement[3]) && (res[3] == complement[3]);
			ZF = ~(|res);	 
		end
		3'b010: 
		begin
			res = ~x;
			CF = 0;
			OF = 0;
			ZF = ~(|res);
		end
		3'b011:
		begin
			res = x & y;
			CF = 0;
			OF = 0;
			ZF = ~(|res);
		end
		3'b100:
		begin
			res = x | y;
			CF = 0;
			OF = 0;
			ZF = ~(|res);
		end
		3'b101:
		begin
			res = x ^ y;
			CF = 0;
			OF = 0;
			ZF = ~(|res);
		end
		3'b110:
		begin
			complement = ~y + 1;
			{CF, res} = x + complement;
			OF = (x[3] != complement[3]) && (res[3] == complement[3]);
			ZF = ~(|res);
			if (((OF == 0 && res[3] == 0) || (OF == 1 && res[3] == 1)) && ZF == 0)
				res = 1;
			else
				res = 0;
		end
		3'b111:
		begin
			complement = ~y + 1;
			{CF, res} = x + complement;
			OF = (x[3] != complement[3]) && (res[3] == complement[3]);
			ZF = ~(|res);
			if(ZF == 1)
				res = 1;
			else
				res = 0;
		end
		default: begin res = 0; CF = 0; OF = 0; ZF = 0; end
	endcase
endmodule
