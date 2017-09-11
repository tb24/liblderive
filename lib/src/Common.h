#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>

int arrayLength(void**arr);
void iterate(void(*f)(void*l), void** list);
void iterateAndInbetween(void(*f)(void*), void(*g)(void), void** list);
void * map(void* (*f)(void*l), void ** list);
void freeArray(void** array);

char * my_strndup (const char *s, size_t n);
#endif
