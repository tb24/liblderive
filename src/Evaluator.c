#ifndef EVALUATOR_C
#define EVALUATOR_C

#define FLOAT_EQ_MAX 0.0001

#include "Evaluator.h"
#include "ILSystem.h"
#include "HLSystem.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

// Evaluate a floating point expression, given the parameters in scope 
float evalF(Bindings bindings, IValExp ive)
{
  switch(ive.iveType)
  {
    case hOpAdd:
      return (
        evalF(bindings, ive.ivePayload.iveArgs[0]) + 
        evalF(bindings, ive.ivePayload.iveArgs[1])
      );
    case hOpSub:
      return (
        evalF(bindings, ive.ivePayload.iveArgs[0]) -
        evalF(bindings, ive.ivePayload.iveArgs[1])
      );
    case hOpMul:
      return (
        evalF(bindings, ive.ivePayload.iveArgs[0]) * 
        evalF(bindings, ive.ivePayload.iveArgs[1])
      );
    case hOpDiv:
      return (
        evalF(bindings, ive.ivePayload.iveArgs[0]) /
        evalF(bindings, ive.ivePayload.iveArgs[1])
      );
    case hOpPow:
      return (
        powf(
          evalF(bindings, ive.ivePayload.iveArgs[0]),
          evalF(bindings, ive.ivePayload.iveArgs[1])
        )
      );
    case hOpFval:
      return ive.ivePayload.iveFVal;
    case hOpId:
      return bindings[ive.ivePayload.iveVariableOffset];
    case hOpAnd:
    case hOpOr: 
    case hOpLt :
    case hOpLte:
    case hOpEq :
    case hOpNeq:
    case hOpGt :
    case hOpGte:
    case hOpBval:
    case hOpNone:
    // This indicates a bug in the semantic checking
      return NAN; 
  }
  return NAN;
}

bool evalB(Bindings bindings, IValExp ive)
{
  switch(ive.iveType)
  {
    case hOpAdd:
      return 0 < (
        evalF(bindings, ive.ivePayload.iveArgs[0]) + 
        evalF(bindings, ive.ivePayload.iveArgs[1])
      );
    case hOpSub:
      return 0 < (
        evalF(bindings, ive.ivePayload.iveArgs[0]) -
        evalF(bindings, ive.ivePayload.iveArgs[1])
      );
    case hOpMul:
      return 0 < (
        evalF(bindings, ive.ivePayload.iveArgs[0]) * 
        evalF(bindings, ive.ivePayload.iveArgs[1])
      );
    case hOpDiv:
      return 0 < (
        evalF(bindings, ive.ivePayload.iveArgs[0]) /
        evalF(bindings, ive.ivePayload.iveArgs[1])
      );
    case hOpPow:
      return 0 < (
        powf(
          evalF(bindings, ive.ivePayload.iveArgs[0]),
          evalF(bindings, ive.ivePayload.iveArgs[1])
        )
      );
    case hOpFval:
      return 0 < ive.ivePayload.iveFVal;
    case hOpId:
      return 0 < bindings[ive.ivePayload.iveVariableOffset];
    case hOpAnd:
      return (
        evalB(bindings, ive.ivePayload.iveArgs[0]) && 
        evalB(bindings, ive.ivePayload.iveArgs[1]) 
      );
    case hOpOr: 
      return (
        evalB(bindings, ive.ivePayload.iveArgs[0]) || 
        evalB(bindings, ive.ivePayload.iveArgs[1]) 
      );
    case hOpLt :
      return (
        evalF(bindings, ive.ivePayload.iveArgs[0]) < 
        evalF(bindings, ive.ivePayload.iveArgs[1]) 
      );
    case hOpLte:
      return (
        evalF(bindings, ive.ivePayload.iveArgs[0]) <=
        evalF(bindings, ive.ivePayload.iveArgs[1]) 
      );
    case hOpEq :
      return ( FLOAT_EQ_MAX > fabs(
        evalF(bindings, ive.ivePayload.iveArgs[0]) -
        evalF(bindings, ive.ivePayload.iveArgs[1]) )
      );
    case hOpNeq:
      return ( FLOAT_EQ_MAX < fabs(
        evalF(bindings, ive.ivePayload.iveArgs[0]) -
        evalF(bindings, ive.ivePayload.iveArgs[1]) )
      );
    case hOpGt :
      return (
        evalF(bindings, ive.ivePayload.iveArgs[0]) >  
        evalF(bindings, ive.ivePayload.iveArgs[1]) 
      );
    case hOpGte:
      return (
        evalF(bindings, ive.ivePayload.iveArgs[0]) >= 
        evalF(bindings, ive.ivePayload.iveArgs[1]) 
      );
    case hOpBval:
      return ive.ivePayload.iveBVal;
    case hOpNone:
      return true; 
  }
  return false;
}

#endif
