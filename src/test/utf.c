#include "test.h"
#include "utf8.h"

#include <stdio.h>

static void
test(void)
{
	const char * test = "神马";
	size_t s = utf8ToWideString(test,NULL,0);
	w_char ws[s+1];
	printf("%u\n",s);	
	utf8ToWideString(test,ws,s+1);
	size_t i;
	for (i=0;i<s;i++) {
		printf("\\u%x",ws[i]);
	}
	printf("\n");
	s = utf8FromWideString(ws,NULL,0);
	printf("%u\n",s);
	unsigned char tmp[s+1];
	utf8FromWideString(ws,(char*)tmp,s+1);
	for (i=0;i<s;i++) {
		printf("\\x%x",tmp[i]);
	}
	printf("\n");
}
