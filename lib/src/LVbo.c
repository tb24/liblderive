#ifndef LVBO_C
#define LVBO_C

#define VERT_BUFFER_INC_ELEMS 65536  // The maximum number of verts in a single buffer 
#define INDX_BUFFER_INC_ELEMS 131072 // The number of indexes in a single buffer 
#define NUM_BUFFER_SETS 32

#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES
#include "Defines.h"
#include GL_H
#include GLX_H

#include "clsys.h"
#include "LVbo.h"

void makeBuffers(LVbo* lvbo);
void deleteBuffers(LVbo* lvbo);
bool makeNextStripBuffer(LVbo* lvbo);
bool makeNextLinesBuffer(LVbo* lvbo);
bool check_indx_buffers_for_room(LVbo* lvbo, unsigned int count);
bool check_vert_buffers_for_room(LVbo* lvbo, unsigned int count);
bool check_line_buffers_for_room(LVbo* lvbo, unsigned int count);

struct LVbo
{

  // **NOTE**
  // Since VBOs on OpenGL ES are indexed by GLushort, 
  // that's a maximum of 65536 vertexs in each buffer. 
  // We actually store a series of vbos on the card;
  // (NUM_BUFFER_SETS), otherwise geometery gets 
  // limited rather fast in mobile applications.

  int strip_buffer_i; // this is the buffer we are writing to for gl_triangle_strips
  int lines_buffer_i; // this is the buffer we are writing to for gl_lines 

  // Pointers to VBOs 
  GLuint* vert_buffer_ids;
  GLuint* indx_buffer_ids;
  GLuint* line_buffer_ids;

  // Number of elements in each VBO 
  GLuint*  vert_buffer_counts;
  GLuint*  indx_buffer_counts;
  GLuint*  line_buffer_counts;

  // A bounding box for all the vetexes 
  // added to the lvbo, in world-coordinates
  LVboBounds bounds;

};

LVboBounds boundingBox(LVbo* lvbo)
{
  return lvbo->bounds;
}


// This will not free up any gpu memory, it
// merely resets the buffer counts to zero
void lvbo_reset(LVbo* lvbo)
{
  for(unsigned int i=0; i < NUM_BUFFER_SETS; i++)
  {
    if(lvbo->vert_buffer_counts) lvbo->vert_buffer_counts[i] = 0;
    if(lvbo->indx_buffer_counts) lvbo->indx_buffer_counts[i] = 0;
    if(lvbo->line_buffer_counts) lvbo->line_buffer_counts[i] = 0;
  }
  lvbo->bounds.x_max = 0.0;
  lvbo->bounds.y_max = 0.0;
  lvbo->bounds.z_max = 0.0;
  lvbo->bounds.x_min = 0.0;
  lvbo->bounds.y_min = 0.0;
  lvbo->bounds.z_min = 0.0;

}

// The drawing mode is currently ignored;
// But it could be used to allow users 
// to specify the format of the vertex data
// that is written to the graphics card
LVbo* lvbo_make(LSysDrawMode drawMode)
{
  LVbo* lvbo;

  if( !(lvbo = malloc(sizeof *lvbo) ) )
    return NULL;

  lvbo->vert_buffer_ids = NULL;
  lvbo->indx_buffer_ids = NULL;
  lvbo->line_buffer_ids = NULL;
  lvbo->vert_buffer_counts = NULL;
  lvbo->indx_buffer_counts = NULL;
  lvbo->line_buffer_counts = NULL;
  
  if(!(lvbo->vert_buffer_counts = calloc(NUM_BUFFER_SETS, sizeof(GLuint))))
    return lvbo_free(lvbo), NULL;
  if(!(lvbo->indx_buffer_counts = calloc(NUM_BUFFER_SETS, sizeof(GLint))))
    return lvbo_free(lvbo), NULL;
  if(!(lvbo->line_buffer_counts = calloc(NUM_BUFFER_SETS, sizeof(GLint))))
    return lvbo_free(lvbo), NULL;

  if(!(lvbo->vert_buffer_ids = calloc(NUM_BUFFER_SETS, sizeof(GLuint))))
    return lvbo_free(lvbo), NULL;
  if(!(lvbo->indx_buffer_ids = calloc(NUM_BUFFER_SETS, sizeof(GLuint))))
    return lvbo_free(lvbo), NULL;
  if(!(lvbo->line_buffer_ids = calloc(NUM_BUFFER_SETS, sizeof(GLuint))))
    return lvbo_free(lvbo), NULL;

  lvbo->bounds.x_max = 0.0;
  lvbo->bounds.y_max = 0.0;
  lvbo->bounds.z_max = 0.0;
  lvbo->bounds.x_min = 0.0;
  lvbo->bounds.y_min = 0.0;
  lvbo->bounds.z_min = 0.0;
  
  makeBuffers(lvbo);

  return lvbo;

}

void makeBuffers(LVbo* lvbo)
{

  glGenBuffers(NUM_BUFFER_SETS, lvbo->vert_buffer_ids);
  glGenBuffers(NUM_BUFFER_SETS, lvbo->indx_buffer_ids);
  glGenBuffers(NUM_BUFFER_SETS, lvbo->line_buffer_ids);

  lvbo->strip_buffer_i = -1;
  lvbo->lines_buffer_i = -1;

  makeNextStripBuffer(lvbo);
  makeNextLinesBuffer(lvbo);
    
}
 
bool makeNextLinesBuffer(LVbo* lvbo)
{

  if(lvbo->lines_buffer_i == (NUM_BUFFER_SETS-1))
    return false;

  lvbo->lines_buffer_i++;

  glBindBuffer(GL_ARRAY_BUFFER, lvbo->line_buffer_ids[lvbo->lines_buffer_i]);
  glBufferData(
    GL_ARRAY_BUFFER,         
    LGL_VERT_STRIDE*VERT_BUFFER_INC_ELEMS, 
    NULL, GL_DYNAMIC_DRAW
  );
  
  lvbo->line_buffer_counts[lvbo->lines_buffer_i] = 0;

  return true;


}

bool makeNextStripBuffer(LVbo* lvbo)
{

  if(lvbo->strip_buffer_i == (NUM_BUFFER_SETS-1))
    return false;

  lvbo->strip_buffer_i++;

// todo check for GL_OUT_OF_MEMORY
  glBindBuffer(GL_ARRAY_BUFFER, lvbo->vert_buffer_ids[lvbo->strip_buffer_i]);
  glBufferData(
    GL_ARRAY_BUFFER,         
    LGL_VERT_STRIDE*VERT_BUFFER_INC_ELEMS, 
    NULL, GL_DYNAMIC_DRAW
  );
  
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lvbo->indx_buffer_ids[lvbo->strip_buffer_i]);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, 
    LGL_INDX_STRIDE*INDX_BUFFER_INC_ELEMS, 
    NULL, GL_DYNAMIC_DRAW  
  );

  lvbo->vert_buffer_counts[lvbo->strip_buffer_i] = 0;
  lvbo->indx_buffer_counts[lvbo->strip_buffer_i] = 0;

  return true;

}

// Releases GPU memory
void deleteBuffers(LVbo* lvbo)
{

  for(unsigned int i=0; i < lvbo->strip_buffer_i; i++)
  {
    if(lvbo->vert_buffer_counts) lvbo->vert_buffer_counts[i] = 0;
    if(lvbo->indx_buffer_counts) lvbo->indx_buffer_counts[i] = 0;
    if(lvbo->line_buffer_counts) lvbo->line_buffer_counts[i] = 0;
  }
  
  if(lvbo->vert_buffer_ids)
    glDeleteBuffers(NUM_BUFFER_SETS, lvbo->vert_buffer_ids);

  if(lvbo->indx_buffer_ids)
    glDeleteBuffers(NUM_BUFFER_SETS, lvbo->indx_buffer_ids);

  if(lvbo->line_buffer_ids)
    glDeleteBuffers(NUM_BUFFER_SETS, lvbo->line_buffer_ids);

}


void lsys_bind_vbo(LVbo* lvbo)
{
  for(unsigned int i=0; i <= lvbo->strip_buffer_i; i++)
  {
      glBindBuffer(GL_ARRAY_BUFFER, lvbo->vert_buffer_ids[i]);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lvbo->indx_buffer_ids[i]);
      glBindBuffer(GL_ARRAY_BUFFER, lvbo->line_buffer_ids[i]);
  }
}

void lvbo_draw(LVbo* lvbo)
{

  for(unsigned int i=0; i <= lvbo->strip_buffer_i; i++)
  {

    if(lvbo->vert_buffer_counts[lvbo->strip_buffer_i] < 3) continue;

    glBindBuffer(GL_ARRAY_BUFFER, lvbo->vert_buffer_ids[i]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lvbo->indx_buffer_ids[i]);

    glDrawElements(
      GL_TRIANGLE_STRIP, 
      lvbo->indx_buffer_counts[i],
      GL_UNSIGNED_SHORT, 
      (const void*)0
    );

  }
  
  for(unsigned int i=0; i <= lvbo->lines_buffer_i; i++)
  {

    glBindBuffer(GL_ARRAY_BUFFER, lvbo->line_buffer_ids[i]);
    glDrawArrays(GL_LINES, 0, lvbo->line_buffer_counts[i]);

  }
  
}

void lvbo_draw_with_pointers(LVbo* lvbo)
{
  lvbo_draw_strips_with_pointers(lvbo);
  lvbo_draw_lines_with_pointers(lvbo);

}

void lvbo_draw_strips_with_pointers(LVbo* lvbo)
{
// These functions are not available in OpenGLES
#ifndef GLES

  GLchar* offset;
  for(unsigned int i=0; i <= lvbo->strip_buffer_i; i++)
  {
    if(lvbo->vert_buffer_counts[i] < 3) continue;

    glBindBuffer(GL_ARRAY_BUFFER, lvbo->vert_buffer_ids[i]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lvbo->indx_buffer_ids[i]);

    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    DBG( fprintf(stderr,"IDS: %x %x\n",lvbo->vert_buffer_ids[i],lvbo->indx_buffer_ids[i]);)
  
    offset = (GLchar*)LGL_VERT_CLR_OFF;
    glColorPointer(LGL_VERT_CLR_SIZE, GL_FLOAT, (GLsizei)LGL_VERT_STRIDE, (const GLvoid*)offset);
    offset = (GLchar*)LGL_VERT_NRM_OFF;
    glNormalPointer(   GL_FLOAT, (GLsizei)LGL_VERT_STRIDE, (const GLvoid*)offset);
    offset = (GLchar*)LGL_VERT_POS_OFF;
    glVertexPointer(LGL_VERT_POS_SIZE, GL_FLOAT, (GLsizei)LGL_VERT_STRIDE, (const GLvoid*)offset);

    glDrawElements(
      GL_TRIANGLE_STRIP, 
      (GLsizei)lvbo->indx_buffer_counts[i], 
      GL_UNSIGNED_SHORT, 
      (const void*)0
    );

  }
#endif
}

void lvbo_draw_lines_with_pointers(LVbo* lvbo)
{
// These functions are not available in OpenGLES
#ifndef GLES
  GLchar* offset;
  for(unsigned int i=0; i <= lvbo->lines_buffer_i; i++)
  {

    if(lvbo->line_buffer_counts[i] < 2) continue;

    glBindBuffer(GL_ARRAY_BUFFER, lvbo->line_buffer_ids[i]);

    glDisableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    offset = (GLchar*)LGL_VERT_CLR_OFF;
    glColorPointer(LGL_VERT_CLR_SIZE, GL_FLOAT, (GLsizei)LGL_VERT_STRIDE, (const GLvoid*)offset);
    offset = (GLchar*)LGL_VERT_NRM_OFF;
    glNormalPointer(   GL_FLOAT, (GLsizei)LGL_VERT_STRIDE, (const GLvoid*)offset);
    offset = (GLchar*)LGL_VERT_POS_OFF;
    glVertexPointer(LGL_VERT_POS_SIZE, GL_FLOAT, (GLsizei)LGL_VERT_STRIDE, (const GLvoid*)offset);

    glDrawArrays(GL_LINES, 0, lvbo->line_buffer_counts[i]);

  }

#endif


}

bool addIndexes(LVbo* lvbo, GLushort* inds, unsigned int count)
{
  if(!check_indx_buffers_for_room(lvbo, count))
    return false;
  
  // Values of inds are assumed to be given relative 
  // to the number of vertices currently in the buffer
  for(unsigned int i=0; i < count; i++){
    inds[i] = lvbo->vert_buffer_counts[lvbo->strip_buffer_i] + inds[i];
    VERB( fprintf(stderr, "%4d ", inds[i]); )
  }
  
  glBindBuffer(
    GL_ELEMENT_ARRAY_BUFFER, 
    lvbo->indx_buffer_ids[lvbo->strip_buffer_i]
  );
  glBufferSubData(
    GL_ELEMENT_ARRAY_BUFFER, 
    lvbo->indx_buffer_counts[lvbo->strip_buffer_i]*sizeof(GLushort), 
    count*sizeof(GLushort), 
    inds
  );
  lvbo->indx_buffer_counts[lvbo->strip_buffer_i] += count;

  return true;
}


bool addVertexs(LVbo* lvbo, Vertex* verts, unsigned int count)
{
  if(!check_vert_buffers_for_room(lvbo, count))
    return false;

  for(unsigned int i=0; i < count; i++){
    if(verts[i].p.x > lvbo->bounds.x_max) lvbo->bounds.x_max = verts[i].p.x;
    if(verts[i].p.y > lvbo->bounds.y_max) lvbo->bounds.y_max = verts[i].p.y;
    if(verts[i].p.z > lvbo->bounds.z_max) lvbo->bounds.z_max = verts[i].p.z;
    if(verts[i].p.x < lvbo->bounds.x_min) lvbo->bounds.x_min = verts[i].p.x;
    if(verts[i].p.y < lvbo->bounds.y_min) lvbo->bounds.y_min = verts[i].p.y;
    if(verts[i].p.z < lvbo->bounds.z_min) lvbo->bounds.z_min = verts[i].p.z;
  }

  glBindBuffer(
    GL_ARRAY_BUFFER, 
    lvbo->vert_buffer_ids[lvbo->strip_buffer_i]
  );
  glBufferSubData(
    GL_ARRAY_BUFFER, 
    lvbo->vert_buffer_counts[lvbo->strip_buffer_i]*sizeof(Vertex), 
    count*sizeof(Vertex), 
    verts
  );
  lvbo->vert_buffer_counts[lvbo->strip_buffer_i] += count;

  return true;
}


bool addLine(LVbo* lvbo, Vertex* verts, unsigned int count)
{
  if(!check_line_buffers_for_room(lvbo, count)) return false;

  for(unsigned int i=0; i < count; i++){
    if     (verts[i].p.x > lvbo->bounds.x_max) lvbo->bounds.x_max = verts[i].p.x;
    else if(verts[i].p.x < lvbo->bounds.x_min) lvbo->bounds.x_min = verts[i].p.x;
    if     (verts[i].p.y > lvbo->bounds.y_max) lvbo->bounds.y_max = verts[i].p.y;
    else if(verts[i].p.y < lvbo->bounds.y_min) lvbo->bounds.y_min = verts[i].p.y;
    if     (verts[i].p.z > lvbo->bounds.z_max) lvbo->bounds.z_max = verts[i].p.z;
    else if(verts[i].p.z < lvbo->bounds.z_min) lvbo->bounds.z_min = verts[i].p.z;
  } 

  glBindBuffer(
    GL_ARRAY_BUFFER, 
    lvbo->line_buffer_ids[lvbo->lines_buffer_i]
  );
  glBufferSubData(
    GL_ARRAY_BUFFER, 
    lvbo->line_buffer_counts[lvbo->lines_buffer_i]*sizeof(Vertex), 
    count*sizeof(Vertex), 
    verts
  );
  lvbo->line_buffer_counts[lvbo->lines_buffer_i] += count;

  return true;
}


bool check_indx_buffers_for_room(LVbo* lvbo, unsigned int count)
{
  // check if we need to create a new buffer or not
  if(count + lvbo->indx_buffer_counts[lvbo->strip_buffer_i] >= INDX_BUFFER_INC_ELEMS)
    return makeNextStripBuffer(lvbo);
  return true;
}

bool check_vert_buffers_for_room(LVbo* lvbo, unsigned int count)
{
  // check if we need to create a new buffer or not
  if(count + lvbo->vert_buffer_counts[lvbo->strip_buffer_i] >= VERT_BUFFER_INC_ELEMS)
    return makeNextStripBuffer(lvbo);
  return true;
}

bool check_line_buffers_for_room(LVbo* lvbo, unsigned int count)
{
  if( count + lvbo->line_buffer_counts[lvbo->lines_buffer_i] >= VERT_BUFFER_INC_ELEMS )
    return makeNextLinesBuffer(lvbo);
  return true;
}

void lvbo_free(LVbo* lvbo)
{
  deleteBuffers(lvbo);

  if(lvbo->vert_buffer_ids) free(lvbo->vert_buffer_ids);
  if(lvbo->indx_buffer_ids) free(lvbo->indx_buffer_ids);
  if(lvbo->line_buffer_ids) free(lvbo->line_buffer_ids);

  if(lvbo->vert_buffer_counts) free(lvbo->vert_buffer_counts);
  if(lvbo->indx_buffer_counts) free(lvbo->indx_buffer_counts);
  if(lvbo->line_buffer_counts) free(lvbo->line_buffer_counts);
  
  free(lvbo);
}

#endif
