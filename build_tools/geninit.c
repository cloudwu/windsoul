#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

static char * exportName =  "";
static bool exportInit = false;

struct importModule {
	const char * name;
	struct importModule *next;
};

struct importAll {
	char **names;
	int count;
};

static bool
exist(const char * name, struct importAll *all)
{
	int i;
	for (i=0;i<all->count;i++) {
		if (strcmp(name, all->names[i])==0) {
			all->names[i][0]='\0';
			return true;
		}
	}
	return false;
}

static void
export(const char * name,struct importAll *all)
{
	char module[1024];
	int i;
	if (name[0] != '_') {
		goto invalid_name;
	}
	for (i=1;name[i];i++) {
		char c = name[i];
		if (c >= 'A' && c <='Z')
			break;
		if (!((c >= 'a' && c <='z') || (c >='0' && c<='9')))
			goto invalid_name;
	}
	if (name[i] == '\0')
		goto invalid_name;
	if (strcmp(name+i,"Init")==0) {
		exportInit = true;
	}
	memcpy(module, name+1, i-1);
	module[i-1] = '\0';
	if (exportName[0] == '\0') {
		if (!exist(module,all)) {
			goto invalid_name;
		}
		exportName = strdup(module);
	} else if (strcmp(module, exportName) !=0 ) {
		goto invalid_name;
	}
	return;
invalid_name:
	fprintf(stderr, "[%s] Invalid name : %s\n",exportName, name);
	exit(1);
}

static void
import(const char *name, struct importAll *all, struct importModule **head)
{
	char module[1024];
	int i;
	if (name[0] != '_') {
		return;
	}
	for (i=1;name[i];i++) {
		char c = name[i];
		if (c >= 'A' && c <='Z')
			break;
		if (!((c >= 'a' && c <='z') || (c >='0' && c<='9')))
			return;
	}
	if (name[i] == '\0')
		return;
	memcpy(module, name+1, i-1);
	module[i-1] = '\0';
	if (exist(module,all)) {
		struct importModule * p = malloc(sizeof(struct importModule));
		p->next = *head;
		p->name = strdup(module);
		*head = p;
	}
}

static void
parser(const char *buf, struct importAll *all, struct importModule **module)
{
	char name[1024];
	int index = 0;
	int sec = 0;
	int fl = 0;
	int ty = 0;
	int scl = 0;
	int nx = 1;
	int address = 0;
	sscanf(buf,"[%d](sec%d)(fl %x)(ty %d)(scl %d) (nx %d) %x %s",
		&index,&sec,&fl,&ty,&scl,&nx,&address,name);
	if (sec==1) {
		if (scl == 2) {
			export(name, all);
		}
	} else if (sec==0) {
		import(name, all, module);
	}
}

#define CR "\n"

static void
header(struct importModule *head, FILE *f)
{
	while(head) {
		fprintf(f, "int _%sInit(void);" CR, head->name);
		head = head->next;
	}
}

static void
output(const char * filename, struct importModule *head)
{
	if (exportName[0] == '\0') {
		fprintf(stderr, "No exported API\n");
		exit(1);
	}
	FILE *f = fopen(filename,"wb");
	if (f==NULL) {
		fprintf(stderr, "Can't write to %s\n",filename);
		exit(1);
	}
	fputs("#include <stdio.h>" CR , f);

	header(head, f);

	fprintf(f, CR "int _%sInit() {" CR , exportName);
	fputs(
		"	static int init = 0;" CR
		"	if (init == 1 ) goto fail;" CR
		"	if (init <0 ) return 0;" CR
		"	init = 1;" CR
		, f);

	while(head) {
		fprintf(f, "	if (_%sInit()) goto fail;" CR, head->name);
		head = head->next;
	}
	if (exportInit) {
		fprintf(f, "	if (%sInit()) goto fail;" CR, exportName);
	}
	fputs(
		"	init = -1;" CR
		"	return 0;" CR
		"fail:" CR
		, f);
	fprintf(f,"	fprintf(stderr, \"[%s] init failed\\n\");" CR, exportName);
	fputs(
		"	return 1;" CR
		"}" CR
		, f);

	fclose(f);
}

int 
main(int argc, char *argv[])
{
	FILE *f;
	char buf[1024];
	int i;
	struct importAll all;
	struct importModule * head = NULL;

	if (argc == 1) {
		fprintf(stderr, "%s output\n",argv[0]);
		return 1;
	}

	all.names = (char **)malloc(sizeof(char *)*(argc-2));
	all.count = argc - 2;

	for (i=2;i<argc;i++) {
		all.names[i-2] = strdup(argv[i]);
	}
	while (!feof(stdin)) {
		char * ret = fgets(buf, 1024, stdin);
		if (ret == NULL) {
			break;
		}
		if (buf[0]!='[')
			continue;
		parser(buf, &all, &head);
	}

	output(argv[1], head);
	
	return 0;
}
