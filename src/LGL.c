#ifndef LGL_C
#define LGL_C

#include "Defines.h"
#include GL_H 

#include "LGL.h"
#include "clsys.h"
#include "LVbo.h"
#include "LString.h"
#include "LSystem.h"
#include "LEngine.h"
#include "TurtleInterpreter.h"

// Warning: DO NOT USE the LVbo object
// while this call is taking place 
extern bool lsys_render_vbo(CLSYS_LSystem* lsys, LVbo* lvbo, LSysDrawMode drawMode)
{

  LString* lstr = ls_blankLString(le_currStr(lsys->le));
 
  if(lvbo == NULL)
    return ls_freeLString(lstr), false;

  // interpret the current lsystem string
  if( !le_interpret(lsys->errors, lsys->le, &lstr ) )
    return ls_freeLString(lstr), false;


  DBG( fprintf(stderr, "done interpreting!\nGoing to write to buffer now...\n"); )
  VERB( fprintf(stderr, "done interpreting!\nGoing to write to buffer now...\n"); )
  
  if( !renderLString(lstr, lsys->ils->ilsDrawModuleTypes, lvbo, drawMode) )
    return ls_freeLString(lstr), false;

  return ls_freeLString(lstr), true;

}

extern void lsys_draw_vbo(LVbo* lvbo)
{
  lvbo_draw(lvbo);
}

extern void lsys_draw_vbo_fixed_function(LVbo* lvbo)
{
  lvbo_draw_with_pointers(lvbo);
}


#endif
