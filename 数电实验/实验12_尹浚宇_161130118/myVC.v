module myVC(

input			CLOCK_50,				//	On Board 50 MHz
input	[3:0]	KEY,					//	Pushbutton[3:0]
input	[5:0]	SW,						//	Toggle Switch[9:0]

inout			AUD_ADCLRCK,			//	ADC LR Clock
input			AUD_ADCDAT,				//	ADC Data
inout			AUD_DACLRCK,			//	DAC LR Clock
output			AUD_DACDAT,				//	DAC Data
inout			AUD_BCLK,				//	Bit-Stream Clock
output			AUD_XCK,				//	Chip Clock

inout			I2C_SDAT,				//	I2C Data
output			I2C_SCLK				//	I2C Clock
);

wire reset = !KEY[0];

// audio module
reg [18:0] delay_cnt;
wire [18:0] delay;
reg snd;

always @(posedge CLOCK_50)
	if(delay_cnt == delay) begin
		delay_cnt <= 0;
		snd <= !snd;
	end else delay_cnt <= delay_cnt + 1;


assign delay = {SW[3:0], 15'd3000};

reg [31:0] sound;
always @(SW)
begin
	if (SW[4])
		sound = {audio_data, 16'b0};
	else
		sound = (SW == 0) ? 0 : snd ? 32'd10000000 : -32'd10000000;
end

wire		[31:0]	left_channel_audio_in;
wire		[31:0]	right_channel_audio_in;

wire		[31:0]	left_channel_audio_out;
wire		[31:0]	right_channel_audio_out;


assign left_channel_audio_out	= (SW == 0) ? left_channel_audio_in : sound;
assign right_channel_audio_out	= (SW == 0) ? right_channel_audio_in : sound;

wire audio_out_allowed, audio_in_available, write_audio_out, read_audio_in;


assign read_audio_in = audio_in_available & audio_out_allowed;
assign write_audio_out = audio_in_available & audio_out_allowed;


Audio_Controller Audio_Controller (	.clk(CLOCK_50), .reset(reset), .clear_audio_in_memory(), .read_audio_in(read_audio_in), .clear_audio_out_memory(),
									.left_channel_audio_out(left_channel_audio_out), .right_channel_audio_out(right_channel_audio_out), .write_audio_out(write_audio_out),
									.AUD_ADCDAT(AUD_ADCDAT), .AUD_BCLK(AUD_BCLK), .AUD_ADCLRCK(AUD_ADCLRCK), .AUD_DACLRCK(AUD_DACLRCK), .I2C_SDAT(I2C_SDAT),
									.audio_in_available(audio_in_available), .left_channel_audio_in(left_channel_audio_in), .right_channel_audio_in(right_channel_audio_in),
									.audio_out_allowed(audio_out_allowed), .AUD_XCK(AUD_XCK), .AUD_DACDAT(AUD_DACDAT), .I2C_SCLK(I2C_SCLK));
defparam
	Audio_Controller.USE_MIC_INPUT = 1; // 0 - for line in or 1 - for microphone in
	
// music play module
wire	[15:0] audio_data;
Music_Player Music_Player(CLOCK_50, reset, AUD_DACLRCK, audio_data, SW[5], KEY[1], KEY[2]);


endmodule