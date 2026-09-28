#include "./app_props.h"
#include <stdlib.h>
#include <string.h>


static int parse_size_arg(char const *arg, double *out);
static int parse_pdf_size(char const *arg, Size2D *out);


int app_parse_props(int argc, char **argv, AppProps *props) {
    unsigned int positional_count = 0;
    for (int i = 1; i < argc; i++) {
        char const*arg = argv[i];
        if (
            strcmp(arg, "-h") == 0
            || strcmp(arg, "--height") == 0
            || strcmp(arg, "--container-height") == 0
        ) {
            i++;
            if (i >= argc) {
                return 1;
            }
            arg = argv[i];
            if (parse_size_arg(arg, &props->page.size.height)) {
                return 1;
            }
        } else if (
            strcmp(arg, "-w") == 0
            || strcmp(arg, "--width") == 0
            || strcmp(arg, "--container-width") == 0
        ) {
            i++;
            if (i >= argc) {
                return 1;
            }
            arg = argv[i];
            if (parse_size_arg(arg, &props->page.size.width)) {
                return 1;
            }
        } else if (strcmp(arg, "--exclude-containers") == 0) {
            props->page.include_containers = 0;
        } else if (strcmp(arg, "--allow-duplicated-containers") == 0) {
            props->page.merge_duplicated_containers = 0;
        } else if (strcmp(arg, "--pdf") == 0) {
            props->output.pdf = 1;
        } else if (strcmp(arg, "--pdf-height") == 0) {
            i++;
            if (i >= argc) {
                return 1;
            }
            arg = argv[i];
            if (parse_size_arg(arg, &props->output.pdf_size.height)) {
                return 1;
            }
            props->output.has_pdf_size = 1;
        } else if (strcmp(arg, "--pdf-width") == 0) {
            i++;
            if (i >= argc) {
                return 1;
            }
            arg = argv[i];
            if (parse_size_arg(arg, &props->output.pdf_size.width)) {
                return 1;
            }
            props->output.has_pdf_size = 1;
        } else if (strcmp(arg, "--pdf-size") == 0) {
            i++;
            if (i >= argc) {
                return 1;
            }
            arg = argv[i];
            if (parse_pdf_size(arg, &props->output.pdf_size)) {
                return 1;
            }
            props->output.has_pdf_size = 1;
        } else if (arg[0] == '-') {
            return 1;
        } else {
            positional_count++;
            if (positional_count == 1) {
                props->input.filename = arg;
            } else if (positional_count == 2) {
                props->output.path = arg;
            } else {
                return 1;
            }
        }
    }
    if (positional_count != 2) {
        return 1;
    }
    if (
        props->output.has_pdf_size
        && (props->output.pdf_size.width <= 0 || props->output.pdf_size.height <= 0)
    ) {
        return 1;
    }
    return 0;
};


void app_print_usage(FILE *out, char const *command) {
    fprintf(out, "Usage: %s [OPTIONS] file.svg output\n", command);
    fprintf(out, "\nIn SVG mode, output is a directory for page-*.svg files.\n");
    fprintf(out, "In PDF mode, output is the final PDF filename.\n");
    fprintf(out, "\nArgumens:\n");
    fprintf(out, "\n\t-h, --height %%d\n");
    fprintf(out, "\n\t-w, --width %%d\n");
    fprintf(out, "\n\t--container-height %%d\n");
    fprintf(out, "\n\t--container-width %%d\n");
    fprintf(out, "\n\t--exclude-containers\n");
    fprintf(out, "\n\t--allow-duplicated-containers\n");
    fprintf(out, "\n\t--pdf\n");
    fprintf(out, "\n\t--pdf-height %%d\n");
    fprintf(out, "\n\t--pdf-width %%d\n");
    fprintf(out, "\n\t--pdf-size a3-portrait|a3-landscape|a4-portrait|a4-landscape|a5-portrait|a5-landscape|letter-portrait|letter-landscape|legal-portrait|legal-landscape\n");
};


int parse_size_arg(char const *arg, double *out) {
    char *end;
    *out = strtod(arg, &end);
    if (arg == end || *out <= 0) {
        return 1;
    }
    return 0;
}


int parse_pdf_size(char const *arg, Size2D *out) {
    struct Preset {
        char const *name;
        Size2D size;
    } presets[] = {
        {"a3-portrait", {.width=841.890, .height=1190.551}},
        {"a3-landscape", {.width=1190.551, .height=841.890}},
        {"a4-portrait", {.width=595.276, .height=841.890}},
        {"a4-landscape", {.width=841.890, .height=595.276}},
        {"a5-portrait", {.width=419.528, .height=595.276}},
        {"a5-landscape", {.width=595.276, .height=419.528}},
        {"letter-portrait", {.width=612, .height=792}},
        {"letter-landscape", {.width=792, .height=612}},
        {"legal-portrait", {.width=612, .height=1008}},
        {"legal-landscape", {.width=1008, .height=612}},
        {NULL, {.width=0, .height=0}},
    };
    for (unsigned int i = 0; presets[i].name; i++) {
        if (strcmp(arg, presets[i].name) == 0) {
            *out = presets[i].size;
            return 0;
        }
    }
    return 1;
}
