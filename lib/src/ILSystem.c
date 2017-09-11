#ifndef ILSYSTEM_C
#define ILSYSTEM_C

#include "Defines.h"
#include "Common.h"
#include "ILSystem.h"

#include <stdio.h>

//
// Builders for ILSystem Datatypes
// 
// Pass a reference to the node as the last parameter
//
// No copies of the other incoming parameters are made.
// So, after you call freeILSystem, don't expect to be able 
// to use anything you have passed in.

bool makeIModTypes(unsigned int length, IModType** imts)
{
  IModType* imtsTmp = NULL;
  if( length > 0 )
  {
    if( !(imtsTmp = malloc(length * sizeof *imtsTmp)) )
      return false;
    for(unsigned int i=0; i < length; i++)
      imtsTmp[i] = GARBAGE_IMODTYPE;
  }
  *imts = imtsTmp;
  return true;
}
void freeIModTypes(unsigned int length, IModType* imts)
{
  free(imts);
}

bool makeIModRefs(unsigned int length, IModRef** imrs)
{
  IModRef* imrsTmp = NULL;

  if( length > 0)
  {
    if( !(imrsTmp = malloc(length * sizeof *imrsTmp) ) )
      return false;
    for(unsigned int i=0; i < length; i++)
      imrsTmp[i] = DEFAULT_MODREF;
  }

  *imrs = imrsTmp;
  return true;
}
void freeIModRefs(unsigned int length, IModRef* imrs)
{
  if(length==0) return;

  for(unsigned int i=0; i < length; i++)
    freeIModRef(imrs[i]);
  free(imrs);
}

bool makeIModRef(IModType imt, IParamId ipis, IModRef* imr)
{
  (*imr).imrType = imt;
  (*imr).imrParamsOffset = ipis;
  return true;
}

void freeIModRef(IModRef imr)
{
// Everything is a float; we don't need to know the 
// offset of each parameter if we know how many
// there are
//  free(imr.imrParamsOffset);
}

bool makeIVarDecs(unsigned int length, IVarDec** ivds)
{
  IVarDec* ivdecs = NULL;

  if( length > 0 )
  {
    if( !(ivdecs = malloc(length * sizeof *ivdecs) ) )
      return false;
    for(unsigned int i=0; i < length; i++)
      ivdecs[i] = DEFAULT_IVARDEC;
  }

  *ivds = ivdecs;
  return true;
}

void freeIVarDecs(unsigned int length, IVarDec* ivds)
{
  if( ivds == NULL ) return;
  for(unsigned int i=0; i < length; i++)
    freeIVarDec(ivds[i]);
  free(ivds);
}

bool makeIVarDec(IValExp ive, unsigned int offset, IVarDec* ivd)
{
  (*ivd).ivdValExp = ive;
  (*ivd).ivdOffset = offset;
  return true;
}

void freeIVarDec(IVarDec ivd)
{
  freeIValExp(ivd.ivdValExp);
}

bool makeIValExps(unsigned int length, IValExp** ives)
{
  IValExp* ivies = NULL;

  if( length > 0 )
  {
    if( !(ivies = malloc(length * sizeof *ivies) ) )
      return false;
    for(unsigned int i=0; i < length; i++)
      ivies[i] = DEFAULT_IVAL;
  }

  *ives = ivies;
  return true;
}

void freeIValExps(unsigned int length, IValExp* ives)
{
  for(unsigned int i=0; i < length; i++)
    freeIValExp(ives[i]);
  free(ives);
}

bool makeIValExpBinary(ops_t iveTy, IValExp e1, IValExp e2, IValExp* ive)
{
  if( ! isBinaryOp(iveTy) ) return false;

  ive->iveType = iveTy;

  if( ! (ive->ivePayload.iveArgs = malloc(2 * sizeof(IValExp)) ) )
    return false;

  ive->ivePayload.iveArgs[0] = e1; 
  ive->ivePayload.iveArgs[1] = e2;

  return true;
}
bool makeIValExpFloat(float value, IValExp* ive)
{
  (*ive).iveType = hOpFval;
  (*ive).ivePayload.iveFVal = value;
  return true;
}

bool makeIValExpBool(bool value, IValExp* ive)
{
  (*ive).iveType = hOpBval;
  (*ive).ivePayload.iveBVal = value;
  return true;
}

void freeIValExp(IValExp ive)
{
  if( isBinaryOp(ive.iveType) )
  {
      if( ive.ivePayload.iveArgs != NULL )
      {
        freeIValExp(ive.ivePayload.iveArgs[0]);
        freeIValExp(ive.ivePayload.iveArgs[1]);
        free(ive.ivePayload.iveArgs);
      }
  }
}

bool makeIModExps(unsigned int length, IModExp** imes)
{
  IModExp* imies = NULL;

  if(! ( length > 0) )
    return false;

  if( !(imies = malloc(length * sizeof *imies) ) )
    return false;

  for(unsigned int i=0; i < length; i++)
    imies[i] = DEFAULT_MODEXP;

  *imes = imies;
  return true;
}

void freeIModExps(unsigned int length, IModExp* imes)
{
  if(!imes) return;
  for(unsigned int i=0; i < length; i++)
    freeIModExp(imes[i]);
    
  free(imes);
}


bool makeIModExp(IModType imt, IValExp* ives, unsigned int ivesLen, IModExp* ime)
{
  (*ime).imeType = imt;
  (*ime).imeVals = ives;
  (*ime).imeNumVals = ivesLen;
  return true;
}

void freeIModExp(IModExp ime){
  for(unsigned int i=0; i < ime.imeNumVals; i++)
    freeIValExp(ime.imeVals[i]);
  if( ime.imeVals ) free(ime.imeVals);
}

bool makeILRules(unsigned int length, ILRule** ilrs)
{
  ILRule* ilrsTmp = NULL;

  if(! ( length > 0) )
    return false;

  if( !(ilrsTmp = malloc(length * sizeof *ilrsTmp) ) )
    return false;

  for(unsigned int i=0; i < length; i++)
    ilrsTmp[i] = DEFAULT_ILRULE;

  *ilrs = ilrsTmp;
  return true;
}

void freeILRules(unsigned int num, ILRule* ilrs)
{
  for(unsigned int i=0; i < num; i++)
    freeILRule(ilrs[i]);
  free(ilrs);
}

void freeILRule(ILRule ilr)
{
  if(ilr.ilrContext) 
    ls_freeLString(ilr.ilrContext);

  freeIVarDecs(ilr.ilrLengthVarDecs, ilr.ilrVardecs);
  freeIValExp(ilr.ilrCondition);
  freeIModExps(ilr.ilrLengthProd, ilr.ilrProduction);

  if(ilr.ilrLCNeighbors) free( ilr.ilrLCNeighbors );
  if(ilr.ilrRCNeighbors) free( ilr.ilrRCNeighbors );
}

ILSystem* emptyILSystem()
{
  ILSystem* ils = malloc(sizeof *ils);
  ils->ilsLengthAxiom         = 0;
  ils->ilsNumProductions      = 0;
  ils->ilsNumInterpretations  = 0;
  ils->ilsNumDecompositions   = 0;
  ils->ilsNumIgnored          = 0;
  ils->ilsNumConsidered       = 0;

  ils->ilsAxiom               = NULL;
  ils->ilsProductions         = NULL;
  ils->ilsDecompositions      = NULL;
  ils->ilsInterpretations     = NULL;
  ils->ilsIgnoredModules      = NULL;
  ils->ilsConsideredModules   = NULL;
                             
  ils->ilsRandomSeed          = 42;
  ils->ilsDerivationLength    = 0;
  ils->ilsDerivationDepth     = 1;
                             
  ils->ilsGlobals             = NULL;
  ils->ilsNumGlobals          = 0;
                             
  ils->ilsNumModules            = 0;
  ils->ilsModuleTypeParamCounts = NULL;
  ils->ilsModuleNames           = NULL;

  return ils;

}


void freeILSystem(ILSystem* ils)
{

  if(ils == NULL) return;

  freeIModExps(ils->ilsLengthAxiom, ils->ilsAxiom);
  
  freeIVarDecs(ils->ilsNumGlobals, ils->ilsGlobals);
  
  freeILRules(ils->ilsNumProductions, ils->ilsProductions);
  freeILRules(ils->ilsNumDecompositions, ils->ilsDecompositions);
  freeILRules(ils->ilsNumInterpretations, ils->ilsInterpretations);

  
  free(ils->ilsIgnoredModules);
  free(ils->ilsConsideredModules);

  free(ils->ilsModuleTypeParamCounts);
  for(unsigned int i=0; i < ils->ilsNumModules; i++)
    free(ils->ilsModuleNames[i]);
  free(ils->ilsModuleNames);

  free(ils);

}


#endif
