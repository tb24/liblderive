#ifndef LSCONTEXT_C
#define LSCONTEXT_C

#include <stdio.h>
#include "Defines.h"
#include "LString.h"

typedef enum LSConParameters
{
  lscpParamOffset,
  lscpCtxTag,
  NUM_CTX_PARAMS
} LSConParameters;

unsigned int paramcounts(IModType mt, void* pcArg)
{
  return NUM_CTX_PARAMS;
}

// Creating Strings
LString* lsc_empty  ()
{
  return ls_emptyLString(&paramcounts, NULL);
}
void     lsc_free   (LString* str)
{
  ls_freeLString(str);
}

bool     lsc_append (LString* str, IModType mt, IParamId po, unsigned int tag)
{
  LStringData data[NUM_CTX_PARAMS];
  data[lscpParamOffset].lsdCtxParamOffset = po;
  data[lscpCtxTag].lsdCtxTag = tag;
  return ls_append(str, mt, data);
}

// Data Access
IParamId  lsc_paramOffset(LStringPosition lsp)
{
  return lsp.lspPos[1 + lscpParamOffset].lsdCtxParamOffset;
}

unsigned int lsc_tag(LStringPosition lsp)
{
  return lsp.lspPos[1 + lscpCtxTag].lsdCtxTag;
}

#endif
