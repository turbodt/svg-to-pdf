#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


static void skip_separators(const char **p) {
    while (**p && (isspace((unsigned char)**p) || **p == ',')) ++(*p);
}

// Checks whether the next character after a number is a valid terminator
static int is_number_terminator(char c) {
    return c == '\0' || isspace((unsigned char)c) || c == ',' || c == '-' || c == '+';
}

// Parses a strictly delimited double
static int parse_strict_double(const char **p, double *out) {
    char *end;
    double val = strtod(*p, &end);

    if (end == *p) return 0; // No progress

    if (!is_number_terminator(*end) && *end != '.') return 0;

    *out = val;
    *p = end;
    return 1;
}

int svg_parse_n_doubles(const char **p, double *out, int count) {
    int i;
    for (i = 0; i < count; ++i) {
        skip_separators(p);
        if (!**p) return 0;

        if (!parse_strict_double(p, &out[i]))
            return 0;
    }

    skip_separators(p);
    return 1;
}

int nearly_equal(double a, double b) {
    return fabs(a - b) < 1e-9;
}

void test_parse(const char *input, const double expected[], int count) {
    const char *p = input;
    double parsed[32] = {0};
    assert(count <= 32);

    int ok = svg_parse_n_doubles(&p, parsed, count);
    if (!ok) {
        printf("FAIL: Could not parse %d values from \"%s\"\n", count, input);
        assert(0);
    }

    for (int i = 0; i < count; ++i) {
        if (!nearly_equal(parsed[i], expected[i])) {
            printf("FAIL: Input=\"%s\" Index=%d Expected=%f Got=%f\n",
                   input, i, expected[i], parsed[i]);
            assert(0);
        }
    }

    printf("PASS: \"%s\"\n", input);
}

void test_fail(const char *input, int count) {
    const char *p = input;
    double dummy[32] = {0};
    int ok = svg_parse_n_doubles(&p, dummy, count);
    if (ok) {
        printf("FAIL: Expected failure but succeeded: \"%s\"\n", input);
        assert(0);
    } else {
        printf("PASS (expected fail): \"%s\"\n", input);
    }
}

int main() {
    // Basic values
    test_parse("1.0 2.0", (double[]){1.0, 2.0}, 2);
    test_parse("10,-20.5,30", (double[]){10.0, -20.5, 30.0}, 3);
    test_parse(" -3.5 , +7.8 ", (double[]){-3.5, 7.8}, 2);

    // Scientific notation
    test_parse("1e3 -2e-3 3.5e+2", (double[]){1000.0, -0.002, 350.0}, 3);
    test_parse("1E3,2E-2", (double[]){1000.0, 0.02}, 2);

    // No separators
    test_parse("7-3", (double[]){7.0, -3.0}, 2);
    test_parse("1.5e2-2.5e1", (double[]){150.0, -25.0}, 2);

    // Mixed separators
    test_parse(" 3.5 ,4.5e1 6.0", (double[]){3.5, 45.0, 6.0}, 3);

    // Edge cases: malformed but partly parseable
    test_fail("3.1.4", 2);      // Second dot is invalid
    test_fail("1e", 1);         // Missing exponent
    test_fail("--2.5", 1);      // Double sign
    test_fail("+", 1);          // Incomplete
    test_fail("", 1);           // Empty
    test_fail("not_a_number", 1);

    return 0;
}
