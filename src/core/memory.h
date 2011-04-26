#ifndef WINDSOUL_MEMORY_H
#define WINDSOUL_MEMORY_H

#include <stddef.h>

void * memoryAlloc(size_t size);
void memoryFree(void *p);
void * memoryExpand(void *p, size_t osize, size_t nsize);
void * memoryPermanent(size_t size);
void * memoryTemp(size_t size);

#endif
