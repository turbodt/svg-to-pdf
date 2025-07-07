#include "./double_parser.h"


static char const *skip_separators(char const *s);
static int parse_exponent(char const **p);
static int parse_number(char const **p);
static int parse_decimal(char const **p);
static int parse_unsigned_integer(char const **p);
static void parse_sign(char const **p);
static int match_char(char const **p, char c);


char const *svg_double_parse(char const *start, double *out) {
    char const *p = start;

    parse_sign(&p);

    int valid_number = parse_decimal(&p) || parse_unsigned_integer(&p);

    if (!valid_number) {
        return start;
    }

    parse_exponent(&p);

    unsigned int len = p - start;
    if (len >= 64) {
        return start;
    }

    char buf[64];
    for (unsigned int i = 0; i < len; ++i) {
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



char const *svg_double_n_parse(
    char const *input,
    unsigned int count,
    double *out
) {
    char const *p = input;
    double values[count];

    for (unsigned int i = 0; i < count; i++) {
        p = skip_separators(p);

        char const *next = svg_double_parse(p, values + i);
        if (next == p) {
            return input;
        }
        p = next;

        if (i + 1 < count) {
            if (!isspace(*p) && *p != ',' && *p != '+' && *p != '-' &&
                !isdigit(*p) && *p != '.') {
                return input;
            }
        }
    }

    for (int i = 0; i < count; ++i) {
        out[i] = values[i];
    }

    return p;
}


char const *skip_separators(char const *s) {
    while (*s == ',' || isspace(*s)) s++;
    return s;
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
