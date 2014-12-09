`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    14:21:23 11/20/2014 
// Design Name: 
// Module Name:    tb 
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
module tb; 
  wire  dummy;
  wire[7:0] sum;
  reg [7:0] A, B;
  
	
  add8_c U0 ( 
  .A    (A), 
  .B  (B), 
  .S  (sum),
.CO(dummy)  
  ); 

    
  initial begin
    A = 8'b11111111; 
	 B = 8'b00000001; 
    #100 
	 A = 8'b01111111; 
	 B = 8'b00000001; 
	 #100 
	 A = 8'b00000000; 
	 B = 8'b00000000; 
	 #100 
	 A = 8'b00000001; 
	 B = 8'b00000001; 
	 #100 
	 A = 8'b10101010; 
	 B = 8'b01010101; 
	end
	

endmodule 

module life_tb; 
  wire  out; 
  reg [7:0] neighbors; 
  reg self;
	
  life_8c U0 ( 
  .self    (self), 
  .neighbors  (neighbors), 
  .out  (out) 
  ); 
    

    
  initial begin
    self = 1'b0; 
	 neighbors = 8'b00000000; 
    #100 neighbors = 8'b00000001; 
	 #100  neighbors = 8'b00000011; 
    #100 neighbors = 8'b00000111; 
    #100 neighbors = 8'b00001111; 
    #100 neighbors = 8'b11110001; 
    #100 neighbors = 8'b11111001; 
	 #100 
	 neighbors = 8'b11111011; 
	 self = 1'b1; 
	 #100 neighbors = 8'b00000001; 
    #100 neighbors = 8'b00000001; 
	 #100 neighbors = 8'b00000011; 
    #100 neighbors = 8'b00000111; 
    #100 neighbors = 8'b00001111; 
    #100 neighbors = 8'b11110001; 
    #100 neighbors = 8'b11111001; 
    #100 neighbors = 8'b11111011; 
	end

endmodule 