#ifndef PARSER_C
#define PARSER_C

#include <stdio.h>

#include "clsys.h"
#include "Parser.h"
#include "LSystem.h"
#include "Errors.h"
#include "Semantics.h"
#include "Memos.h"


// Hackery - Please read up on re-entrant modes for bison and flex documentation
int yyparse(ParserParam *param);
int yylex_init_extra(void*data, void* scanner);
int yylex_destroy(void* scanner);
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_string(char* string, void* scanner);
int yy_delete_buffer(YY_BUFFER_STATE, void* scanner);



int initParserParam(ParserParam* param, LSysErrors* errors)
{
    int ret = 0;
  
    if( ! (param->parseMemos = memos_new()) )
      return 0;

    param->parseValue = NULL;
    param->errors = errors;

    if( (ret = yylex_init_extra( param->parseMemos, &param->scanner ) ) )
      return memos_free(param->parseMemos), ret;
     
    return ret;
}

int destroyParserParam(ParserParam* param)
{
  // We can now get rid of the data only needed during the parse
  memos_free(param->parseMemos);
  return yylex_destroy(param->scanner);
}

// Returns the internal representation of an CLSYS_LSystem  or NULL; stores any errors that happen
ILSystem* parseFromString(char* lsysString, LSysErrors* errors)
{
  ILSystem* ils;
  YY_BUFFER_STATE state;
  ParserParam parseParam;

  if (initParserParam(&parseParam, errors))
    return addGeneralError(
              errors, 
              lsys_error_failed_initialization, 
              "Couldn't initialize parser parameters"
           ), NULL;


  state = yy_scan_string(lsysString, parseParam.scanner);

  // The parser sets appropriate error messages
  if ( yyparse(&parseParam) )
    return addGeneralError(
              errors, 
              lsys_error_parsing_error, 
              "Parser could not recognize L-System" 
           ), 
           destroyParserParam(&parseParam), 
           NULL;
  
  // Now check if semantic value is valid
  if( ! ( ils = check_hlsystem(parseParam.parseValue, errors) ) )
    return addGeneralError(
              errors, 
              lsys_error_parsing_error, 
              "There were semantic errors"
           ), 
      yy_delete_buffer(state, parseParam.scanner),  
      destroyParserParam(&parseParam), 
      NULL;
  
  // cleanup parser data
  yy_delete_buffer(state, parseParam.scanner);
  destroyParserParam(&parseParam);

  return ils; 
}

#endif // PARSER_C
