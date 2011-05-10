#include "vfs.h"
#include "atom.h"
#include "memory.h"
#include "path.h"
#include "nativefs.h"

#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 4096
#define NONE_EXIST (size_t)(-1)
#define MODE_READ 1
#define MODE_WRITE 2

struct filesystem {
	void * fs;
	int (*list)(void *fs, const char *name, atom *buffer, int sz);
	int (*chsize)(void *fs, const char *name, size_t sz);
	/* 'c' for create , 'q' for query , 'd' for mkdir , 'r' for remove */
	int (*create)(void *fs,const char *name, char mode);
	size_t (*size)(void *fs , const char *name);
	int (*read)(void *fs , const char *name, size_t pos, void *buffer, int sz);
	int (*write)(void *fs , const char *name, size_t pos, const void *buffer, int sz);
};

struct file {
	struct filesystem *fs;
	const char * name;
	size_t size;
	size_t pos;
	int mode;
};

int 
vfsList(atom pathname, atom *array, int n)
{
	const char * root = atomToString(pathname);
	if (root[0] == '\0' || (root[0] == '/' && root[1]=='\0')) {
		return pathList(array,n);
	}
	const char *name=NULL;
	struct filesystem * fs = pathGet(pathname, &name);
	if (fs == NULL)
		return -1;
	return fs->list(fs->fs,name,array,n);
}

atom 
vfsMkdir(atom pathname, const char *dir)
{
	const char * name = NULL;
	struct filesystem * fs = pathGet(pathname, &name);
	if (fs == NULL)
		return NULL;
	if (fs->create(fs->fs, name ,'d')) {
		return pathLocate(pathname, dir);
	} else {
		return NULL;
	}
}

int 
vfsRemove(atom pathname)
{
	const char *name=NULL;
	struct filesystem * fs = pathGet(pathname, &name);
	if (fs == NULL)
		return 0;
	return fs->create(fs->fs, name ,'r');
}

int 
vfsChSize(atom pathname, size_t sz)
{
	const char *name=NULL;
	struct filesystem * fs = pathGet(pathname, &name);
	if (fs == NULL)
		return 0;
	if (fs->chsize == NULL)
		return 0;
	return fs->chsize(fs->fs, name, sz);
}

struct file * 
vfsOpen(atom pathname, const char * m)
{
	const char *name=NULL;
	struct filesystem * fs = pathGet(pathname, &name);
	if (fs == NULL)
		return NULL;
	int mode = 0;
	if (strchr(m,'r')) {
		mode |= MODE_READ;
		if (fs->read == NULL)
			return NULL;
	}
	if (strchr(m,'w')) {
		mode |= MODE_WRITE;
		if (fs->write == NULL)
			return NULL;
	}
	if (mode & (MODE_WRITE|MODE_READ)) {
		if (!fs->create(fs->fs,name,'q')) {
			if (!fs->create(fs->fs,name,'c')) {
				return NULL;
			}
		}
	} else if (mode & MODE_WRITE) {
		if (!fs->create(fs->fs,name,'c')) {
			return NULL;
		}
	} else if (mode & MODE_READ) {
		if (!fs->create(fs->fs,name,'q')) {
			return NULL;
		}
	} else {
		return NULL;
	}

	size_t sz = fs->size(fs->fs,name);
	if (sz == NONE_EXIST) {
		return NULL;
	}
	struct file *f = memoryAlloc(sizeof(struct file));
	f->fs = fs;
	f->name = name;
	f->size = sz;
	f->pos = 0;
	f->mode = mode;
	return f;
}

void 
vfsClose(struct file *f)
{
	if (f==NULL)
		return;
	memoryFree(f);
}

size_t 
vfsSeek(struct file *f, int64_t pos, int where)
{
	int64_t current = (int64_t)f->pos;
	switch(where) {
	case VFS_SEEK_SET:
		current = pos;
		break;
	case VFS_SEEK_CURRENT:
		current += pos;
		break;
	case VFS_SEEK_END:
		current = (int64_t)f->size + pos;
		break;
	}
	if (current < 0)
		current = 0;
	else if (current > (int64_t)f->size)
		current = (int64_t)f->size;
	f->pos = (size_t) current;
	return f->pos;
}

int 
vfsRead(struct file *f, void *buffer, int sz)
{
	int n = f->fs->read(f->fs->fs,f->name,f->pos,buffer,sz);
	if (n>=0) {
		f->pos += n;
	}
	return n;
}

int 
vfsWrite(struct file *f, const void *buffer, int sz)
{
	int n = f->fs->write(f->fs->fs,f->name,f->pos,buffer,sz);
	if (n>=0) {
		f->pos += n;
	}
	return n;
}

int 
vfsMount(const char * root, const char * type, void * arg)
{
	struct filesystem * fs = NULL;
	if (strcmp(type,"native")==0) {
		void * f = nativefsCreateFS(arg);
		if (f==NULL)
			return 0;
		fs = memoryPermanent(sizeof(*fs));
		memset(fs,0,sizeof(*fs));
		fs->fs = f;
		fs->list = nativefsList;
		fs->create = nativefsCreate;
		fs->read = nativefsRead;
	} else {
		return 0;
	}
	pathMount(root, fs);

	return 1;
}

int
vfsInit(void)
{
	vfsMount("_","native",".");
	return 0;
}
