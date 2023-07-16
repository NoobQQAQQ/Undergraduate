module Song(mode, clk, keyplay, keynext, freq);
input mode;
input clk;
input keyplay;
input keynext;
output reg [15:0] freq;


reg [8:0] count = 9'b000000000;
reg state = 0;

reg [1:0]song_number = 2'b0;

(* ram_init_file = "Song1.mif" *) reg [15:0] song1[415:0];
(* ram_init_file = "Song2.mif" *) reg [15:0] song2[383:0];
(* ram_init_file = "Song3.mif" *) reg [15:0] song3[415:0];
(* ram_init_file = "Song4.mif" *) reg [15:0] song4[415:0];
(* ram_init_file = "rand.mif" *) reg [1:0] rand[31:0];

integer index = 0;
always @(posedge keyplay)// play and stop
	state = ~state;
always @(posedge clk)
begin
	if(!keynext)
	begin
	    if(mode)
		    song_number <= (song_number + 1) % 4;
	    else
	    begin song_number <= rand[index]; index <= (index + 1) % 31; end
	    count <= 9'b0;
	end

	else 
    begin
	    if(state)
        begin
	        case(song_number)
		    0:
            begin 
			    freq <= song1[count]; count <= count+1; 		
			    if(count == 416) 
                begin 
				    count <= 9'b0;
				    if(mode)
					    song_number <= (song_number + 1) % 4;
				    else
						begin song_number <= rand[index]; index <= (index + 1) % 31; end
			    end
		    end
            1:
            begin
                freq <= song2[count]; count <= count+1; 		
                if(count == 384) 
                begin 
                    count <= 9'b0;
                    if(mode)
                        song_number <= (song_number + 1) % 4;
                    else
                        begin song_number <= rand[index]; index <= (index + 1) % 31; end
                end
            end
            2:
            begin
                freq <= song3[count]; count <= count + 1; 		
                if(count == 176) 
                begin 
                    count <= 9'b0;
                    if(mode)
                        song_number <= (song_number + 1) % 4;
                    else
                        begin song_number <= rand[index]; index <= (index + 1) % 31; end
                end
            end
            3:
            begin
                freq <= song4[count]; count <= count + 1; 		
                if(count == 320) 
                begin 
                    count <= 9'b0;
                    if(mode)
                        song_number <= (song_number + 1) % 4;
                    else
                        begin song_number <= rand[index]; index <= (index + 1) % 31; end
                end
            end
            endcase
        end
        else 
        begin
            freq <=16'h0000;
            count <=count;
        end
	end
end
endmodule
