#ifndef LENGINE_C
#define LENGINE_C

#define LE_MAX_STR_LEN 124288

#include "Defines.h"
#include "LEngine.h"
#include "LSystem.h"
#include "LString.h"
#include "LSContext.h"
#include "LSModules.h"
#include "ILSystem.h"
#include "Evaluator.h"
#include "LRuleNeighbors.h"

#include <stdbool.h>
#include <string.h>
#include <stdio.h>

struct CLSYS_LEngine
{
  // Internal representation of the lsystem
  ILSystem*      leIls;

  // Derivation State
  LString*       leCurrStr;
  unsigned int*  leHaltFlag;
  unsigned int   leStepsTaken;

  // Holds parameter values used in rule application
  float*         leParamBuffer;
  unsigned int   leParamBufferSize;

  // Stores those types of modules 
  // that could be appended to the context
  // for each group of productions 
  LRuleNeigbors* leDeriveNeigbors;
  LRuleNeigbors* leDecompNeigbors;
  LRuleNeigbors* leInterpNeigbors;

  // Stores whether a given rule was fired or not 
  // during derivation
  bool*          leIsActiveProduction;
  bool*          leIsActiveDecomposition;
  bool*          leIsActiveInterpretation;

  // The engine will not derive further
  // if the length of the string
  // grows greater than this value
  unsigned int leMaxStringLength;

};

bool matchRules(
    LRuleNeigbors*  lrns,
    ILRule*         rules, 
    unsigned int numRules, 
    LStringPosition   lsp, 
    LString*      currStr,
    LString*      nextStr,
    float*         params,
    bool*         ignored,
    bool**    activeRules 
    );

bool le_contextEq(
  LStringPosition ctxPos,
  LStringPosition    lsp,
  bool*      visited_ctx,
  float*          params,
  LRuleNeigbors*    lrns,
  bool*          ignored
);

bool le_contextMatches(
    LRuleNeigbors*    lrns,
    LStringPosition    lsp, 
    LString*          lstr, 
    ILRule*         lrules, 
    unsigned int   lrule_i, 
    bool*          ignored,
    float* params
    );

static void le_setVariables(
    ILRule*         lrules,
    float* varBinds
    );


static bool le_applyProduction(IModExp* imes, unsigned int imesLength, float* paramBindings, LString* lstr);
//static bool le_copyModule(LStringPosition, LString*, LString*);

bool le_initActiveRuleTable(bool ** activeRuleTable, unsigned int numRules);
void le_freeActiveRuleTable(bool * activeRuleTable);
static void le_initGlobals(unsigned int numGlobals, IVarDec* globals, float* varBinds);

// These functions provide information that can help choose more appropriate mutations  
bool le_isNeighbor(CLSYS_LEngine* le, bool** neighbors, unsigned int rule_i, char* moduleName);
bool le_isPLCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName);
bool le_isPRCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName);
bool le_isDLCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName);
bool le_isDRCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName);
bool le_isILCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName);
bool le_isIRCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName);
bool le_isProductionActive    (CLSYS_LEngine * le, unsigned int rule_i);
bool le_isDecompositionActive (CLSYS_LEngine * le, unsigned int rule_i);
bool le_isInterpretationActive(CLSYS_LEngine * le, unsigned int rule_i);

// ================== 
// BEGIN LENGINE CODE 
// ================== 

// Get the engine ready to derive
CLSYS_LEngine* le_initFromIls(LSysErrors* lerrs, ILSystem* ils)
{
  CLSYS_LEngine* le;

  if ( !( le = malloc(sizeof *le)) )
    return NULL;

  le->leIls = ils;
  le->leHaltFlag = NULL; 
  le->leStepsTaken = 0; 

  // create space for the variable bindings
  le->leParamBufferSize = le->leIls->ilsMaxNumVariables + le->leIls->ilsNumGlobals;
  if ( !( le->leParamBuffer = malloc(le->leParamBufferSize * sizeof *le->leParamBuffer) ) )
    return freeLEngine(le), NULL;

  // evaluate the global variables
  le_initGlobals(le->leIls->ilsNumGlobals, le->leIls->ilsGlobals, le->leParamBuffer);

  // build the axiom
  if( ! (le->leCurrStr = lsm_empty(le->leIls->ilsModuleTypeParamCounts) ) ||
      ! le_applyProduction(le->leIls->ilsAxiom, le->leIls->ilsLengthAxiom, le->leParamBuffer, le->leCurrStr)
    ) return freeLEngine(le), NULL;

  // Create the context neighbors data buffers
  // Assumes the number of rules won't change during derivation
  le->leDeriveNeigbors = NULL; 
  le->leDecompNeigbors = NULL; 
  le->leInterpNeigbors = NULL; 

  if( !lrn_reset(&le->leDeriveNeigbors, le->leIls->ilsNumProductions,     le->leIls->ilsNumModules) ||
      !lrn_reset(&le->leDecompNeigbors, le->leIls->ilsNumDecompositions,  le->leIls->ilsNumModules) ||
      !lrn_reset(&le->leInterpNeigbors, le->leIls->ilsNumInterpretations, le->leIls->ilsNumModules)
    ) return addGeneralError(
      lerrs, 
      lsys_error_memory_full, 
      "Could not create buffer to hold context neighbors" ), 
      lrn_free(le->leDeriveNeigbors, le->leIls->ilsNumProductions),
      lrn_free(le->leDecompNeigbors, le->leIls->ilsNumDecompositions),
      lrn_free(le->leInterpNeigbors, le->leIls->ilsNumInterpretations),
      NULL;

  le->leIsActiveProduction     = NULL;
  le->leIsActiveDecomposition  = NULL;
  le->leIsActiveInterpretation = NULL;

  if( !le_initActiveRuleTable(&le->leIsActiveProduction,     le->leIls->ilsNumProductions    ) ||
      !le_initActiveRuleTable(&le->leIsActiveDecomposition,  le->leIls->ilsNumDecompositions ) ||
      !le_initActiveRuleTable(&le->leIsActiveInterpretation, le->leIls->ilsNumInterpretations)
    ) return addGeneralError( lerrs, lsys_error_memory_full,
      "Could not create active rule tables"),
      le_freeActiveRuleTable(le->leIsActiveProduction),
      le_freeActiveRuleTable(le->leIsActiveDecomposition),
      le_freeActiveRuleTable(le->leIsActiveInterpretation),
      NULL;

  // There is actually no way to change this value, save recompiling the libarary
  le->leMaxStringLength = LE_MAX_STR_LEN;

  // All done.
  return le;

}
void le_initGlobals(unsigned int numGlobals, IVarDec* globals, float* varBinds)
{
  for(unsigned int i=0; i < numGlobals; i++)
    varBinds[globals[i].ivdOffset] = evalF(varBinds, globals[i].ivdValExp);
}

void le_setHaltFlagPtr(CLSYS_LEngine* le, unsigned int* hfp)
{
  le->leHaltFlag = hfp;
}

// Will derive the current string, replacing it with the result of derivation 
// Returns false if there was a problem or if derivation has completed
// (check the errors if you care which one it was)
bool le_derive(LSysErrors* lerrs, CLSYS_LEngine* le)
{

  if( le->leStepsTaken >= le->leIls->ilsDerivationLength ) return false;

  // Create a blank string to hold the next iteration
  LString* nextStr = lsm_empty(le->leIls->ilsModuleTypeParamCounts);

  for(
      LStringPosition lsp = lsp_head(le->leCurrStr);
      (le->leHaltFlag ? *le->leHaltFlag : true ) && 
      !lsp_isAtEnd(lsp);
      lsp = lsp_moveR(lsp)
     ) if(
       !matchRules( 
         le->leDeriveNeigbors,
         le->leIls->ilsProductions,
         le->leIls->ilsNumProductions,
         lsp,
         le->leCurrStr, 
         nextStr,
         le->leParamBuffer,
         le->leIls->ilsIgnoredModules,
         &le->leIsActiveProduction 
         )
       ) return ls_freeLString(nextStr), false;

  // if the string got too long, break
  if(ls_bytesLString(nextStr) > le->leMaxStringLength)
  {
    ls_freeLString(nextStr);
    le->leStepsTaken++;
    return false;
  }

  // otherwise, set the new string to the next string
  // Replace old string with new 
  ls_freeLString(le->leCurrStr);
  le->leCurrStr = nextStr;

  le->leStepsTaken++;

  return true;
}

// Given a position in the string:
// Goes through all the rules and attempts to match context.
//  If successful:
//    - sets variables
//    - checks condition; if successful:
//      - builds successor
//      - appends successor to the nextString
//  Else copies predecessor to the nextString
// Returns false if there isn't enough memory
bool matchRules(
    LRuleNeigbors*   lrns,
    ILRule*         rules, 
    unsigned int numRules, 
    LStringPosition   lsp, 
    LString*      currStr,
    LString*      nextStr,
    float*         params,
    bool*         ignored,
    bool**    activeRules
    )
{
  VERB( fprintf(stderr, "matching %d rules... %s\n", numRules, (rules?"":"(none to match)")););
  // We apply the first rule that matches
  for(unsigned int i=0; rules && i < numRules; i++){
    VERB( fprintf(stderr, "checking rule... "); );
    if( 
      le_contextMatches( lrns, lsp, currStr, rules, i, ignored, params) &&
      ( le_setVariables(&rules[i], params),
        evalB(params, rules[i].ilrCondition)
      )
    ){
      VERB( fprintf(stderr, "rule %d matches\n",i););
      (*activeRules)[i] = true;
      return le_applyProduction(rules[i].ilrProduction, rules[i].ilrLengthProd, params, nextStr);
    }
    VERB( fprintf(stderr, "nope\n"););
  }
  VERB( fprintf(stderr, "no rules match\n"););
  // and if that doesn't happen; copy the predecessor to the successor
  //return ls_appendModule(lsp, nextStr); 
  return lsm_append(nextStr, lsp_mt(lsp),  lsm_params(lsp)); 

}


// Will also populate the bindings with values from the context
bool le_contextMatches(
    LRuleNeigbors*    lrns,
    LStringPosition    lsp, 
    LString*          lstr, 
    ILRule*         lrules, 
    unsigned int   lrule_i, 
    bool*          ignored,
    float* params
){

// Just need to create the array 
// that stores the visited nodes
  bool* visiteds;
  if ( ! (visiteds = calloc( sizeof(bool), lrules[lrule_i].ilrLengthCtx ) ) )
    return free(visiteds), false;

// now the top level recursive call 
// to the context matching algoritm
  return
  (
    le_contextEq(
      lrules[lrule_i].ilrPredecessor,
      lsp,
      visiteds,
      params,
      lrns,
      ignored
    ) ? (free(visiteds),  true):
        (free(visiteds), false)
  );

}

bool le_contextEq(
  LStringPosition ctxPos,
  LStringPosition    lsp,
  bool*      visited_ctx,
  float*          params,
  LRuleNeigbors*    lrns,
  bool*          ignored
){
  VERB( fprintf(stderr, "\nctx tag: %d\n", lsc_tag(ctxPos)););
  VERB( fprintf(stderr, "\nctx mt:  %d\n", lsp_mt(ctxPos) ););
  VERB( fprintf(stderr, "ctx start? %s\n", lsp_isAtStart(ctxPos)?"t":"f"););
  VERB( fprintf(stderr, "ctx end?   %s\n", lsp_isAtEnd(ctxPos)?"t":"f"););

  bool parentOk = true;
  bool childrenOk = true;

  // if we have already visited this member of the context, ok
  if ( visited_ctx[lsc_tag(ctxPos)] ) return true;

  // if it is a ending branch point, it should match anything in the string
  if ( lsp_mt(ctxPos) == mtPop ) return true;

  VERB( fprintf(stderr, "%d =?= %d\n", lsp_mt(ctxPos), lsp_mt(lsp)); )
  // now, ensure they are indeed the same type
  if ( lsp_mt(ctxPos) != lsp_mt(lsp) ) return false;

  VERB( fprintf(stderr, "ok...\n"); )
  // this node is ok, mark it as visited 
  visited_ctx[lsc_tag(ctxPos)] = true;

  // set the parameter values. offset into array is lexically determined
  for(
    unsigned int i=0, 
    j = ls_numParams(lsp_lstring(lsp), lsp_mt(lsp)); 
    i < j; 
    i++
  ) params[lsc_paramOffset(ctxPos) + i] = lsm_paramI(lsp, i);

  // all of the relatives must match up too

  // first check up the tree ( to the left ) 
  if ( !lsp_isAtStart(ctxPos) )
  {
    LStringPosition parentLsp = lsp_parentOf(lsp);
    // skip over consecutive ignored ancestors in the lstring
    while( ignored[lsp_mt(parentLsp)] && !lsp_isAtStart(parentLsp) )
      parentLsp = lsp_parentOf(parentLsp);

    // now ensure the parent is ok
    parentOk = 
      le_contextEq (
        lsp_parentOf(ctxPos),
        parentLsp,
        visited_ctx, params, lrns, ignored
      );
  }

  VERB( fprintf(stderr, "parent %s\n", parentOk?"ok":"not ok"); )

  // Ok, check the children too
  for (
    LStringPosition
      ctxChild = lsp_moveR(ctxPos), 
      lspChild = lsp_moveR(lsp);

    parentOk && childrenOk &&
    !lsp_isAtEnd(ctxChild);

    ctxChild = lsp_nextChild(ctxChild),
    lspChild = lsp_nextChild(lspChild)
  ){

    // skip ignored modules if not at a lateral branch 
    if ( lsp_mt(ctxChild) != mtPush )
      while( ignored[lsp_mt(lspChild)] && !lsp_isAtStart(lspChild) )
        lspChild = lsp_moveR(lspChild);

    childrenOk = 
      le_contextEq (
        ctxChild, lspChild,
        visited_ctx, params, lrns, ignored
      );
       
  }
  VERB( fprintf(stderr, "children %s\n", childrenOk?"ok":"not ok"); )

  return parentOk && childrenOk;
}

void le_setVariables(ILRule*  lrule, float* varBinds)
{
  VERB( fprintf(stderr, "setting variables...\n"); );
  for(unsigned int i=0; i < lrule->ilrLengthVarDecs; i++)
    varBinds[lrule->ilrVardecs[i].ivdOffset] = evalF(varBinds, lrule->ilrVardecs[i].ivdValExp);
}


bool le_applyProduction
  (
    IModExp* imes,
    unsigned int imesLength,
    float* paramBindings, 
    LString* lstr
  )
{

  VERB( fprintf(stderr,"Applying production\n"); );

  float vals[MAX_NUM_MODULE_PARAMETERS];

  for(unsigned int i=0; i < imesLength; i++)
  {
    // evalutate each of the arguments first;
    for(unsigned int j=0; j < imes[i].imeNumVals; j++)
      vals[j] = evalF(paramBindings, imes[i].imeVals[j]);

    if( ! lsm_append(lstr, imes[i].imeType, vals) )
    {
      VERB( fprintf(stderr,"did not append to string!\n"); );
      return false; 
    }

    VERB( fprintf(stderr,"appended %d to string\n", imes[i].imeType); );
  }

  VERB( fprintf(stderr,"Applied production\n"); );

  return true;

}

void le_fprintLString(FILE* fout, CLSYS_LEngine* le)
{
  fprintf(fout, "\n"); 
  for(
      LStringPosition lsp = lsp_head(le->leCurrStr); 
      !lsp_isAtEnd(lsp);
      lsp = lsp_moveR(lsp)
     ){
    // print out the module names and the module params
    fprintf(fout, "%s", le->leIls->ilsModuleNames[lsp_mt(lsp)]);
    if( le->leIls->ilsModuleTypeParamCounts[lsp_mt(lsp)] > 0 )
    {
      fprintf(fout,"(");
      for(unsigned int i=0;;)
      {
        fprintf(fout,"%f", lsm_paramI(lsp, i));

        if( ++i < le->leIls->ilsModuleTypeParamCounts[lsp_mt(lsp)] )
          fprintf(fout,",");
        else break;

      }
      fprintf(fout,")");
    }
    fprintf(fout, " "); 
  }

  fprintf(fout, "\n"); 

}

LString* le_currStr(CLSYS_LEngine* le)
{
  return le->leCurrStr;
}

bool le_interpret(
    LSysErrors* lerrs,
    CLSYS_LEngine* le,
    LString** lstr)
{

  VERB( fprintf(stderr, "interpreting %d rules for %d steps\n", le->leIls->ilsNumInterpretations, le->leIls->ilsDerivationDepth); );
  LString* currStr;

  // Write into the provided string
  LString* nextStr = *lstr; 

  currStr = le->leCurrStr;

  if( le->leIls->ilsDerivationDepth <= 0 )
    return *lstr = currStr, true;

  for(
      unsigned int depth = 0;
      depth < le->leIls->ilsDerivationDepth;
      depth++
     )
  {
    VERB( fprintf(stderr, "interp: next step\n" ); );
    for(
        LStringPosition lsp = lsp_head(currStr);
        (le->leHaltFlag ? *le->leHaltFlag : true ) && 
        !lsp_isAtEnd(lsp);
        lsp = lsp_moveR(lsp) 
       ){
      if( !matchRules(
            le->leInterpNeigbors,
            le->leIls->ilsInterpretations,
            le->leIls->ilsNumInterpretations,
            lsp,
            currStr, 
            nextStr,
            le->leParamBuffer,
            le->leIls->ilsIgnoredModules,
            &le->leIsActiveInterpretation)   
        ){
        if(currStr != le->leCurrStr)
          ls_freeLString(currStr);
        ls_freeLString(nextStr);
        return false;
      }
    }

    // if the string got too long, exit!
    if(ls_bytesLString(nextStr) > le->leMaxStringLength)
    {
      if(nextStr != *lstr)
        ls_freeLString(nextStr);
      *lstr = currStr;
      return false;
    }

    // otherwise, set the new string to the next string
    if(currStr != le->leCurrStr)
      ls_freeLString(currStr);

    currStr = nextStr;
    nextStr = lsm_empty(le->leIls->ilsModuleTypeParamCounts);

  }

  ls_freeLString(nextStr);
  *lstr = currStr;
  return true;
}

bool le_initActiveRuleTable(bool ** activeRuleTable, unsigned int numRules)
{
  if( !(*activeRuleTable = malloc(numRules * sizeof(bool)) ) )
    return false;
  for(unsigned int i=0; i < numRules; i++) 
    (*activeRuleTable)[i] = false;

  return true ;

}

void le_freeActiveRuleTable(bool * activeRuleTable)
{
  if( activeRuleTable )
    free(activeRuleTable);
}

void freeLEngine(CLSYS_LEngine* le)
{
  if ( le == NULL ) return;

  ls_freeLString(le->leCurrStr);

  lrn_free(le->leDeriveNeigbors, le->leIls->ilsNumProductions);
  lrn_free(le->leDecompNeigbors, le->leIls->ilsNumDecompositions);
  lrn_free(le->leInterpNeigbors, le->leIls->ilsNumInterpretations);

  free(le->leParamBuffer);
  free(le);
}

unsigned int le_bytesLString(CLSYS_LEngine*le)
{
  return ls_bytesLString(le->leCurrStr);
}

// These are extras used rather specifically
// for automated mutation algoritms

bool le_isProductionActive    (CLSYS_LEngine * le, unsigned int rule_i)
{ return le->leIsActiveProduction     && le->leIsActiveProduction[rule_i];     }

bool le_isDecompositionActive (CLSYS_LEngine * le, unsigned int rule_i)
{ return le->leIsActiveDecomposition  && le->leIsActiveDecomposition[rule_i];  }

bool le_isInterpretationActive(CLSYS_LEngine * le, unsigned int rule_i)
{ return le->leIsActiveInterpretation && le->leIsActiveInterpretation[rule_i]; }

bool le_isNeighbor
(
  CLSYS_LEngine* le, 
  bool** neighbors, 
  unsigned int rule_i, 
  char* moduleName
)
{
  if(neighbors == NULL || neighbors[rule_i] == NULL) return false;
  for(unsigned int i=0; i < le->leIls->ilsNumModules; i++)
  {
    if( strcmp(le->leIls->ilsModuleNames[i], moduleName) == 0)
      return  neighbors[rule_i][i];
  }
  return false;
}


bool le_isPLCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName)
{  return le_isNeighbor(le, le->leDeriveNeigbors->lerLCNeighbors, rule_i, modName);}

bool le_isPRCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName)
{  return le_isNeighbor(le, le->leDeriveNeigbors->lerRCNeighbors, rule_i, modName);}

bool le_isDLCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName)
{  return le_isNeighbor(le, le->leDecompNeigbors->lerLCNeighbors, rule_i, modName);}

bool le_isDRCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName)
{  return le_isNeighbor(le, le->leDecompNeigbors->lerRCNeighbors, rule_i, modName);}

bool le_isILCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName)
{  return le_isNeighbor(le, le->leInterpNeigbors->lerLCNeighbors, rule_i, modName);}

bool le_isIRCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName)
{  return le_isNeighbor(le, le->leInterpNeigbors->lerRCNeighbors, rule_i, modName);}

unsigned int le_lengthLString(CLSYS_LEngine*le)
{
  return ls_bytesLString(le->leCurrStr);
}


#endif
