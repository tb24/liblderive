%{

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

#include "Memos.h"
#include "Common.h"
#include "Defines.h"
#include "Parser.h"

#include "AstNode.h"
#include "y.tab.h"


// For keeping track of the locations of tokens 
#define YY_USER_ACTION yylloc->first_line = yylloc->last_line = yylineno; \
  yylloc->first_column = yycolumn; yylloc->last_column = yycolumn+yyleng-1; \
  yycolumn += yyleng;

int comment_nesting = 0;
%}


digit [0-9]
integer {digit}+
decimal {digit}+"."{digit}+|{digit}*"."{digit}+
posExponent (E|e)\+?{digit}+
negExponent (E|e)\-?{digit}+
alpha [a-zA-Z]
id {alpha}({alpha}|{digit}|\_)*

%start COMMENT SCOMMENT ECOMMENT SYMBOL EXPRESSION
%option reentrant noyywrap never-interactive nounistd
%option warn nodefault
%option bison-bridge
%option bison-locations
%option extra-type="ParserParam*"
%option yylineno
%option header-file="lexer.h"

%%

<COMMENT>"//".*\n
<COMMENT>"/*" { comment_nesting++; }
<COMMENT>"*/" { if (--comment_nesting<= 0 ) { BEGIN INITIAL; } }
<COMMENT>.|\n 
"//".*\n
"/*" { BEGIN COMMENT; comment_nesting=0; }

\n 
[ \t]+

(A|a)xiom":"          { return(tAXIOM); }
(L|l)ength":"         { BEGIN EXPRESSION; return(tLENGTH); }
(D|d)erivation" "(L|l)ength":"         { BEGIN EXPRESSION; return(tLENGTH); }
(D|d)epth":"          { BEGIN EXPRESSION; return(tDEPTH); }
(S|s)eed":"           { BEGIN EXPRESSION; return(tSEED); }
(C|c)onsider":"       { return(tCONSIDER); }
(I|i)gnore":"         { return(tIGNORE); }
(P|p)roduction?(s)":"     { return(tPRODUCTIONS); }
(D|d)ecomposition?(s)":"  { return(tDECOMPOSITIONS); }
(I|i)nterpretation?(s)":" { return(tINTERPRETATIONS); }

"<<"   { return(tLCON);     }
">>"   { return(tRCON);     }
"-->"  { return(tYIELDS);   }
":"    { return(tCOLON);    }
";"    { return(tSEMI);     }
","    { return(tCOMMA);    }
"="    { return(tASSIGN);   }
"+"    { return(tPLUS);     }
"-"    { return(tMINUS);    }
"*"    { return(tASTERISK);    }
"\\"   { return(tBSLASH);   }
"/"    { return(tFSLASH);   }
"%"    { return(tPERCENT);      }
"^"    { return(tCARET);      }
"<"    { return(tLT);       }
">"    { return(tGT);       }
"=="   { return(tEQ);    }
"!="   { return(tNEQ); }
"<>"   { return(tNEQ); }
"<="   { return(tLE);       }
">="   { return(tGE);       }
"!"    { return(tEXCLAIM);      }
"!h"   { return(tEXCLAIMH);      }
"!s"   { return(tEXCLAIMS);      }
"!v"   { return(tEXCLAIMV);      }
"&&"   { return(tAND);      }
"||"   { return(tOR);       }
"("    { return(tLPAREN);   }
")"    { return(tRPAREN);   }
"[["   { return(tDOUBLELBRACKET); }
"]]"   { return(tDOUBLERBRACKET); }
"["    { return(tLBRACKET); }
"]"    { return(tRBRACKET); }
"{"    { return(tLBRACE);   }
"}"    { return(tRBRACE);   }
"#"    { return(tHASH);     }
"@"    { return(tAMP);      }
"|"    { return(tBAR);  }

"true" { 
  yylval->bval = true; 
  return(tBVAL);
}

"false" { 
  yylval->bval = false; 
  return(tBVAL);
}

{id} {
  yylval->name = (char*)memoize(yyextra->parseMemos, (void*)my_strndup(yytext, MAX_LEN_IDENTIFIER));
  return(tNAME);
}

{decimal}{posExponent} {
  yylval->fval = strtof(yytext, NULL);
  if( yylval->fval == HUGE_VALF || yylval->fval == -HUGE_VALF )
  { 
    char* buf = malloc(255 * sizeof *buf);
    sprintf(buf, "Value %s is out of range", yytext);
    yylval->name = (char*)memoize(yyextra->parseMemos, (void*)my_strndup(buf, MAX_LEN_IDENTIFIER));
    free(buf);
    return(tFVAL_INVALID);
  }
  return(tFVAL);
}

{decimal}{negExponent} {
  yylval->fval = strtof(yytext, NULL);
  if( yylval->fval == 0 ) 
  { 
    char* buf = malloc(255 * sizeof *buf);
    sprintf(buf, "Value %s truncated to zero", yytext);
    yylval->name = (char*)memoize(yyextra->parseMemos, (void*)my_strndup(buf, MAX_LEN_IDENTIFIER));
    free(buf);
    return(tFVAL_INVALID);
  }
  return(tFVAL);
}

{integer}|{decimal} {
  yylval->fval = strtof(yytext, NULL);
  return(tFVAL);
}

%%
