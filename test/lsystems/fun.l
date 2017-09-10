derivation length: 6;
depth: 12;
axiom: !(90.00, 0.34, 1.00) #(0.25) a ;
productions: 
F(P1) F(P3) << a >> ]  --> [ [ ] -(22.54) a ] /(150.42) a a a ;
F(P1) F(P3) << a >> ] /(P2)  --> [ -(23.46) a ] /(151.12) a a a ;
F(P1) << a >> ] /(P2)  --> [ -(27.67) a ] /(138.65) a a ;
F(P1) F(P2) << a  --> [ -(28.56) a [ ] a ] /(228.68) a ;
F(P1) << a  --> [ -(40.72) a a ] /(245.18) a ;
F(P1) << a >> ]  --> [ -(30.70) a ] /(162.05) a ;
F(P1) << a  --> [ -(8.91) F(1.69) a ] /(138.63) a ;
/(P1) << a  --> F(3.48) F(5.45) F(4.13) a ;
a  --> F(6.16) F(2.93) F(5.52) F(4.93) a ;
a >> ]  --> F(3.90) [ F(4.86) a ] F(3.01) F(5.46) F(3.67) a ;
a >> ]  --> [ F(4.41) a ] F(2.74) F(2.96) F(2.98) a ;
-(P1) << a >> ]  --> [ F(5.32) a ] [ ] F(4.56) F(7.04) F(4.90) a ;
a >> ]  --> [ F(4.82) a ] [ ] F(4.87) F(4.87) a ;
a  --> [ F(3.87) a ] /(127.34) [ ] F(7.12) F(3.28) a ;
a >> ]  --> [ F(4.11) a ] /(115.04) [ ] F(4.62) a ;
a  --> [ F(4.78) a ] /(98.01) F(4.29) a ;
a  --> [ -(15.53) F(8.96) a ] /(169.92) F(2.63) a ;
a  --> F(0.25) [ -(9.26) F(2.64) a ] /(109.23) F(2.18) a ;
interpretations: 
/(P2) ] << F(x)  --> !h((-0.03 - 0.50)) !h((-0.03 - 0.50)) F(x) F(x) ;
/(P2) << F(x)  --> !h((-0.03 - 0.57)) F(x) F(x) ;
F(x) >> F(P2)  --> F(x) ;
F(x) >> F(P2) /(P3)  --> !h((-0.02 - 0.25)) F(x) ;
F(x) >> F(P2)  --> !h((-0.02 - 0.33)) !h((-0.02 - 0.31)) F(x) ;
F(x)  --> F(x) F(x) F(x) ;
F(x) >> a  --> F(x) F(x) ;
F(x)  --> F(x) ;
F(x)  --> !h((-0.01 - 0.27)) F(x) ;
a  --> [ b ] /(232.58) b [ b /(174.36) ] ;
a  --> [ b ] /(292.36) [ b /(134.27) ] ;
[ << b >> /(P1) F(P2)  --> b !h(2.49) [ +(31.68) b !h(3.24) { TP ] TP c } ;
b >> /(P1) F(P2)  --> b !h(1.98) [ +(36.14) b { } !h(2.94) { TP ] TP c } ;
b >> /(P1)  --> b !h(3.02) [ +(23.26) b !h(3.89) { TP ] TP c } ;
b >> /(P1)  --> !h(7.83) [ +(22.54) b !h(2.70) { TP ] TP c } ;
b >> /(P1)  --> !h(8.33) [ +(16.77) b { TP ] TP c } ;
[ << b >> !h(P1)  --> !h(6.74) [ b c { TP TP ] TP c TP +(35.98) } ;
+(P2) << b >> !h(P1)  --> !h(7.84) [ +(23.45) b { TP TP ] TP c TP +(39.70) } ;
b >> !h(P1)  --> !h(5.73) [ +(31.27) b c { TP TP ] TP c TP +(47.16) } ;
b  --> !h(7.72) [ +(32.09) b c { TP ] TP c TP +(35.20) } ;
b  --> !h(13.89) [ +(68.83) b c { TP ] TP c TP } ;
b  --> !h(8.70) [ +(45.60) b { TP ] TP c TP } ;
b << c >> TP  --> c c f(0.32) c ;
c >> TP  --> c f(0.20) c ;
TP << c >> f(P2)  --> f(1.06) f(0.19) f(0.24) ;
c >> f(P2)  --> f(1.11) f(0.19) ;
f(P1) << c >> f(P2)  --> f(0.75) f(1.02) f(0.35) ;
f(P1) << c  --> f(0.79) f(0.30) ;
f(P1) << c  --> f(0.42) f(0.16) c ;
c  --> ;
b << c  --> ;
b << c  --> ;
b << c  --> ;
TP << c  --> ;
c  --> ;
c  --> c ;
c  --> f(0.23) c ;