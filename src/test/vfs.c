#include "test.h"
#include "atom.h"
#include "log.h"
#include "vfs.h"

static void
list(const char *root)
{
	atom a = atomString(root);
	int n = vfsList(a, NULL, 0);
	atom name[n];
	vfsList(a, name, n);
	int i;
	for (i=0;i<n;i++) {
		logPrintf("%s",atomToString(name[i]));
	}
}

static void
test()
{
	list("/");
	list("/_");
}

