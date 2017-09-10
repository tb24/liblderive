derivation length: 270;

depth: 10;

initdelay  = 20;
delay      =  2;

axiom: !(100, 0.82, 0.44) R(initdelay) A(delay);

ignore: / F + !;

productions:

J << A(x)           --> K(0) [!(80,1,1) tris(0)];
     A(x) : x > 0   --> A(x-1);
     A(x) : x == 0  --> /(180) [ !(355, 1, 1) +(45) F(1) B ] I A(delay);

R(x) : x > 0  --> R(x-1);
R(x) : x == 0 --> J; 

J << I         --> J;
     J >> K(x) --> K(x+1);  

K(x) << B --> R(x) A(7);

tris(r) --> tris(r+1);


interpretations:

H    --> !( 35, 0.5, 1) F(1);
I    --> !( 75, 0.5, 1) F(1);
J    --> !(325, 0.5, 1) F(1);
K(x) --> !(185+x, 0.5, 1) F(1);
R(x) --> !(225, 0.5, 1) F(1);


tris(r) --> !h(r*16) a; 

a --> [b] /(180) [b];
b --> [-(9) b {  TP ] TP c TP }; 
c --> f(0.4) c;
