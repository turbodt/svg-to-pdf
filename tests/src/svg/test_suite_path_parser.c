#include "./shared.h"
#include <math.h>
#include <custom-svg.h>


static int test_001(void) {
    SVGPath * path = svg_path_make_from_string("");

    ASSERT_NOT_NULL(path);
    svg_path_destroy(path);
    return 0;
}


static int test_002(void) {
    SVGPath * path = svg_path_make_from_string("M 0 0");
    ASSERT_NOT_NULL(path);
    ASSERT_EQ(svg_path_command_count(path), 0);
    svg_path_destroy(path);
    return 0;
}


static int test_003(void) {
    SVGPath * path = svg_path_make_from_string("M 0 0 L 1 1");
    ASSERT_NOT_NULL(path);
    ASSERT_EQ(svg_path_command_count(path), 1);

    SVGPathCommand const *command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command);

    Point2D start = {0,0};
    Point2D end = {1,1};
    ASSERT_PATH_LINE_IS(*command, start, end);

    svg_path_destroy(path);
    return 0;
}


static int test_004(void) {
    Point2D start, end;
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 L 1 1 1 0 Z");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 3, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    start = (Point2D){0,0};
    end = (Point2D){1,1};
    ASSERT_PATH_LINE_IS(*command, start, end, "Unexpected first command");

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    start = (Point2D){1,1};
    end = (Point2D){1,0};
    ASSERT_PATH_LINE_IS(*command, start, end, "Unexpected second command");

    command = svg_path_get_command(path, 2);
    ASSERT_NOT_NULL(command, "Expected third command to be not NULL");
    start = (Point2D){1,0};
    end = (Point2D){0,0};
    ASSERT_PATH_LINE_IS(*command, start, end, "Unexpected third command");

    svg_path_destroy(path);
    return 0;
}


static int test_009(void) {
    SVGPath * path = svg_path_make_from_string(
        "M -1.03,-1.03 Q -1.03,-1.03 -0.08,-2.62 0.86,-4.20 2.16,-5.19 3.47,"
        "-6.18 4.97,-6.90 6.48,-7.63 7.95,-8.20 9.42,-8.77 11.06,-9.26 12.70,"
        "-9.76 14.36,-10.22 16.02,-10.67 17.50,-11.02 18.99,-11.37 20.53,"
        "-11.63 22.07,-11.89 23.52,-12.01 24.97,-12.13 26.23,-12.19 27.50,"
        "-12.25 28.61,-12.04 29.73,-11.83 30.75,-11.04 31.77,-10.25 32.20,"
        "-8.86 32.63,-7.47 32.18,-6.00 31.73,-4.54 30.61,-3.29 29.50,-2.04"
        " 28.38,-0.99 27.26,0.05 25.79,1.20 24.33,2.35 22.71,3.47 21.10,"
        "4.59 19.37,5.59 17.64,6.60 16.01,7.31 14.37,8.03 12.77,8.54 11.17,"
        "9.06 9.61,9.29 8.06,9.53 6.99,9.48 5.92,9.43 3.62,6.52 1.31,"
        "3.62 1.20,3.44 1.10,3.26 1.05,3.05 1.01,2.84 1.03,2.63 1.06,"
        "2.42 1.15,2.23 1.24,2.03 1.39,1.88 1.53,1.73 1.72,1.63 1.91,"
        "1.53 2.12,1.50 2.33,1.47 2.54,1.50 2.75,1.54 2.94,1.64 3.12,"
        "1.74 3.27,1.90 3.41,2.05 3.50,2.25 3.59,2.44 3.61,2.65 3.63,"
        "2.86 3.58,3.07 3.54,3.28 3.42,3.46 3.31,3.64 3.15,3.77 2.99,"
        "3.91 2.79,3.99 2.59,4.06 2.38,4.07 2.17,4.08 1.96,4.03 1.76,3.97 "
        "1.58,3.85 1.41,3.73 1.28,3.56 1.15,3.38 1.09,3.18 1.02,2.98 1.02,"
        "2.77 1.03,2.56 1.10,2.36 1.16,2.15 1.29,1.99 1.42,1.82 1.60,1.70 "
        "1.78,1.58 1.98,1.53 2.19,1.47 2.40,1.49 2.61,1.50 2.81,1.58 3.01,"
        "1.66 3.17,1.80 3.33,1.94 3.33,1.94 3.33,1.94 4.71,3.70 6.10,5.47 "
        "6.61,4.28 7.11,3.10 8.17,2.95 9.22,2.80 10.50,2.42 11.78,2.03 13.09,"
        "1.47 14.40,0.92 15.92,0.06 17.44,-0.78 18.92,-1.77 20.40,-2.76 "
        "21.69,-3.72 22.99,-4.69 24.09,-5.65 25.20,-6.60 26.42,-6.86 27.63,"
        "-7.11 26.49,-7.09 25.35,-7.07 24.11,-6.99 22.87,-6.91 21.50,-6.70 "
        "20.13,-6.49 18.72,-6.16 17.32,-5.84 15.72,-5.42 14.12,-5.00 12.64,"
        "-4.57 11.16,-4.13 9.83,-3.65 8.50,-3.17 7.39,-2.70 6.27,-2.23 5.23,"
        "-1.54 4.19,-0.85 2.61,0.08 1.03,1.03 0.89,1.14 0.76,1.25 0.59,1.33 "
        "0.43,1.40 0.26,1.43 0.08,1.46 -0.08,1.45 -0.26,1.44 -0.43,1.39 "
        "-0.60,1.34 -0.75,1.24 -0.90,1.15 -1.03,1.03 -1.15,0.90 -1.24,0.75 "
        "-1.34,0.60 -1.39,0.43 -1.44,0.26 -1.45,0.08 -1.46,-0.08 -1.43,-0.26 "
        "-1.40,-0.43 -1.33,-0.59 -1.25,-0.76 -1.14,-0.89 -1.03,-1.03 -1.03,"
        "-1.03 "
        "L -1.03,-1.03"
        "Z"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");

    svg_path_destroy(path);
    return 0;
}


static Test tests[] = {
    {.name="Basic parsing 01", .call=&test_001},
    {.name="Basic parsing 02", .call=&test_002},
    {.name="Basic parsing 03", .call=&test_003},
    {.name="Basic parsing 04", .call=&test_004},
    {.name="Real case", .call=&test_009},
    {.name="", .call=NULL},
};


static TestSuite test_suite = (TestSuite) {
    .name = "Path parsing",
    .tests = tests,
};


TestSuite const * test_suite_svg_get_2(void) {
    return &test_suite;
};
