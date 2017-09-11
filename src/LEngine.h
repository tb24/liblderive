#ifndef LENGINE_H
#define LENGINE_H

#include <stdio.h>
#include <stdbool.h>

#include "Errors.h"
#include "LString.h"
#include "ILSystem.h"

// Returns null on error
CLSYS_LEngine* le_initFromIls(LSysErrors* lerrs, ILSystem*);

// Returns false on error
bool     le_derive(LSysErrors* lerrs, CLSYS_LEngine* le);

// Interprets the lstring.
bool     
  le_interpret(
    LSysErrors* lerrs,
    CLSYS_LEngine* le,
    LString** lstrPtr
  );

// Needed for making lstrings for use with this engine
// It's the number of parameters each module has
LString* le_currStr(CLSYS_LEngine* le);

// Can be useful; the haltFlag is checked during each derivation step
void le_setHaltFlagPtr(CLSYS_LEngine*, unsigned int*);
void le_fprintLString(FILE* fout, CLSYS_LEngine* le);

void freeLEngine(CLSYS_LEngine*);

typedef enum {
  lsys_lc_productions,
  lsys_rc_productions,

  lsys_lc_decompositions,
  lsys_rc_decompositions,

  lsys_lc_interpretations,
  lsys_rc_interpretations

} LSysRuleNeighborType;


bool le_isPLCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName);
bool le_isPRCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName);
bool le_isDLCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName);
bool le_isDRCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName);
bool le_isILCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName);
bool le_isIRCNeighborForRule(CLSYS_LEngine* le, unsigned int rule_i, char* modName);

// This is in bytes
unsigned int le_lengthLString(CLSYS_LEngine*le);

#endif
