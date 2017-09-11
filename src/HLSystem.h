#ifndef HLSYSTEM_H
#define HLSYSTEM_H

#include <stdbool.h>
#include "Defines.h"

// For YYLTYPE
#include "y.tab.h"

 
typedef struct HId {
  char*    hidString;
  PreDefModTypes hidMt;
  YYLTYPE yylocation;
} HId;

HId* makeHId(char*, YYLTYPE);
HId* makePreDefId(enum PreDefModTypes, YYLTYPE);
void freeHId(HId*);

typedef struct HModRef {
  HId*        hmrType;
  HId** hmrParameters;
  YYLTYPE  yylocation;
} HModRef;


typedef struct HValExp {
  ops_t          hveType;
  float          hveFVal;
  bool           hveBVal;
  int            hveIVal;
  HId*        hveVarName;
  union HVEArgs* hveArgs;
  YYLTYPE     yylocation;
} HValExp;

typedef struct HValExpUnary {
  HValExp* hve;
} HValExpUnary;

typedef struct HValExpBinary {
  HValExp* hve1;
  HValExp* hve2;
} HValExpBinary;

typedef union HVEArgs {
  HValExpUnary  hveaUnary;
  HValExpBinary hveaBinary;
} HVEArgs;

enum directives {
  dAXIOM,
  dLENGTH,
  dDEPTH,
  dSEED,
  dIGNORE,
  dCONSIDER,
  dDEFINE 
};
typedef enum directives dir_t;

typedef struct HVarDec {
  HId*         hvdId;
  HValExp*    hvdVal;
  YYLTYPE yylocation;
} HVarDec;

HVarDec* 
  makeHVarDec
  (
    HId*       varId, 
    HValExp*  varExp, 
    YYLTYPE location
  );

typedef struct HModExp {
  HId*       hmeType;
  HValExp**  hmeVals;
  YYLTYPE yylocation;
} HModExp;

union HDirectiveData {
  int hddDerivationLength;
  int hddDerivationDepth;
  int hddRandomSeed; 
  HVarDec* hddDefine; 
  // should be a null terminated array of pointers
  HModExp** hddModExps;

};

typedef struct HDirective {
  dir_t hdType;
  union HDirectiveData hdData;
  YYLTYPE    yylocation;
} HDirective;

typedef struct HLRule
{
  HModRef** hlrLC;  
  HModRef*  hlrSP;  
  HModRef** hlrRC; 
  HModExp** hlrProduction;
  HVarDec** hlrVardecs;
  HValExp*  hlrCondition;
  YYLTYPE   yylocation;
} HLRule;

// Top-level AST datatype 
typedef struct HLSystem {

  struct HDirective** hlsDirectives; 
  struct HLRule**     hlsProductions;
  struct HLRule**     hlsInterpretations;
  struct HLRule**     hlsDecompositions;
  
  YYLTYPE    yylocation;

} HLSystem;

// Below are helpers for creating the AST nodes

struct HLSystem* makeHLSystem(
  struct HDirective**,
  struct HLRule**,
  struct HLRule**,
  struct HLRule**,
  YYLTYPE yylocation
);

struct HLRule* makeHRule(
  struct HModRef**,
  struct HModRef*,  
  struct HModRef**, 
  struct HVarDec**,
  struct HValExp*,
  struct HModExp**,
  YYLTYPE yylocation
 );

struct HModRef* makeHModRef(HId* refName, HId** paramRefs, YYLTYPE locations);

struct HDirective* makeHDirectiveDefine(struct HVarDec*, YYLTYPE yylocation);
struct HDirective* makeHDirectiveAxiom(struct HModExp** hmes, YYLTYPE yylocation);
struct HDirective* makeHDirectiveIgnore(struct HModExp** hmes, YYLTYPE yylocation);
struct HDirective* makeHDirectiveConsider(struct HModExp** hmes, YYLTYPE yylocation);
struct HDirective* makeHDirectiveSeed(int, YYLTYPE yylocation);
struct HDirective* makeHDirectiveLength(int, YYLTYPE yylocation);
struct HDirective* makeHDirectiveDepth(int, YYLTYPE yylocation);
struct HModExp*    makeHModExp(HId*, struct HValExp **, YYLTYPE yylocation);

struct HValExp*     BinaryExp(ops_t, union HVEArgs*, YYLTYPE yylocation);
union  HVEArgs* BinaryArgs(struct HValExp*, struct HValExp*);
struct HValExp* IdExp(HId* id, YYLTYPE locations);
struct HValExp* RealExp(float, YYLTYPE yylocation);
struct HValExp* BoolExp(bool, YYLTYPE yylocation);
struct HValExp* IntExp(int, YYLTYPE yylocation);

void hlsys_free(HLSystem*);
void freeHLsys(struct HLSystem*);
void freeHLRules(struct HLRule ** hlrs);
void freeHLRule(struct HLRule * hlrs);
void freeHDirs(struct HDirective **);
void freeHDir(struct HDirective *);
void freeHModExps(struct HModExp **);
void freeHModExp(struct HModExp *);
void freeHModRefs(struct HModRef**);
void freeHModRef(struct HModRef*);
void freeHValExps(struct HValExp **);
void freeHValExp(struct HValExp*);
void freeHValExpUnaryArgs(union HVEArgs*);
void freeHValExpBinaryArgs(union HVEArgs*);
void freeHVardec(struct HVarDec*);

void ppLSystem(struct HLSystem*);
void ppLRule(struct HLRule*);
void ppHModExps(struct HModExp**);
void ppHModRefs(struct HModRef** hmes);
void ppHModRef(struct HModRef* hmr);
void ppDirectives(struct HDirective**);
void ppDirective(struct HDirective*);
void ppHValExps(struct HValExp**);
void ppHValExp(struct HValExp*);
void ppBinaryOperator(ops_t, union HVEArgs*);
void ppHId(HId*);
void ppComma();
void ppSpace();
void ppNewln();


#endif
