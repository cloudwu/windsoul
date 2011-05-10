#include "nativefs.h"
#include "memory.h"
#include <string.h>
#include <stdio.h>

struct nativefs {
	size_t sz;
	char root[1];
};


#ifdef __WIN32__

#include <windows.h>
#include "utf8.h"

#define PATH_MAX 4096

static char *
realpath(const char * file_name, char * resolved_name)
{
	w_char name[PATH_MAX];
	size_t s = utf8ToWideString(file_name, name,PATH_MAX);
	size_t i;
	for (i=0;i<s;i++) {
		if (name[i] == '/') {
			name[i] = '\\';
		}
	}

	w_char tmp[PATH_MAX];

	DWORD r = GetFullPathNameW(name,PATH_MAX,tmp,NULL);
	if (r==0 || r==PATH_MAX) {
		return NULL;
	}
	tmp[r]='\0';

	s = utf8FromWideString(tmp, resolved_name, PATH_MAX);

	for (i=0;i<s;i++) {
		if (resolved_name[i]=='\\') {
			resolved_name[i]='/';
		}
	}
	if (s>0 && resolved_name[s-1]=='/') {
		resolved_name[s-1] = '\0';
	}

	return resolved_name;
}

static atom
nextfile(WIN32_FIND_DATAW *data, HANDLE handle)
{
	const w_char *name = data->cFileName;
	while (name[0]=='.' && (name[1]=='\0' || (name[1]=='.' && name[2]=='\0'))) {
		if (FindNextFileW(handle,data)==0) {
			return NULL;
		}
		name = data->cFileName;
	}
	char str[PATH_MAX];
	size_t sz = utf8FromWideString(name, str, PATH_MAX);
	return atomBuild(str,sz);
}

static size_t
_name(const char *root,const char *name, w_char *buf)
{
	size_t s = utf8ToWideString(root, buf, PATH_MAX);
	size_t s2 = utf8ToWideString(name, buf+s, PATH_MAX-s);
	return s+s2;
}

int 
nativefsList(void *_fs, const char *name, atom *buffer, int sz)
{
	struct nativefs *fs = _fs;
	WIN32_FIND_DATAW data;
	w_char buf[PATH_MAX];
	size_t s = _name(fs->root,name, buf);
	if (s + 2 >= PATH_MAX) {
		return -1;
	}
	buf[s] = '\\';
	buf[s+1] = '*';
	buf[s+2] = '\0';

	HANDLE handle = FindFirstFileW(buf,&data);
	if (handle == INVALID_HANDLE_VALUE) {
		return -1;
	}

	int n = 0;

	for (;;) {
		atom name = nextfile(&data, handle);
		if (n<sz) {
			buffer[n] = name;
		}
		++n;
		if (name == NULL)
			break;
		if (FindNextFileW(handle,&data)==0) {
			break;
		}
	}
	FindClose(handle);

	return n;
}

size_t 
nativefsSize(void *_fs , const char *name)
{
	struct nativefs *fs = _fs;
	w_char buf[PATH_MAX];
	size_t s = _name(fs->root,name, buf);
	if (s + 1 >= PATH_MAX) {
		return 0;
	}
	HANDLE fd=CreateFileW(
		buf,
		GENERIC_READ,
		FILE_SHARE_READ,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if ( fd == INVALID_HANDLE_VALUE)
		return 0;
	LARGE_INTEGER size;
	size.u.LowPart = GetFileSize(fd,&size.u.HighPart);
	if (size.u.LowPart == INVALID_FILE_SIZE) {
		if (GetLastError() != NO_ERROR) {
			CloseHandle(fd);
			return 0;
		}
	}


	return (size_t)size.QuadPart;
}

int 
nativefsRead(void *_fs , const char *name, size_t pos, void *buffer, int sz)
{
	struct nativefs *fs = _fs;
	w_char buf[PATH_MAX];
	size_t s = _name(fs->root,name, buf);
	if (s + 1 >= PATH_MAX) {
		return -1;
	}
	HANDLE fd=CreateFileW(
		buf,
		GENERIC_READ,
		FILE_SHARE_READ,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		0);
	if ( fd == INVALID_HANDLE_VALUE)
		return 0;
	LARGE_INTEGER move;
	move.QuadPart = pos;
	if (SetFilePointerEx(fd, move, NULL, FILE_BEGIN)) {
		DWORD ret = 0;
		ReadFile(fd, buffer, sz, &ret, 0) ; 
		CloseHandle(fd);
		return (int)ret;				
	} 
	CloseHandle(fd);
	return -1;
}

int 
nativefsCreate(void *_fs,const char *name, char mode)
{
	struct nativefs *fs = _fs;
	w_char buf[PATH_MAX];
	size_t s = _name(fs->root,name, buf);
	if (s + 1 >= PATH_MAX) {
		return 0;
	}
	switch (mode) {
	case 'q': {
		HANDLE fd=CreateFileW(
			buf,
			GENERIC_READ,
			FILE_SHARE_READ,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0);
		if ( fd == INVALID_HANDLE_VALUE)
			return 0;
		CloseHandle(fd);
		}
		return 1;
	}
	return 0;
}

#else

#endif

void* 
nativefsCreateFS(void *arg)
{
	if (arg == NULL) {
		arg = ".";
	}
	char tmp[PATH_MAX];
	realpath(arg,tmp);
	size_t s = strlen(tmp);
	struct nativefs *fs = memoryPermanent(sizeof(struct nativefs) + s);
	fs->sz = s;
	strcpy(fs->root,tmp);

	return fs;
}

