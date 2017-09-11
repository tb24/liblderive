#ifndef LVBO_H
#define LVBO_H

// The vertex data is arranged in a triangle strip
// Each vertex has 10 float components
#define LGL_VERT_POS_SIZE 3
#define LGL_VERT_NRM_SIZE 3
#define LGL_VERT_CLR_SIZE 4
// They are a single interleaved vertex array
#define LGL_VERT_POS_OFF  28
#define LGL_VERT_NRM_OFF  16
#define LGL_VERT_CLR_OFF  0
// Thus having 40 bytes between each vertex.
#define LGL_VERT_STRIDE   40
// And each vertex indexed by a GLushort
#define LGL_INDX_STRIDE   2

#include "Defines.h"
#include GL_H 

#include "clsys.h"
#include "LGL.h"
#include "Vector.h"

// An important note:
// The actual values of the indexes are assumed to be relative
// to the number of vertexes currently in the vertex array
bool addIndexes(LVbo* lvbo, GLushort* inds, unsigned int count);
bool addVertexs(LVbo* lvbo, Vertex* verts, unsigned int count);

bool addLine(LVbo* lvbo, Vertex* verts, unsigned int count);

// Bind the buffers
void lvbo_bind(LVbo* lvbo);

// Call glDrawElements with appropriate arguments
void lvbo_draw(LVbo* lvbo);
void lvbo_draw_lines(LVbo* lvbo);

void lvbo_draw_with_pointers(LVbo* lvbo);
void lvbo_draw_lines_with_pointers(LVbo* lvbo);


#endif
