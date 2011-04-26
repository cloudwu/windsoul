#ifndef windsoul_test_module_h
#define windsoul_test_module_h

static void test(void);

int _setInit(void);
int _atomInit(void);
int _mapInit(void);
int _logInit(void);

int
main()
{
	if (_setInit()) return 1;
	if (_atomInit()) return 1;
	if (_mapInit()) return 1;
	if (_logInit()) return 1;

	test();

	return 0;
}


#endif
