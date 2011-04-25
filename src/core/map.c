#include "memory.h"
#include "map.h"

#include <stdint.h>
#include <stddef.h>
#include <string.h>

struct key;
struct value;

#define KEY(op) ((struct key *)((op)->key.p))
#define VALUE(op) ((struct value *)((op)->value))

struct node {
	struct value *value;
	struct key *key;
	struct node *next;
};

struct map {
	int freenode;
	int size;
	struct node* buffer;
};

static inline void
init_table(struct map *m)
{
	memset(m->buffer,0,m->size*sizeof(struct node));
}

struct map* 
mapCreate(void)
{
	struct map *m=(struct map*)memoryAlloc(sizeof(struct map));
	m->size=2;
	m->freenode=0;
	m->buffer=(struct node*)memoryAlloc(sizeof(struct node)*m->size);
	init_table(m);	

	return m;
}

void
mapRelease(struct map *m)
{
	if (m) {
		memoryFree(m->buffer);
		memoryFree(m);
	}
}

static inline size_t
hash(struct map *m,struct key *key)
{
	intptr_t k=(intptr_t)key;
	return (size_t)(((k>>3)^k)&(m->size-1));
}

static struct node*
map_find(struct map *m,struct key *key)
{
	size_t k=hash(m,key);
	struct node *temp=m->buffer+k;
	do {
		if (temp->key==key)
			return temp;
		temp=temp->next;
	} while (temp);
	return 0;
}

static struct node *
map_begin(struct map *m)
{
	int i;
	for (i=0;i<m->size;i++) {
		if (m->buffer[i].value!=0) {
			return &(m->buffer[i]);
		}
	}
	return 0;
}

static struct node *
map_next(struct map *m,struct node *next)
{
	for (;;) {
		++next;
		if (next-m->buffer>=m->size) {
			return 0;
		}
		else if (next->value!=0) {
			return next;
		}
	}
}

static struct node *
expand_search(struct map *m,struct key *key);

static struct node *
map_search(struct map *m,struct key *key)
{
	int i;
	size_t k=hash(m,key);
	struct node *temp=m->buffer+k;
	for (;;) {
		if (temp->value==0) {
			struct node *slot=temp;
			temp=temp->next;
			while (temp) {
				if (temp->key==key) {
					return temp;
				}
				temp=temp->next;
			}
			slot->key=key;
			return slot;
		}
		else if (temp->key==key)
			return temp;
		if (temp->next==0) {
			break;
		}
		temp=temp->next;
	}
	for (i=m->freenode;i<m->size;i++) {
		if (m->buffer[i].value==0) {
			m->buffer[i].key=key;
			temp->next=m->buffer+i;
			m->freenode=i+1;
			return &(m->buffer[i]);
		}
	}

	return expand_search(m,key);
}

static struct node *
expand_search(struct map *m,struct key *key)
{	
	int i;
	int s=m->size;
	struct node *old=m->buffer;

	struct node *buffer=(struct node*)memoryAlloc(m->size*2*sizeof(struct node));
	if (buffer==0) {
		return 0;
	}

	m->size*=2;
	m->freenode=0;
	m->buffer=buffer;
	init_table(m);

	for (i=0;i<s;i++) {
		if (old[i].value!=0) {
			map_search(m,old[i].key)->value = old[i].value;
		}
	}

	memoryFree(old);

	return map_search(m,key);
}

static void *
_findfirst(struct map *m, struct map_op * op)
{
	struct node *v = map_begin(m);
	op->value = v;
	if (v == NULL) {
		op->key.p = NULL;
		return NULL;
	}
	else {
		op->key.p = v->key;
	}
	return v->value;
}

static void *
_findnext(struct map *m, struct map_op * op)
{
	if (op->value == NULL) {
		return _findfirst(m,op);
	}
	struct node *v = map_next(m, op->value);
	op->value = v;
	if (v == NULL) {
		op->key.p = NULL;
		return NULL;
	}
	else {
		op->key.p = v->key;
	}
	return v->value;
}

void * 
mapSearch(struct map * m,struct map_op * op)
{
	switch (op->op) {
	case MAP_SEARCH: {
		struct node *n = map_find(m,KEY(op));
		if (n==NULL) {
			return NULL;
		} else {
			return n->value;
		}
	}
	case MAP_INSERT: {
		struct node *v = map_search(m, KEY(op));
		void * ret = v->value;
		v->value = op->value;
		return ret;
	}
	case MAP_REMOVE: {
		struct node *n = map_find(m,KEY(op));
		if (n==NULL) {
			return NULL;
		} else {
			void *ret = n->value;
			n->value = NULL;
			return ret;
		}
	}
	case MAP_TRAVERSE: {
		return _findnext((struct map *)m, op);
	}
	case MAP_SLOT: {
		struct node *v = map_search(m, KEY(op));
		return (void *) &(v->value);
	}
	default:
		return NULL;
	};
}
