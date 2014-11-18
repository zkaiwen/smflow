module add4_ks( a, b, s0, s1, s2, s3, CO );

	input [3:0] a;
	input [3:0] b;
	output s0;
	output s1;
	output s2;
	output s3;
	output CO;


	wire N0_a0xF82D02AND__90_35;
	wire N0_a0xF83602AND__96_57;
	wire N0_a0xF83802OR__104_46;
	wire N0_a0xF82F02AND__123_54;
	wire N0_a0xF83302AND__94_78;
	wire N0_a0xF83502OR__104_67;
	wire N0_a0xF83102AND__121_72;
	wire N0_a0xF82C02AND__58_69;
	wire N0_a0xF83402AND__94_71;
	wire N0_a0xF83702AND__94_50;
	wire N0_a0xF82902AND__58_30;
	wire N0_a0xC2180303AND__74_17_XOR_Bit_Bit_58_37;
	wire N0_a0xF82B02AND__58_49;
	wire N0_a0xC2280303AND__74_17_XOR_Bit_Bit_58_77;
	wire N0_a0xC2200303AND__74_17_XOR_Bit_Bit_58_56;
	wire N0_a0xF83002OR__133_50;
	wire N0_a0xF82A02AND__58_8;
	wire N0_a0xF82E02OR__100_31;
	wire a_0;
	wire a_1;
	wire a_2;
	wire a_3;
	wire b_0;
	wire b_1;
	wire b_2;
	wire b_3;
	wire N0_a0xC2100103OR__37_16_XOR_Bit_Bit_58_15;
	wire N0_a0xC2100003INVERT__49_24_XOR_Bit_Bit_58_15;
	wire N0_a0xC2100203AND__37_24_XOR_Bit_Bit_58_15;
	wire N0_a0xC2180103OR__37_16_XOR_Bit_Bit_58_37;
	wire N0_a0xC2180003INVERT__49_24_XOR_Bit_Bit_58_37;
	wire N0_a0xC2180203AND__37_24_XOR_Bit_Bit_58_37;
	wire N0_a0xC2200103OR__37_16_XOR_Bit_Bit_58_56;
	wire N0_a0xC2200003INVERT__49_24_XOR_Bit_Bit_58_56;
	wire N0_a0xC2200203AND__37_24_XOR_Bit_Bit_58_56;
	wire N0_a0xC2280103OR__37_16_XOR_Bit_Bit_58_77;
	wire N0_a0xC2280003INVERT__49_24_XOR_Bit_Bit_58_77;
	wire N0_a0xC2280203AND__37_24_XOR_Bit_Bit_58_77;
	wire N0_a0xC2300103OR__37_16_XOR_Bit_Bit_138_29;
	wire N0_a0xC2300003INVERT__49_24_XOR_Bit_Bit_138_29;
	wire N0_a0xC2300203AND__37_24_XOR_Bit_Bit_138_29;
	wire N0_a0xC2380103OR__37_16_XOR_Bit_Bit_148_48;
	wire N0_a0xC2380003INVERT__49_24_XOR_Bit_Bit_148_48;
	wire N0_a0xC2380203AND__37_24_XOR_Bit_Bit_148_48;
	wire N0_a0xC2400103OR__37_16_XOR_Bit_Bit_153_66;
	wire N0_a0xC2400003INVERT__49_24_XOR_Bit_Bit_153_66;
	wire N0_a0xC2400203AND__37_24_XOR_Bit_Bit_153_66;


	assign a_0 = a[0];
	assign a_1 = a[1];
	assign a_2 = a[2];
	assign a_3 = a[3];
	assign b_0 = b[0];
	assign b_1 = b[1];
	assign b_2 = b[2];
	assign b_3 = b[3];


	AND2 Ia0xF82902AND__58_30( 
		.O(N0_a0xF82902AND__58_30), 
		.I0(b_1), 
		.I1(a_1) 
	);
	AND2 Ia0xF82A02AND__58_8( 
		.O(N0_a0xF82A02AND__58_8), 
		.I0(a_0), 
		.I1(b_0) 
	);
	AND2 Ia0xF82B02AND__58_49( 
		.O(N0_a0xF82B02AND__58_49), 
		.I0(b_2), 
		.I1(a_2) 
	);
	AND2 Ia0xF82C02AND__58_69( 
		.O(N0_a0xF82C02AND__58_69), 
		.I0(a_3), 
		.I1(b_3) 
	);
	AND2 Ia0xF82D02AND__90_35( 
		.O(N0_a0xF82D02AND__90_35), 
		.I0(N0_a0xF82A02AND__58_8), 
		.I1(N0_a0xC2180303AND__74_17_XOR_Bit_Bit_58_37) 
	);
	OR2 Ia0xF82E02OR__100_31( 
		.I0(N0_a0xF82902AND__58_30), 
		.I1(N0_a0xF82D02AND__90_35), 
		.O(N0_a0xF82E02OR__100_31) 
	);
	AND2 Ia0xF82F02AND__123_54( 
		.O(N0_a0xF82F02AND__123_54), 
		.I0(N0_a0xF82A02AND__58_8), 
		.I1(N0_a0xF83602AND__96_57) 
	);
	OR2 Ia0xF83002OR__133_50( 
		.I0(N0_a0xF83802OR__104_46), 
		.I1(N0_a0xF82F02AND__123_54), 
		.O(N0_a0xF83002OR__133_50) 
	);
	AND2 Ia0xF83102AND__121_72( 
		.O(N0_a0xF83102AND__121_72), 
		.I0(N0_a0xF82E02OR__100_31), 
		.I1(N0_a0xF83302AND__94_78) 
	);
	OR2 Ia0xF83202OR__132_68( 
		.I0(N0_a0xF83502OR__104_67), 
		.I1(N0_a0xF83102AND__121_72), 
		.O(CO) 
	);
	AND2 Ia0xF83302AND__94_78( 
		.O(N0_a0xF83302AND__94_78), 
		.I0(N0_a0xC2280303AND__74_17_XOR_Bit_Bit_58_77), 
		.I1(N0_a0xC2200303AND__74_17_XOR_Bit_Bit_58_56) 
	);
	AND2 Ia0xF83402AND__94_71( 
		.O(N0_a0xF83402AND__94_71), 
		.I0(N0_a0xC2280303AND__74_17_XOR_Bit_Bit_58_77), 
		.I1(N0_a0xF82B02AND__58_49) 
	);
	OR2 Ia0xF83502OR__104_67( 
		.I0(N0_a0xF82C02AND__58_69), 
		.I1(N0_a0xF83402AND__94_71), 
		.O(N0_a0xF83502OR__104_67) 
	);
	AND2 Ia0xF83602AND__96_57( 
		.O(N0_a0xF83602AND__96_57), 
		.I0(N0_a0xC2200303AND__74_17_XOR_Bit_Bit_58_56), 
		.I1(N0_a0xC2180303AND__74_17_XOR_Bit_Bit_58_37) 
	);
	AND2 Ia0xF83702AND__94_50( 
		.O(N0_a0xF83702AND__94_50), 
		.I0(N0_a0xC2200303AND__74_17_XOR_Bit_Bit_58_56), 
		.I1(N0_a0xF82902AND__58_30) 
	);
	OR2 Ia0xF83802OR__104_46( 
		.I0(N0_a0xF82B02AND__58_49), 
		.I1(N0_a0xF83702AND__94_50), 
		.O(N0_a0xF83802OR__104_46) 
	);
	OR2 Ia0xC2100103OR__37_16_XOR_Bit_Bit_58_15( 
		.I0(a_0), 
		.I1(b_0), 
		.O(N0_a0xC2100103OR__37_16_XOR_Bit_Bit_58_15) 
	);
	AND2 Ia0xC2100303AND__74_17_XOR_Bit_Bit_58_15( 
		.O(s0), 
		.I0(N0_a0xC2100103OR__37_16_XOR_Bit_Bit_58_15), 
		.I1(N0_a0xC2100003INVERT__49_24_XOR_Bit_Bit_58_15) 
	);
	AND2 Ia0xC2100203AND__37_24_XOR_Bit_Bit_58_15( 
		.O(N0_a0xC2100203AND__37_24_XOR_Bit_Bit_58_15), 
		.I0(a_0), 
		.I1(b_0) 
	);
	INV Ia0xC2100003INVERT__49_24_XOR_Bit_Bit_58_15( 
		.I(N0_a0xC2100203AND__37_24_XOR_Bit_Bit_58_15), 
		.O(N0_a0xC2100003INVERT__49_24_XOR_Bit_Bit_58_15) 
	);
	OR2 Ia0xC2180103OR__37_16_XOR_Bit_Bit_58_37( 
		.I0(b_1), 
		.I1(a_1), 
		.O(N0_a0xC2180103OR__37_16_XOR_Bit_Bit_58_37) 
	);
	AND2 Ia0xC2180303AND__74_17_XOR_Bit_Bit_58_37( 
		.O(N0_a0xC2180303AND__74_17_XOR_Bit_Bit_58_37), 
		.I0(N0_a0xC2180103OR__37_16_XOR_Bit_Bit_58_37), 
		.I1(N0_a0xC2180003INVERT__49_24_XOR_Bit_Bit_58_37) 
	);
	AND2 Ia0xC2180203AND__37_24_XOR_Bit_Bit_58_37( 
		.O(N0_a0xC2180203AND__37_24_XOR_Bit_Bit_58_37), 
		.I0(b_1), 
		.I1(a_1) 
	);
	INV Ia0xC2180003INVERT__49_24_XOR_Bit_Bit_58_37( 
		.I(N0_a0xC2180203AND__37_24_XOR_Bit_Bit_58_37), 
		.O(N0_a0xC2180003INVERT__49_24_XOR_Bit_Bit_58_37) 
	);
	OR2 Ia0xC2200103OR__37_16_XOR_Bit_Bit_58_56( 
		.I0(b_2), 
		.I1(a_2), 
		.O(N0_a0xC2200103OR__37_16_XOR_Bit_Bit_58_56) 
	);
	AND2 Ia0xC2200303AND__74_17_XOR_Bit_Bit_58_56( 
		.O(N0_a0xC2200303AND__74_17_XOR_Bit_Bit_58_56), 
		.I0(N0_a0xC2200103OR__37_16_XOR_Bit_Bit_58_56), 
		.I1(N0_a0xC2200003INVERT__49_24_XOR_Bit_Bit_58_56) 
	);
	AND2 Ia0xC2200203AND__37_24_XOR_Bit_Bit_58_56( 
		.O(N0_a0xC2200203AND__37_24_XOR_Bit_Bit_58_56), 
		.I0(b_2), 
		.I1(a_2) 
	);
	INV Ia0xC2200003INVERT__49_24_XOR_Bit_Bit_58_56( 
		.I(N0_a0xC2200203AND__37_24_XOR_Bit_Bit_58_56), 
		.O(N0_a0xC2200003INVERT__49_24_XOR_Bit_Bit_58_56) 
	);
	OR2 Ia0xC2280103OR__37_16_XOR_Bit_Bit_58_77( 
		.I0(a_3), 
		.I1(b_3), 
		.O(N0_a0xC2280103OR__37_16_XOR_Bit_Bit_58_77) 
	);
	AND2 Ia0xC2280303AND__74_17_XOR_Bit_Bit_58_77( 
		.O(N0_a0xC2280303AND__74_17_XOR_Bit_Bit_58_77), 
		.I0(N0_a0xC2280103OR__37_16_XOR_Bit_Bit_58_77), 
		.I1(N0_a0xC2280003INVERT__49_24_XOR_Bit_Bit_58_77) 
	);
	AND2 Ia0xC2280203AND__37_24_XOR_Bit_Bit_58_77( 
		.O(N0_a0xC2280203AND__37_24_XOR_Bit_Bit_58_77), 
		.I0(a_3), 
		.I1(b_3) 
	);
	INV Ia0xC2280003INVERT__49_24_XOR_Bit_Bit_58_77( 
		.I(N0_a0xC2280203AND__37_24_XOR_Bit_Bit_58_77), 
		.O(N0_a0xC2280003INVERT__49_24_XOR_Bit_Bit_58_77) 
	);
	OR2 Ia0xC2300103OR__37_16_XOR_Bit_Bit_138_29( 
		.I0(N0_a0xC2180303AND__74_17_XOR_Bit_Bit_58_37), 
		.I1(N0_a0xF82A02AND__58_8), 
		.O(N0_a0xC2300103OR__37_16_XOR_Bit_Bit_138_29) 
	);
	AND2 Ia0xC2300303AND__74_17_XOR_Bit_Bit_138_29( 
		.O(s1), 
		.I0(N0_a0xC2300103OR__37_16_XOR_Bit_Bit_138_29), 
		.I1(N0_a0xC2300003INVERT__49_24_XOR_Bit_Bit_138_29) 
	);
	AND2 Ia0xC2300203AND__37_24_XOR_Bit_Bit_138_29( 
		.O(N0_a0xC2300203AND__37_24_XOR_Bit_Bit_138_29), 
		.I0(N0_a0xC2180303AND__74_17_XOR_Bit_Bit_58_37), 
		.I1(N0_a0xF82A02AND__58_8) 
	);
	INV Ia0xC2300003INVERT__49_24_XOR_Bit_Bit_138_29( 
		.I(N0_a0xC2300203AND__37_24_XOR_Bit_Bit_138_29), 
		.O(N0_a0xC2300003INVERT__49_24_XOR_Bit_Bit_138_29) 
	);
	OR2 Ia0xC2380103OR__37_16_XOR_Bit_Bit_148_48( 
		.I0(N0_a0xC2200303AND__74_17_XOR_Bit_Bit_58_56), 
		.I1(N0_a0xF82E02OR__100_31), 
		.O(N0_a0xC2380103OR__37_16_XOR_Bit_Bit_148_48) 
	);
	AND2 Ia0xC2380303AND__74_17_XOR_Bit_Bit_148_48( 
		.O(s2), 
		.I0(N0_a0xC2380103OR__37_16_XOR_Bit_Bit_148_48), 
		.I1(N0_a0xC2380003INVERT__49_24_XOR_Bit_Bit_148_48) 
	);
	AND2 Ia0xC2380203AND__37_24_XOR_Bit_Bit_148_48( 
		.O(N0_a0xC2380203AND__37_24_XOR_Bit_Bit_148_48), 
		.I0(N0_a0xC2200303AND__74_17_XOR_Bit_Bit_58_56), 
		.I1(N0_a0xF82E02OR__100_31) 
	);
	INV Ia0xC2380003INVERT__49_24_XOR_Bit_Bit_148_48( 
		.I(N0_a0xC2380203AND__37_24_XOR_Bit_Bit_148_48), 
		.O(N0_a0xC2380003INVERT__49_24_XOR_Bit_Bit_148_48) 
	);
	OR2 Ia0xC2400103OR__37_16_XOR_Bit_Bit_153_66( 
		.I0(N0_a0xC2280303AND__74_17_XOR_Bit_Bit_58_77), 
		.I1(N0_a0xF83002OR__133_50), 
		.O(N0_a0xC2400103OR__37_16_XOR_Bit_Bit_153_66) 
	);
	AND2 Ia0xC2400303AND__74_17_XOR_Bit_Bit_153_66( 
		.O(s3), 
		.I0(N0_a0xC2400103OR__37_16_XOR_Bit_Bit_153_66), 
		.I1(N0_a0xC2400003INVERT__49_24_XOR_Bit_Bit_153_66) 
	);
	AND2 Ia0xC2400203AND__37_24_XOR_Bit_Bit_153_66( 
		.O(N0_a0xC2400203AND__37_24_XOR_Bit_Bit_153_66), 
		.I0(N0_a0xC2280303AND__74_17_XOR_Bit_Bit_58_77), 
		.I1(N0_a0xF83002OR__133_50) 
	);
	INV Ia0xC2400003INVERT__49_24_XOR_Bit_Bit_153_66( 
		.I(N0_a0xC2400203AND__37_24_XOR_Bit_Bit_153_66), 
		.O(N0_a0xC2400003INVERT__49_24_XOR_Bit_Bit_153_66) 
	);


endmodule

