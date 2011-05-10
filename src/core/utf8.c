#include "utf8.h"

#include <string.h>

size_t 
utf8ToWideString(const char * utf8, w_char * utf16, size_t sz)
{
	while (((unsigned char)*utf8 & 0xc0) == 0x80) {
		++utf8;
	}
	if (sz==0) {
		size_t n=0;
		while (*utf8) {
			signed char c=*utf8;
			if ((c & 0xc0)!=0x80) {
				++n;
			}
			++utf8;
		}
		return n;
	}
	if (*utf8 == '\0') {
		utf16[0] = 0;
		return 0;
	}

	w_char *ptr = utf16-1;
	
	while (*utf8) {
		signed char c=*utf8;
		if ((c & 0xc0)==0x80) {
			*ptr=*ptr<<6 | (c & 0x3f);
		}
		else  {
			++ptr;
			if (ptr - utf16 >= sz) {
				*(ptr-1) = 0;
				return sz-1;
			}
			*ptr=c&(0x0f | (~(c>>1) &0x1f) | ~(c>>7));
		}
		++utf8;
	}
	
	if (ptr - utf16 < sz) {
		++ptr;
	}
	*ptr=0;
	return ptr - utf16;
}

size_t 
utf8FromWideString(const w_char * utf16, char * utf8, size_t sz)
{
	unsigned char *ptr = (unsigned char *)utf8;
	if (sz < 3) {
		while (*utf16) {
			w_char c=*utf16;
			if (c>0x7F) {
				if (c>0x7ff) {
					++ptr;
				}
				++ptr;
			}
			++ptr;
			++utf16;
		}
		return (char *)ptr - utf8;
	}

	sz-=2;

	unsigned char * last;
	
	while (*utf16) {
		w_char c=*utf16;
		last = ptr;
		if (c<=0x7F) {
			*ptr=(unsigned char)c;
		}
		else {
			if (c<=0x7ff) {
				*ptr=(unsigned char)((c>>6) | 0xc0);
			}
			else {
				*ptr=(unsigned char)((c>>12) | 0xe0);
				++ptr;
				*ptr=(unsigned char)(((c>>6) & 0x3f) | 0x80);
			}
			++ptr;
			*ptr=(unsigned char)((c&0x3f) | 0x80);
		}
		++ptr;
		if ((char *)ptr - utf8 >= sz + 2) {
			ptr = last;
			break;
		}
		++utf16;
	}
	*ptr=0;
	return (char *)ptr - utf8;
}
