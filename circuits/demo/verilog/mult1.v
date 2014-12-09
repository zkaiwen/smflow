`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    17:15:52 11/19/2014 
// Design Name: 
// Module Name:    mult1 
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
module mult1(A, B, P
    );
	 input[7:0] Din, // Data input for load
	 input[2:0] B0,
	 input[2:0] B1,
	 input[2:0] B2,
	 input[2:0] B3,
    output reg [7:0] Dout);

  reg [7:0] D0, D1, D2, D3; 
  
  always @(posedge CLK) begin
    if (reset) begin
      D0 <= 0; D1 <= 0; D2 <= 0; D3 <= 0;
    end else begin
      D3 <= Din; D2 <= D3; D1 <= D2; D0 <= D1; 
      Dout<=  (D0*B0) +  (D1*B1) +  (D2*B2) +  (D3*B3); endmodule

module multiply (A, B, P);
	
endmodule /* TopLevel6288 */

/*************************************************************************/

