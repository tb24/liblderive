#ifndef LIBLSYS_H
#define LIBLSYS_H

/* 
 * LibLSys
 * (c) Thomas Burt 2012
 * algorithmicbotany.org
 *
 * A library for creating and rendering L-systems.
 * Include this header file to use the library. 
 * 
 */

#include <stdio.h>
#include <stdbool.h>
// An opaque reference to the internal data stucture used to store L-Systems
typedef struct CLSYS_LSystem CLSYS_LSystem;
typedef struct CLSYS_LEngine CLSYS_LEngine; 
// lsys_initFrom*: CLSYS_LSystem  description --> CLSYS_LSystem 
// Arguments:
// - A human readable representation of the L-System.
// See CPFG documentation for L-System syntax.
// - The address of a poiner variable to hold the L-System data 
// 
// Return values:
//
// If the function returns TRUE, the value of the second argument
// will be a pointer to an L-System that can be passed to the rest
// of the library functions.
//
// If the function returns FALSE, and the value of the second argument
// is not NULL, it means that there was an error during parsing or semantic
// analysis. The error code can be inspected with lsys_error* functions. 
//
// If the function returns FALSE, and the value of the second argument
// is NULL, you should probably duck and run for cover. 
extern bool lsys_initFromFile(FILE* lsysFile, CLSYS_LSystem** lsys);
extern bool lsys_initFromString(char* lsysString, CLSYS_LSystem** lsys);

// Frees up storage used by the lsystem
extern void lsys_freeLSystem(CLSYS_LSystem*);

// Runs the lsystem to completion
// Returns false if there was an error
extern bool lsys_derive(CLSYS_LSystem* lsys);

// Runs the lsystem for one derivation step
// Returns false if the derivation has completed or if there is an error
extern bool lsys_deriveOnce(CLSYS_LSystem* lsys);

extern bool lsys_interpret(CLSYS_LSystem* lsys);

// Prints the current lstring to the specified file pointer
extern void lsys_fprint_lstring(FILE* fout, CLSYS_LSystem* lsys);

// This is the length in bytes
extern unsigned int lsys_length_lstring(CLSYS_LSystem* lsys);

extern void lsys_set_max_age(unsigned int age, CLSYS_LSystem* lsys);


// The library functions will always return FALSE if any errors have happened
// If this happens, the error code can be inspected 
typedef enum {

  // run for your life errors 
  lsys_error_memory_full,
  lsys_error_failed_initialization,
  lsys_error_compiler_error,
  
  // not so bad errors
  lsys_error_file_io,
  lsys_error_file_not_found,

  // syntax errors
  lsys_error_invalid_float_value,
  lsys_error_invalid_declaration,

  lsys_error_parsing_error,

  // semantic errors
  lsys_error_module_parameter_count_mismatch,
  lsys_error_module_expression,
  lsys_error_module_reference,
  lsys_error_module_undefined,
  lsys_error_parameter_binding,
  lsys_error_variable_declaration,
  lsys_error_variable_undefined,
  lsys_error_duplicate_global_id,
  lsys_error_inconsistent_module_argument_counts,
  lsys_error_inconsistent_type,
  lsys_error_derivation_error,

  // no errors
  lsys_error_none

} LSysErrorCode;

extern bool          lsys_hasErrors   (CLSYS_LSystem*);
extern unsigned int  lsys_numErrors   (CLSYS_LSystem*);
extern LSysErrorCode lsys_getErrorCode(CLSYS_LSystem*, unsigned int);
extern char*         lsys_getErrorMsg (CLSYS_LSystem*, unsigned int);

extern CLSYS_LEngine*      lsys_getLEngine(CLSYS_LSystem*);

#endif
