#ifndef LSMODULES_H
#define LSMODULES_H

#include "Defines.h"
#include "ILSystem.h"

// Creating Strings

LString* lsm_empty  (ModuleTypeInfo mti);
void     lsm_free   (LString* str);
bool     lsm_append (LString* str, IModType mt, float* params);

// Data Access
float  lsm_paramI (LStringPosition lsp, unsigned int i);
float* lsm_params (LStringPosition lsp);
                  
#endif
