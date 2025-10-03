#ifndef APP_PROPS_H
#define APP_PROPS_H


#include <stdio.h>
#include <geometry.h>


typedef struct {
    struct {
        Size2D size;
        float tol;
        int include_containers : 1;
        int merge_duplicated_containers : 1;
    } page;
    struct {
        char const *filename;
    } input;
    struct {
        char file_template[128];
        char const *dirname;
    } output;
} AppProps;


int app_parse_props(int argc, char **argv, AppProps *props);
void app_print_usage(FILE *,char const *);


#endif
