#ifndef windsoul_path_h
#define windsoul_path_h

#include "atom.h"

struct filesystem;

void pathMount(const char * root, struct filesystem *);
int pathList(atom *, int n);
atom pathLocate(atom, const char *relative);
struct filesystem* pathGet(atom, const char **path);

#endif
