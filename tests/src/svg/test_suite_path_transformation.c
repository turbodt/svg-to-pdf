#include "./shared.h"
#include <math.h>
#include <custom-svg.h>


static int test_001(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 L 1 1 M 2 2 L 4 4");
    ASSERT_NOT_NULL(path);

    Transform *t = geo_transform_make();

    svg_path_apply_transform(path, t);

    ASSERT_EQ(svg_path_command_count(path), 2);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command);
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command);
    ASSERT_PATH_IS_LINE(*command, 2,2, 4,4);

    geo_transform_destroy(t);
    svg_path_destroy(path);
    return 0;
}

static int test_002(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 L 1 1 M 2 2 L 4 4");
    ASSERT_NOT_NULL(path);

    Transform *t = geo_transform_make();
    geo_transform_translate(t, (Size2D){10, -10});

    svg_path_apply_transform(path, t);

    ASSERT_EQ(svg_path_command_count(path), 2);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command);
    ASSERT_PATH_IS_LINE(*command, 10, -10, 11, -9);

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command);
    ASSERT_PATH_IS_LINE(*command, 12,-8, 14,-6);

    geo_transform_destroy(t);
    svg_path_destroy(path);
    return 0;
}


static int test_003(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 Q 0 1 1 1");
    ASSERT_NOT_NULL(path);

    Transform *t = geo_transform_make();
    geo_transform_rotate(t, M_PI/2, (Point2D){1, 1});

    svg_path_apply_transform(path, t);

    ASSERT_EQ(svg_path_command_count(path), 1);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command);
    ASSERT_PATH_IS_QBEZIER(*command, 2,0, 1,0, 1,1);

    geo_transform_destroy(t);
    svg_path_destroy(path);
    return 0;
}


static Test tests[] = {
    {.name="Basic translation 001", .call=&test_001},
    {.name="Basic translation 002", .call=&test_002},
    {.name="Basic rotation 003", .call=&test_003},
    {.name="", .call=NULL},
};


static TestSuite test_suite = (TestSuite) {
    .name = "Path transformation",
    .tests = tests,
};


TestSuite const * test_suite_svg_get_3(void) {
    return &test_suite;
};
