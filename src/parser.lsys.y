%{
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h> // Link with -lm

#include "clsys.h"
#include "Memos.h"
#include "Errors.h"
#include "Parser.h"
#include "Defines.h"
#include "AstNode.h"
#include "LSystem.h"
#include "HLSystem.h"
#include "LinkedList.h"

// Bison doesn't like stdbool? This should work
#define false 0
#define true  1

// It's a reentrant scanner with locations and a custom parameter 
// Thus, three arguments
int yylex(YYSTYPE *lvalp, YYLTYPE *yylloc, ParserParam* pparam);

void yyerror(YYLTYPE* locp, ParserParam* pparam, const char *msg)
{
  addParseError(pparam->errors, lsys_error_parsing_error, (char*)msg, *locp);
}


%}

%code requires{
typedef struct ParserParam ParserParam;
}


%define api.pure full
%error-verbose
%locations
%defines
%yacc

%param { ParserParam* pparam }


%token tAXIOM tLENGTH tDEPTH tSEED tCONSIDER tIGNORE
%token tPRODUCTIONS tDECOMPOSITIONS tINTERPRETATIONS
%token tLCON tRCON
%token tYIELDS tCOLON tSEMI tNEWLINE tCOMMA tASSIGN 
%token tPLUS tMINUS tASTERISK tBSLASH tFSLASH tPERCENT tCARET
%token tLT tGT tEQ tNEQ tLE tGE tEXCLAIM tEXCLAIMH tEXCLAIMS tEXCLAIMV tAND tOR
%token tLPAREN tRPAREN tLBRACKET tRBRACKET tLBRACE tRBRACE 
%token tDOUBLELBRACKET tDOUBLERBRACKET 
%token tHASH tAMP tF_UPPER tF_LOWER tO_LOWER tO_UPPER tBAR 
%token tEOF

%token <fval> tFVAL
%token <bval> tBVAL
%token <name> tNAME 
%token <name> tFVAL_INVALID

%type <hlsys> LSystem;
%type <ll>  Productions;
%type <ll>  Interpretations;
%type <ll>  Decompositions;
%type <hlr> Rule;
%type <ll>  Rules;
%type <hve> Condition;
%type <ll>  VarDecs;
%type <ll>  VarDecs1;
%type <hvd> VarDec;
%type <ll>  Successor;

%type <hdir> Directive;
%type <ll>   Directives;

%type <hve> Expression;

%type <hmr> ModRef;
%type <ll>  ModRefs;

%type <ll> IdSeq;
%type <ll> IdSeq1;

%type <hid> Id;

%type <hme> ModExp;
%type <ll>  ModExps;

%type <ll> ValExpSeq;
%type <ll> ValExpSeq1;

%nonassoc tAND tOR

%left tLT tLTE tGT tGTE tNEQ tEQ
%left tPLUS tMINUS
%left tASTERISK tFSLASH
%left tCARET 
%left tLPAREN tRPAREN

%right tASSIGN

%%
input   : LSystem  
        { ((ParserParam*)pparam)->parseValue = $1; }

LSystem : Directives Productions Decompositions Interpretations
        {
          $$ = memoize(
                  ((ParserParam*)pparam)->parseMemos,
                  makeHLSystem(
                    memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($1)),
                    memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($2)),
                    memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($3)),
                    memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($4)),
                    @$
                  )
                );
        };

Productions    : tPRODUCTIONS Rules { $$ = $2; }
               ;

Decompositions : tDECOMPOSITIONS Rules { $$ = $2; }
               | /* ε */ { $$ = NULL; } 
               ;


Interpretations: tINTERPRETATIONS Rules { $$ = $2; }
               | /* ε */  { $$ = NULL; }
               ;

Rules : Rules Rule { $$ = memoize(((ParserParam*)pparam)->parseMemos, cons($2, $1)); }
      | /* ε */  { $$ = NULL; }
      ;

Rule : ModRef VarDecs Condition Successor StatementEnd
     {
       $$ = memoize(
              ((ParserParam*)pparam)->parseMemos, 
              makeHRule(
                NULL, // lcon
                $1,
                NULL, // rcon
                memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($2)), 
                $3,
                memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($4)),
                @$
              )
            );
     }
     | ModRefs tLCON ModRef VarDecs Condition Successor StatementEnd
     {
       $$ = memoize(
              ((ParserParam*)pparam)->parseMemos, 
              makeHRule(
               memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($1)),
               $3,
               NULL, // rcon
               memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($4)), 
               $5,
               memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($6)),
               @$
              )
            );
     }
     | ModRef tRCON ModRefs VarDecs Condition Successor StatementEnd
     {
       $$ = memoize(
              ((ParserParam*)pparam)->parseMemos, 
              makeHRule(
                NULL, // lcon
                $1,
                memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($3)),
                memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($4)), 
                $5,
                memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($6)),
                @$
              )
       );
     }
     | ModRefs tLCON ModRef tRCON ModRefs VarDecs Condition Successor StatementEnd
     {
       $$ = memoize(
              ((ParserParam*)pparam)->parseMemos, 
              makeHRule(
                memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($1)),
                $3,
                memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($5)),
                memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($6)), 
                $7,
                memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($8)),
                @$
              )
       );
     }
     ;

VarDecs: /* ε */                  { $$ = NULL; }
// need a better symbol       | tLBRACE VarDecs1 tRBRACE {  $$ = $2;  }

VarDecs1: VarDecs1 VarDec StatementEnd
        {
          $$ = memoize(((ParserParam*)pparam)->parseMemos, cons($2, $1));
        }
        | /* ε */ { $$ = NULL; }
        ;

VarDec: Id tASSIGN Expression
      {
        $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHVarDec($1, $3, @$));
      } 
      ;

Condition: /* ε */           { $$ = NULL; }
         | tCOLON Expression { $$ = $2;} 
         ;

Successor : tYIELDS ModExps { $$ = $2; }
          ;

ModRefs : ModRefs ModRef { $$ = memoize(((ParserParam*)pparam)->parseMemos, cons($2, $1));   }
        | ModRef         { $$ = memoize(((ParserParam*)pparam)->parseMemos, cons($1, NULL)); }
        ;

ModRef  : Id { $$ = memoize(((ParserParam*)pparam)->parseMemos,makeHModRef($1, NULL, @$)); }
        | Id tLPAREN IdSeq tRPAREN
        {
          HId** ids = (HId**)memoize(((ParserParam*)pparam)->parseMemos,llToReversedArray($3));
          $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHModRef($1, ids, @$));
        };

IdSeq : /* ε */ { $$ = NULL; }
      | IdSeq1  { $$ = $1;   }
      ;
   
IdSeq1 : Id 
       { $$ = memoize(((ParserParam*)pparam)->parseMemos, cons($1,NULL));  }
       | IdSeq1 tCOMMA Id 
       { $$ = memoize(((ParserParam*)pparam)->parseMemos, cons($3,$1)); }
       ;

Id : tNAME     { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId($1,   @$)); }
   | tBSLASH   { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("\\", @$));  }
   | tFSLASH   { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("/",  @$));  }
   | tPLUS     { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("+",  @$));  }
   | tMINUS    { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("-",  @$));  }
   | tCARET    { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("^",  @$));  }
   | tAMP      { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("&",  @$));  }
   | tBAR      { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("|",  @$));  }
   | tHASH     { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("#",  @$));  }
   | tEXCLAIM  { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("!",  @$));  }
   | tEXCLAIMH { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("!h",  @$));  }
   | tEXCLAIMS { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("!s",  @$));  }
   | tEXCLAIMV { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("!v",  @$));  }

   
   | tDOUBLELBRACKET { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("[[",  @$));  }
   | tDOUBLERBRACKET { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("]]",  @$));  }
   | tLBRACKET { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("[",  @$));  }
   | tRBRACKET { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("]",  @$));  }
   | tLBRACE   { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("{",  @$));  }
   | tRBRACE   { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHId("}",  @$));  }
   ;

      

Directives : Directives Directive 
           {
             $$ = memoize(((ParserParam*)pparam)->parseMemos, cons($2, $1));
           }
           | /* ε */ { $$ = NULL; }
           ;

Directive : tAXIOM ModExps StatementEnd 
          { 
            struct HModExp** hmes = (struct HModExp**)memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($2));
            $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHDirectiveAxiom(hmes, @$));
          }
          | tLENGTH tFVAL StatementEnd    { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHDirectiveLength((int)$2, @$)); }
          | tSEED   tFVAL StatementEnd    { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHDirectiveSeed((int)$2, @$));   }
          | tDEPTH  tFVAL StatementEnd    { $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHDirectiveDepth((int)$2, @$));   }
          | tCONSIDER ModExps StatementEnd  
          { 
            struct HModExp** hmes = (struct HModExp**)memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($2));
            $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHDirectiveConsider(hmes, @$));   
          }
          | tIGNORE ModExps StatementEnd  
          { 
            struct HModExp** hmes = (struct HModExp**)memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($2));
            $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHDirectiveIgnore(hmes, @$));   
          }
          | VarDec StatementEnd
          {
            $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHDirectiveDefine($1, @$));
          }
          ;

ModExps  : /* ε */ 
         { $$ = NULL; }
         | ModExps ModExp 
         { $$ = memoize(((ParserParam*)pparam)->parseMemos, cons($2, $1)); }
         ;

ModExp  : Id  
        {
          $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHModExp($1, NULL, @$));
        }
        | Id tLPAREN ValExpSeq tRPAREN
        {
          struct HValExp** modVals = (struct HValExp**)memoize(((ParserParam*)pparam)->parseMemos, llToReversedArray($3)); 
          $$ = memoize(((ParserParam*)pparam)->parseMemos, makeHModExp($1, modVals, @$));
        }
        ;


ValExpSeq : /* ε */ { $$ = NULL; }
          | ValExpSeq1  { $$ = $1;   }
          ;
   
ValExpSeq1 : Expression { $$ = memoize(((ParserParam*)pparam)->parseMemos, cons($1,NULL));  }
           | ValExpSeq1 tCOMMA Expression 
           { $$ = memoize(((ParserParam*)pparam)->parseMemos, cons($3,$1)); }
           ;

Expression 
  : Expression tAND    Expression  
  { $$ = 
      memoize(
        ((ParserParam*)pparam)->parseMemos, 
        BinaryExp(
          hOpAnd, 
          memoize(
            ((ParserParam*)pparam)->parseMemos, 
            BinaryArgs($1, $3)), 
          @$
          )); 
  }
  | Expression tOR     Expression  
  { $$ = memoize(((ParserParam*)pparam)->parseMemos, BinaryExp(hOpOr,  memoize(((ParserParam*)pparam)->parseMemos, BinaryArgs($1, $3)), @$ )); }
  | Expression tLT     Expression  
  { $$ = memoize(((ParserParam*)pparam)->parseMemos, BinaryExp(hOpLt,  memoize(((ParserParam*)pparam)->parseMemos, BinaryArgs($1, $3)), @$ )); }
  | Expression tLTE    Expression  
  { $$ = memoize(((ParserParam*)pparam)->parseMemos, BinaryExp(hOpLte, memoize(((ParserParam*)pparam)->parseMemos, BinaryArgs($1, $3)), @$ )); }
  | Expression tEQ     Expression  
  { $$ = memoize(((ParserParam*)pparam)->parseMemos, BinaryExp(hOpEq,  memoize(((ParserParam*)pparam)->parseMemos, BinaryArgs($1, $3)), @$ )); }
  | Expression tNEQ    Expression                                                                                                      
  { $$ = memoize(((ParserParam*)pparam)->parseMemos, BinaryExp(hOpNeq, memoize(((ParserParam*)pparam)->parseMemos, BinaryArgs($1, $3)), @$ )); }
  | Expression tGT     Expression                                                                                                      
  { $$ = memoize(((ParserParam*)pparam)->parseMemos, BinaryExp(hOpGt,  memoize(((ParserParam*)pparam)->parseMemos, BinaryArgs($1, $3)), @$ )); }
  | Expression tGTE    Expression  
  { $$ = memoize(((ParserParam*)pparam)->parseMemos, BinaryExp(hOpGte, memoize(((ParserParam*)pparam)->parseMemos, BinaryArgs($1, $3)), @$ )); }
  | Expression tPLUS   Expression                                                                                                      
  { $$ = memoize(((ParserParam*)pparam)->parseMemos, BinaryExp(hOpAdd, memoize(((ParserParam*)pparam)->parseMemos, BinaryArgs($1, $3)), @$ )); }
  | Expression tMINUS  Expression                                                                                                      
  { $$ = memoize(((ParserParam*)pparam)->parseMemos, BinaryExp(hOpSub, memoize(((ParserParam*)pparam)->parseMemos, BinaryArgs($1, $3)), @$ )); }
  | Expression tASTERISK  Expression  
  { $$ = memoize(((ParserParam*)pparam)->parseMemos, BinaryExp(hOpMul, memoize(((ParserParam*)pparam)->parseMemos, BinaryArgs($1, $3)), @$ )); }
  | Expression tFSLASH Expression                                                                                                      
  { $$ = memoize(((ParserParam*)pparam)->parseMemos, BinaryExp(hOpDiv, memoize(((ParserParam*)pparam)->parseMemos, BinaryArgs($1, $3)), @$ )); }
  | Expression tCARET    Expression                                                                                                      
  { $$ = memoize(((ParserParam*)pparam)->parseMemos, BinaryExp(hOpPow, memoize(((ParserParam*)pparam)->parseMemos, BinaryArgs($1, $3)), @$ )); }
  | tLPAREN Expression tRPAREN     { $$ = $2; }
  | tMINUS tFVAL { $$ = memoize(((ParserParam*)pparam)->parseMemos, RealExp(-1*$2, @$)); }
  | tFVAL { $$ = memoize(((ParserParam*)pparam)->parseMemos, RealExp($1, @$)); }
  | tBVAL { $$ = memoize(((ParserParam*)pparam)->parseMemos, BoolExp($1, @$)); }
  | Id { $$ = memoize(((ParserParam*)pparam)->parseMemos, IdExp($1, @$));   }
  | tFVAL_INVALID 
  {
    addParseError(((ParserParam*)pparam)->errors, lsys_error_invalid_float_value, $1, @$);
    $$ = RealExp(0.0, @$); // I guess we won't fail the parse because of this
  }
  ;
StatementEnd : tSEMI | tNEWLINE;

%%


