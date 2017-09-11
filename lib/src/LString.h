#ifndef LSTRING_H 
#define LSTRING_H

#include "Defines.h"
#include <stdbool.h>

typedef struct LString LString; 
typedef union LStringData {

  // if the module is storing parameters 
  float        lsdFValue; // this will access float values params
  bool         lsdBValue; // ditto for bools

  // if it is a module reference
  // store where params from the string should be placed
  IParamId     lsdCtxParamOffset;
  // and a unique tag not exceeding the size of the string 
  unsigned int lsdCtxTag;         
 
  // for all strings (they can all contain branches)
  union LStringData* lsdMatchingBranchAddr; // if the module is a branch point 

  // These occupy the spaces in between each module
  struct {
    IModType prevMt; 
    IModType nextMt; 
  } lsdDelim;

  // for when we do not know what the data type is
  // (everything should be a word anyway)
  void* lsdValue;

} LStringData;
// The position is always in-between two modules of the string
typedef struct LStringPosition {
  LStringData* lspPos;
  LString* lspStr;
} LStringPosition;
#define DEFAULT_LSTRING_POSITION (LStringPosition) { NULL, NULL}

// We need to pass in a function "f";
// it maps each module type 
// to the number of data values it has.
// the second parameter to emptyLString will be passed to "f" 
// in case a closure is needed
LString* ls_emptyLString(unsigned int (*paramCounts)(IModType, void*), void*);
LString* ls_blankLString(LString* str);
void     ls_freeLString(LString* str);

unsigned int ls_numParams(LString* str, IModType mt);
LString* lsp_lstring(LStringPosition lsp);

bool ls_append(LString* str, IModType mt, LStringData* data);

IModType lsp_mt(LStringPosition lsp);

LStringPosition lsp_head(LString* str);
LStringPosition lsp_tail(LString* str);

LStringPosition lsp_moveL(LStringPosition lsp);
LStringPosition lsp_moveR(LStringPosition lsp);

LStringPosition lsp_skipBranchL(LStringPosition lsp);
LStringPosition lsp_skipBranchR(LStringPosition lsp);

// Each member of the tree has one parent
LStringPosition lsp_parentOf(LStringPosition lsp);
// Children are distinguished by lexical (left-right) ordering
LStringPosition lsp_childIOf(LStringPosition lsp, unsigned int child_i);

// will skip over branches, otherwise identical to moveR 
LStringPosition lsp_nextChild(LStringPosition lsp);

// Convenience
bool lsp_isAtStart(LStringPosition lsp);
bool lsp_isAtEnd(LStringPosition lsp);

unsigned int ls_bytesLString(LString*str);

#endif
