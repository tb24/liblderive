derivation length: 7;
depth: 24;

angle = 7;

axiom: #(0.1) !(126, 0.22, 1) { y } ;

productions:

y --> y  #(0.1) !h(-6) x(1);

interpretations:

x(i) --> f(i) +(360/angle) !h(6) TP [ +(39) f(3) TP ] x(i+1) ;
 
