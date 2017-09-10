derivation length: 2;
depth: 17;

axiom: !(180, 1, 1) x ;

productions:
x --> i(1) [ !h(12) +(42) i(6) x ]  \(180) i(10) x;
i(len) --> i(len*1.21);

interpretations:
i(len) --> F(len);
x --> TP { [a] }  /(180){ [a]  } TP ;
f(x) << a --> !h(2) TP  f(1)  TP f(4.53) a;
a --> !h(2) TP  f(1) [ !h(12) +(90) c TP ] TP f(4.53) a;
c --> f(2.82) +(5.7) c;
