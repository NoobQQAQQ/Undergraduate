module keyboard_top(clk, clrn, ps2_clk, ps2_data, ascii_code, pressing);

input clk, clrn, ps2_clk, ps2_data;
output [7:0] ascii_code;
output pressing;

wire [7:0] data; // keycode
wire ready;
wire overflow;
wire [7:0] dout;

// get keycode
ps2_keyboard get_keycode(clk, clrn, ps2_clk, ps2_data, data, ready, 1'b0, overflow);

// keyboard state machine
state_machine myFSM(clk, data, ready, dout, pressing);

// keycode to ascii_code
key2ascii get_ascii(dout, ascii_code);

endmodule