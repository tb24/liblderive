#ifndef LTURTLE_H
#define LTURTLE_H

#include "Stack.h"
#include "Colors.h"
#include "Vector.h"

typedef struct LTurtleState 
{
  Vec3f pos;	
  Vec3f up;
  Vec3f head;
  Vec3f left;
  float width;
  ColorHSVA color;	

} LTurtleState;

typedef struct LTurtle 
{
  LTurtleState* currentState;
  Stack* turtleStack;
} LTurtle;


LTurtle* lturtle_make();
void     lturtle_free(LTurtle*);

void tForward   ( LTurtle* turt, float dist );
void tLeft      ( LTurtle* turt, float angle );
void tRight     ( LTurtle* turt, float angle );
void tUp        ( LTurtle* turt, float angle );
void tDown      ( LTurtle* turt, float angle );
void tRollLeft  ( LTurtle* turt, float angle );
void tRollRight ( LTurtle* turt, float angle );
void tSetWidth  ( LTurtle* turt, float width );
void tSetColor  ( LTurtle* turt, ColorHSVA c );
void tShiftHue  ( LTurtle* turt, float amt   );
void tShiftSat  ( LTurtle* turt, float amt   );
void tShiftVal  ( LTurtle* turt, float amt   );
void tPush      ( LTurtle* turt );
void tPop       ( LTurtle* turt );
void tUnPush    ( LTurtle* turt );
void tRePush    ( LTurtle* turt );


#endif
