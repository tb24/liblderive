#ifndef LSTIRNG_C
#define LSTRING_C

#include "LString.h"
#include "Defines.h"
#include "ILSystem.h"
#include "Stack.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

IModType lsp_nextMt(LStringPosition lsp);
IModType lsp_prevMt(LStringPosition lsp);

// This one is only used here
LStringData* ls_lsdMatchingBranchAddr(LStringPosition lsp);


struct LString {

  LStringData*  lsTail; 
  unsigned int  lsBufSize; 
  unsigned int  lsLength;
  LStringData*  lsData;

  // Each module type has some number of parameters,
  // this function should return that
  unsigned int (*lsPc)(IModType, void*);
  void* lsPcArg;
  // it will be passed this argument

  // The top of this stack should hold the string position 
  // of the start of the current branch
  Stack* branchPointStack;

};

// ******************** 
//  LOCAL FUNCTIONS     
// ******************** 

LString* ls_resizeLString(LString* str);

// ************************** 
//    BEGIN IMPLEMENTATION    
// ************************** 

unsigned int lsp_numParams(LStringPosition lsp)
{
  return lsp.lspStr->lsPc(lsp_prevMt(lsp), lsp.lspStr->lsPcArg);
}


unsigned int ls_bytesLString(LString*str)
{
  return (str->lsTail-str->lsData);
}

LString* ls_emptyLString(unsigned int (*pc)(IModType, void*), void* pcArg)
{
  
  LString* str = NULL;
  
  if( ! (str = malloc(sizeof *str) ) )
    return NULL;

  if(!(str->lsData = 
        malloc(LSTRING_BUFFER_INCREMENT * 
        sizeof (*str->lsData)) 
      )
    ) return free(str), NULL;

  if(!(str->branchPointStack = stack_make()))
    return ls_freeLString(str), NULL;

  str->lsTail = str->lsData;
  str->lsBufSize = LSTRING_BUFFER_INCREMENT;
  str->lsTail->lsdDelim.prevMt = mtStartString;
  str->lsTail->lsdDelim.nextMt = mtEndString;
  str->lsPc = pc;
  str->lsPcArg = pcArg;

  return str;

}

LString* ls_blankLString(LString* str)
{
  return ls_emptyLString(str->lsPc, str->lsPcArg);
}

void ls_freeLString(LString* str)
{
  if(str==NULL) return;
  if(str->lsData != NULL) free(str->lsData);
  str->lsData = NULL;
  if(str->branchPointStack != NULL) stack_free(str->branchPointStack);
  free(str);
}

LString* ls_resizeLString(LString* str)
{
  LStringData * newData;
  unsigned int newBufSize = str->lsBufSize + LSTRING_BUFFER_INCREMENT;

  if( !(newData = realloc(str->lsData, newBufSize*sizeof*str->lsData) ) )
    return ls_freeLString(str), NULL;
  str->lsTail = str->lsTail - str->lsData + newData;

  str->lsData = newData;
  str->lsBufSize = newBufSize;
  return str;
}

// Appends to the end of the string.
// Returns false if allocation fails
bool ls_append(LString* str, IModType mt, LStringData* data)
{
  // ensure room for module data; and delimiters (+4)
  if( ( str->lsBufSize < 
        ls_bytesLString(str) + str->lsPc(mt,str->lsPcArg) + 4 
      ) &&
      (! ls_resizeLString(str) )
    )  return false;

  str->lsTail->lsdDelim.nextMt = mt;
 
  VERB( fprintf(stderr, "append mt:  %d\n", str->lsTail->lsdDelim.nextMt ););

  // handle branches
  // if this is a start branch, push it's location onto the stack
  if( str->lsTail->lsdDelim.nextMt == mtPush )
  {
    StackData sd;
    sd.sdData = (void*)str->lsTail;
    DBG( fprintf(stderr, "push: %x", sd.sdData); )
    if(!stack_push(str->branchPointStack,  sd ) )
      return false;
  }

  // if it's an end branch, store the positions of both branch points 
  // after the last parameter of each branch module
  if(str->lsTail->lsdDelim.nextMt == mtPop )
  {
    StackData sd;
    if( !stack_pop(str->branchPointStack, &sd) )
      return false;
    DBG( fprintf(stderr, "pop:  %x", sd.sdData); )
    LStringData* startBranchPos = (LStringData*)sd.sdData;
    (startBranchPos + str->lsPc(mt, (void*)str->lsPcArg) + 1)->lsdMatchingBranchAddr = str->lsTail;
    (str->lsTail    + str->lsPc(mt, (void*)str->lsPcArg) + 1)->lsdMatchingBranchAddr = startBranchPos;
  }

  // update data 
  for(unsigned int i=0; i < str->lsPc(mt, str->lsPcArg); i++)
    (str->lsTail + i + 1)->lsdValue = data[i].lsdValue;

  // and get ready for the next module
  str->lsTail += 1 + str->lsPc(mt, str->lsPcArg) + (mt == mtPush || mt == mtPop ? 1:0); 
  str->lsTail->lsdDelim.prevMt = mt;
  str->lsTail->lsdDelim.nextMt = mtEndString;

  return true;

}

IModType lsp_mt(LStringPosition lsp)
{
  return lsp.lspPos->lsdDelim.nextMt;
}


LStringPosition lsp_head(LString* str)
{
  return (LStringPosition) { str->lsData, str };
}

LStringPosition lsp_moveL(LStringPosition lsp)
{
  if( lsp_prevMt(lsp) == mtStartString ) 
    return lsp; 
  
  LStringPosition newPos;

  newPos.lspStr = lsp.lspStr;
  newPos.lspPos = lsp.lspPos -
    (1 + lsp.lspStr->lsPc(lsp_prevMt(lsp), lsp.lspStr->lsPcArg) +
     (lsp_prevMt(lsp) == mtPush || lsp_prevMt(lsp) == mtPop ? 1:0)
    );

  return newPos;

}

LStringPosition lsp_moveR(LStringPosition lsp)
{
  if( lsp_nextMt(lsp) == mtEndString) 
    return lsp; 

  LStringPosition newPos;

  newPos.lspStr = lsp.lspStr;
  newPos.lspPos = lsp.lspPos + 
    (1 + lsp.lspStr->lsPc(lsp_nextMt(lsp), lsp.lspStr->lsPcArg) +
     (lsp_nextMt(lsp) == mtPush || lsp_nextMt(lsp) == mtPop ? 1:0)
    );

  return newPos;
}

LStringPosition lsp_parentOf(LStringPosition lsp)
{
  lsp = lsp_moveL(lsp);
  while(  lsp_mt(lsp) == mtPop )
  {
    lsp.lspPos = 
      lsp.lspPos[ 1 +  
      lsp.lspStr->lsPc(mtPop, (void*)lsp.lspStr->lsPcArg)
      ].lsdMatchingBranchAddr;
    lsp = lsp_moveL(lsp);
  }

  return lsp; 

}

// will skip over branch contents
LStringPosition lsp_nextChild(LStringPosition lsp)
{
  if(lsp_mt(lsp) == mtPush)
  {
    lsp.lspPos =  
        lsp.lspPos[ 1 +  
          lsp.lspStr->lsPc(mtPush, (void*)lsp.lspStr->lsPcArg)
        ].lsdMatchingBranchAddr;
  }

  return lsp_moveR(lsp);
}

IModType lsp_nextMt(LStringPosition lsp)
{
  return lsp.lspPos->lsdDelim.nextMt;
}

IModType lsp_prevMt(LStringPosition lsp)
{
  return lsp.lspPos->lsdDelim.prevMt;
}

bool lsp_isAtStart(LStringPosition lsp)
{
  return ( lsp_prevMt(lsp) == mtStartString );
}

bool lsp_isAtEnd(LStringPosition lsp)
{
  return ( lsp_nextMt(lsp) == mtEndString);
}

LStringPosition lsp_tail(LString* str)
{
  return (LStringPosition) { str->lsTail, str };
}

// A bit dangerous
float* ls_params(LStringPosition lsp)
{
  return &lsp.lspPos[1].lsdFValue;
}

/*
LStringData* ls_lsdMatchingBranchAddr(LStringPosition lsp)
{
  return lsp.lspPos[3].lsdMatchingBranchAddr;
}
*/

unsigned int ls_numParams(LString* str, IModType mt)
{
  return str->lsPc(mt, str->lsPcArg);
}

LString* lsp_lstring(LStringPosition lsp)
{
  return lsp.lspStr;
}

bool lsp_null(LStringPosition lsp)
{
  return lsp.lspPos == NULL || lsp.lspStr == NULL;
}
#endif
