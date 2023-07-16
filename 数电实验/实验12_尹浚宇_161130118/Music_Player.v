module Music_Player(CLOCK_50, reset, AUD_DACLRCK, audio_data, SW, K1, K2);
input CLOCK_50, reset, AUD_DACLRCK, SW, K1, K2;
output [15:0] audio_data;


wire	[15:0] freq;
wire    clk_song;
clkgen #(5) my_song_clk(CLOCK_50, reset, 1'b1, clk_song);
Sin_Generator sin_wave(AUD_DACLRCK, 1'b1, freq, audio_data);
Song Song(SW, clk_song, K1, K2, freq);


endmodule