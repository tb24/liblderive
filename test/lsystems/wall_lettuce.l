derivation length: 100;

depth: 10;

initdelay  = 70;
delay      =  2;

ACRO_THRESH = 60;
BASI_THRESH = 60;
ACRO_INC    = 0;
BASI_INC    = 0;
ACRO_INIT   = 10;
BASI_INIT   = 10;
ACRO_DIFFUSE = 0.71;
BASI_DIFFUSE = 0.71;

axiom: !(100, 0.82, 0.44) R(initdelay) A(delay);

ignore: / F + !;

productions:

I(a,b) << A(x) : a > ACRO_THRESH --> I(a,BASI_INIT) [ tris(0) ];
          A(x) : x > 0 --> A(x-1);
          A(x)         --> /(137.5) [ !(355, 1, 1) +(45) F(1) B ] I(ACRO_INIT,BASI_INIT) A(delay);
          
R(x) : x > 0  --> R(x-1);
R(x)          --> I(ACRO_INIT, 0); 

I(al,bl) << I(a,b) >> I(ar,br) --> I(a + (a*ACRO_INC)+ al*ACRO_DIFFUSE, b + BASI_INC + br*BASI_DIFFUSE);
I(al,bl) << I(a,b)             --> I(a + (a*ACRO_INC) + al*ACRO_DIFFUSE, b + BASI_INC                  );
            I(a,b) >> I(ar,br) --> I(a + (a*ACRO_INC)                  , b + BASI_INC + br*BASI_DIFFUSE);

I(a,b) << B : b > BASI_THRESH --> R(b) A(delay);

tris(r) --> tris(r+1);


interpretations:

I(a,b) --> !( 75+a-b, 0.5, 1) F(1);


tris(r) --> !h(r*6) a; 

a --> [b] /(180) [b];
b --> [-(9) b {  TP ] TP c TP }; 
c --> f(0.4) c;
