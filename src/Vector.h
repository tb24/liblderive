#ifndef VECTOR_H 
#define VECTOR_H 

#include <stdio.h>
#include "Defines.h"
#include "Colors.h"
#include GL_H 

typedef struct Vec3f
{
  GLfloat x, y, z;
} Vec3f;


typedef struct Vertex
{

  ColorRGBA c;
  Vec3f n;
  Vec3f p;

} Vertex;

typedef struct VertexCP
{
  ColorRGBA c;
  Vec3f p;
} VertexCP;

void vNorm  (Vec3f* vecToNormalize );
void vAdd   (Vec3f* vec1, Vec3f* vec2,  Vec3f* result);
void vSub   (Vec3f* vec1, Vec3f* vec2,  Vec3f* result);
void vScale (Vec3f* vec,  float amount, Vec3f* result);
// sets $result to the the vector resulting from a rotatation of $v around $axis by $angle degrees
void vRotate(Vec3f* axis, float angle,  Vec3f* v, Vec3f* result);
// Like above, but returns a copy instead of setting an argument
Vec3f vRotateCopy(Vec3f* axis, float angle, Vec3f* v);

Vec3f vec3f(float x, float y, float z);

float vLength(Vec3f *v);

void printVertex(Vertex *v, FILE* stream);

#endif
