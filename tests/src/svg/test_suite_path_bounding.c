#include "./shared.h"
#include <math.h>
#include <custom-svg.h>


static int test_001(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 C -1 0 1 2 1 1");
    ASSERT_NOT_NULL(path);
    ASSERT_EQ(svg_path_command_count(path), 1);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command);
    Box2D bbox = svg_path_command_get_bbox(command);
    ASSERT_BOX_COORDS_EQ(bbox, -0.28,0, 1,1.28);

    svg_path_destroy(path);
    return 0;
}


static int test_002(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 C 0 1.5 1 -0.5 1 1");
    ASSERT_NOT_NULL(path);
    ASSERT_EQ(svg_path_command_count(path), 1);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command);
    Box2D bbox = svg_path_command_get_bbox(command);
//LOG_BOX(bbox);LOG("\n");
    ASSERT_BOX_COORDS_EQ(bbox, 0,0, 1,1);

    svg_path_destroy(path);
    return 0;
}


static int test_101(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 C -1 0 1 2 1 1");
    ASSERT_NOT_NULL(path);
    ASSERT_EQ(svg_path_command_count(path), 1);

    Box2D bbox = svg_path_get_bbox(path);
    ASSERT_BOX_COORDS_EQ(bbox, -0.28,0, 1,1.28);

    svg_path_destroy(path);
    return 0;
}


static Test tests[] = {
    {.name="CBezier bounds 001", .call=&test_001},
    {.name="CBezier bounds 002", .call=&test_002},
    {.name="CBezier bounds 101", .call=&test_101},
    {.name="", .call=NULL},
};


static TestSuite test_suite = (TestSuite) {
    .name = "Path bounding box",
    .tests = tests,
};


TestSuite const * test_suite_svg_get_4(void) {
    return &test_suite;
};
