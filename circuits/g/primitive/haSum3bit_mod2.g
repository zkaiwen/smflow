26
6 0 A1 1 B1 16 A2XORB2 17 B2ANDA2 31 A3 32 B3
1 46 S 
0 IN 0 1 11 O
1 IN 0 1 11 O
11 ANDX 2 0 I0 1 I1 1 25 O
16 IN 0 1 25 O 
17 IN 0 1 28 O
25 ANDX 2 16 I0 11 I1 1 27 O
27 INVX 1 25 I0 1 29 O
28 INVX 1 17 I0 1 29 O
29 ANDX 2 27 I0 28 I1 1 30 O
30 INVX 1 29 I0 1 41 O 
31 IN 0 2 33 O 34 O
32 IN 0 2 33 O 35 O
33 ANDX 2 31 I0 32 I1 1 37 O
34 INV 1 31 I0 1 36 O
35 INV 1 32 I0 1 36 O
36 ANDX 2 34 I0 35 I1 1 38 O
37 INV 1 33 I0 1 39 O
38 INV 1 36 I0 1 39 O
39 ANDX 2 38 I0 37 I1 2 40 O 41 O
40 ANDX 2 39 I0 30 I1 1 44 O
41 INV 1 39 I0  1 43 O
42 INV 1 30 I0 1 43 O
43 ANDX 2 41 I0 42 I1 1 45 O
44 INV 1 40 I0 1 46 O
45 INV 1 43 I0 1 46 O
46 ANDX 2 44 I0 45 I1 0  