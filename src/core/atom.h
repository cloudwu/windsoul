#ifndef windsoul_atom_h
#define windsoul_atom_h

#include <stddef.h>

struct atom_t;
typedef struct atom_t * atom;

atom atomBuild(const void *data, size_t sz);
atom atomBuildSep(const char *str, const char *sep, size_t *len);
size_t atomLength(atom);

#define atomString(a) ((const char *)(a))

#endif

