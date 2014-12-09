`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    18:59:58 11/20/2014 
// Design Name: 
// Module Name:    TB 
// Project Name: 
// Target Devices: 
// Tool versions: 
// Description: 
//
// Dependencies: 
//
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module TB(
    );
  wire  [7:0] out; 
  reg [7:0] x3, x1, x2; 

	
  median U0 ( 
  .x2_y1    (x3), 
  .x2_y0  (x1), 
  .x2_ym1  (x2),
  .m (out) 
  ); 
    

    
  initial begin
	 x1 = 8'b00001100; 
	 x2 = 8'b00100000; 
	 x3 = 8'b10000000; 
    #100 
     x1 = 8'b00001100; 
	 x3 = 8'b00100000; 
	 x2 = 8'b10000000;  
	#100 
     x2 = 8'b00001100; 
	 x1 = 8'b00100000; 
	 x3 = 8'b10000000;
	 #100 
     x2 = 8'b00001100; 
	 x3 = 8'b00100000; 
	 x1 = 8'b10000000;
	 #100 
     x3 = 8'b00001100; 
	 x2 = 8'b00100000; 
	 x1 = 8'b10000000;
	 #100 
     x3 = 8'b00001100; 
	 x1 = 8'b00100000; 
	 x2 = 8'b10000000;
	end


endmodule
