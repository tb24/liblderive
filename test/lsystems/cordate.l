derivation length: 5;
depth: 35;

axiom: !(180, 1, 1) x ;

productions:
x --> i(1) [ !h(22) +(42) i(6) x ]  \(180) i(10) x;
i(len) --> i(len*1.141);

interpretations:
i(len) --> F(len);
x --> [a] /(180) [a];
a --> [+(6) a {  TP ] TP c TP }; 
b --> [-(6) b {  TP ] TP c TP }; 
c --> f(1) c;
