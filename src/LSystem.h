#ifndef LSYSTEM_H
#define LSYSTEM_H

#include "clsys.h"
#include "Errors.h"
#include "LEngine.h"
#include "ILSystem.h"
#include "Defines.h"

struct CLSYS_LSystem  {

  ILSystem* ils; // Internal representation
  CLSYS_LEngine*  le;  // Holds derivation state

  LSysErrors* errors; // Error reporting

};

CLSYS_LSystem* emptyLSystem();
void     freeLSystem(CLSYS_LSystem** lsys);

extern bool lsys_derive(CLSYS_LSystem* lsys);
extern bool lsys_deriveOnce(CLSYS_LSystem* lsys);

extern bool          lsys_hasErrors   (CLSYS_LSystem*);
extern unsigned int  lsys_numErrors   (CLSYS_LSystem*);
extern LSysErrorCode lsys_getErrorCode(CLSYS_LSystem*, unsigned int);
extern char*         lsys_getErrorMsg (CLSYS_LSystem*, unsigned int);

#endif
