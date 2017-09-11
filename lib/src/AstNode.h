/*
 * AstNode.h
 * Definition of the structure used internally by the parser and lexer
 * to exchange data.
 */
 
#ifndef AST_NODE_H
#define AST_NODE_H
 
#include "HLSystem.h"
#include "LinkedList.h"
 
/**
 * Definition of nodes in our AST
 */
typedef union tagASTNode {

// literals
  float  fval;
  int    bval;
  char*  name;

// nodes of the syntax tree
  HId*   hid;
  HLSystem*   hlsys;
  HDirective* hdir;
  HLRule*     hlr;
  HModRef*    hmr;
  HModExp*    hme;
  HValExp*    hve;
  HVarDec*    hvd;

// lists of those nodes
  LinkedList* ll;
  /*
  LinkedList* hlrs;
  LinkedList* hmrs;
  LinkedList* hmes;
  LinkedList* hves;
  LinkedList* hvds;
  LinkedList* names;
  */

} ASTNode;
 
// define this as the type for flex and bison
#define YYSTYPE ASTNode
 
#endif // AST_NODE_H
