derivation length: 3;
depth: 4;

axiom: #(0.2) !(120, 1, 1) a;

productions:
a         --> F(2) [ -(26.00) F(2) a ] F(2) /(137.0) F(2) a ;
F(len)    --> F(len *1.1) ;

interpretations: 

a --> !h(-42) [b] /(180) [b];
b --> !h(6) [-(9) b {  TP ] TP c TP }; 
c --> f(0.4) c;
