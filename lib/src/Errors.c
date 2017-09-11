#ifndef ERRORS_C
#define ERRORS_C

#include <stdbool.h>
#include <stdlib.h>

#include "clsys.h"
#include "Errors.h"
#include "LSystem.h"
#include "Defines.h"
#include "LinkedList.h"
#include "y.tab.h" // For YYLTYPE (via %bison-locations)


struct LSysErrors {
  bool hasErrors;
  unsigned int numErrors;
  LinkedList* errors;
};

typedef struct LSysError {
  LSysErrorCode errType;
  char errMsg[ERROR_BUFFER_MAX_LENGTH];
} LSysError;

static bool addError(LSysErrors* lerrs, LSysError* lerr);
static void freeLError(LSysError* le);
static char* errCodeToString(LSysErrorCode code);
static bool sprintLocation(char* buf, unsigned int size, YYLTYPE location);

LSysErrors* makeLErrors()
{
  LSysErrors* lerrs = malloc(sizeof *lerrs);

  if ( ! lerrs ) return NULL;

  lerrs->hasErrors = false;
  lerrs->numErrors = 0;
  lerrs->errors    = NULL;

  return lerrs;

}

void freeLErrors(LSysErrors* errs)
{
  if(errs==NULL) return;
  foreach(errs->errors, (void*)(void*)&freeLError);
  free(errs);
}

void freeLError(LSysError* le){ ; } 

bool addGeneralError(LSysErrors* lerrs, LSysErrorCode errCode, char* errMsg)
{
  LSysError* lerr;

  if ( ! ( lerr = malloc(sizeof *lerr) ) )
    return false;

  lerr->errType = errCode;

  if( 
      snprintf(lerr->errMsg, LSYS_ERROR_BUFFER_MAX_LENGTH,"%s", errMsg) < 0 ||
      !addError(lerrs, lerr) 
    ) return freeLError(lerr), false;

  return true;  
}

bool addParseError(LSysErrors* lerrs, LSysErrorCode errCode, char* errMsg, YYLTYPE location)
{

  LSysError* lerr;

  if ( ! ( lerr = malloc(sizeof *lerr) ) )
    return false;

  lerr->errType = errCode;

  char locStr[LSYS_ERROR_BUFFER_MAX_LENGTH];

  if( 
      !sprintLocation(locStr, LSYS_ERROR_BUFFER_MAX_LENGTH, location) ||
      snprintf(
        lerr->errMsg, LSYS_ERROR_BUFFER_MAX_LENGTH, 
        "%s %s: %s", 
        locStr,
        errCodeToString(errCode), 
        errMsg?errMsg:""
        ) < 0 ||
      !addError(lerrs, lerr)
    ) return free(lerr), false;

  return true;  

}

bool sprintLocation(char* buf, unsigned int size, YYLTYPE location)
{

  if ( location.first_line == location.last_line )
  {
    if( location.first_column == location.last_column )
      return ( snprintf(buf, size, "|%d:%-d|", location.first_line+1, location.first_column) > 0 );
    else     
      return ( snprintf(buf, size, "|%d:%-d-%-d|", location.first_line+1, location.first_column, location.last_column) > 0 );
  } else 
    return ( snprintf(buf, size, "|%d:%-d - %d:%-d|", location.first_line+1, location.first_column, location.last_line+1, location.last_column) > 0 );

}

bool addErrorPredefinedModuleParameterCountMismatch(
    LSysErrors*     lerrs,
    char*      moduleName,
    unsigned int expected,
    unsigned int received,
    YYLTYPE  locSeenAgain
    )
{


  LSysError* lerr;

  if ( ! ( lerr = malloc(sizeof *lerr) ) )
    return false;
  lerr->errType = lsys_error_module_parameter_count_mismatch; 

  char againLocStr[LSYS_ERROR_BUFFER_MAX_LENGTH];

  if( 
      !sprintLocation(againLocStr, LSYS_ERROR_BUFFER_MAX_LENGTH, locSeenAgain) ||
      snprintf(
        lerr->errMsg, LSYS_ERROR_BUFFER_MAX_LENGTH, 
        "%s Expected %d parameters but recieved %d for predefined module '%s'",
        againLocStr, expected, received, moduleName
        ) < 0 ||
      !addError(lerrs, lerr)
    ) return free(lerr), false;

  return true;
}

bool addErrorModuleParameterCountMismatch(
    LSysErrors*     lerrs,
    char*      moduleName,
    unsigned int expected,
    unsigned int received,
    YYLTYPE  locSeenFirst,
    YYLTYPE  locSeenAgain
    ){

  LSysError* lerr;

  if ( ! ( lerr = malloc(sizeof *lerr) ) )
    return false;
  lerr->errType = lsys_error_module_parameter_count_mismatch; 

  char firstLocStr[LSYS_ERROR_BUFFER_MAX_LENGTH];
  char againLocStr[LSYS_ERROR_BUFFER_MAX_LENGTH];



  if( 
      !sprintLocation(firstLocStr, LSYS_ERROR_BUFFER_MAX_LENGTH, locSeenFirst) ||
      !sprintLocation(againLocStr, LSYS_ERROR_BUFFER_MAX_LENGTH, locSeenAgain) ||
      snprintf(
        lerr->errMsg, LSYS_ERROR_BUFFER_MAX_LENGTH, 
        "%s Expected %d parameters but recieved %d for module '%s' (first seen at %s)",
        againLocStr, expected, received, moduleName, firstLocStr
        ) < 0 ||
      !addError(lerrs, lerr)
    ) return free(lerr), false;

  return true;  

}
bool addErrorDuplicateParameterName(
    LSysErrors*     lerrs,
    char*   parameterName,
    YYLTYPE locBoundFirst,
    YYLTYPE locBoundAgain
    ){

  LSysError* lerr;

  if ( ! ( lerr = malloc(sizeof *lerr) ) )
    return false;
  lerr->errType = lsys_error_parameter_binding;

  if( 
      snprintf(
        lerr->errMsg, LSYS_ERROR_BUFFER_MAX_LENGTH, 
        "|%3d:%-3d - %3d:%-3d| Identifier '%s' cannot be bound twice in the same scope (first defined at |%3d:%-3d - %3d:%-3d|)",
        locBoundAgain.first_line, locBoundAgain.first_column, 
        locBoundAgain.last_line,  locBoundAgain.last_column, 
        parameterName, 
        locBoundFirst.first_line, locBoundFirst.first_column, 
        locBoundFirst.last_line,  locBoundFirst.last_column
        ) < 0 ||
      !addError(lerrs, lerr)
    ) return free(lerr), false;

  return true;  

}


char* errCodeToString(LSysErrorCode code)
{
  switch(code)
  {
    case lsys_error_memory_full:                         return "Memory Full";
    case lsys_error_failed_initialization:               return "Failed Initialization";
    case lsys_error_compiler_error:                      return "Compiler Error";
    case lsys_error_file_io:                             return "File IO";
    case lsys_error_file_not_found:                      return "File Not Found";
    case lsys_error_invalid_float_value:                 return "Invalid Float Value";
    case lsys_error_invalid_declaration:                 return "Invalid Declaration";
    case lsys_error_module_parameter_count_mismatch:     return "Module Parameter Count Mismatch";
    case lsys_error_module_expression:                   return "Module Expression";
    case lsys_error_module_reference:                    return "Module Reference";
    case lsys_error_module_undefined:                    return "Module Undefined";
    case lsys_error_parameter_binding:                   return "Parameter Binding";
    case lsys_error_variable_declaration:                return "Variable Declaration";
    case lsys_error_variable_undefined:                  return "Variable Undefined";
    case lsys_error_duplicate_global_id:                 return "Duplicate Global Id";
    case lsys_error_inconsistent_module_argument_counts: return "Inconsistent Module Argument Counts";
    case lsys_error_inconsistent_type:                   return "Inconsistent Type in Expression";
    case lsys_error_parsing_error:                       return "Error";
    case lsys_error_derivation_error:                    return "Derivation Error";
    case lsys_error_none:                                return "No Error";
  }
  return "";

}

bool addError(LSysErrors* lerrs, LSysError* lerr)
{

  LinkedList* ll = cons(lerr, lerrs->errors);
  if ( !ll  )
    return false;

  lerrs->errors = ll;
  lerrs->numErrors++;
  lerrs->hasErrors = true;

  return true;
}



extern bool          lsys_hasErrors   (CLSYS_LSystem * lsys)
{
  if(lsys == NULL || lsys->errors == NULL) return false;

  return lsys->errors->hasErrors;

}
extern unsigned int  lsys_numErrors   (CLSYS_LSystem * lsys)
{
  if(lsys == NULL || lsys->errors == NULL) return false;

  return lsys->errors->numErrors;

}
extern LSysErrorCode lsys_getErrorCode(CLSYS_LSystem * lsys, unsigned int err_i)
{

  if(lsys == NULL || lsys->errors == NULL) return lsys_error_none;

  LinkedList* ll = lsys->errors->errors;
  for(unsigned int i=1; i <= err_i && ll; i++, ll = ll->llNext)
    if( err_i == i )
      return ((LSysError*)ll->llData)->errType;

  return lsys_error_none;

}
extern char*         lsys_getErrorMsg (CLSYS_LSystem * lsys, unsigned int err_i)
{

  if(lsys == NULL || lsys->errors == NULL) return errCodeToString(lsys_error_none);  

  LinkedList* ll = lsys->errors->errors;
  for(unsigned int i=1; i <= err_i && ll; i++, ll = ll->llNext)
    if( err_i == i )
      return ((LSysError*)ll->llData)->errMsg;

  return errCodeToString(lsys_error_none);

}

#endif
