#ifndef DEFINES_C
#define DEFINES_C

#include "Defines.h"

bool isBinaryOp(ops_t op)
{
  switch(op)
  {
    case hOpAnd:
    case hOpOr :
    case hOpLt :
    case hOpLte:
    case hOpEq :
    case hOpNeq:
    case hOpGt :
    case hOpGte:
    case hOpAdd:
    case hOpSub:
    case hOpMul:
    case hOpDiv:
    case hOpPow:
      return true;
    
    default:
      return false;
  }

}

#endif
