#ifndef TESTS_SHARED_ASSERTIONS_BASIC_H
#define TESTS_SHARED_ASSERTIONS_BASIC_H


#include "./config.h"
#include <math.h>


#define CALL(fn_call) { int err = fn_call; if (err != 0) return err; }

#define __GET_4TH_ARG(arg1, arg2, arg3, arg4, ...) arg4
#define __ASSERT_1(exp) if (!(exp)) {test_fail_reason = NULL; return 1;}
#define __ASSERT_2(exp, text) if (!(exp)) {test_fail_reason = text; return 1; }
#define __ASSERT_3(exp, text, code) if (!(exp)) {test_fail_reason = text; return code; }
#define __ASSERT_STRING_CHOOSER(...) \
    __GET_4TH_ARG(__VA_ARGS__, __ASSERT_3, __ASSERT_2, __ASSERT_1, )
#define ASSERT(...) __ASSERT_STRING_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define ASSERT_TRUE(exp, ...) ASSERT(exp, ## __VA_ARGS__)
#define ASSERT_FALSE(exp, ...) ASSERT(!(exp), ## __VA_ARGS__)
#define ASSERT_EQ(a, b, ...) ASSERT((a) == (b), ## __VA_ARGS__)
#define ASSERT_NEQ(a, b, ...) ASSERT((a) != (b), ## __VA_ARGS__)
#define ASSERT_NULL(exp, ...) ASSERT_EQ(exp, NULL, ## __VA_ARGS__)
#define ASSERT_NOT_NULL(exp, ...) ASSERT_NEQ(exp, NULL, ## __VA_ARGS__)
#define ASSERT_ZERO(exp, ...) ASSERT_EQ(exp, 0, ## __VA_ARGS__)
#define ASSERT_NOT_ZERO(exp, ...) ASSERT_NEQ(exp, 0, ## __VA_ARGS__)
#define ASSERT_STR_EQ(a, b, ...) ASSERT_ZERO(strcmp(a, b), ## __VA_ARGS__)
#define ASSERT_STR_NEQ(a, b, ...) ASSERT_ZERO(!strcmp(a, b), ## __VA_ARGS__)
#define ASSERT_FLOAT_EQ(a, b, ...) ASSERT(fabs((double)(a)-(double)(b)) < 1e-9, ## __VA_ARGS__)


#endif
