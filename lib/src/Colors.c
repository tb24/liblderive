#ifndef COLOR_C
#define COLOR_C

#include <math.h>
#include "Defines.h"
#include GL_H
#include "Colors.h"

ColorHSVA colorHSVA(GLfloat h, GLfloat s, GLfloat v, GLfloat a)
{
  ColorHSVA c = { h, s, v, a };
  return c;
}

ColorHSVA rgbaToHsva(ColorRGBA* c)
{
  GLfloat min, max, delta;
  GLfloat h,s,v;
  min = c->r < c->g ? (c->r < c->b ? c->r : c->b) : (c->g < c->b ? c->g : c->b );
  max = c->r > c->g ? (c->r > c->b ? c->r : c->b) : (c->g > c->b ? c->g : c->b );

  v = max;                         
  delta = max - min;
  if( max != 0 )
    s = delta / max; 
  else 
    return colorHSVA(0,0,0,0);
  if( c->r == max )
    h = ( c->g - c->b ) / delta;         // between yellow & magenta
  else if( c->g == max )
    h = 2 + ( c->b - c->r ) / delta;     // between cyan & yellow
  else
    h = 4 + ( c->r - c->g ) / delta;     // between magenta & cyan
  h *= 60;                               // degrees
  if( h < 0 )
    h += 360;

  return colorHSVA(h,s,v,c->a);
       
}

ColorRGBA colorRGBA(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
{
  ColorRGBA c = { r, g, b, a };
  return c;
}

ColorRGBA hsva2rgba(ColorHSVA* c)
{
 GLfloat f,m,n,k;
 int  i; 

 if ( c->s <= 0.0001 ) return colorRGBA(c->v, c->v, c->v, c->a);
 if ( c->h >= 359.999 )
   i = 0;
 else 
   i = floor(c->h/60.0);
 f = c->h/60 - i;
 m = c->v * ( 1 - c->s );
 n = c->v * ( 1 - c->s * f );
 k = c->v * ( 1 - c->s * ( 1 - f ) );

 switch(i) {
   case  0: return colorRGBA(c->v,    k,    m, c->a);
   case  1: return colorRGBA(   n, c->v,    m, c->a);
   case  2: return colorRGBA(   m, c->v,    k, c->a);
   case  3: return colorRGBA(   m,    n, c->v, c->a);
   case  4: return colorRGBA(   k,    m, c->v, c->a);
   case  5: return colorRGBA(c->v,    m,    n, c->a);
   default: return colorRGBA(c->v, c->v, c->v, c->a);
 }

}



#endif
