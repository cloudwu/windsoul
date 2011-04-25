#include <stdio.h>
#include "map.h"
#include "atom.h"

int _mapInit(void);
int _atomInit(void);

static void
insert(struct map *m, const char *key, const char *value)
{
	struct map_op op;
	op.op = MAP_INSERT;
	op.key.p = atomString(key);
	op.value = atomString(value);
	mapSearch(m,&op);
}

static void
test()
{
	struct map *m = mapCreate();
	insert(m,"1st","Hello");
	insert(m,"2nd","World");

	struct map_op op;
	op.op = MAP_TRAVERSE;
	op.value = NULL;

	const char *v =NULL;
	while ((v=mapSearch(m,&op))!=NULL) {
		printf("%s : %s\n",(const char *)(op.key.p),v);
	}

	mapRelease(m);
}

int
main()
{
	if (_mapInit()) return 1;
	if (_atomInit()) return 1;

	test();

	return 0;
}
