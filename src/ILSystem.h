#ifndef ILSYSTEM_H
#define ILSYSTEM_H

/*
 * Interpreted CLSYS_LSystem  Datatype
 * 
 */

#include <stdbool.h>
#include <stdlib.h>
#include "Defines.h"
#include "LString.h"

bool makeIModTypes(unsigned int length, IModType**);
void freeIModTypes(unsigned int length, IModType*);

typedef unsigned int * ModuleTypeInfo;

// Used for rule matching and parameter binding
typedef struct IModRef {
  IModType  imrType;         // For matching rules
  IParamId  imrParamsOffset; // For binding parameters (indexes where to start copying values into the parameter array)
  unsigned int imrNumParams;

} IModRef;
#define DEFAULT_MODREF (IModRef) { GARBAGE_IMODTYPE, 0 }

bool makeIModRefs(unsigned int length, IModRef** imrs);
void freeIModRefs(unsigned int length, IModRef* imrs);

bool makeIModRef(IModType, IParamId, IModRef*);
void freeIModRef(IModRef);

// Used for creating values for use on the string and in variable declrations 
typedef struct IValExp {
  ops_t       iveType;
  union {
    IParamId iveVariableOffset; 
    float    iveFVal;
    bool     iveBVal;
    struct IValExp* iveArgs; // Length is implied by iveType
  } ivePayload;
} IValExp;
#define DEFAULT_IVAL (IValExp) { hOpNone, { .iveArgs = NULL} }

bool makeIValExps(unsigned int length, IValExp** ives);
void freeIValExps(unsigned int length, IValExp* ives);

bool makeIValExpBinary(ops_t, IValExp, IValExp, IValExp*);
bool makeIValExpFloat(float, IValExp*);
bool makeIValExpBool(bool, IValExp*);
void freeIValExp(IValExp);


typedef struct IModExp {
  IModType imeType;
  IValExp* imeVals; 
  unsigned int imeNumVals;
} IModExp;
#define DEFAULT_MODEXP (IModExp) { GARBAGE_IMODTYPE, NULL, 0 }

bool makeIModExps(unsigned int length, IModExp** imes);
void freeIModExps(unsigned int length, IModExp* imes);

bool makeIModExp(IModType, IValExp*, unsigned int, IModExp*);
void freeIModExp(IModExp);

typedef struct IVarDec
{
  IValExp      ivdValExp;
  unsigned int ivdOffset;
} IVarDec;
#define DEFAULT_IVARDEC (IVarDec) { DEFAULT_IVAL, 0 }
bool makeIVarDec(IValExp, unsigned int, IVarDec*);
void freeIVarDec(IVarDec);
bool makeIVarDecs(unsigned int length, IVarDec** ivds);
void freeIVarDecs(unsigned int length, IVarDec*  ivds);

typedef struct ILRule
{
  LString*        ilrContext;
  LStringPosition ilrPredecessor;
  unsigned int    ilrLengthCtx;  

  IVarDec*     ilrVardecs;
  IValExp      ilrCondition;
  IModExp*     ilrProduction;
  unsigned int ilrLengthProd;
  unsigned int ilrLengthVarDecs; 

  //unsigned int ilrLengthLC;  
  //unsigned int ilrLengthRC;  
  // Stores whether the module type was a neighbor during derivation
  bool*        ilrLCNeighbors;
  bool*        ilrRCNeighbors;

} ILRule;
#define DEFAULT_ILRULE (ILRule) { NULL, { NULL, NULL}, 0, NULL, DEFAULT_IVAL, NULL, 0, 0, NULL, NULL }
//#define DEFAULT_ILRULE (ILRule) { NULL, { NULL, NULL}, NULL, DEFAULT_IVAL, NULL, 0, 0, 0, 0, NULL, NULL }

bool makeILRules(unsigned int length, ILRule** imes);
void freeILRules(unsigned int length, ILRule* imes);

void freeILRule(ILRule);

// Internal CLSYS_LSystem  Datatype representation
// This gets passed to the CLSYS_LEngine verbatim 
typedef struct ILSystem {

  unsigned int ilsLengthAxiom;
  unsigned int ilsNumProductions;
  unsigned int ilsNumInterpretations;
  unsigned int ilsNumDecompositions;
  unsigned int ilsNumIgnored;
  unsigned int ilsNumConsidered;

  IModExp*  ilsAxiom;
  ILRule*   ilsProductions;
  ILRule*   ilsDecompositions;
  ILRule*   ilsInterpretations;

  // keyed by module type 
  bool*     ilsIgnoredModules;
  bool*     ilsConsideredModules;

  int ilsRandomSeed;
  int ilsDerivationLength;
  int ilsDerivationDepth;
  
  // These guys are evaluated before the first derivation step
  // They get placed in the parameter array; This information is then discarded
  IVarDec* ilsGlobals;
  unsigned int ilsNumGlobals;

  IModType* ilsDrawModuleTypes; // Each module type used in the string 
                                // is drawn as if it had backticks
                                // removed from its name. This maps
                                // the backtick'd module type
                                // to the effective sans-backtick'd
                                // module type. 
 
  // There is a certain number of moduleTypes 
  unsigned int   ilsNumModules;
  // And each is associated with some number of parameters
  ModuleTypeInfo ilsModuleTypeParamCounts;
  char**         ilsModuleNames; // For printing human readable LStrings 
  
  // This tells us the maximum number of variables that *any* expression could use
  // We don't allocate space for variables at every new scope level at the moment 
  // because there are only 4 possible scope levels.
  // (sub-lsystems change all that)
  unsigned int ilsMaxNumVariables; // maximum taken out of all the rules

} ILSystem;

ILSystem* emptyILSystem();
void freeILSystem(ILSystem *ils);

#endif
