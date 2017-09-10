axiom: a
{

loop: 8 { P [I DRAW] } 

P:>

a  : 1 --> [ -(30.00) i(1.00) a ] [ +(30.00) i(1.00) a ] \(137.00) i(1.00) a ;
i(len)  : 1 --> i((len + 1.00)) ;

I:200>

i(len)  : 1 --> F((len + 1.00));
a(len)  : 1 --> @(1.00);

DRAW:<

fold:
{
 f(x): t --> t' = tForward(t); line(tPos(t), tPos(t')); t';
 [   : t --> tPush(t);
 ]   : t --> tPop;

}

}
