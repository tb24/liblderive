#ifndef COMMON_C
#define COMMON_C

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int arrayLength(void**arr)
{
  if(arr == NULL) return 0;
  unsigned int i = -1;
  while(arr[++i] != NULL);
  return i;
}

// supplied function must return pointer or pointer sized values
void * map(void* (*f)(void*l), void ** list)
{
  if(list==NULL) return NULL;

  void** rval = malloc(sizeof(void*)*arrayLength(list));
  void* l;
  int i=-1;
  while(true)
  {
    if( ( l = list[++i]) == NULL ) break;
    rval[i] = (void*)(*f)(l);
  }
  return rval; 
}

// Takes a null-terminated list and calls a void function on each element
void iterate(void(*f)(void*), void** list)
{
  if(list==NULL) return;
  
  void *l;
  int i=-1;
  while(true)
  {
    if( ( l = list[++i]) == NULL ) break;
    (*f)(l);
  }
}

// Calls free on each element of a null terminated array
void freeArray(void** array)
{
  for(unsigned int i=0; array[i] != NULL; i++)
    free(array[i]);
}

void iterateAndInbetween(void(*f)(void*), void(*g)(void), void** list)
{
  if(list==NULL) return;
  
  void *l;
  int i=-1;
  while(true)
  {
    if( ( l = list[++i]) == NULL ) break;
    (*f)(l);
    if( list[i+1] != NULL ) (*g)();
  }
}

// Some systems don't have this function
char* my_strndup(const char *s, size_t n)
{
  char *result;
  size_t len = strlen (s);

  if (n < len)
    len = n;

  result = (char *) malloc (len + 1);
  if (!result)
    return 0;

  result[len] = '\0';
  return (char *) memcpy (result, s, len);
}

#endif
