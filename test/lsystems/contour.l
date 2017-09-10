derivation length: 5;
depth: 3;

axiom: !(100, 1, 1) strip;

productions:

strip --> [{ a(1.12) }] /(180) [{ a(1.12) }];
a(x) --> i(1) [ +(18) i(x+1.15) TP ] TP i(1) TP a(x+1) ;
i(len) --> i(len*1.12);

interpretations: 

i(len) --> f(len) !h(12) ;
