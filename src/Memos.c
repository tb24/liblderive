#ifndef MEMOS_C
#define MEMOS_C

#include "Memos.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

struct Memos* memos_new()
{
  struct Memos* memos = malloc(sizeof(*memos));
  memos->data = malloc(sizeof(void*)*MEMOS_CHUNK_SIZE);
  memos->size = MEMOS_CHUNK_SIZE;
  memos->next = 0;
  memos->data[memos->next] = NULL;
  return memos;
}

void* memoize(struct Memos* memos, void*ptr)
{
  if(memos_add(memos,ptr)) return ptr;
  return NULL;
}

bool memos_add(struct Memos* memos, void*ptr)
{
  if(ptr == NULL) return true;

  if(memos->next >= memos->size-1)
    if(!resize(memos)) return false;

  memos->data[memos->next] = ptr; 
  memos->data[++(memos->next)] = NULL; 

  return true;

}

bool resize(struct Memos* memos)
{
  memos->size += MEMOS_CHUNK_SIZE;
  void** newData = realloc(memos->data, sizeof(void*)*memos->size);
  
  if( newData == NULL )
  {
    memos_free(memos);
    return false;
  }
  memos->data = newData; 
  return true;
}

void memos_free(struct Memos* memos)
{
  for(unsigned int i=0; memos->data[i] != NULL; i++) 
    free(memos->data[i]);
  free(memos->data);
  free(memos);
}

void memos_forget(struct Memos* memos)
{
  free(memos->data);
}

#endif
