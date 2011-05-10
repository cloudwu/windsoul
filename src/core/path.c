#include "path.h"
#include "atom.h"
#include "map.h"
#include "log.h"

#include <string.h>
#include <stdbool.h>

#define MAX_SIZE 4096

struct map *g_fs = NULL;

int
pathInit(void)
{
	g_fs =mapCreate();
	return 0;
}

int 
pathList(atom *name, int n)
{
	struct map_op op;
	op.op = MAP_TRAVERSE;
	op.value = NULL;

	const char *v =NULL;

	int ret = 0;

	while ((v=mapSearch(g_fs,&op))!=NULL) {
		if (ret < n) {
			name[ret] = op.key.p;
		}
		++ret;
	}
	return ret;	
}

void 
pathMount(const char * root, struct filesystem *fs)
{
	struct map_op op;
	op.op = MAP_INSERT;
	op.key.p = atomString(root);
	op.value = fs;
	void * old = mapSearch(g_fs,&op);
	if (old!=NULL) {
		logFatal("%s is already mounted.",atomToString(op.key.p));
	}
}

struct filesystem* 
pathGet(atom name, const char **path)
{
	const char * str = atomToString(name);
	if (str[0]=='/')
		++str;
	size_t len = 0;
	atom key = atomBuildSep(str,"/",&len);
	struct map_op op;
	op.op = MAP_SEARCH;
	op.key.p = key;
	struct filesystem * ret = mapSearch(g_fs,&op);
	*path = str + len;
	if (**path == '/') {
		++ *path;
	}
	return ret;
}

#define STRCMP(str1,str2) (memcmp("" str1, (str2), sizeof(str1)-1) == 0)

static char *
realname(char *path)
{
	char *p[MAX_SIZE/2];
	int depth = 0;
	char *parser = path;
	p[depth] = parser;
	if (parser[0]=='/') {
		++parser;
		++path;
	}
	while (*parser) {
		*path = *parser;
		if (*parser == '/') {
			if (STRCMP("../",p[depth])) {
				if (depth==0)
					return NULL;
				--depth;
				path = p[depth];
			}
			else if (STRCMP("./",p[depth])) {
				path = p[depth];
			} else {
				++depth;
				if (depth >= MAX_SIZE/2)
					return NULL;
				++path;
				p[depth] = path;
			}
		} else {
			++path;
		}
		++parser;
	}
	*path = '\0';
	if (p[0][0]=='\0') {
		p[0][0]='/';
		p[0][1]='\0';
	}
	return p[0];
}

static void
fixpath(char *path)
{
	size_t len = strlen(path);
	if (len<1)
		return;
	if (path[len-1]=='/')
		return;
	if (len == 1) {
		if (path[0]=='.') {
			path[1]='/';
			path[2]='\0';
		}
		return;
	}
	if (len == 2) {
		if (path[0]=='.' && path[1]=='.') {
			path[2]='/';
			path[3]='\0';
		}
		return;
	}
	if ((path[len-1]=='.' && path[len-2]=='/') ||
		(path[len-1]=='.' && path[len-2]=='.' && path[len-3]=='/')) {
		path[len]='/';
		path[len+1]='\0';
	}
}

atom
pathLocate(atom name, const char *relative)
{
	size_t len = atomLength(name);
	if (len > MAX_SIZE-3)
		return NULL;
	const char *path = atomToString(name);
	if (path[0]=='/') {
		++path;
		--len;
	}
	char realpath[MAX_SIZE+1];
	realpath[MAX_SIZE-1]='\0';
	if (relative[0]=='/' || len==0) {
		strncpy(realpath,relative,MAX_SIZE-1);
	} else {
		strcpy(realpath,path);
		if (path[len-1]!='/') {
			realpath[len]='/';
			++len;
		}
		strncpy(realpath+len,relative,MAX_SIZE-1-len);
	}
	fixpath(realpath);
	char * ret = realname(realpath);
	if (ret == NULL)
		return NULL;
	return atomString(ret);
}
