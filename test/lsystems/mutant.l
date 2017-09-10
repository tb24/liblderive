derivation length: 8;
axiom: !(90.00, 0.34, 1.00) #(0.25) a ;
productions: 
a  --> i(0.93) [ -(25.76) i(0.89) a ] /(122.43) i(1.22) a ;
i(len) >> a  --> ;
i(len)  --> i((len + 0.86)) ;
interpretations: 
i(len)  --> F(len) ;
a  --> o(1.00) ;

