#ifndef windsoul_vfs_h
#define windsoul_vfs_h

#include "atom.h"

#include <stddef.h>
#include <stdint.h>

#define VFS_SEEK_SET 0
#define VFS_SEEK_CURRENT 1
#define VFS_SEEK_END 2

struct file;

int vfsMount(const char * root, const char * type, void * arg);
int vfsList(atom, atom *, int n);
atom vfsMkdir(atom, const char *name);
int vfsRemove(atom);
int vfsChSize(atom, size_t sz);

struct file * vfsOpen(atom, const char * mode);
void vfsClose(struct file *);
int vfsRead(struct file *, void *buffer, int sz);
int vfsWrite(struct file *, const void *buffer, int sz);
size_t vfsSeek(struct file *, int64_t pos, int where);

#endif
