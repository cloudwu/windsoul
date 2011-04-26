#include <stdlib.h>
#include <string.h>

#include "memory.h"

void *
memoryAlloc(size_t size)
{
	return malloc(size);
}

void
memoryFree(void *p)
{
	free(p);
}

void *
memoryExpand(void *p, size_t osize, size_t nsize)
{
	if (p==NULL)
		return malloc(nsize);
	void *np = malloc(nsize);
	memcpy(np,p,osize);
	free(p);
	return np;
}

void *
memoryPermanent(size_t size)
{
	return malloc(size);
}

void *
memoryTemp(size_t size)
{
	static void *temp = NULL;
	static size_t sz = 0;
	if (size > sz) {
		sz = size;
		free(temp);
		temp = malloc(sz);
	}
	return temp;
}

