derivation length: 5;
axiom: #(1) a; 
productions:
a --> x l x l x l x;
x --> x x l x l x l x l x x ;

interpretations: 

x --> F(10);
l --> -(90);
r --> +(90);
