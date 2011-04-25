#include "set.h"
#include "map.h"

#include <stdint.h>

struct set;

#define MAP(a) ((struct map *)a)
#define SET(a) ((struct set *)a)

struct set* 
setCreate(void)
{
	return SET(mapCreate());
}

void 
setRelease(struct set *s)
{
	mapRelease(MAP(s));
}

int 
setOperate(struct set *s, void *e, int op)
{
	struct map_op mop;
	switch (op) {
	case SET_EXIST:
		mop.op = MAP_SEARCH;
		mop.key.p = e;
		return (intptr_t)mapSearch(MAP(s),&mop);
	case SET_PUSH: {
		mop.op = MAP_SLOT;
		mop.key.p = e;
		intptr_t * v = (intptr_t *)mapSearch(MAP(s), &mop);
		int ret = (int) *v;
		++ *v;
		return ret;
	}
	case SET_POP: {
		mop.op = MAP_SLOT;
		mop.key.p = e;
		intptr_t * v = (intptr_t *)mapSearch(MAP(s), &mop);
		int ret = (int) *v;
		if (ret>0) {
			-- *v;
		}
		return ret - 1;
	}
	case SET_PUSHONCE: {
		mop.op = MAP_SLOT;
		mop.key.p = e;
		intptr_t * v = (intptr_t *)mapSearch(MAP(s), &mop);
		if (*v == 0) {
			*v = 1;
			return 0;
		}
		return (int)*v;
	}
	default:
		return 0;
	}
}

void* 
setTraverse(struct set *s, void ** state)
{
	struct map_op mop;
	mop.op = MAP_TRAVERSE;
	mop.value = *state;
	mapSearch(MAP(s), &mop);
	*state = mop.value;
	return mop.key.p;
}
