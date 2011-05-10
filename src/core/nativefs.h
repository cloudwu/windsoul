#ifndef windsoul_native_fs_h
#define windsoul_native_fs_h

#include "atom.h"
#include <stddef.h>

size_t nativefsSize(void *fs , const char *name);
int nativefsRead(void *fs , const char *name, size_t pos, void *buffer, int sz);
int nativefsList(void *fs, const char *name, atom *buffer, int sz);
int nativefsCreate(void *fs,const char *name, char mode);

void* nativefsCreateFS(void *arg);

#endif
