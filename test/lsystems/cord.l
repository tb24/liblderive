derivation length: 2;
depth: 8;

axiom: !(120, 1, 1) x ;

productions:
x --> i(1) [ !h(-12) +(32) i(6) x ]  \(180) i(10) x;
i(len) --> i(len*1.51);

interpretations:
i(len) --> F(len); 
x --> { [a] /(180) [a] };
a --> !h(2) TP  f(1) [ +(12) c TP ]  TP f(4.53) a ;
c --> f(1) -(5.7) c;
