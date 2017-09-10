#ifndef LINKEDLIST_C
#define LINKEDLIST_C

#include "LinkedList.h"
#include <stdlib.h>

LinkedList* cons(void* data, LinkedList* tail)
{
  LinkedList* head;
  
  if( data == NULL ) return tail;
  if( !( head = malloc( sizeof *head) ) )
    return NULL;

  head->llData = (void*)data;
  head->llNext = tail;
  return head;
}

int lengthLL(struct LinkedList*ll)
{
  if ( ll != NULL ) return (1 + lengthLL(ll->llNext));
  else return 0;
}

// Create a null-terminated array of pointers to data from the linked list
void** llToReversedArray(struct LinkedList* ll)
{ 
  int elems = lengthLL(ll)+1;
  void** data = malloc( elems * sizeof *data );

  if( data )
  {
    struct LinkedList* this = ll;

    data[elems-1] = NULL;

    for(int i=elems-2; this != NULL; i--, this=this->llNext)
      data[i] = (void*)this->llData;

  }

  return data;
}

void free_ll(struct LinkedList* l)
{
  struct LinkedList* curr = l;
  while( curr != NULL ){ 
    struct LinkedList* tmp = curr->llNext;
    free(curr);
    curr = tmp;
  }
}

void foreach(LinkedList* ll, void(*f)(void*))
{
  for(LinkedList* curr = ll; curr; curr = curr->llNext)
    f(curr->llData);
}

#endif
