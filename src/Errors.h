#ifndef ERRORS_H 
#define ERRORS_H

#define ERROR_BUFFER_MAX_LENGTH 1024

#include <stdbool.h>
#include "clsys.h"
#include "y.tab.h"

typedef struct LSysErrors LSysErrors;

LSysErrors* makeLErrors () ;
void        freeLErrors (LSysErrors*);

bool 
 addGeneralError(
  LSysErrors*     lerrs,
  LSysErrorCode errCode,
  char*          errMsg
 );
  

bool 
 addParseError(
  LSysErrors*     lerrs,
  LSysErrorCode errCode,
  char*          errMsg,
  YYLTYPE      location
 );

bool addErrorModuleParameterCountMismatch(
  LSysErrors*     lerrs,
  char*      moduleName,
  unsigned int expected,
  unsigned int received,
  YYLTYPE  locSeenFirst,
  YYLTYPE  locSeenAgain

 );

bool addErrorDuplicateParameterName(
  LSysErrors*     lerrs,
  char*   parameterName,
  YYLTYPE locBoundFirst,
  YYLTYPE locBoundAgain
 );

bool addErrorPredefinedModuleParameterCountMismatch(
    LSysErrors*     lerrs,
    char*      moduleName,
    unsigned int expected,
    unsigned int received,
    YYLTYPE  locSeenAgain
  );


#endif
