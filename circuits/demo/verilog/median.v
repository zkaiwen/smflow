`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date:    18:25:47 11/20/2014 
// Design Name: 
// Module Name:    median 
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
module median(
    input [7:0] x2_y1,
    input [7:0] x2_y0,
    input [7:0] x2_ym1,

    output [7:0] m

);

    // Connection signals
    wire [7:0] node_u0_hi;
    wire [7:0] node_u0_lo;

    wire [7:0] node_u4_hi;
    wire [7:0] node_u4_lo;

    wire [7:0] node_u8_hi;
    wire [7:0] node_u8_lo;



    assign m = node_u8_lo;


    // Column 3
    node node_u0 (
        .data_a(x2_y1),
        .data_b(x2_y0),

        .data_hi(node_u0_hi),
        .data_lo(node_u0_lo)
    );


    node  node_u4 (
        .data_a(node_u0_lo),
        .data_b(x2_ym1),

        .data_hi(node_u4_hi),
        .data_lo(node_u4_lo)
    );


    node node_u8 (
        .data_a(node_u0_hi),
        .data_b(node_u4_hi),

        .data_hi(node_u8_hi),
        .data_lo(node_u8_lo)
    );

 

endmodule


module node
(
    input [7:0] data_a,
    input [7:0] data_b,

    output  [7:0] data_hi,
    output  [7:0] data_lo
);


	assign data_hi = (data_a < data_b) ? data_b : data_a;
	assign data_lo = (data_a < data_b) ? data_a : data_b;

endmodule