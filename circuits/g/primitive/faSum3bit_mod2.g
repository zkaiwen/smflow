31
6 0 A1XORB1 1 B1ANDA1 2 CI 16 A2 17 B2 31 A3XORB3
1 38 SUM 
0 IN 0 1 10 O
1 IN 0 1 13 O
2 IN 0 1 10 O 
10 ANDX 2 0 I0 2 I1 1 12 O
12 INVX 1 10 I0 1 14 O
13 INVX 1 1 I0 1 14 O
14 ANDX 2 12 I0 13 I1 1 15 O
15 INVX 1 14 I0 1 25 O
16 IN 0 3 18 O 19 O 26 O
17 IN 0 3 18 O 20 O 26 O
18 ANDX 2 16 I0 17 I1 1 22 O
19 INV 1 16 I0  1 21 O
20 INV 1 17 I0 1 21 O
21 ANDX 2 19 I0 20 I1 1 23 O
22 INV 1 18 I0 1 24 O
23 INV 1 21 I0 1 24 O
24 ANDX 2 22 I0 23 I1 1 25 O
25 ANDX 2 24 I0 15 I1 1 27 O
26 ANDX 2 16 I0 17 I1 1 28 O
27 INVX 1 25 I0 1 29 O
28 INVX 1 26 I0 1 29 O
29 ANDX 2 27 I0 28 I1 1 30 O
30 INVX 1 29 I0 2 32 O 34 O 
31 IN 0 2 32 O 33 O
32 ANDX 2 31 I0 30 I1 1 36 O
33 INV 1 31 I0  1 35 O
34 INV 1 30 I0 1 35 O
35 ANDX 2 33 I0 34 I1 1 37 O
36 INV 1 32 I0 1 38 O
37 INV 1 35 I0 1 38 O
38 ANDX 2 36 I0 37 I1 0 