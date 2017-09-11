#ifndef LGL_H
#define LGL_H

#include "Defines.h"
#include GL_H

#include <stdbool.h>
#include "clsys.h"


// The following is a handle to the geometry created from a call to lsys_render_vbo;
typedef struct LVbo LVbo;

typedef enum {
  lsys_draw_mode_linear,
  lsys_draw_mode_volumetric,
  NUM_LSYS_DRAW_MODE
} LSysDrawMode;


// This will send data to the graphics card
extern bool lsys_render_vbo(CLSYS_LSystem* lsys, LVbo* lvbo, LSysDrawMode drawMode);


extern void lsys_bind_vbo(LVbo*);
extern void lsys_draw_vbo(LVbo*);

// if no shaders are set up
extern void lsys_draw_vbo_fixed_function(LVbo* lvbo);

LVbo* lvbo_make();
void  lvbo_free(LVbo* lvbo);
void  lvbo_reset(LVbo* lvbo);

// The bounding box of the geometry is available in world-coordinates
typedef struct LVboBounds
{
  GLfloat x_max, y_max, z_max, x_min, y_min, z_min;
} LVboBounds;

LVboBounds boundingBox(LVbo* lvbo);

void lvbo_draw_strips_with_pointers(LVbo* lvbo);
void lvbo_draw_lines_with_pointers(LVbo* lvbo);
#endif
