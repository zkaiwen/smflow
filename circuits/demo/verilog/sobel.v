`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    17:37:06 11/20/2014 
// Design Name: 
// Module Name:    sobel 
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
/* Verilog code to calculate Sobel
 * Free to download modify and use
 * 
 */
module sobel_c( p0, p1, p2, p3, p5, p6, p7, p8, out);

input  [15:0] p0,p1,p2,p3,p5,p6,p7,p8;	// 8 bit pixels inputs 
output [15:0] out;					// 8 bit output pixel 

wire signed [15:0] gx,gy;    //11 bits because max value of gx and gy is  
//255*4 and last bit for sign					 
wire signed [15:0] abs_gx,abs_gy;	//it is used to find the absolute value of gx and gy 
wire [15:0] sum;			//the max value is 255*8. here no sign bit needed. 

sobel_horizontal(abs_gx, p0, p2, p3, p5, p6, p8);
sobel_vertical(abs_gy, p0, p6, p1, p7, p2, p8);

	add16_c add16( abs_gx, abs_gy, sum);
assign out = (|sum[15:8])?8'hff : sum[7:0];	// to limit the max value to 255  

endmodule

module sobel_horizontal( abs_gx, p0, p2, p3, p5, p6, p8);
	input  [15:0] p0,p2,p3,p5,p6,p8;	// 8 bit pixels inputs 
	wire signed [15:0] gx;    //11 bits because max value of gx and gy is  
//255*4 and last bit for sign					 
	output  [15:0] abs_gx;	//it is used to find the absolute value of gx and gy 

	assign gx=((p2-p0)+((p5-p3)<<1)+(p8-p6));//sobel mask for gradient in horiz. direction 
	assign abs_gx = (gx[10]? ~gx+1 : gx);	// to find the absolute value of gx. 

endmodule

module sobel_vertical(abs_gy, p0, p6, p1, p7, p2, p8);
input  [15:0] p0,p1,p2,p6,p7,p8;	// 8 bit pixels inputs 
	wire signed [15:0] gy;    //11 bits because max value of gx and gy is  
//255*4 and last bit for sign					 
	output  [15:0] abs_gy;	//it is used to find the absolute value of gx and gy 

	assign gy=((p0-p6)+((p1-p7)<<1)+(p2-p8));//sobel mask for gradient in vertical direction 

	assign abs_gy = (gy[10]? ~gy+1 : gy);	// to find the absolute value of gy. 

endmodule

module add16_c(A, B, S, CO
    );
	 
	 input[15:0] A, B;
	 output[15:0] S;
	 output CO;
	 wire w1, w2, w3, w4, w5, w6, w7, w8, w9, w10,w11, w12, w13, w14, w15;
	 
	ha ha0(A[0], B[0], S[0], w1);	
	fa fa1(A[1], B[1], w1, S[1], w2);
	fa fa2(A[2], B[2], w2, S[2], w3);
	fa fa3(A[3], B[3], w3, S[3], w4);
	fa fa4(A[4], B[4], w4, S[4], w5);
	fa fa5(A[5], B[5], w5, S[5], w6);
	fa fa6(A[6], B[6], w6, S[6], w7);
	fa fa7(A[7], B[7], w7, S[7], w8);
	fa fa11(A[8], B[8], w8, S[8], w9);
	fa fa10(A[9], B[9], w9, S[9], w10);
	fa fa12(A[10], B[10], w10, S[10], w11);
	fa fa13(A[11], B[11], w11, S[11], w12);
	fa fa14(A[12], B[12], w12, S[12], w13);
	fa fa15(A[13], B[13], w13, S[13], w14);
	fa fa16(A[14], B[14], w14, S[14], w15);
	fa fa17(A[15], B[15], w15, S[15], CO);

endmodule

module fa(A, B, C, S, CO);
	input A, B, C;
	output S, CO;
	wire w1, w2, w3;
	
	XOR2 x1(w1, A, B);
	XOR2 x2(S, w1, C);
	AND2 a1(w2, C, w1);
	AND2 a2(w3, A, B);
	OR2 o1(CO, w2, w3);
endmodule

module ha(A, B, S, CO);
	input A, B;
	output S, CO;
	XOR2 x1(S, A, B);
	AND2 a2(CO, A, B);
endmodule