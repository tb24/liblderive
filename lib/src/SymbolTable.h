#ifndef SYMBOL_TABLE_H 
#define SYMBOL_TABLE_H

/* 
 * SymbolTable
 *
 * Assists conversion between elements of the human-readable lsystem
 * into the internal representation used by the derivation engine.
 *
 * Reports errors when this conversion is detected to fail
 *
 */
#include <stdbool.h>

#include "HLSystem.h"
#include "ILSystem.h"
#include "Errors.h"

typedef enum {
  slPredefined = -1,
  slGlobal,
  slContext,
  slProduction,
  slNumScopeLevels
} ScopeLevel;

typedef struct SymbolTable SymbolTable;

SymbolTable* st_new();

// Essentially, users can only introduce the following two types of symbols
//
// Modules have an id and the number of arguments
// The bindings all exist at global scope
bool st_insert_moduleType(LSysErrors*, SymbolTable*, HId*, unsigned int);
//
// Variables are identified by id 
// The lexical position is ignored; it's order of insertion into the symbol table that matters
bool st_insert_variableBinding(LSysErrors*, SymbolTable*, ScopeLevel, HId*);
//
// Remember to do this when we ascending out of a scope level
bool st_clear_scope(SymbolTable*, ScopeLevel);

// Once all the symbols at a given scope have been inserted, 
// the following will provide methods to determine their
// internal representations
//
// Sets the last argument to the offset of the given variable identifier
bool st_get_var_offset(LSysErrors*, SymbolTable*, ScopeLevel, HId* id, IParamId*);


// These always return
int   st_num_variables(SymbolTable*, ScopeLevel);
int   st_num_moduleTypes(SymbolTable*);
// These can fail
int   st_num_module_parameters(LSysErrors*, SymbolTable*, HId* moduleId);
int   st_num_module_parameters_fromMt(LSysErrors*, SymbolTable* st, IModType mt);
char* st_module_name_fromMt(LSysErrors*, SymbolTable*, IModType);
//
// Sets final param to the id of the module
bool st_get_mod_id(LSysErrors*, SymbolTable*, HId* id, IModType*);

IModType st_draw_type_from_module_name(LSysErrors* lerrs, SymbolTable* st, char* modname);

IModType st_max_modType(LSysErrors* lerrs, SymbolTable* st);

void st_destroy(SymbolTable*);

#endif
