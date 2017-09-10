#ifndef LSYSTEM_C
#define LSYSTEM_C

#include "clsys.h"
#include "LSystem.h"
#include "Parser.h"
#include "Errors.h"

static bool lsys_parseAndInitialize(char* string, CLSYS_LSystem * lsys);

CLSYS_LEngine* lsys_getLEngine(CLSYS_LSystem* lsys)
{
  return  lsys->le;
}

void lsys_set_max_age(unsigned int age, CLSYS_LSystem* lsys)
{
  lsys->ils->ilsDerivationLength = age;
}

extern bool lsys_initFromFile(FILE* lsysFile, CLSYS_LSystem ** lsys)
{
  long lSize;
  char * buffer;
  size_t result;
  bool retVal;
  
  *lsys = NULL;

  if (lsysFile==NULL)
    return false;
  
  if( !( *lsys = emptyLSystem() ) )
    return false;

  // obtain file size:
  fseek (lsysFile , 0 , SEEK_END);
  lSize = ftell (lsysFile);
  rewind (lsysFile);

  // allocate memory to contain the whole file plus room for a null character
  if (! (buffer = (char*) malloc (lSize+1 * sizeof *buffer) ) )
    return addGeneralError(
            (*lsys)->errors, 
            lsys_error_memory_full, 
            "Could not create buffer to hold file contents"
           ), false;

  // copy the file into the buffer:
  result = fread (buffer,1,lSize,lsysFile);
  if (result != lSize)
    return addGeneralError(
            (*lsys)->errors, 
            lsys_error_file_io, 
            "Problems when reading from file"
           ), false;

  // don't forget the eof  marker ;)
  buffer[lSize] = '\0';

  retVal = lsys_parseAndInitialize(buffer, *lsys);
  
  free (buffer);
  return retVal;
}

extern bool lsys_initFromString(char* lsysString, CLSYS_LSystem ** lsys)
{

  *lsys = NULL;
  if( !( *lsys = emptyLSystem() ) )
    return false;
  
  return  lsys_parseAndInitialize(lsysString, *lsys);
}

bool lsys_parseAndInitialize(char* string, CLSYS_LSystem * lsys)
{
  
  // First try and parse the string
  if( ! (lsys->ils = parseFromString(string, lsys->errors) ) )
    return addGeneralError(
            lsys->errors, 
            lsys_error_failed_initialization, 
            "Couldn't parse L-System description"
           ), false;

  // Now initalize the CLSYS_LEngine for derivation
  if( !( lsys->le = le_initFromIls(lsys->errors, lsys->ils) ) )
    return addGeneralError(
            lsys->errors, 
            lsys_error_failed_initialization, 
            "Couldn't initialize derivation engine"
           ), false;

  return true;
}

extern void lsys_freeLSystem (CLSYS_LSystem * lsys)
{
  if(lsys == NULL) return;

  freeLEngine(lsys->le);
  freeILSystem(lsys->ils);
  freeLErrors(lsys->errors);
  free(lsys);

}

//extern bool lsys_derive(CLSYS_LSystem * lsys);
extern bool lsys_deriveOnce(CLSYS_LSystem * lsys)
{
  return le_derive(lsys->errors, lsys->le);
}

extern unsigned int lsys_length_lstring(CLSYS_LSystem* lsys)
{
  return le_lengthLString(lsys->le);
}
//extern LsysVertexBuffersES lsys_vertex_buffers(CLSYS_LSystem * lsys);
extern void lsys_fprint_lstring(FILE* fout, CLSYS_LSystem * lsys)
{
  le_fprintLString(fout, lsys->le);
}

//extern bool          lsys_hasErrors   (CLSYS_LSystem *);
//extern unsigned int  lsys_numErrors   (CLSYS_LSystem *);
//extern LsysErrorCode lsys_getErrorCode(CLSYS_LSystem *, unsigned int);
//extern char*         lsys_getErrorMsg (CLSYS_LSystem *, unsigned int);

CLSYS_LSystem * emptyLSystem ()
{
  CLSYS_LSystem * lsys = NULL;

  if( ! ( lsys = malloc(sizeof *lsys) ) )
    return NULL;
  
  lsys->le = NULL;
  lsys->ils = NULL;

  if ( ! (lsys->errors = makeLErrors()) )
    return free(lsys), NULL;

  return lsys;
}

#endif
