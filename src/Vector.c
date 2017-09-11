#ifndef VECTOR_C
#define VECTOR_C

#include <math.h>
#include <stdio.h>
#include "Vector.h"

Vec3f vec3f(float x, float y, float z){
  Vec3f v = { x, y, z };
  return v;
}


void vAdd(Vec3f * v1, Vec3f * v2, Vec3f* result)
{
  result->x = v1->x + v2->x;
  result->y = v1->y + v2->y;
  result->z = v1->z + v2->z;	
}

void vSub(Vec3f * v1, Vec3f * v2, Vec3f* result)
{
  result->x = v1->x - v2->x;
  result->y = v1->y - v2->y;
  result->z = v1->z - v2->z;	
}

void vScale(Vec3f * v1, float amount, Vec3f* result)
{
  result->x = v1->x * amount;
  result->y = v1->y * amount;
  result->z = v1->z * amount;	
}

void vNorm(Vec3f * v)
{
  float len;
  len  = vLength(v); 
  v->x = v->x/len;
  v->y = v->y/len;
  v->z = v->z/len;
}

float vLength(Vec3f *v)
{
  return sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
}

Vec3f vRotateCopy(Vec3f* axis, float angle, Vec3f* v)
{
  Vec3f vnew;
  vRotate(axis, angle, v, &vnew);
  return vnew;
}
void vRotate(Vec3f* axis, float angle, Vec3f* v, Vec3f* newv)
{

  vNorm(v);
  vNorm(axis);

  float xz = axis->x*axis->z;
  float xy = axis->x*axis->y;                                                   
  float yz = axis->y*axis->z;

  float cosa = cos (angle*M_PI/180); 
  float sina = sin (angle*M_PI/180);                                          
  float mcosa = 1 - cosa;

  float xs = axis->x*sina;                                                      
  float ys = axis->y*sina;
  float zs = axis->z*sina;

  float m11 = powf(axis->x,2.0)*mcosa + cosa;
  float m12 = xy*mcosa - zs;
  float m13 = xz*mcosa + ys;
  float m21 = xy*mcosa + zs;
  float m22 = powf(axis->y,2.0)*mcosa + cosa;
  float m23 = yz*mcosa - xs;
  float m31 = xz*mcosa - ys;
  float m32 = yz*mcosa + xs;
  float m33 = powf(axis->z,2.0)*mcosa + cosa;


  newv->x = v->x*m11 + v->y*m12 + v->z*m13;
  newv->y = v->x*m21 + v->y*m22 + v->z*m23;
  newv->z = v->x*m31 + v->y*m32 + v->z*m33;

  vNorm(newv);

}
        
void printVertex(Vertex *v, FILE* stream)
{
  fprintf(stream,
    "%+4.4f %+4.4f %+4.4f | %+4.4f %+4.4f %+4.4f | %+4.4f %+4.4f %+4.4f",
    v->p.x, v->p.y, v->p.z,
    v->n.x, v->n.y, v->n.z,
    v->c.r, v->c.g, v->c.b 
  ); 
}


#endif
