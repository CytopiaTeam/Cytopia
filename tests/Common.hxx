#ifndef TEST_HEADERS_HXX_
#define TEST_HEADERS_HXX_

extern void finish_test();
extern void start_test();

#define START_TEST start_test();
#define FINISH_TEST finish_test();

#endif // TEST_HEADERS_HXX_