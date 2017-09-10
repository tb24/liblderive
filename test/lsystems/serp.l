derivation length: 6;
axiom: #(0.2) fr ;

productions:

fl --> fr r fl r fr ;
fr --> fl l fr l fl ;

interpretations:

fl --> F(1);
fr --> F(1);
l  --> -(60);
r  --> +(60);
