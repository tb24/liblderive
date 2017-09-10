#ifndef STACK_H
#define STACK_H

typedef struct Stack Stack;
typedef union StackData
{
  void*           sdData;
} StackData;

Stack* stack_make ( );
void   stack_free ( Stack* );
bool   stack_push(Stack* stack, StackData data);
bool   stack_pop  ( Stack* s, StackData* sd);

// Not really needed at the moment, no pointers stored on stack alone
//void       stack_make_with_finalizer(Stack*, void (*finalizer)(void*data));
#endif

