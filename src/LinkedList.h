#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct LinkedList
{
  void* llData;
  struct LinkedList* llNext;
} LinkedList;


LinkedList* cons(void*, struct LinkedList*);
void*       head(struct LinkedList*);
LinkedList* tail(struct LinkedList*);
int lengthLL(struct LinkedList*);
void** llToReversedArray(struct LinkedList*);

// Will not call free() on the data
void free_ll(struct LinkedList*);

void foreach(LinkedList*, void(*f)(void*l));

#endif
