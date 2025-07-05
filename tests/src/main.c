#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


static char const *svg_skip_separators(char const *s);
static char const *parse_double(char const *s, double *out);
static int parse_exponent(char const **p);
static int parse_number(char const **p);
static int parse_decimal(char const **p);
static int parse_unsigned_integer(char const **p);
static void parse_sign(char const **p);
static int match_char(char const **p, char c);


char const *svg_double_n_parse(char const *input, int expected_count, double *out) {
    char const *p = input;
    double values[expected_count];
    int count = 0;

    while (count < expected_count) {
        p = svg_skip_separators(p);

        char const *next = parse_double(p, values + count);
        if (next == p) {
            return input;
        }

        count++;
        p = next;

        if (count < expected_count) {
            if (!isspace(*p) && *p != ',' && *p != '+' && *p != '-' &&
                !isdigit(*p) && *p != '.') {
                return input;
            }
        }
    }

    for (int i = 0; i < expected_count; ++i) {
        out[i] = values[i];
    }

    return p;
}


static int nearly_equal(double a, double b) {
    return fabs(a - b) < 1e-9;
}


static void test_svg_parse(
    char const *input,
    int expected_n,
    const double *expected_vals,
    char const *expected_rest
) {
    double out[10] = {0};
    char const *rest = svg_double_n_parse(input, expected_n, out);

    if (rest == input) {
        printf("FAIL: \"%s\" → failed to parse, expected %d values\n", input, expected_n);
        assert(0);
    }

    for (int i = 0; i < expected_n; ++i) {
        if (fabs(out[i] - expected_vals[i]) > 1e-10) {
            printf("FAIL: \"%s\" → mismatch at index %d: got %.10g, expected %.10g\n",
                   input, i, out[i], expected_vals[i]);
            assert(0);
        }
    }

    if (expected_rest && strcmp(rest, expected_rest) != 0) {
        printf("FAIL: \"%s\" → remaining \"%s\", expected \"%s\"\n",
               input, rest, expected_rest);
        assert(0);
    }

    printf("PASS: \"%s\"\n", input);
}


static void test_svg_fail(char const *input, int expected_n) {
    double out[10] = {0};
    char const *rest = svg_double_n_parse(input, expected_n, out);
    if (rest != input) {
        printf("FAIL: \"%s\" → expected failure but succeeded\n", input);
        assert(0);
    } else {
        printf("PASS (fail as expected): \"%s\"\n", input);
    }
}


static void test_parse_double(char const *s, double expected) {
    double value;
    char const *end = parse_double(s, &value);
    if (end == s) {
        printf("FAIL to parse \"%s\": No parsing\n", s);
        assert(0);
    } else if (!nearly_equal(value, expected)) {
        printf("FAIL to parse \"%s\": Actual %f != Expected %f\n", s, value, expected);
        assert(0);
    } else {
        printf("PASS: \"%.*s\" -> %f, rest: \"%s\"\n", (int)(end - s), s, value, end);
    }
}


static void test_parse_double_fail(char const *s) {
    double value;
    char const *end = parse_double(s, &value);
    if (end != s) {
        printf("FAIL: Expected \"%s\" to fail but got \"%f\" with remaining text \"%s\".\n", s, value, end);
        assert(0);
    }
    printf("PASS: Parse failed \"%s\"\n", s);
}


int main() {
    test_parse_double("3.14e+10foo", 3.14e+10);
    test_parse_double("-.5E-2more", -0.5e-2);
    test_parse_double("+2.", 2.0);
    test_parse_double(".1abc", 0.1);
    test_parse_double("42", 42.0);
    test_parse_double("0.0e-10xxx", 0.0);
    test_parse_double("3.4.5", 3.4);
    test_parse_double("1e+", 1);
    test_parse_double_fail(".e2");

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

    // Extra trailing separator
    test_svg_parse("1,2,3,", 3, (double[]){1, 2, 3}, ",");

    // === FAILING TESTS ===
    test_svg_fail("", 1);                      // Empty input
    test_svg_fail("abc", 1);                   // Not a number
    test_svg_fail("1e+ 2", 2);                 // Broken exponent
    test_svg_fail(".e2", 1);                   // Invalid decimal
    test_svg_fail("3.14,,2.71", 3);            // Double comma
    test_svg_fail("1.2+ +2.3", 2);             // Invalid extra `+`

    // Basic values
    test_svg_parse("1.0 2.0",2, (double[]){1.0, 2.0},"");
    test_svg_parse("10,-20.5,30",3, (double[]){10.0, -20.5, 30.0},"");
    test_svg_parse(" -3.5 , +7.8 ",2, (double[]){-3.5, 7.8}," ");

    // Scientific notation
    test_svg_parse("1e3 -2e-3 3.5e+2",3, (double[]){1000.0, -0.002, 350.0},"");
    test_svg_parse("1E3,2E-2",2, (double[]){1000.0, 0.02},"");

    // No separators
    test_svg_parse("7-3",2, (double[]){7.0, -3.0},"");
    test_svg_parse("1.5e2-2.5e1",2, (double[]){150.0, -25.0},"");

    // Mixed separators
    test_svg_parse(" 3.5 ,4.5e1 6.0", 3, (double[]){3.5, 45.0, 6.0}, "");

    // Concatenation
    test_svg_parse(" 3.4,.4,1.3e-2THIS IS ADDITIONAL CONTENT",3, (double[]){3.4, 0.4, 1.3e-2},"THIS IS ADDITIONAL CONTENT");

    // Edge cases: malformed but partly parseable
    test_svg_parse("1e", 1, (double[]){1.0}, "e");
    test_svg_fail("--2.5", 1);
    test_svg_fail("+", 1);
    test_svg_fail("", 1);
    test_svg_fail("not_a_number", 1);

    return 0;
}

char const *svg_skip_separators(char const *s) {
    while (*s == ',' || isspace(*s)) s++;
    return s;
}


char const *parse_double(char const *start, double *out) {
    char const *p = start;

    parse_sign(&p);

    int valid_number = parse_decimal(&p) || parse_unsigned_integer(&p);

    if (!valid_number) {
        return start;
    }

    parse_exponent(&p);

    size_t len = p - start;
    if (len >= 64) {
        return start;
    }

    char buf[64];
    for (size_t i = 0; i < len; ++i) {
        buf[i] = start[i];
    }
    buf[len] = '\0';

    char *endptr;
    *out = strtod(buf, &endptr);
    if (*endptr != '\0') {
        return start;
    }

    return p;
}


int parse_exponent(char const **p) {
    char const *q = *p;
    if (match_char(&q, 'e') || match_char(&q, 'E')) {
        parse_sign(&q);
        int digits = parse_unsigned_integer(&q);
        if (digits > 0) {
            *p = q;
            return 1;
        }
    }
    return 0;
}


int parse_number(char const **p) {
    char const *q = *p;
    parse_sign(&q);

    int digits = parse_unsigned_integer(&q);
    if (digits > 0) {
        *p = q;
        return 1;
    }

    if (parse_decimal(&q)) {
        *p = q;
        return 1;
    }

    return 0;
}


int parse_decimal(char const **p) {
    char const *start = *p;

    char const *q = *p;
    int left = parse_unsigned_integer(&q);
    if (match_char(&q, '.')) {
        int right = parse_unsigned_integer(&q);
        if (left > 0 || right > 0) {
            *p = q;
            return 1;
        }
    }

    q = *p;
    if (match_char(&q, '.')) {
        int right = parse_unsigned_integer(&q);
        if (right > 0) {
            *p = q;
            return 1;
        }
    }

    return 0;
}


int parse_unsigned_integer(char const **p) {
    char const *start = *p;
    while (isdigit(**p)) {
        (*p)++;
    }
    return *p - start;
}


void parse_sign(char const **p) {
    match_char(p, '+') || match_char(p, '-');
}


int match_char(char const **p, char c) {
    if (**p == c) {
        (*p)++;
        return 1;
    }
    return 0;
}
