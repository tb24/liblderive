#ifndef LTURTLE_C
#define LTURTLE_C

#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#include "Colors.h"
#include "Vector.h"
#include "LTurtle.h"
#include "Stack.h"

LTurtleState* lturtstate_make();
void          lturtstate_free(LTurtleState*);
LTurtleState* lturtstate_copy(LTurtleState* ts);

LTurtle* lturtle_make()
{ 
  LTurtle* turt;

  if( !(turt = malloc(sizeof *turt) ) )
    return NULL;

  if( !(turt->currentState = lturtstate_make() ) )
    return 
      free(turt), 
      NULL;

  if( !(turt->turtleStack  = stack_make() ) )
    return
      lturtstate_free(turt->currentState),
      free(turt),
      NULL;

  return turt;
}

void lturtle_free(LTurtle* turt)
{
  StackData sd;
  while( stack_pop(turt->turtleStack,&sd) )
    lturtstate_free((LTurtleState*)sd.sdData);

  lturtstate_free(turt->currentState);
  stack_free(turt->turtleStack); 

  free(turt);
}

void tForward   ( LTurtle* turt, float dist )
{

  turt->currentState->pos.x = turt->currentState->pos.x + turt->currentState->head.x * dist;
  turt->currentState->pos.y = turt->currentState->pos.y + turt->currentState->head.y * dist;
  turt->currentState->pos.z = turt->currentState->pos.z + turt->currentState->head.z * dist;

}
void tLeft     ( LTurtle* turt, float angle )
{
  turt->currentState->head = vRotateCopy(&turt->currentState->up, angle, &turt->currentState->head);
  turt->currentState->left = vRotateCopy(&turt->currentState->up, angle, &turt->currentState->left);
}
void tRight     ( LTurtle* turt, float angle )
{
  turt->currentState->head = vRotateCopy(&turt->currentState->up, -1.0*angle, &turt->currentState->head);
  turt->currentState->left = vRotateCopy(&turt->currentState->up, -1.0*angle, &turt->currentState->left);	
}
void tUp        ( LTurtle* turt, float angle )
{
  turt->currentState->head = vRotateCopy(&turt->currentState->left, angle, &turt->currentState->head);
  turt->currentState->up   = vRotateCopy(&turt->currentState->left, angle, &turt->currentState->up);	
}
void tDown      ( LTurtle* turt, float angle )
{
  turt->currentState->head = vRotateCopy(&turt->currentState->left, -1.0*angle, &turt->currentState->head);
  turt->currentState->up   = vRotateCopy(&turt->currentState->left, -1.0*angle, &turt->currentState->up);	
}
void tRollLeft  ( LTurtle* turt, float angle )
{
  turt->currentState->up   = vRotateCopy(&turt->currentState->head, angle, &turt->currentState->up);
  turt->currentState->left = vRotateCopy(&turt->currentState->head, angle, &turt->currentState->left);	
}
void tRollRight ( LTurtle* turt, float angle )
{
  turt->currentState->up   = vRotateCopy(&turt->currentState->head, -1.0*angle, &turt->currentState->up);
  turt->currentState->left = vRotateCopy(&turt->currentState->head, -1.0*angle, &turt->currentState->left);	
}
void tSetWidth  ( LTurtle* turt, float newWidth )
{
  turt->currentState->width = newWidth;
}
void tSetColor  ( LTurtle* turt, ColorHSVA color )
{
  if( 0.0 <= color.h && color.h <= 360 &&
      0.0 <= color.s && color.s <= 1.0 &&
      0.0 <= color.v && color.v <= 1.0 &&
      0.0 <= color.a && color.a <= 1.0	
  )
    turt->currentState->color = color;
}

void tShiftHue  ( LTurtle* turt, float amt   ){ 
  VERB( fprintf(stderr, "hsv: %f %f %f\n", 
    turt->currentState->color.h,
    turt->currentState->color.s,
    turt->currentState->color.v
    );
  )
  turt->currentState->color.h = turt->currentState->color.h + amt;
  if(turt->currentState->color.h < 0.0 )
    turt->currentState->color.h = 360.0;
  if(turt->currentState->color.h > 360.0 )
    turt->currentState->color.h = 0.0;

}
void tShiftSat  ( LTurtle* turt, float amt   ){ 
  turt->currentState->color.s = turt->currentState->color.s;
  if(turt->currentState->color.s < 0.0 )
    turt->currentState->color.s = 0.0;
  if(turt->currentState->color.s > 1.0 )
    turt->currentState->color.s = 1.0;

}
void tShiftVal  ( LTurtle* turt, float amt   ){ 
  turt->currentState->color.v = fmod(turt->currentState->color.v + amt, 1.0);
  if(turt->currentState->color.v < 0.0 )
    turt->currentState->color.v = 0.0;
  if(turt->currentState->color.v > 1.0 )
    turt->currentState->color.v = 1.0;
}

void tPush      ( LTurtle* turt )
{
  if( turt )
  {
    StackData sd = { lturtstate_copy(turt->currentState) };
    stack_push(turt->turtleStack, sd);
  }
}

void tPop       ( LTurtle* turt )
{
  StackData sd;
  if( stack_pop(turt->turtleStack,&sd) )
  {
    LTurtleState* tmpState = sd.sdData;
    lturtstate_free(turt->currentState);
    turt->currentState = tmpState;
  }
  
}
//  It's like a pop that can be reversed (until the next pop)
void tUnPush      ( LTurtle* turt )
{
// the stack now becomes 
//  if( turt )
//    turtleStack_push(turt->turtleStack, lturtstate_copy(turt->currentState));
}

// This reverses the previous call to UnPush
void tRePush      ( LTurtle* turt )
{
// if( turt )
//    turtleStack_push(turt->turtleStack, lturtstate_copy(turt->currentState));
}

LTurtleState* lturtstate_make()
{
  LTurtleState* ts;
  if( !(ts = malloc(sizeof *ts) ) )
    return NULL;

  ts->pos   = vec3f(  0,  0,  0 ); 
  ts->up    = vec3f(  0,  0, -1 );
  ts->head  = vec3f(  0,  1,  0 );
  ts->left  = vec3f( -1,  0,  0 );

  ts->color = colorHSVA(1, 1, 1, 1);

  ts->width = 1.0;

  return ts;

}

LTurtleState* lturtstate_copy(LTurtleState* ts)
{
  LTurtleState* tsCopy;

  if( ! ( tsCopy = lturtstate_make() ) )
    return NULL;

  tsCopy->pos   = ts->pos;
  tsCopy->up    = ts->up;
  tsCopy->head  = ts->head;
  tsCopy->left  = ts->left;
  tsCopy->color = ts->color;	
  tsCopy->width = ts->width;

  return tsCopy;

}


void lturtstate_free(LTurtleState* ts)
{
  free(ts);
}

#endif
