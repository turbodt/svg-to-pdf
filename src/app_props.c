#include "./app_props.h"
#include <stdlib.h>
#include <string.h>


int app_parse_props(int argc, char **argv, AppProps *props) {
    if (argc < 3) {
        return 1;
    }
    props->input.filename = argv[1];
    props->output.dirname = argv[2];

    for (int i = 3; i < argc; i++) {
        char *end;
        char const*arg = argv[i];
        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--height") == 0) {
            i++;
            if (i >= argc) {
                return 1;
            }
            arg = argv[i];
            props->page.size.height = strtod(arg, &end);
            if (arg == end) {
                return 1;
            }
        } else if (strcmp(arg, "-w") == 0 || strcmp(arg, "--width") == 0) {
            i++;
            if (i >= argc) {
                return 1;
            }
            arg = argv[i];
            props->page.size.width = strtod(arg, &end);
            if (arg == end) {
                return 1;
            }
        } else if (strcmp(arg, "--exclude-containers") == 0) {
            props->page.include_containers = 0;
        } else {
            return 1;
        }
    }
    return 0;
};


void app_print_usage(FILE *out, char const *command) {
    fprintf(out, "Usage: %s file.svg out_dirname\n", command);
    fprintf(out, "\nArgumens:\n");
    fprintf(out, "\n\t-h, --height %%d\n");
    fprintf(out, "\n\t-w, --width %%d\n");
    fprintf(out, "\n\t--exclude-containers\n");
};
