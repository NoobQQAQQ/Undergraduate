module ePiano(clk, clrn, ps2_clk, ps2_data, freq, pitch, HEX1, HEX0);

input clk, clrn, ps2_clk, ps2_data;
output [15:0] freq;
output [7:0] pitch;
output [6:0] HEX1;
output [6:0] HEX0;

wire [7:0] data; // keycode
wire ready;
wire overflow;
wire [7:0] dout;

// get keycode
ps2_keyboard get_keycode(clk, clrn, ps2_clk, ps2_data, data, ready, 1'b0, overflow);

// keyboard state machine
state_machine myFSM(clk, data, ready, dout);

// keycode to freq and pitch
get_data my_get_data(dout, freq, pitch);

//display part
seg_display my_display(pitch, HEX1, HEX0);

endmodule