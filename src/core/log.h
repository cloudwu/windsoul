#ifndef windsoul_log_h
#define windsoul_log_h

void logPrintf(const char *format, ... );
void logFatal(const char *format, ... );
void logInject(void (*puts)(const char *string));

#endif
