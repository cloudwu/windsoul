#include "log.h"
#include "memory.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_SIZE 4096

static void (*g_puts)(const char *string) = NULL;

static void
logv(const char *format, va_list ap)
{
	char * buffer = memoryTemp(MAX_SIZE);
	vsnprintf(buffer, MAX_SIZE, format, ap);
	g_puts(buffer);
}

void 
logPrintf(const char *format, ... )
{
	va_list ap;
	va_start(ap,format);
	logv(format, ap);
	va_end(ap);
}

void 
logFatal(const char *format, ... )
{
	va_list ap;
	va_start(ap,format);
	logv(format, ap);
	va_end(ap);
	exit(1);
}

void logInject(void (*_puts)(const char *string))
{
	g_puts = _puts;
}

static void
logerr(const char *string)
{
	fputs(string, stderr);
}

int
logInit(void)
{
	logInject(logerr);
	return 0;
}
