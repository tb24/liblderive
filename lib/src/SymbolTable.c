#ifndef SYMBOL_TABLE_C 
#define SYMBOL_TABLE_C

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "SymbolTable.h"
#include "Common.h"
#include "LinkedList.h"

// For YYLTYPE
#include "Parser.h"
#include "y.tab.h"

typedef enum {
  scModule,
  scVariable,
  scFunctionCall
} SymbolClass;

typedef enum {
  iscModule,
  iscVariable,
  iscFunctionCall
} ISymbolClass;

typedef enum {
  stFloat,
  stBool,
  stModule
} SymbolType;

// Symbols stored internally
typedef struct {
  char*        sId;      
  SymbolClass  sClass;
  SymbolType   sType;
  unsigned int sModNumArgs;
  unsigned int sModTypeId;
  YYLTYPE      sLocation;
} Symbol;

// Symbols coming out
typedef struct {
  IParamId     isParamId;
  IModType     isModId;
  ISymbolClass isClass;
  unsigned int isModParamCount;
} ISymbol;


struct SymbolTable {
  Symbol*predefines;
  Symbol *symbols[slNumScopeLevels];
  unsigned int numSymbols[slNumScopeLevels];
  unsigned int numVariables[slNumScopeLevels];
  unsigned int symbolsLength[slNumScopeLevels];
  unsigned int numModuleTypes;
};

// Needs to be in the same order as PreDefModTypes in Defines.h  
Symbol predefs[] = {
  { "``"  , scModule, stModule , 0, mtStartString },
  { "''"  , scModule, stModule , 0, mtEndString   },
  { "*"   , scModule, stModule , 0, mtAny         }, // not currently parsed
  { "F"   , scModule, stModule , 1, mtForward     },
  { "f"   , scModule, stModule , 1, mtTiptoe      },
  { "o"   , scModule, stModule , 1, mtCircle      },
  { "O"   , scModule, stModule , 1, mtSphere      },
  { "\\"  , scModule, stModule , 1, mtRollLeft    },
  { "/"   , scModule, stModule , 1, mtRollRight   },
  { "+"   , scModule, stModule , 1, mtLeft        },
  { "-"   , scModule, stModule , 1, mtRight       },
  { "^"   , scModule, stModule , 1, mtUp          },
  { "&"   , scModule, stModule , 1, mtDown        },
  { "|"   , scModule, stModule , 0, mtAboutFace   },
  { "#"   , scModule, stModule , 1, mtWidth       },
  { "!"   , scModule, stModule , 3, mtColor       },
  { "!h"  , scModule, stModule , 1, mtColorHue    },
  { "!s"  , scModule, stModule , 1, mtColorSat    },
  { "!v"  , scModule, stModule , 1, mtColorVal    },
  { "["   , scModule, stModule , 0, mtPush        },
  { "]"   , scModule, stModule , 0, mtPop         },
  { "[["  , scModule, stModule , 0, mtRePush      },
  { "]]"  , scModule, stModule , 0, mtUnPush      },
  { "{"   , scModule, stModule , 0, mtTriStart    },
  { "}"   , scModule, stModule , 0, mtTriEnd      },
  { "TP"  , scModule, stModule , 0, mtTriPoint    }
};

static void freeScope(SymbolTable*, ScopeLevel);
static void freeSymbol(Symbol);

static bool st_make_more_room(SymbolTable* st, ScopeLevel sl);
static bool st_sym_eq_hid(Symbol s, HId* hid);

SymbolTable* st_new()
{
  SymbolTable * st = malloc(sizeof(*st));


  st->numModuleTypes = NUM_PREDEF_MOD_TYPES;
  st->predefines = predefs;

  for(unsigned int i=0; i < slNumScopeLevels; i++)
  {
    st->symbols[i] = NULL;
    st->numSymbols[i] = 0;
    st->numVariables[i] = 0;
    st->symbolsLength[i] = 0;
  }

  return st;
}

bool st_get_var_offset(LSysErrors* lerrs, SymbolTable* st, ScopeLevel sl, HId* id, IParamId* offset)
{
  switch(sl)
  {
    case slProduction:
      for(unsigned int i=0; st->symbols[slProduction] != NULL && i < st->numSymbols[slProduction]; i++)
        if( st->symbols[slProduction][i].sClass == scVariable && 
            st_sym_eq_hid(st->symbols[slProduction][i], id)
        ){
          *offset = st->numVariables[slGlobal] + st->numVariables[slContext] + i;
          return true;
        }
    case slContext:
      for(unsigned int i=0; st->symbols[slContext] != NULL && i < st->numSymbols[slContext]; i++)
        if( st->symbols[slContext][i].sClass == scVariable && 
            st_sym_eq_hid(st->symbols[slContext][i], id)
        ){
          *offset = st->numVariables[slGlobal] + i;
          return true;
        }
    case slGlobal:  // Remember the module declarations are also in there
      for(unsigned int i=0, j=0; st->symbols[slGlobal] && i < st->numSymbols[slGlobal]; i++){
        if(st->symbols[slGlobal][i].sClass == scVariable)
        {
          if(st_sym_eq_hid(st->symbols[slGlobal][i], id))
            return *offset = j, true;
          j++;
        }
      }
    // There are no predefines variables at the moment
    // Consider adding PI, PHI, the cosmological constant, 42, etc
    case slPredefined:
    case slNumScopeLevels:
      break;
  }
  // Report error that the variable is out of scope
  return addParseError(lerrs, lsys_error_variable_undefined, id->hidString, id->yylocation), false;
}

bool st_get_mod_id(LSysErrors* lerrs, SymbolTable* st, HId* id, IModType* imt)
{
  // Inspect the global and predefines scopes for module id's
  for(unsigned int i=0; st->symbols[slGlobal] != NULL && i < st->numSymbols[slGlobal]; i++) 
    if( st->symbols[slGlobal][i].sClass == scModule && 
        st_sym_eq_hid(st->symbols[slGlobal][i], id) 
    ) return *imt = st->symbols[slGlobal][i].sModTypeId, true;
    
  for(unsigned int i=0; i < NUM_PREDEF_MOD_TYPES; i++)
    if( st->predefines[i].sClass == scModule && 
        st_sym_eq_hid(st->predefines[i], id) 
    ) return *imt = st->predefines[i].sModTypeId, true;
  
  // TODO ADD ERROR BIGTIME TODO //
  *imt = 0;
  addParseError(lerrs, lsys_error_module_undefined, id->hidString, id->yylocation);
  return false;
}

// Module types are all available at global scope
bool st_insert_moduleType(LSysErrors* lerrs, SymbolTable* st, HId* modId, unsigned int numArgs)
{

  // Check for predefined modules first
  // There is no need to insert these
  for(unsigned int i=0; i < NUM_PREDEF_MOD_TYPES; i++)
    if( st->predefines[i].sClass == scModule && 
        st_sym_eq_hid(st->predefines[i], modId) 
    ) return ( 
        (numArgs == st->predefines[i].sModNumArgs) ? 
        true : 
        (addErrorPredefinedModuleParameterCountMismatch(
          lerrs, modId->hidString,
          st->predefines[i].sModNumArgs, numArgs,
          modId->yylocation 
          ),
        false)
      );

  // Now check existing modules
  // there may be a mismatch in the number of parameters
  unsigned int i;
  for(i=0; st->symbols[slGlobal] != NULL && i < st->numSymbols[slGlobal]; i++)
    if( st->symbols[slGlobal][i].sClass == scModule &&
        st_sym_eq_hid(st->symbols[slGlobal][i], modId) 
    ) return ( 
        (st->symbols[slGlobal][i].sModNumArgs == numArgs ) ? 
        true :  
        (addErrorModuleParameterCountMismatch(
          lerrs,
          modId->hidString,
          st->symbols[slGlobal][i].sModNumArgs, numArgs,
          st->symbols[slGlobal][i].sLocation,   modId->yylocation 
          ),
        false)
        );

  // We didn't find any modules with the same id; so insert a new one

  // Make sure there is space, of course
  if( i == st->symbolsLength[slGlobal] )
    if ( ! st_make_more_room(st, slGlobal) )
      return addGeneralError(lerrs, lsys_error_memory_full, NULL), false;

  st->symbols[slGlobal][i].sId = my_strndup(modId->hidString, MAX_LEN_IDENTIFIER);
  st->symbols[slGlobal][i].sClass = scModule;
  st->symbols[slGlobal][i].sType  = stModule;
  st->symbols[slGlobal][i].sModNumArgs = numArgs;
  st->symbols[slGlobal][i].sModTypeId = st->numModuleTypes++;
  st->symbols[slGlobal][i].sLocation  = modId->yylocation;
  st->numSymbols[slGlobal]++;

  return true;
}

bool st_insert_variableBinding(LSysErrors* lerrs, SymbolTable* st, ScopeLevel sl, HId* varId)
{
  if( sl == slPredefined || sl == slNumScopeLevels ) 
      return 
        addGeneralError(
          lerrs, 
          lsys_error_compiler_error, 
          "Attempted to add variable to an invalid scope!"
        ), false;

  unsigned int i;
  for(i=0; st->symbols[sl] != NULL && i < st->numSymbols[sl]; i++){
    if( st->symbols[sl][i].sClass == scVariable && 
        st_sym_eq_hid(st->symbols[sl][i], varId)
    ) return 
        addErrorDuplicateParameterName( 
          lerrs, varId->hidString,
          st->symbols[sl][i].sLocation,
          varId->yylocation
        ), false;
 }
  if( i == st->symbolsLength[sl] )
    if ( ! st_make_more_room(st, sl) )
        addGeneralError(
          lerrs, 
          lsys_error_memory_full, 
          "Couldn't create more room in symbol table"
        ), false;

  st->symbols[sl][i].sId = my_strndup(varId->hidString, MAX_LEN_IDENTIFIER);
  st->symbols[sl][i].sClass = scVariable;
  st->symbols[sl][i].sType  = stFloat; // yep, all floats for now
  st->symbols[sl][i].sLocation = varId->yylocation; 
  st->numSymbols[sl]++;

  st->numVariables[sl]++;
  
  return true;
}

static bool st_sym_eq_hid(Symbol s, HId* hid)
{
  return (strcmp(s.sId, hid->hidString) == 0);
}

static bool st_make_more_room(SymbolTable* st, ScopeLevel sl)
{
  Symbol* tmpSyms = realloc(st->symbols[sl], (st->symbolsLength[sl]+=50)*sizeof(*st->symbols[sl]));

  if ( tmpSyms == NULL ) 
    return false;
  
  st->symbols[sl] = tmpSyms;
  return true;
}

bool st_clear_scope(SymbolTable* st, ScopeLevel sl)
{
  switch(sl)
  {
    case slGlobal:
    case slContext:
    case slProduction:
      for(unsigned int i=sl; i < slNumScopeLevels; i++) 
      {
        freeScope(st, i);
      }
      return true;
    break;
    default: // Cannot clear predefines scope
      return false;
  }
}


void st_destroy(SymbolTable* st)
{
  freeScope(st, slGlobal);
  freeScope(st, slContext);
  freeScope(st, slProduction);
  free(st);
}

/// Helpers that are not exposed to the interface

void freeScope(SymbolTable* st, ScopeLevel sl)
{
  if( sl < 0 || sl >= slNumScopeLevels || sl == slPredefined) 
    return;

  if(st->symbols[sl] != NULL)
  { 
    for(unsigned int i=0; i < st->numSymbols[sl]; i++)
      freeSymbol(st->symbols[sl][i]);
    free(st->symbols[sl]);
  }
  st->symbols[sl] = NULL; 
  st->numSymbols[sl] = 0;
  st->numVariables[sl] = 0;
  st->symbolsLength[sl] = 0;

}

void freeSymbol(Symbol s)
{
  if(s.sId != NULL) 
    free(s.sId);
}

int st_num_variables(SymbolTable* st, ScopeLevel sl)
{
  return st->numVariables[sl];
}

int st_num_moduleTypes(SymbolTable* st)
{
  return st->numModuleTypes;
}

int st_num_module_parameters_fromMt(LSysErrors* lerrs, SymbolTable* st, IModType mt)
{
  for(unsigned int i=0; i < NUM_PREDEF_MOD_TYPES; i++)
    if ( st->predefines[i].sClass == scModule &&
         st->predefines[i].sModTypeId == mt
    ) return st->predefines[i].sModNumArgs;

  for(unsigned int i=0; i < st->numSymbols[slGlobal]; i++)
    if ( st->symbols[slGlobal][i].sClass == scModule &&
         st->symbols[slGlobal][i].sModTypeId == mt
    ) return st->symbols[slGlobal][i].sModNumArgs;

  // TODO ERROR TODO Couldn't find module id;
  return 0;

}

char* st_module_name_fromMt(LSysErrors* lerrs, SymbolTable* st, IModType mt)
{
  for(unsigned int i=0; i < NUM_PREDEF_MOD_TYPES; i++)
    if ( st->predefines[i].sClass == scModule &&
         st->predefines[i].sModTypeId == mt
    ) return st->predefines[i].sId;

  for(unsigned int i=0; i < st->numSymbols[slGlobal]; i++)
    if ( st->symbols[slGlobal][i].sClass == scModule &&
         st->symbols[slGlobal][i].sModTypeId == mt
    ) return st->symbols[slGlobal][i].sId;

  // TODO ERROR TODO Couldn't find module id;
  //

  return NULL;
}
int st_num_module_parameters(LSysErrors* lerrs, SymbolTable* st, HId* moduleId)
{
  for(unsigned int i=0; i < NUM_PREDEF_MOD_TYPES; i++)
    if( st->predefines[i].sClass == scModule &&
        st_sym_eq_hid(st->predefines[i], moduleId) 
    ) return st->predefines[i].sModNumArgs;

  for(unsigned int i=0; i < st->numSymbols[slGlobal]; i++)
    if( st->symbols[slGlobal][i].sClass == scModule &&
        st_sym_eq_hid(st->symbols[slGlobal][i], moduleId) 
    ) return st->symbols[slGlobal][i].sModNumArgs;

  addGeneralError(
   lerrs, lsys_error_compiler_error, 
    "Invalid use of symbol table; unknown module was requested it's parameter count"
  );
  return -1;
}


IModType st_draw_type_from_module_name(LSysErrors* lerrs, SymbolTable* st, char* modname)
{
    HId tmpHid;
    IModType drawType;
    char* abbrModName;
//    char* tmp;
    abbrModName = strdup(modname);
    // now null terminate it at any occurance of '^'
    for(char * tmp = abbrModName; *tmp != '\0'; tmp++)
      if( *tmp == '^' )
        *tmp = '\0';
 
    tmpHid.hidString = abbrModName;
    if( !st_get_mod_id(lerrs, st, &tmpHid, &drawType) )
      return addGeneralError( lerrs, lsys_error_compiler_error,
        "Couldn't find module type to draw" ), free(abbrModName), 0;

    free(abbrModName);
    return drawType;
}

IModType st_max_modType(LSysErrors* lerrs, SymbolTable* st)
{
  IModType max = 0;

  
  for(unsigned int i=0; i < NUM_PREDEF_MOD_TYPES; i++)
    if ( st->predefines[i].sClass == scModule &&
         st->predefines[i].sModTypeId > max
    ) max = st->predefines[i].sModTypeId;

  for(unsigned int i=0; i < st->numSymbols[slGlobal]; i++)
    if ( st->symbols[slGlobal][i].sClass == scModule &&
         st->symbols[slGlobal][i].sModTypeId > max
    ) max = st->predefines[i].sModTypeId;

  return max;

}

#endif
