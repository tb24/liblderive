#ifndef STACK_C
#define STACK_C

#include <stdlib.h>
#include <stdbool.h>

#include "Stack.h"
#include "LTurtle.h"

typedef struct StackElement StackElement;

struct StackElement
{
  StackData     seData;
  StackElement* seTail;
};

struct Stack
{
  StackElement* se;
};

Stack* stack_make()
{
  Stack*  stack;
  if( ! ( stack = malloc(sizeof *stack) ) )
    return NULL;
  stack->se = NULL;
  return stack;
}

bool stack_push(Stack* stack, StackData data)
{
  StackElement* se;
  
  if( ! ( se = malloc(sizeof *se) ) )
    return false;

  se->seData.sdData = data.sdData;
  se->seTail = stack->se;
  stack->se = se;

  return true;

}

bool stack_pop  ( Stack* s, StackData* sd)
{
  if( !s || !s->se )
    return false;

  sd->sdData = s->se->seData.sdData;

  StackElement* tmpSe = s->se->seTail;

  free(s->se);
  s->se = tmpSe; 

  DBG( fprintf(stderr, "pop:  %x", sd->sdData); )
  return true;
}

void stack_free(Stack* s)
{
  if(s)
  {
    StackData ignored;
    while(stack_pop(s, &ignored))
    { ; }
    free(s);
  }
}


#endif
