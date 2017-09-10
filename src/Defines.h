#ifndef DEFINES_H
#define DEFINES_H

#include <stdbool.h>

#define MAX_NUM_MODULES 65536 
#define MAX_NUM_MODULE_PARAMETERS 32
#define MAX_LEN_IDENTIFIER 40

#define DEFAULT_RANDOM_SEED 42
#define DEFAULT_DERIVATION_DEPTH 1

// 100,000 * 32bytes = 320k each malloc
#define LSTRING_BUFFER_INCREMENT 100000

#define LSYS_ERROR_BUFFER_MAX_LENGTH 1024
// for whatever reason strdup isn't on ubuntu?

#ifdef DEBUG
#define DBG(x) x
#else
#define DBG(x) 
#endif

#ifdef VERBOSE 
#define VERB(x) x
#else
#define VERB(x) 
#endif

#ifdef GL_DIR
#define GL_H <GL_DIR/gl.h>
#define GLX_H <GL_DIR/glext.h>
#else
#define GL_H <GL/gl.h>
#define GLX_H <GL/glext.h>
#endif



enum ops {
  hOpAnd,
  hOpOr,
  hOpLt,
  hOpLte,
  hOpEq,
  hOpNeq,
  hOpGt,
  hOpGte,
  hOpAdd,
  hOpSub,
  hOpMul,
  hOpDiv,
  hOpPow,
  hOpFval,
  hOpBval,
  hOpId,
  hOpNone
 };

typedef enum ops ops_t;


typedef enum PreDefModTypes {
  
  // Some necessary internal definitions
  mtStartString,
  mtEndString,
  // a wildcard ( whee! )
  mtAny,
  // and all the turtle commands
  mtForward  ,
  mtTiptoe   ,
  mtCircle   ,
  mtSphere   ,
  mtRollLeft ,
  mtRollRight,
  mtLeft     ,
  mtRight    ,
  mtUp       ,
  mtDown     ,
  mtAboutFace,
  mtWidth    ,
  mtColor    ,
  mtColorHue ,
  mtColorSat ,
  mtColorVal ,
  mtPush     ,
  mtPop      ,
  mtUnPush   ,
  mtRePush   ,
  mtTriStart ,
  mtTriEnd   ,
  mtTriPoint ,
  NUM_PREDEF_MOD_TYPES // keep this last
} PreDefModTypes;

typedef char* Id;

typedef float* Bindings;

bool isBinaryOp(ops_t);

// These guys need to get packed into the lstring
typedef unsigned short IModType;
#define GARBAGE_IMODTYPE 0xffff

// These are used as indexes when writing 
// to arrays of bound variables
typedef unsigned int IParamId;

#endif
