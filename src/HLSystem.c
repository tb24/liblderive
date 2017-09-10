#ifndef HLSYSTEM_C
#define HLSYSTEM_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HLSystem.h"
#include "Common.h"
#include "Defines.h"


HId* makeHId(char* str, YYLTYPE location)
{
  HId* hid = malloc(sizeof(*hid));

  hid->hidString  = str;
  hid->hidMt      = NUM_PREDEF_MOD_TYPES;
  hid->yylocation = location;
    
  return hid;
}

HId* makePreDefId(PreDefModTypes mt, YYLTYPE location)
{
  HId* hid = malloc(sizeof(*hid));
  
  hid->hidString  = NULL;
  hid->hidMt      = mt;
  hid->yylocation = location;

  return hid;
}

void freeHId(HId* hid)
{
 free(hid);
}

union HVEArgs* BinaryArgs(struct HValExp* e1, struct HValExp* e2)
{
 union HVEArgs* hvea = malloc(sizeof(*hvea));
 hvea->hveaBinary.hve1 = e1;
 hvea->hveaBinary.hve2 = e2;
 return hvea;
}

struct HValExp* BinaryExp(ops_t type, union HVEArgs* args, YYLTYPE locations)
{
  struct HValExp* v = malloc(sizeof(*v));
  v->hveType = type;
  v->hveArgs = args; 
  v->hveVarName   = NULL;
  v->hveIVal = 0;
  v->hveFVal = 0.0;
  v->hveBVal = false;
  v->yylocation = locations;
  return v;
}

struct HValExp* IdExp(HId* id, YYLTYPE locations)
{
  struct HValExp* v = malloc(sizeof(*v));
  v->hveType = hOpId;
  v->hveArgs = NULL; 
  v->hveVarName = id; 
  v->hveIVal = 0;
  v->hveFVal = 0.0;
  v->hveBVal = false;
  v->yylocation = locations;
  return v;
}

struct HValExp* RealExp(float val, YYLTYPE locations)
{
  struct HValExp* v = malloc(sizeof(*v));
  v->hveType = hOpFval;
  v->hveArgs = NULL; 
  v->hveVarName   = NULL; 
  v->hveIVal = 0;
  v->hveFVal = val;
  v->hveBVal = false;
  v->yylocation = locations;
  return v;
}

struct HValExp* BoolExp(bool val, YYLTYPE locations)
{
  struct HValExp* v = malloc(sizeof(*v));
  v->hveType = hOpBval;
  v->hveArgs = NULL; 
  v->hveVarName   = NULL; 
  v->hveIVal = 0;
  v->hveFVal = 0.0;
  v->hveBVal = val;
  v->yylocation = locations;
  return v;
}


struct HLSystem* makeHLSystem(
  struct HDirective** dirs,
  struct HLRule**     prods,
  struct HLRule**     decomps,
  struct HLRule**     interps,
  YYLTYPE locations
){
  struct HLSystem* hls = malloc(sizeof(*hls));
  hls->hlsDirectives = dirs;
  hls->hlsProductions = prods;
  hls->hlsInterpretations = interps;
  hls->hlsDecompositions = decomps;
  return hls;
}

struct HLRule* makeHRule( 
  struct HModRef** lc,
  struct HModRef*  sp,  
  struct HModRef** rc, 
  struct HVarDec** vardecs,
  struct HValExp*  condition,
  struct HModExp** production,
  YYLTYPE locations

){  
  struct HLRule* hlr = malloc(sizeof(*hlr));
  hlr->hlrLC         = lc;
  hlr->hlrSP         = sp;
  hlr->hlrRC         = rc;
  hlr->hlrProduction = production;
  hlr->hlrVardecs    = vardecs;
  hlr->hlrCondition  = condition;
  return hlr;
}

struct HModRef* makeHModRef(HId* refName, HId** paramRefs, YYLTYPE locations)
{
 struct HModRef* mr = malloc(sizeof(*mr));
 mr->hmrType       =    refName;
 mr->hmrParameters = paramRefs;
 mr->yylocation    =  locations;
 return mr;
}


struct HDirective* makeHDirectiveDefine(struct HVarDec* hvd, YYLTYPE locations)
{
  struct HDirective* hd = malloc(sizeof(*hd));
  hd->hdType = dDEFINE;
  hd->hdData.hddDefine = hvd;
  return hd;
}

HVarDec* makeHVarDec(HId* varId, struct HValExp* varExp, YYLTYPE locations)
{
  HVarDec* hvd  = malloc(sizeof(*hvd));
  hvd->hvdId  = varId;
  hvd->hvdVal = varExp;
  hvd->yylocation = locations;
  return hvd;
}

struct HDirective* makeHDirectiveConsider(struct HModExp** hmes, YYLTYPE locations)
{
  struct HDirective* hd = malloc(sizeof(*hd));
  hd->hdType = dCONSIDER;
  hd->hdData.hddModExps = hmes; 
  return hd;
}

struct HDirective* makeHDirectiveIgnore(struct HModExp** hmes, YYLTYPE locations)
{
  struct HDirective* hd = malloc(sizeof(*hd));
  hd->hdType = dIGNORE;
  hd->hdData.hddModExps = hmes; 
  return hd;
}

struct HDirective* makeHDirectiveAxiom(struct HModExp** hmes, YYLTYPE locations)
{
  struct HDirective* hd = malloc(sizeof(*hd));
  hd->hdType = dAXIOM;
  hd->hdData.hddModExps = hmes; 
  return hd;
}

struct HDirective* makeHDirectiveSeed(int seed, YYLTYPE locations)
{
  struct HDirective * hd = malloc(sizeof(*hd));
  hd->hdType = dSEED;
  hd->hdData.hddRandomSeed = seed;
  return hd;
}

struct HDirective* makeHDirectiveDepth(int depth, YYLTYPE locations)
{
  struct HDirective * hd = malloc(sizeof(*hd));
  hd->hdType = dDEPTH;
  hd->hdData.hddDerivationDepth = depth;
  return hd;
}

struct HDirective* makeHDirectiveLength(int length, YYLTYPE locations)
{
  struct HDirective * hd = malloc(sizeof(*hd));
  hd->hdType = dLENGTH;
  hd->hdData.hddDerivationLength = length;
  return hd;
}

HModExp* makeHModExp(HId* modName, HValExp** modValExps, YYLTYPE locations)
{

  HModExp* hme = malloc(sizeof(*hme));
  hme->hmeType = modName;
  hme->hmeVals = modValExps;
  hme->yylocation = locations;
  return hme;
}

void ppLSystem(struct HLSystem* lsys)
{
  ppDirectives(lsys->hlsDirectives);
  fprintf(stdout, "Productions:\n");
  iterateAndInbetween((void*)(void*)&ppLRule, &ppNewln, (void**)lsys->hlsProductions);
  fprintf(stdout, "\n");

}

void ppLRule(struct HLRule* lr)
{
  if(lr->hlrLC != NULL)
  {
    ppHModRefs(lr->hlrLC);
    fprintf(stdout, " < ");
  }
  
  ppHModRef(lr->hlrSP);

  if(lr->hlrRC != NULL)
  {
    fprintf(stdout, " > ");
    ppHModRefs(lr->hlrRC);
  }
  //lr->hlrVardecs;
  //lr->hlrCondition;
  fprintf(stdout, " -> ");
  ppHModExps(lr->hlrProduction);
}

void ppHModRefs(struct HModRef** hmes)
{
  if(hmes == NULL) return;
  iterateAndInbetween((void*)(void*)&ppHModRef, &ppSpace, (void**)hmes);
}

void ppHModRef(struct HModRef* hmr)
{
  if(hmr == NULL) return;
  ppHId(hmr->hmrType);
  if(hmr->hmrParameters != NULL)
  {
    fprintf(stdout,"(");
    iterateAndInbetween((void*)(void*)&ppHId, &ppComma, (void**)hmr->hmrParameters);
    fprintf(stdout,")");
  }
}

void ppHId(HId* param)
{
  if(param == NULL) return;
  fprintf(stdout, "%s", param->hidString);
}

// should be a null terminated array of pointers
void ppDirectives(struct HDirective** hd)
{
  iterate((void*)(void*)&ppDirective, (void**)hd);
}

void ppDirective(struct HDirective* dir)
{
  switch(dir->hdType)
  {
    case dAXIOM:
     fprintf(stdout,"Axiom: ");
     ppHModExps(dir->hdData.hddModExps);
     fprintf(stdout,"\n");
    break;
    case dIGNORE:
     fprintf(stdout,"Ignore: ");
     ppHModExps(dir->hdData.hddModExps);
     fprintf(stdout,"\n");
    break;
    case dCONSIDER:
     fprintf(stdout,"Consider: ");
     ppHModExps(dir->hdData.hddModExps);
     fprintf(stdout,"\n");
    break;
    case dSEED:
     fprintf(stdout,"Seed: %d\n", dir->hdData.hddRandomSeed);
    break;
    case dLENGTH:
     fprintf(stdout,"Length: %d\n", dir->hdData.hddDerivationLength);
    break;
    case dDEPTH:
     fprintf(stdout,"Depth: %d\n", dir->hdData.hddDerivationDepth);
    break;
    case dDEFINE:
     fprintf(stdout,"%s = ",dir->hdData.hddDefine->hvdId->hidString);
     ppHValExp(dir->hdData.hddDefine->hvdVal);
     fprintf(stdout,"\n");
  }
}

void ppHModExps(struct HModExp** mes)
{
  struct HModExp * curr;
  int i = -1;
  while(true)
  {
    if( (curr = mes[++i]) == NULL) break;
    
    ppHId(curr->hmeType);

    if(curr->hmeVals != NULL)
    {
      fprintf(stdout,"(");
      
      ppHValExps(curr->hmeVals);
  
      fprintf(stdout,")");
    }
    if( mes[i+1] != NULL)   
      fprintf(stdout," ");
  }
}

void ppComma(){ fprintf(stdout, ","); }
void ppSpace(){ fprintf(stdout, " "); }
void ppNewln(){ fprintf(stdout,"\n"); }

void ppHValExp(struct HValExp* hve)
{
    switch(hve->hveType)
    {
      case hOpAnd:
      case hOpOr :
      case hOpLt :
      case hOpLte:
      case hOpEq :
      case hOpNeq:
      case hOpGt :
      case hOpGte:
      case hOpAdd:
      case hOpSub:
      case hOpMul:
      case hOpDiv:
      case hOpPow:
        ppBinaryOperator(hve->hveType, hve->hveArgs);
      break;
      case hOpFval:
        fprintf(stdout,"%0.2f",hve->hveFVal);
      break;
      case hOpBval:
        if(hve->hveBVal) fprintf(stdout,"true");
        else              fprintf(stdout,"false");
      break;
      case hOpId:
        ppHId(hve->hveVarName);
      case hOpNone:
        fprintf(stdout,"(noop)");
      break;
    }
}
void ppHValExps(struct HValExp** hves)
{
  struct HValExp * curr;
  int i = -1;
  while(true)
  {
    if( (curr = hves[++i]) == NULL) break;
    ppHValExp(curr);
    if( hves[i+1] != NULL)   
      fprintf(stdout,", ");
  }
}

void ppOp(ops_t op)
{
  switch(op)
  {
      case hOpAnd: fprintf(stdout,"&&"); break;
      case hOpOr : fprintf(stdout,"||"); break;
      case hOpLt : fprintf(stdout,"<");  break;
      case hOpLte: fprintf(stdout,"<="); break;
      case hOpEq : fprintf(stdout,"=="); break;
      case hOpNeq: fprintf(stdout,"!="); break;
      case hOpGt : fprintf(stdout,">");  break;
      case hOpGte: fprintf(stdout,">="); break;
      case hOpAdd: fprintf(stdout,"+");  break;
      case hOpSub: fprintf(stdout,"-");  break;
      case hOpMul: fprintf(stdout,"*");  break;
      case hOpDiv: fprintf(stdout,"/");  break;
      case hOpPow: fprintf(stdout,"^");  break;
      default: break;
  }
}

void ppBinaryOperator(ops_t op, union HVEArgs* args)
{
  ppHValExp(args->hveaBinary.hve1);
  fprintf(stdout, " ");
  ppOp(op);
  fprintf(stdout, " ");
  ppHValExp(args->hveaBinary.hve2);
}

void hlsys_free(HLSystem* hls)
{
  freeHLsys(hls);
}
void freeHLsys(struct HLSystem* hls)
{
  if ( hls == NULL ) return;
  freeHDirs(hls->hlsDirectives);
  freeHLRules(hls->hlsProductions);
  freeHLRules(hls->hlsInterpretations);
  freeHLRules(hls->hlsDecompositions);
  // what about that memory pool for character strings?
  // (needed for error handling.....)
  free(hls);
}


void freeHDirs(struct HDirective ** hds)
{
  if ( hds == NULL ) return;
  iterate((void*)(void*)&freeHDir, (void**)hds);
  free(hds);
}

void freeHDir(struct HDirective* hd)
{
  if ( hd == NULL ) return;
  switch(hd->hdType)
  {
    case dAXIOM:
      freeHModExps(hd->hdData.hddModExps);
    break;
    case dIGNORE:
      freeHModExps(hd->hdData.hddModExps);
    break;
    case dCONSIDER:
      freeHModExps(hd->hdData.hddModExps);
    break;
    case dDEFINE:
      free(hd->hdData.hddDefine->hvdId);
      freeHValExp(hd->hdData.hddDefine->hvdVal);
      free(hd->hdData.hddDefine);
    break;
    case dSEED:
    case dLENGTH:
    case dDEPTH:
    break;
  }
  free(hd);
}

void freeHLRules(struct HLRule ** hlrs)
{
  if ( hlrs == NULL ) return;
  iterate((void*)(void*)&freeHLRule, (void**)hlrs);
  free(hlrs);
}

void freeHLRule(struct HLRule* hlr)
{
  if(hlr == NULL) return;
  iterate((void*)(void*)&freeHModRef, (void**)hlr->hlrLC);
  freeHModRef(hlr->hlrSP);
  iterate((void*)(void*)&freeHModRef, (void**)hlr->hlrRC);
  iterate((void*)(void*)freeHModExp, (void**)hlr->hlrProduction);
  iterate((void*)(void*)freeHVardec, (void**)hlr->hlrVardecs);
  freeHValExp(hlr->hlrCondition);
  free(hlr);
}

void freeHVardec(struct HVarDec* hvd)
{
  if(hvd == NULL) return;
  free(hvd->hvdId);
  freeHValExp(hvd->hvdVal);
  free(hvd);
}
void freeHModRef(struct HModRef* hmr)
{
  if( hmr == NULL) return;
  free(hmr->hmrType);
  iterate(free, (void*)hmr->hmrParameters);
  free(hmr);
}

void freeHModExps(struct HModExp ** mes)
{
  if ( mes == NULL ) return;
  iterate((void*)(void*)freeHModExp, (void**)mes); 
  free(mes);
}

void freeHModExp(struct HModExp* hme)
{
  if( hme == NULL ) return;
  if(hme->hmeVals != NULL)
  {
    freeHValExps(hme->hmeVals);
  }
  free(hme->hmeType);
  free(hme);
}

void freeHValExps(struct HValExp ** hves)
{
  if ( hves == NULL ) return;
  iterate((void*)(void*)&freeHValExp, (void**)hves);
  free(hves);
}

void freeHValExp(struct HValExp* hve)
{
  if(hve == NULL) return;
  
  switch(hve->hveType)
  {
    case hOpAnd:
    case hOpOr :
    case hOpLt :
    case hOpLte:
    case hOpEq :
    case hOpNeq:
    case hOpGt :
    case hOpGte:
    case hOpAdd:
    case hOpSub:
    case hOpMul:
    case hOpDiv:
    case hOpPow:
      freeHValExpBinaryArgs(hve->hveArgs);
    break;
    case hOpFval:
    case hOpBval:
    case hOpNone:
    break;
    case hOpId:
      free(hve->hveVarName);
    break;
  }
  free(hve);
}

void freeHValExpBinaryArgs(union HVEArgs* hveargs)
{
  freeHValExp(hveargs->hveaBinary.hve1);
  freeHValExp(hveargs->hveaBinary.hve2);
  free(hveargs);
}

void freeHValExpUnaryArgs(union HVEArgs* hveargs)
{
  freeHValExp(hveargs->hveaUnary.hve);
  free(hveargs);
}

#endif
