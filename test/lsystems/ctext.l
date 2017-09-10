derivation length: 7;

ignore: + - F;

axiom: F(1) b F(1) a F(1) b F(1) b;

productions:

b << a >> b --> b;
a >> b b --> b;

a << b >> b --> b [ +(20) F(1) b F(1) b ];
b << b >> b --> b [ -(20) F(1) b F(1) b ];

