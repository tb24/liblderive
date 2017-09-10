derivation length: 5;
depth: 8;

axiom: !(100, 1, 1) strip;

productions:

strip --> [{ a(1.12) }] /(180) [{ a(1.12) }];
a(x) --> i(1) TP [ +(48) i(x*1.75) TP -(31) i(x*1) TP -(31) i(x*1) TP  ] TP  i(x*2) TP a(x+1) ;
i(len) --> i(len*1.12);

interpretations: 

i(len) --> f(len) !h(-52) ;




