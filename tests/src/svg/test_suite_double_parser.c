#include "./shared.h"
#include <math.h>
#include <custom-svg.h>


static int test_svg_parse(
    char const *input,
    int expected_n,
    const double *expected_vals,
    char const *expected_rest
) {
    double out[10] = {0};
    char const *rest = svg_double_n_parse(input, expected_n, out);

    ASSERT_NEQ(rest, input, "Failed to parse");

    for (int i = 0; i < expected_n; ++i) {
        ASSERT(
            fabs(out[i] - expected_vals[i]) > 1e-10,
            "Differnt parsed value"
        );
    }

    if (expected_rest && strcmp(rest, expected_rest) != 0) {
        ASSERT_STR_EQ(rest, expected_rest, "Expected same rest");
    }
    return 0;
}


static int test_svg_fail(char const *input, int expected_n) {
    double out[10] = {0};
    char const *rest = svg_double_n_parse(input, expected_n, out);
    ASSERT_EQ(rest, input, "Expected failure");
    return 0;
}


static int test_parse_double(char const *s, double expected) {
    double value;
    char const *end = svg_double_parse(s, &value);
    ASSERT_NEQ(end, s, "Failed to parse");
    ASSERT(
        fabs(value - expected) > 1e-10,
        "Differnt parsed value"
    );
    return 0;
}


static int test_parse_double_fail(char const *s) {
    double value;
    char const *end = svg_double_parse(s, &value);
    ASSERT_EQ(end, s, "Expected parse to fail");
    return 0;
}


static int test_001(void) {
    test_parse_double("3.14e+10foo", 3.14e+10);
    test_parse_double("-.5E-2more", -0.5e-2);
    test_parse_double("+2.", 2.0);
    test_parse_double(".1abc", 0.1);
    test_parse_double("42", 42.0);
    test_parse_double("0.0e-10xxx", 0.0);
    test_parse_double("3.4.5", 3.4);
    test_parse_double("1e+", 1);
    return 0;
}


static int test_002(void) {
    test_parse_double_fail(".e2");
    return 0;
}


static int test_003(void) {
    test_svg_parse("3.14,2.71,1.0", 3, (double[]){3.14, 2.71, 1.0}, "");
    test_svg_parse("3.4-5.6+7.8", 3, (double[]){3.4, -5.6, 7.8}, "");
    test_svg_parse("  1.0 , 2.0 , 3.0  ", 3, (double[]){1.0, 2.0, 3.0}, "  ");
    test_svg_parse("1e3 2e-2 -3e+1", 3, (double[]){1000.0, 0.02, -30.0}, "");
    test_svg_parse("1.5E10,2.5E-3", 2, (double[]){1.5e10, 0.0025}, "");
    test_svg_parse(".5,1.,2", 3, (double[]){0.5, 1.0, 2.0}, "");
    test_svg_parse("+3.0 -2.0 +1.0", 3, (double[]){3.0, -2.0, 1.0}, "");
    test_svg_parse("3.4.5", 1, (double[]){3.4}, ".5");
    test_svg_parse("1.2 2.3 3.4.5", 3, (double[]){1.2,2.3,3.4},".5");
    test_svg_parse("1.2 2.3 3.4.5", 4, (double[]){1.2,2.3,3.4,0.5},"");
    test_svg_parse("1e+", 1, (double[]){1.0}, "e+");
    test_svg_parse("3.4,.4,1.3e-2A", 3, (double[]){3.4, 0.4, 1.3e-2}, "A");
    test_svg_parse("3.4 .4,1.3e-2A", 3, (double[]){3.4, 0.4, 1.3e-2}, "A");
    test_svg_parse("3.4.4,1.3e-2A", 3, (double[]){3.4, 0.4, 1.3e-2}, "A");
    test_svg_parse("1,2,3,", 3, (double[]){1, 2, 3}, ",");
    test_svg_parse("3.14,2.71,1.0", 3, (double[]){3.14, 2.71, 1.0}, "");
    return 0;
}


static int test_006(void) {
    test_svg_fail("", 1);                      // Empty input
    test_svg_fail("abc", 1);                   // Not a number
    test_svg_fail("1e+ 2", 2);                 // Broken exponent
    test_svg_fail(".e2", 1);                   // Invalid decimal
    test_svg_fail("3.14,,2.71", 3);            // Double comma
    test_svg_fail("1.2+ +2.3", 2);             // Invalid extra `+`
    return 0;
}


static int test_007(void) {
    // Basic values
    test_svg_parse("1.0 2.0",2, (double[]){1.0, 2.0},"");
    test_svg_parse("10,-20.5,30",3, (double[]){10.0, -20.5, 30.0},"");
    test_svg_parse(" -3.5 , +7.8 ",2, (double[]){-3.5, 7.8}," ");
    return 0;
}


static int test_008(void) {
    // Scientific notation
    test_svg_parse("1e3 -2e-3 3.5e+2",3, (double[]){1000.0, -0.002, 350.0},"");
    test_svg_parse("1E3,2E-2",2, (double[]){1000.0, 0.02},"");
    return 0;
}


static int test_009(void) {
    // No separators
    test_svg_parse("7-3",2, (double[]){7.0, -3.0},"");
    test_svg_parse("1.5e2-2.5e1",2, (double[]){150.0, -25.0},"");
    return 0;
}


static int test_010(void) {
    // Mixed separators
    test_svg_parse(" 3.5 ,4.5e1 6.0", 3, (double[]){3.5, 45.0, 6.0}, "");
    return 0;
}


static int test_011(void) {
    // Concatenation
    test_svg_parse(" 3.4,.4,1.3e-2THIS IS ADDITIONAL CONTENT",3, (double[]){3.4, 0.4, 1.3e-2},"THIS IS ADDITIONAL CONTENT");

    return 0;
}


static int test_012(void) {
    // Edge cases: malformed but partly parseable
    test_svg_parse("1e", 1, (double[]){1.0}, "e");
    test_svg_fail("--2.5", 1);
    test_svg_fail("+", 1);
    test_svg_fail("", 1);
    test_svg_fail("not_a_number", 1);

    return 0;
}


static Test tests[] = {
    {.name="Basic parsing", .call=&test_001},
    {.name="Basic failing", .call=&test_002},
    {.name="Multiple parsing", .call=&test_003},
    {.name="Multiple failing", .call=&test_006},
    {.name="Multiple parsing", .call=&test_007},
    {.name="Multiple parsing scientific notation", .call=&test_008},
    {.name="Multiple parsing without separators", .call=&test_009},
    {.name="Multiple parsing mixed separators", .call=&test_010},
    {.name="Multiple parsing with additional content", .call=&test_011},
    {.name="Edge cases", .call=&test_012},
    {.name="", .call=NULL},
};


static TestSuite test_suite = (TestSuite) {
    .name = "Double parsing",
    .tests = tests,
};


TestSuite const * test_suite_svg_get_1(void) {
    return &test_suite;
};
