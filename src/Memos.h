#ifndef MEMOS_H
#define MEMOS_H

#include <stdbool.h>

#define MEMOS_CHUNK_SIZE 200

typedef struct Memos
{
  void** data;
  unsigned int size;
  unsigned int next;
} Memos;


// Creates a new, empty, memory pool
Memos* memos_new();

// Returns the original pointer on success
// Returns NULL on failure
void* memoize(Memos*, void*);


// Adds a pointer that was allocated with malloc() to the pool
bool memos_add(Memos*, void*);

// Frees all the pointers and frees the Memos 
void memos_free(Memos*);

// Leaves the pointers intact but frees the Memos
void memos_forget(Memos*);

bool resize(Memos* memos);

#endif
