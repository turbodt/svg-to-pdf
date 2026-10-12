#include "./shared.h"
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


static int parse_color(char const *value, PdfColor *color);


void apply_style(RenderState *state, char const *name, char const *value) {
    while (isspace((unsigned char)*name)) name++;
    while (isspace((unsigned char)*value)) value++;
    if (strcmp(name, "fill") == 0) {
        parse_color(value, &state->fill);
    } else if (strcmp(name, "stroke") == 0) {
        parse_color(value, &state->stroke);
    } else if (strcmp(name, "stroke-width") == 0) {
        char *end;
        double n = strtod(value, &end);
        if (end != value && n >= 0) {
            state->stroke_width = n;
        }
    } else if (strcmp(name, "stroke-linecap") == 0) {
        state->linecap = strcmp(value, "round") == 0 ? 1 : 0;
    } else if (strcmp(name, "stroke-dasharray") == 0) {
        snprintf(state->dash, sizeof(state->dash), "%s", value);
    }
}


int parse_url_id(char const *value, char *out, size_t out_size) {
    char const *p = strstr(value, "url(#");
    if (!p) return 0;
    p += 5;
    char const *end = strchr(p, ')');
    if (!end || (size_t)(end - p) >= out_size) return 0;
    memcpy(out, p, (size_t)(end - p));
    out[end - p] = '\0';
    return 1;
}


double attr_double(xmlNode *node, char const *name, double fallback) {
    char *s = attr_string(node, name);
    if (!s) {
        return fallback;
    }
    char *end;
    double value = strtod(s, &end);
    xmlFree(s);
    return end == s ? fallback : value;
}


char *attr_string(xmlNode *node, char const *name) {
    return (char *)xmlGetProp(node, (xmlChar const *)name);
}


static int parse_color(char const *value, PdfColor *color) {
    if (strcmp(value, "none") == 0 || strcmp(value, "transparent") == 0) {
        color->enabled = 0;
        return 1;
    }
    if (value[0] != '#') {
        return 0;
    }
    unsigned int r, g, b;
    if (strlen(value) >= 7 && sscanf(value + 1, "%2x%2x%2x", &r, &g, &b) == 3) {
        *color = (PdfColor){1, r/255.0, g/255.0, b/255.0};
        return 1;
    }
    if (strlen(value) >= 4) {
        int rv = isxdigit((unsigned char)value[1]) ? (int)strtol((char[]){value[1],0}, NULL, 16) : -1;
        int gv = isxdigit((unsigned char)value[2]) ? (int)strtol((char[]){value[2],0}, NULL, 16) : -1;
        int bv = isxdigit((unsigned char)value[3]) ? (int)strtol((char[]){value[3],0}, NULL, 16) : -1;
        if (rv >= 0 && gv >= 0 && bv >= 0) {
            *color = (PdfColor){1, rv/15.0, gv/15.0, bv/15.0};
            return 1;
        }
    }
    return 0;
}
