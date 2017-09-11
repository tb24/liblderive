#ifndef COLOR_H
#define COLOR_H

#include <stdio.h>
#include "Defines.h"
#include GL_H 

typedef struct ColorRGBA
{
  GLfloat r, g, b, a;
} ColorRGBA;


typedef struct ColorHSVA
{
  GLfloat h, s, v, a;
} ColorHSVA;

ColorRGBA colorRGBA(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
ColorHSVA colorHSVA(GLfloat h, GLfloat s, GLfloat v, GLfloat a);

ColorHSVA rgba2hsva(ColorRGBA*);
ColorRGBA hsva2rgba(ColorHSVA*);


#endif
