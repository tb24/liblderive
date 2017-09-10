#ifndef LSMODULES_C
#define LSMODULES_C

#include "Defines.h"
#include "LString.h"
#include "ILSystem.h"

unsigned int mtFromParams(IModType mt, unsigned int* paramCounts);
// Creating Strings
LString* lsm_empty  (ModuleTypeInfo mti)
{
  return ls_emptyLString((unsigned int (*)(IModType, void*))&mtFromParams, (void*)mti);
}

void     lsm_free   (LString* str)
{
  ls_freeLString(str);
}
bool     lsm_append (LString* str, IModType mt, float* params)
{
  return ls_append(str, mt, (LStringData*)params);
}

// Data Access
float  lsm_paramI (LStringPosition lsp, unsigned int i)
{
  return lsp.lspPos[i+1].lsdFValue;
}

float* lsm_params (LStringPosition lsp)
{
  return &lsp.lspPos[1].lsdFValue;
}

unsigned int mtFromParams(IModType mt, unsigned int* paramCounts)
{
  return paramCounts[mt];
}

#endif
