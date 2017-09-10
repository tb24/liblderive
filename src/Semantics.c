#ifndef SEMANTICS_C
#define SEMANTICS_C

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "Common.h"
#include "Errors.h"
#include "Defines.h"
#include "HLSystem.h"
#include "ILSystem.h"
#include "SymbolTable.h"
#include "Semantics.h"
#include "LSContext.h"

bool checkDirectives(LSysErrors* lerrs, SymbolTable* st, HLSystem* hls, ILSystem* ils);

bool buildAxiom    (LSysErrors*, SymbolTable*, HDirective**, IModExp**,  unsigned int*);
bool buildGlobals  (LSysErrors*, SymbolTable*, HDirective**, IVarDec**,  unsigned int*);
bool buildIgnores  (LSysErrors*, SymbolTable*, HDirective**, bool**,     unsigned int*);
//bool buildConsiders(LSysErrors*, SymbolTable*, HDirective**, bool**,     unsigned int*);

bool buildRule     (LSysErrors*, SymbolTable*, HLRule*,  ILRule*);
bool buildModRef(LSysErrors* lerrs, SymbolTable* st, HModRef* hmr, LString* str, unsigned int tag);

bool buildRules    (LSysErrors*, SymbolTable*, HLRule**, ILRule**, unsigned int*);

bool buildVarDec(LSysErrors* lerrs, SymbolTable* st, ScopeLevel sl, HVarDec* hvd, IVarDec* ive);

bool buildValExps  (LSysErrors*, SymbolTable*, ScopeLevel, HValExp**, IValExp**);
bool buildValExp   (LSysErrors*, SymbolTable*, ScopeLevel, HValExp*,  IValExp* );
bool buildModExps  (LSysErrors*, SymbolTable*, ScopeLevel, HModExp**, IModExp**);
bool buildModExp   (LSysErrors*, SymbolTable*, ScopeLevel, HModExp*,  IModExp* );

bool buildParamIdsFromModExps(LSysErrors* lerrs, SymbolTable* st, HModExp** hmes, IModType** ids, unsigned int* numIds);


bool axiomOk(HDirective **, char ** globals);
bool lengthOk(HDirective **);
bool globalIdsOk(HDirective**);
bool globalValsOk(HDirective**);

void setGlobals(HDirective** hdirs, float * globals);

bool hasBooleanOperands(ops_t);
bool hasFloatOperands(ops_t);
bool verifyIsBooleanType(ops_t op, LSysErrors* lerrs);
bool verifyIsFloatType(ops_t);

unsigned int maxRuleBindings(HLSystem *);
unsigned int numRuleBindings(HLRule *);
unsigned int directiveLength(HDirective**, dir_t);
unsigned int numModRefParams(HModRef *);

// Helpers for running through the HLSystem types
bool addGlobalParams(LSysErrors*, SymbolTable*, HLSystem*);

// We don't require the user to declare modules; these functions add
// the modules to the symbol table and check for inconsistencies 
bool addUserModules (LSysErrors*, SymbolTable*, HLSystem*);
bool addModuleSymbolsFromRule   (LSysErrors*, SymbolTable*, HLRule*  );
bool addModuleSymbolsFromModExps(LSysErrors*, SymbolTable*, HModExp**);
bool addModuleSymbolsFromModExp (LSysErrors*, SymbolTable*, HModExp* );
bool addModuleSymbolsFromModRefs(LSysErrors*, SymbolTable*, HModRef**);
bool addModuleSymbolsFromModRef (LSysErrors*, SymbolTable*, HModRef* );

unsigned int count_backticks(char* string);

void semantics_cleanup(SymbolTable*, ILSystem*);

// Arguments: 
//  - A human readable Lsystem representation
//  - A pointer to the error structure
// Returns: 
//  - A pointer to the internal representation of the CLSYS_LSystem 
//  - If unsuccessful, the return value will be NULL 
//  - If unsuccessful, the error code should be set
//  - If successful, the reference to the ILSystem pointer 
//    should be ready for derivation by the CLSYS_LEngine functions.
ILSystem* check_hlsystem(HLSystem* hls, LSysErrors* lerrs)
{
  // Needed locally
  SymbolTable* st = st_new();
  ILSystem*   ils = emptyILSystem();

  // Each function sets errors (if necessary)
  if(  !checkDirectives( lerrs, st, hls, ils) ||
       !addGlobalParams( lerrs, st, hls)  ||
       !buildGlobals(    lerrs, st, hls->hlsDirectives,      &ils->ilsGlobals,           &ils->ilsNumGlobals        )  ||
       !addUserModules(  lerrs, st, hls)  ||
       !buildAxiom(      lerrs, st, hls->hlsDirectives,      &ils->ilsAxiom,             &ils->ilsLengthAxiom       )  ||
       !buildIgnores(    lerrs, st, hls->hlsDirectives,      &ils->ilsIgnoredModules,    &ils->ilsNumIgnored        )  ||
//       !buildConsiders(  lerrs, st, hls->hlsDirectives,      &ils->ilsConsideredModules, &ils->ilsNumConsidered     )  ||
       !buildRules(      lerrs, st, hls->hlsProductions,     &ils->ilsProductions,       &ils->ilsNumProductions    )  ||
       !buildRules(      lerrs, st, hls->hlsDecompositions,  &ils->ilsDecompositions,    &ils->ilsNumDecompositions )  ||
       !buildRules(      lerrs, st, hls->hlsInterpretations, &ils->ilsInterpretations,   &ils->ilsNumInterpretations) 
  ) return semantics_cleanup(st,ils), NULL;

  ils->ilsMaxNumVariables = maxRuleBindings(hls);
  ils->ilsNumModules = st_num_moduleTypes(st); 

  // This is a map from the module type id to the number of paramters it has (user & predefined modules)
  // ModuleTypeId's start at zero, so we can just use an array for the map implementation
  ils->ilsModuleTypeParamCounts = malloc(ils->ilsNumModules * sizeof *ils->ilsModuleTypeParamCounts);
  for(IModType i=0; i < ils->ilsNumModules; i++)
    ils->ilsModuleTypeParamCounts[i] = st_num_module_parameters_fromMt(lerrs, st, i);
 
  // We also hold on to the names of the modules for user benefit; and Mutate.h functionality
  if( !(ils->ilsModuleNames = malloc(ils->ilsNumModules * sizeof *ils->ilsModuleNames) ) )
      return semantics_cleanup(st, ils), NULL;

  for(IModType i=0; i < ils->ilsNumModules; i++)
    ils->ilsModuleNames[i] = my_strndup(st_module_name_fromMt(lerrs, st, i), MAX_LEN_IDENTIFIER);

  VERB( fprintf(stderr, "%d production     rules\n", ils->ilsNumProductions    ); )
  VERB( fprintf(stderr, "%d decomposition  rules\n", ils->ilsNumDecompositions ); )
  VERB( fprintf(stderr, "%d interpretation rules\n", ils->ilsNumInterpretations); )
  

  // We also need a mapping from each module type to the module type that it 
  // would be if it had backticks removed from the module name. This allows 
  // for a bit more complexity in the interpretation phase, since it allows 
  // there to be a whole suite of rules effective for a given turtle command. 
  if( !(ils->ilsDrawModuleTypes = malloc(ils->ilsNumModules * sizeof *ils->ilsDrawModuleTypes)) )
    return semantics_cleanup(st, ils), NULL;
  for(IModType mt=0; mt < ils->ilsNumModules; mt++)
    ils->ilsDrawModuleTypes[mt] = st_draw_type_from_module_name(lerrs, st, st_module_name_fromMt(lerrs,st,mt));

  st_destroy(st);
  
  return ils;
}

bool checkDirectives(LSysErrors* lerrs, SymbolTable* st, HLSystem* hls, ILSystem* ils)
{
  for(unsigned int i=0; hls->hlsDirectives && hls->hlsDirectives[i]; i++)
    switch(hls->hlsDirectives[i]->hdType)
    {
      case dDEPTH:
        ils->ilsDerivationDepth = hls->hlsDirectives[i]->hdData.hddDerivationDepth;
        break;
      case dLENGTH:
        ils->ilsDerivationLength = hls->hlsDirectives[i]->hdData.hddDerivationLength;
        if ( ils->ilsDerivationLength < 0 ) 
          return 
            addParseError(
                lerrs, 
                lsys_error_invalid_declaration, 
                "Derivation Length cannot be negative",
                hls->hlsDirectives[i]->yylocation
                ), false;
        break;
      case dSEED:
        ils->ilsRandomSeed = hls->hlsDirectives[i]->hdData.hddRandomSeed;
        break;
      default:
        break;
    }
  return true;
}


bool buildRules(LSysErrors* lerrs, SymbolTable* st, HLRule** hlrs, ILRule** ilrs, unsigned int* numRules)
{

  ILRule* ilrsTmp;
  unsigned int numIlrs;

  numIlrs = arrayLength((void**)hlrs);

  if( !(numIlrs > 0 ) )
  {
    *ilrs = NULL;
    *numRules = 0;
    return true;
  }

  VERB( fprintf(stderr, "num rules: %d\n", numIlrs); )

  if ( !makeILRules(numIlrs, &ilrsTmp) )
    return false;

  for(unsigned int i=0; i < numIlrs; i++)
    ilrsTmp[i] = DEFAULT_ILRULE;

  
  VERB( fprintf(stderr, "nummer rules: %d\n", numIlrs); )
  for(unsigned int i=0; hlrs && hlrs[i]; i++)
    if( ! buildRule(lerrs, st, hlrs[i], &ilrsTmp[i]) )
    {
      freeILRules(numIlrs, ilrsTmp);
      return false;
    }

  *ilrs = ilrsTmp;
  *numRules = numIlrs;
  VERB( fprintf(stderr, "nummy rules: %d\n", numIlrs); )
  return true;

}

bool buildRule(LSysErrors* lerrs, SymbolTable* st, HLRule* hlr, ILRule* ilr)
{

  // first build the predecessor and the context
  st_clear_scope(st, slContext);
  st_clear_scope(st, slProduction);

  ilr->ilrContext = lsc_empty();

  VERB( fprintf(stderr, "building rule context\n");)

  // the first thing to do is to determine whether
  // the context needs to have its branches balanced.
  int unmatchedPushes = 0, unmatchedPops = 0;
  {
    IModType mt;
    for(unsigned int i=0; hlr->hlrLC && hlr->hlrLC[i]; i++)
    {
      if(!st_get_mod_id(lerrs, st, hlr->hlrLC[i]->hmrType, &mt))
        return false;
      VERB( fprintf(stderr, "mt: %d \n", mt);)
      if ( mt == mtPop  ) unmatchedPushes--; 
      if ( mt == mtPush ) unmatchedPushes++; 
      if ( unmatchedPushes < 0 )
        unmatchedPops++, unmatchedPushes=0;
    }

    if(!st_get_mod_id(lerrs, st, hlr->hlrSP->hmrType, &mt))
      return false;
    if ( mt == mtPop  ) unmatchedPushes--; 
    if ( mt == mtPush ) unmatchedPushes++; 
    if ( unmatchedPushes < 0 )
      unmatchedPops++, unmatchedPushes=0;

    for(unsigned int i=0; hlr->hlrRC && hlr->hlrRC[i]; i++)
    {
      if(!st_get_mod_id(lerrs, st, hlr->hlrRC[i]->hmrType, &mt))
        return false;
      if ( mt == mtPop  ) unmatchedPushes--; 
      if ( mt == mtPush ) unmatchedPushes++; 
      if ( unmatchedPushes < 0 )
        unmatchedPops++, unmatchedPushes=0;
    }
    VERB( fprintf(stderr, "pops : %d \n", unmatchedPops);)
    VERB( fprintf(stderr, "pushs: %d \n", unmatchedPushes);)
  }

  ilr->ilrLengthCtx = 0;

  // balance branches if necessary
  if(unmatchedPops > 0 )
  {
    VERB( fprintf(stderr, "got pops");)
    lsc_append(ilr->ilrContext, mtAny, 0, ilr->ilrLengthCtx++);
    while(unmatchedPops-- > 0)
    {
      VERB( fprintf(stderr, "pops: %d \n", unmatchedPops);)
      lsc_append(ilr->ilrContext, mtPush, 0, ilr->ilrLengthCtx++);
    }
  }
    
  for(unsigned int i = 0; hlr->hlrLC && hlr->hlrLC[i]; i++)
    if( !buildModRef(lerrs, st, hlr->hlrLC[i], ilr->ilrContext, ilr->ilrLengthCtx++) )
      return lsc_free(ilr->ilrContext), false;

  ilr->ilrPredecessor = lsp_tail(ilr->ilrContext);
  if( !buildModRef(lerrs, st, hlr->hlrSP, ilr->ilrContext, ilr->ilrLengthCtx++) )
    return lsc_free(ilr->ilrContext), false;

  for(unsigned int i = 0; hlr->hlrRC && hlr->hlrRC[i]; i++)
    if( !buildModRef(lerrs, st, hlr->hlrRC[i], ilr->ilrContext, ilr->ilrLengthCtx++) )
      return lsc_free(ilr->ilrContext), false;

  // balance branches if necessary
  while(unmatchedPushes-- > 0)
  {
    VERB( fprintf(stderr, "got pushes: %d \n", unmatchedPushes);)
    lsc_append(ilr->ilrContext, mtPop, 0, ilr->ilrLengthCtx++);
  }
  
  VERB( fprintf(stderr, "balanced ctx size: %d\n", ilr->ilrLengthCtx);)
  VERB( fprintf(stderr, "building vardecs in rule\n");)

  // we can build the variable declarations now
  ilr->ilrLengthVarDecs = arrayLength((void**)hlr->hlrVardecs);
  if( !makeIVarDecs(ilr->ilrLengthVarDecs, &ilr->ilrVardecs) )
    return st_clear_scope(st, slContext), false;

  for(unsigned int i=0; hlr->hlrVardecs && hlr->hlrVardecs[i]; i++)
    if( !buildVarDec(lerrs, st, slProduction, hlr->hlrVardecs[i], &ilr->ilrVardecs[i]) )
      return st_clear_scope(st, slProduction), st_clear_scope(st, slContext), false;

  // the variable declarations should now be in the symbol table
  // we can go ahead with the condition and the production 
  
  ilr->ilrLengthProd = arrayLength((void**)hlr->hlrProduction);

  VERB( fprintf(stderr, "building production of %d modules\n", ilr->ilrLengthProd); )

  if( !buildValExp(lerrs, st, slProduction, hlr->hlrCondition, &ilr->ilrCondition) ||
      !buildModExps(lerrs, st, slProduction, hlr->hlrProduction, &ilr->ilrProduction)
    ) return st_clear_scope(st, slProduction), st_clear_scope(st, slContext), false;
 
  VERB( fprintf(stderr, "built rule\n"); )
  // everything went okay; clear the scope and return
  return st_clear_scope(st, slProduction), st_clear_scope(st, slContext), true;

}



bool buildModRef(LSysErrors* lerrs, SymbolTable* st, HModRef* hmr, LString* str, unsigned int tag)
{
 
  IModType      moduleType;
  IParamId      paramOffset;
  
  if( 
  // These will throw the appropriate error
    !st_get_mod_id(lerrs, st, hmr->hmrType, &moduleType) ||
    (-1 == (paramOffset = st_num_variables(st, slContext)))
  ) return false;
  
  VERB( fprintf(stderr, "append: %s %d %d %d\n", hmr->hmrType->hidString, moduleType, paramOffset, tag); ) 
  if(!lsc_append(str, moduleType, paramOffset, tag))
    return false;
   
  // This keeps the parameter offset count correct
  for(unsigned int i=0; hmr->hmrParameters && hmr->hmrParameters[i]; i++)
    if( !st_insert_variableBinding(lerrs, st, slContext, hmr->hmrParameters[i]) )
      return st_clear_scope(st, slContext), false;
  
  return true;

}

bool buildParamIdsFromModExps(LSysErrors* lerrs, SymbolTable* st, HModExp** hmes, IModType** ids, unsigned int* numIds)
{
  
  *numIds = arrayLength((void**)hmes);

  IModType* idsTmp;

  if( *numIds == 0 )
    return *ids = NULL, true;

  // Must delcare it like this so pointer arithmetic is correct
  if( !makeIModTypes(*numIds, &idsTmp) )
    return false;

  for(unsigned int i=0; hmes && hmes[i]; i++)
    if ( ! st_get_mod_id(lerrs, st, hmes[i]->hmeType, &idsTmp[i]) )
      return freeIModTypes(*numIds, idsTmp), false;
 
  *ids = idsTmp;

  return true;
}


bool buildIgnores(LSysErrors* lerrs, SymbolTable* st, HDirective** hdirs, bool** igns, unsigned int* numIgns)
{
  IModType tmpIgn;
  bool*   tmpIgns;

  *igns = NULL;
  *numIgns = 0; 

  if( !(tmpIgns = calloc(st_max_modType(lerrs, st), sizeof(bool))) )
    return false;

  for(unsigned int i=0; hdirs && hdirs[i]; i++)
  {
    if ( hdirs[i]->hdType == dIGNORE )
    {
      *numIgns = arrayLength((void**)hdirs[i]->hdData.hddModExps);

      for(unsigned int j=0; hdirs[i]->hdData.hddModExps && hdirs[i]->hdData.hddModExps[j]; j++)
      {
        if ( !st_get_mod_id(lerrs, st, hdirs[i]->hdData.hddModExps[j]->hmeType, &tmpIgn) )
          return free(tmpIgns), false;
        tmpIgns[tmpIgn] = true;
      }
    }
  }

  *igns = tmpIgns; 
  return true;
}

/*
bool buildConsiders(LSysErrors* lerrs, SymbolTable* st, HDirective** hdirs, IModType** ids, unsigned int* numIds)
{
  IModType tmpIgn;
  bool*   tmpIgns;

  *igns = NULL;

  if( !(tmpIgns = calloc(st_max_modType(lerrs, st), sizeof(bool))) )
    return false;

  for(unsigned int i=0; hdirs && hdirs[i]; i++)
    if ( hdirs[i]->hdType == dCONSIDER)
    {
      *numIgns = arrayLength((void**)hdirs[i]->hdData.hddModExps);
      for(unsigned int i=0; hdirs[i]->hdData.hddModExps && hdirs[i]->hdData.hddModExps[i]; i++)
      {
        if ( !st_get_mod_id(lerrs, st, hdirs[i]->hdData.hddModExps[i]->hmeType, &tmpIgn) )
          return free(tmpIgns), false;

        tmpIgns[tmpIgn] = true;
      }
    }

  *igns = tmpIgns; 
  return true;
}
*/
bool buildAxiom(LSysErrors* lerrs, SymbolTable* st, HDirective** hdirs, IModExp** axiom, unsigned int* axiomLength)
{
  bool gotAxiom = false;
  
  // There can only one axiom directive
  HModExp** ax  = NULL;
  for(unsigned int i=0; hdirs && hdirs[i]; i++)
    if( hdirs[i]->hdType == dAXIOM )
    {
      if( gotAxiom ) return false;
      else {
        ax =  hdirs[i]->hdData.hddModExps;
        gotAxiom = true;
      }
    }

  if( ! gotAxiom ) return false;

  // Now, build it
  if ( ! buildModExps(lerrs, st, slGlobal, ax, axiom) )
    return false;
  
  *axiomLength = directiveLength(hdirs, dAXIOM);

  return true;

}

bool buildGlobals(LSysErrors* lerrs, SymbolTable* st, HDirective** hdirs, IVarDec** globals, unsigned int* numGlobals)
{
  
  IVarDec* globs = NULL;
  
  *numGlobals = st_num_variables(st, slGlobal);

  if( *numGlobals == 0 )
    return *globals = NULL, true;
  
  if( ! makeIVarDecs(*numGlobals, &globs) )
      return 
        addGeneralError(
          lerrs, 
          lsys_error_compiler_error, 
          "Could not create global variable declarations"
        ), false;
  
  // We do actually need to check the scope first. 
  st_clear_scope(st, slGlobal);
  for(unsigned int i=0,j=0; hdirs && hdirs[i] && j < *numGlobals; i++)
    if( hdirs[i]->hdType == dDEFINE)
      if( ! buildVarDec(lerrs, st, slGlobal, hdirs[i]->hdData.hddDefine, &globs[j++]) 
      ) return freeIVarDecs(*numGlobals, globs), false;
              
  *globals = globs;
  return true;
}

bool buildVarDec(LSysErrors* lerrs, SymbolTable* st, ScopeLevel sl, HVarDec* hvd, IVarDec* ive)
{
  
  return ( buildValExp(lerrs, st, sl, hvd->hvdVal, &ive->ivdValExp) &&
     st_insert_variableBinding(lerrs, st, sl, hvd->hvdId) && 
     st_get_var_offset(lerrs, st, sl, hvd->hvdId,  &ive->ivdOffset) 
  );
         
}


bool buildModExps(LSysErrors* lerrs, SymbolTable* st, ScopeLevel sl, HModExp ** hmes, IModExp** imes)
{
  if( arrayLength((void**)hmes) == 0 )
  { 
    *imes = NULL;
    return true;
  }
  IModExp* imies;
  unsigned int numExps = arrayLength((void**)hmes);
  
  VERB( fprintf(stderr, "building module expressions\n"); )

  if( ! makeIModExps(numExps, &imies) )
    return false;

  for(unsigned int i=0; hmes[i]; i++)
  {
    imies[i] = DEFAULT_MODEXP;
    if( ! buildModExp(lerrs, st, sl, hmes[i], &imies[i]) )
    {
      freeIModExps(numExps, imies);
      return false;
    }
  }
  *imes = imies;
  return true;
}

bool buildModExp(LSysErrors* lerrs, SymbolTable* st, ScopeLevel sl, HModExp* hme, IModExp* ime)
{
  // Translate the string identifier to an int 
  IModType imt;
  VERB( fprintf(stderr, "gettting mod id\n"); )
  if(! st_get_mod_id(lerrs, st, hme->hmeType, &imt) )
    return false;
  
  // Build it's value expressions; if any. 
  IValExp* ives = NULL;
  unsigned int numIves = arrayLength((void**)hme->hmeVals);

  VERB( fprintf(stderr, "checking module argument count: %d == %d ?\n", numIves, st_num_module_parameters(lerrs, st, hme->hmeType) ); )
  // Just to be sure, we check that it was passed the correct number of args
  if( numIves != st_num_module_parameters(lerrs, st, hme->hmeType) )
    return false;

  VERB( fprintf(stderr, "building parameter value expressions\n"); )
  if( numIves > 0 )
  {
    if( !makeIValExps(numIves, &ives) ) 
      return false;

    for(unsigned int i=0; hme->hmeVals && hme->hmeVals[i]; i++)
      if( ! buildValExp(lerrs, st, sl, hme->hmeVals[i], &ives[i]) )
        return freeIValExps(numIves, ives), false;
  }
  
  VERB( fprintf(stderr, "finalizing all that...\n"); )
  if( !makeIModExp(imt, ives, numIves, ime) )
    return freeIValExps(numIves, ives), false;
  
  return true;
}

bool addGlobalParams(LSysErrors* lerrs, SymbolTable* st, HLSystem* hls)
{
  for(unsigned int i=0; hls->hlsDirectives[i]; i++)
    if(hls->hlsDirectives[i]->hdType == dDEFINE)
      if( ! st_insert_variableBinding(lerrs, st, slGlobal, hls->hlsDirectives[i]->hdData.hddDefine->hvdId)) 
        return 
          addGeneralError(
            lerrs, lsys_error_parameter_binding, 
            "Could not bind global variable"
          ), false;
  return true;

}

bool addUserModules(LSysErrors* lerrs, SymbolTable* st, HLSystem * hls) 
{
  // First go through the axiom, as well as ignore and consider statements 
  for(unsigned int i=0; hls->hlsDirectives[i]; i++)
  {
    if ( hls->hlsDirectives[i]->hdType == dAXIOM )
      if( ! addModuleSymbolsFromModExps(lerrs, st, hls->hlsDirectives[i]->hdData.hddModExps) ) 
        return false;
  }
  // Now go through the rules
  for(unsigned int i=0; hls->hlsProductions[i]; i++)
    if( ! addModuleSymbolsFromRule(lerrs, st, hls->hlsProductions[i]) ) return false; 
  
  for(unsigned int i=0; hls->hlsDecompositions[i]; i++)
    if( ! addModuleSymbolsFromRule(lerrs, st, hls->hlsDecompositions[i]) ) return false; 
  
  for(unsigned int i=0; hls->hlsInterpretations[i]; i++)
    if( ! addModuleSymbolsFromRule(lerrs, st, hls->hlsInterpretations[i]) ) return false; 
  
  return true;
}

bool addModuleSymbolsFromRule(LSysErrors* lerrs, SymbolTable* st, HLRule * hlr)
{
  if ( ! addModuleSymbolsFromModRefs(lerrs, st, hlr->hlrLC) ) return false;
  if ( ! addModuleSymbolsFromModRef (lerrs, st, hlr->hlrSP) ) return false;
  if ( ! addModuleSymbolsFromModRefs(lerrs, st, hlr->hlrRC) ) return false;
  if ( ! addModuleSymbolsFromModExps(lerrs, st, hlr->hlrProduction) ) return false;
  return true;
}

bool addModuleSymbolsFromModExps(LSysErrors* lerrs, SymbolTable* st, HModExp** hmes)
{
  if(hmes == NULL) return true;
  for(unsigned int i=0; hmes[i]; i++)
    if( ! addModuleSymbolsFromModExp(lerrs, st, hmes[i]) ) return false;
  return true;
}

bool addModuleSymbolsFromModExp(LSysErrors* lerrs, SymbolTable* st, HModExp* hme)
{
  if( !st_insert_moduleType( lerrs, st, hme->hmeType, arrayLength( (void**)hme->hmeVals) ) )
    return false;

  return true;
}

bool addModuleSymbolsFromModRefs(LSysErrors* lerrs, SymbolTable* st, HModRef** hmrs)
{
  if(hmrs == NULL) return true;
  for(unsigned int i=0; hmrs[i]; i++)
    if( ! addModuleSymbolsFromModRef(lerrs, st, hmrs[i]) ) return false;
  return true;
}

bool addModuleSymbolsFromModRef(LSysErrors* lerrs, SymbolTable* st, HModRef* hmr)
{
  if( !st_insert_moduleType(lerrs, st, hmr->hmrType, arrayLength((void**)hmr->hmrParameters)) )
    return false;
  return true;
}

bool lengthOk(HDirective ** hdirs)
{
  bool gotLength = false;
  for(unsigned int i=0; hdirs[i]; i++)
  {
    if ( hdirs[i]->hdType == dLENGTH && !gotLength) gotLength = true;
    else return false;
  }
  return gotLength;
}

bool buildValExp(LSysErrors* lerrs, SymbolTable* st, ScopeLevel sl, HValExp* hve, IValExp* ive)
{
  IValExp e1 = DEFAULT_IVAL; 
  IValExp e2 = DEFAULT_IVAL; 

  if( hve == NULL || hve->hveType == hOpNone ) 
    return ive->iveType = hOpNone, true;

  if( hasBooleanOperands(hve->hveType) )
  {
    if( !verifyIsBooleanType(hve->hveArgs->hveaBinary.hve1->hveType, lerrs) ||
        !verifyIsBooleanType(hve->hveArgs->hveaBinary.hve2->hveType, lerrs) ||
        !buildValExp(lerrs, st, sl, hve->hveArgs->hveaBinary.hve1, &e1) ||
        !buildValExp(lerrs, st, sl, hve->hveArgs->hveaBinary.hve2, &e2) ||
        !makeIValExpBinary(hve->hveType, e1, e2, ive) 
    ) return freeIValExp(e1), freeIValExp(e2), false;
  }
  else if( hasFloatOperands(hve->hveType) )
  {
    if( !verifyIsFloatType(hve->hveArgs->hveaBinary.hve1->hveType) ||
        !verifyIsFloatType(hve->hveArgs->hveaBinary.hve2->hveType) ||
        !buildValExp(lerrs, st, sl, hve->hveArgs->hveaBinary.hve1, &e1) || 
        !buildValExp(lerrs, st, sl, hve->hveArgs->hveaBinary.hve2, &e2) ||
        !makeIValExpBinary(hve->hveType, e1, e2, ive)
    ) return freeIValExp(e1), freeIValExp(e2), false;
  }
  else
    switch(hve->hveType)
    {
      case hOpFval:
        ive->iveType = hve->hveType;
        ive->ivePayload.iveFVal = hve->hveFVal;
        break;
      case hOpBval:
        ive->iveType = hve->hveType;
        ive->ivePayload.iveFVal = hve->hveFVal;
        break;
      case hOpId:
      { IParamId offset;
        if( ! st_get_var_offset(lerrs, st, sl, hve->hveVarName, &offset))
          return false; 
        ive->iveType = hve->hveType;
        ive->ivePayload.iveVariableOffset = offset;
        break;
      }
      default: break;
    }

  return true;

}

bool verifyIsFloatType(ops_t op)
{
  switch(op){
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
    // TODO set the error flag TODO
      return false;
    case hOpAdd:
    case hOpSub:
    case hOpMul:
    case hOpDiv:
    case hOpPow:
    case hOpFval:
    case hOpId:
      return true;
  }
  // Should never happen
  return true;
}

bool hasFloatOperands(ops_t op)
{
  switch(op){
    case hOpAnd:
    case hOpOr: 
    case hOpBval:
    case hOpFval:
    case hOpId:
    case hOpNone:
      return false;
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
  }
  // TODO set compiler error flag //
  return false;
}

bool verifyIsBooleanType(ops_t op, LSysErrors* lerrs)
{
  switch(op){
    case hOpAnd:
    case hOpOr: 

    case hOpLt :
    case hOpLte:
    case hOpEq :
    case hOpNeq:
    case hOpGt :
    case hOpGte:
    
    case hOpBval:
      return true;

    case hOpAdd:
    case hOpSub:
    case hOpMul:
    case hOpDiv:
    case hOpPow:
    
    case hOpFval:
    case hOpId:
    case hOpNone:
      return 
        addGeneralError(
          lerrs, 
          lsys_error_inconsistent_type, 
          "Expected boolean type"
        ), false;
  }
  // TODO set compiler error flag //
      return 
        addGeneralError(
          lerrs, 
          lsys_error_compiler_error, 
          "Operator type was not parsed correctly"
        ), false;
}

bool hasBooleanOperands(ops_t op)
{
  switch(op){
    case hOpAnd:
    case hOpOr: 
      return true;
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

    case hOpFval:
    case hOpBval:
    case hOpNone:
    case hOpId:
      return false;
  }
  // TODO set compiler error flag //
  return false;
}

unsigned int maxRuleBindings(struct HLSystem * hls)
{
  unsigned int max = 0;

  for(unsigned int i=0; hls && hls->hlsProductions[i]; i++)
  {
    unsigned int j = numRuleBindings(hls->hlsProductions[i]);
    if ( j > max ) max = j;
  }

  for(unsigned int i=0; hls && hls->hlsDecompositions[i]; i++)
  {
    unsigned int j = numRuleBindings(hls->hlsDecompositions[i]);
    if ( j > max ) max = j;
  }

  for(unsigned int i=0; hls && hls->hlsInterpretations[i]; i++)
  {
    unsigned int j = numRuleBindings(hls->hlsInterpretations[i]);
    if ( j > max ) max = j;
  }

  return max;
}

unsigned int numRuleBindings(struct HLRule * hlr)
{
  if( hlr == NULL ) return 0;

  unsigned int num = numModRefParams(hlr->hlrSP);

  for(unsigned int i=0; hlr->hlrLC && hlr->hlrLC[i]; i++) 
    num += numModRefParams(hlr->hlrLC[i]);
  
  for(unsigned int i=0; hlr->hlrRC && hlr->hlrRC[i]; i++) 
    num += numModRefParams(hlr->hlrRC[i]);

  num += arrayLength((void**)hlr->hlrVardecs);

  return num;
}

unsigned int numModRefParams(struct HModRef * hmr)
{ 
  unsigned int num = 0;
  while(hmr && hmr->hmrParameters && hmr->hmrParameters[num]) num++;
  return num;
}

unsigned int directiveLength(HDirective** hdirs, dir_t hdTy)
{
  switch(hdTy)
  {
    case dAXIOM:
    case dIGNORE:
    case dCONSIDER:
      for(unsigned int i=0; hdirs && hdirs[i]; i++)
          if( hdirs[i]->hdType == hdTy)
            return arrayLength((void**)hdirs[i]->hdData.hddModExps);
      return 0;
    case dDEFINE:
    {
      unsigned int j=0;
      for(unsigned int i=0; hdirs && hdirs[i]; i++)
        if(hdirs[i]->hdType == dDEFINE) 
          j++;
      return j;
    }
    break;
    case dLENGTH:
    case dSEED:
    case dDEPTH:
      return -1;
  }
  return 0;
}

void semantics_cleanup(SymbolTable* st, ILSystem* ils)
{
    st_destroy(st);
    freeILSystem(ils);
}

#endif
