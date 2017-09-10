derivation length: 5;
depth: 7;
//axiom: !(90.00, 0.34, 1.00) #(0.25) a ;
axiom: !(100, 0.74, 0.82) #(0.25) a ;
productions: 
a  --> F(1.00) [ -(30.00) F(4.00) a ] /(137.00) F(4.00) a ;
interpretations: 
F(x)  --> !h((0.00 - 0.25)) F(x) ;
a  --> [ b ] /(180.00) [ b /(180.00) ] ;
b  --> !h(6.00) [ +(22.00) b { TP ] TP c TP } ;
c  --> f(0.71) c ;
