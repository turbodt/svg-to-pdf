#ifndef TESTS_SHARED_CONFIG_H
#define TESTS_SHARED_CONFIG_H


#ifndef TEST_PROTECTED
#define TEST_PROTECTED __attribute__((visibility("hidden")))
#endif


#ifndef TEST_DIR
#define TEST_DIR "./tests"
#endif


extern char const * test_fail_reason;
typedef int(*TestFn)(void);
typedef struct Test {
    char name[128];
    TestFn call;
} Test;
typedef struct TestSuite {
    char name[128];
    Test * tests;
} TestSuite;


#endif
