#ifndef windsoul_utf8_h
#define windsoul_utf8_h

#include <stddef.h>

typedef unsigned short w_char;

size_t utf8ToWideString(const char * utf8, w_char * utf16, size_t sz);
size_t utf8FromWideString(const w_char * utf16, char * utf8, size_t sz);

#endif
