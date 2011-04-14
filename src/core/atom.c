#include "atom.h"
#include "memory.h"

#include <stddef.h>
#include <string.h>

#define POOL_PAGE 0x1000
#define INIT_SIZE 2

struct hash_string {
	unsigned long hash;
	size_t sz;
	char str[1];
};

struct node {
	struct hash_string *value;
	struct node *next;
};

static struct map {
	int freenode;
	int size;
	struct node* buffer;
} g_map;

static unsigned long
hash(const char *name,size_t len)
{
	unsigned long h=(unsigned long)len;
	size_t step = (len>>5)+1;
	size_t i;
	for (i=len; i>=step; i-=step)
	    h = h ^ ((h<<5)+(h>>2)+(unsigned long)name[i-1]);
	return h;
}

static struct hash_string *
push_string(const char *str,size_t len,unsigned long hash)
{
	struct hash_string *ret;

	size_t flen=len+offsetof(struct hash_string, str)+1;

	ret=(struct hash_string *)memoryPermanent(flen);
	ret->hash = hash;
	ret->sz = len;

	memcpy(ret->str,str,len);

	ret->str[len]='\0';

	return ret;
}

static int 
hash_string_compare(struct hash_string *hs,const char *str,size_t len,unsigned long h)
{
	int r;
	if (hs->str==str) {
		return 0;
	}
	if (hs->hash!=h) {
		return 1;
	}
	r=memcmp(hs->str,str,len);
	if (r==0) {
		return hs->str[len];
	}
	else {
		return 1;
	}
}

static inline void
init_table()
{
	memset(g_map.buffer,0,g_map.size*sizeof(struct node));
}

static void
copy_slot(struct hash_string *hs)
{
	int i;
	struct node *temp=g_map.buffer+(hs->hash & (g_map.size-1));
	if (temp->value==0) {
		temp->value=hs;
		return;
	}

	for (i=g_map.freenode;i<g_map.size;i++) {
		if (g_map.buffer[i].value==0) {
			g_map.freenode=i+1;
			g_map.buffer[i].value=hs;
			g_map.buffer[i].next=temp->next;
			temp->next=g_map.buffer+i;
			break;
		}
	}
}

static const struct hash_string *
build_internal(const char *str,size_t len,unsigned long k)
{
	int i;
	struct node *temp=g_map.buffer+(k & (g_map.size-1));

	for (;;) {
		if (temp->value==0) {
			temp->value=push_string(str,len,k);
			return temp->value;
		}
		else if (!hash_string_compare(temp->value,str,len,k))
			return temp->value;
		if (temp->next==0) {
			break;
		}
		temp=temp->next;
	}

	for (i=g_map.freenode;i<g_map.size;i++) {
		if (g_map.buffer[i].value==0) {
			g_map.buffer[i].value=push_string(str,len,k);
			temp->next=g_map.buffer+i;
			g_map.freenode=i+1;
			return g_map.buffer[i].value;
		}
	}

	return 0;
}

static const struct hash_string *
expand_search(const char *str, size_t len, unsigned long k)
{	
	int i;
	int s=g_map.size;
	struct node *tmp=g_map.buffer;

	struct node *buffer=(struct node*)memoryAlloc(g_map.size*2*sizeof(struct node));
	if (buffer==0) {
		return 0;
	}

	g_map.size*=2;
	g_map.freenode=0;
	g_map.buffer=buffer;
	init_table();

	for (i=0;i<s;i++) {
		copy_slot(tmp[i].value);
	}

	memoryFree(tmp);

	return build_internal(str,len,k);
}

static inline const void *
_atom_build(const void *data, size_t len)
{
	const char *str = data;
	unsigned long k=hash(str,len);

	const struct hash_string *ret=build_internal(str,len,k);

	if (ret==0) {
		return expand_search(str,len,k)->str;
	}
	else {
		return ret->str;
	}
}

atom
atomBuild(const void *data, size_t len)
{
	return (atom)_atom_build(data,len);
}

size_t 
atomLength(atom str)
{
	char * ptr = (char *)str;
	ptr -= offsetof(struct hash_string, str);
	struct hash_string *hs = (struct hash_string *)ptr;
	return hs->sz;
}

atom 
atomBuildSep(const char *str,const char *sep,size_t *outlen)
{
	size_t len=0;
	const char *tmp=str;
	while (*tmp!='\0' && !strchr(sep,*tmp)) {
		++tmp;
		++len;
	}
	if (outlen) {
		*outlen = len;
	}

	return (atom)_atom_build(str,len);
}

int
atomInit()
{
	g_map.size=INIT_SIZE;
	g_map.freenode=0;
	g_map.buffer=(struct node*)memoryAlloc(sizeof(struct node)*g_map.size);
	init_table();	
	return 0;
}
