derivation length: 4;
axiom: x l x l x l x;

productions:

x1 >> l --> x1 x1 l x1 l x1 l x1 l x1 l r x1;
x1 --> x1 x1 l x1 l x1 l l x1 l x1 r x1;
x >> l --> x x1 l x l x l x l x l x r x;
x --> x x l x l x l x l x l x r x;

interpretations: 
x1 --> F(4.00);
x --> F(4.00);
l --> -(90);
r --> +(90);
