derivation length: 4;
depth: 6;

axiom: !(100, 1, 1) a;

productions:

a --> c c a;
c --> [+(37) i(4) a] \(180) i(4);
i(x) --> i(x+5);

interpretations: 

      i(x) >> [ --> F(x) TP [;
]  << i(x)      --> ] TP F(x) TP ;
i(len) --> F(len+1);
F(len) --> F(len+1);
//
//a --> leaf; 
//a --> [+(43) F(1) leaf] \(180) [+(43) F(1) leaf] \(180) F(1) leaf a;
//a --> [+(43)  leaf] \(180) [+(43)  leaf] \(180) f(1) leaf a;
//leaf --> [b] \(180) [b];
//b --> TP [ +(112) f(1) TP ] f(1) TP b;
//f(len) --> f(len+1);
//i(x) --> !h(-12) F(x);
//
