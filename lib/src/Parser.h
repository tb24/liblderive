/*
 * Parser.h
 *
 * Definition of the structure used internally by the parser and lexer
 * to exchange data.
 *
 * Definitions of the parameters for the reentrant functions
 * of flex (yylex) and bison (yyparse)
 */
 
#ifndef PARSER_H
#define PARSER_H
 
#ifndef YY_NO_UNISTD_H
#define YY_NO_UNISTD_H 1
#endif // YY_NO_UNISTD_H

#include "clsys.h" 
#include "Memos.h"
#include "Errors.h"
#include "AstNode.h"
#include "HLSystem.h"
#include "ILSystem.h"

/**
 * @brief structure given as argument to the reentrant 'yyparse' function.
 */
#ifndef YY_TYPEDEF_YY_SCANNER_T
#define YY_TYPEDEF_YY_SCANNER_T
typedef void* yyscan_t;
#endif

typedef struct ParserParam
{
    yyscan_t   scanner;
    HLSystem*  parseValue; // The sematic value of a successful parse    
    Memos*     parseMemos; // For keeping track of temporary storage 
    LSysErrors*    errors; // For reporting errors 
    
} ParserParam;

int initParserParam(ParserParam* param, LSysErrors* errors);
int destroyParserParam(ParserParam* param);
ILSystem* parseFromString(char* lsysString, LSysErrors* errors);

// the parameter name (of the reentrant 'yyparse' function)
// data is a pointer to a 'ParserParam' structure
#define YYPARSE_PARAM pparam 
 
#endif // PARSER_H
