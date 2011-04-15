#ifndef windsoul_map_h
#define windsoul_map_h

#include <stdint.h>

struct map;

#define MAP_SEARCH 0
#define MAP_INSERT 1
#define MAP_REMOVE 2
#define MAP_ITERATOR 3

struct map_op {
	int op;
	union {
		uintptr_t i;
		void *p;
	} key;
	void * value;
};

struct map * mapCreate(void);
void mapRelease(struct map *);
void* mapSearch(struct map *, struct map_op * op);

#endif
