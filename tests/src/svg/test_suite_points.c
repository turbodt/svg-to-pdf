#include "./shared.h"
#include <custom-svg.h>


static int expect_point(
    SVGPoints const *points,
    unsigned int index,
    double x,
    double y
) {
    ASSERT(index < svg_points_count(points), "Point index out of range");
    Point2D p = svg_points_get(points, index);
    ASSERT_FLOAT_EQ(p.x, x, "Unexpected point x");
    ASSERT_FLOAT_EQ(p.y, y, "Unexpected point y");
    return 0;
}


static int expect_count(char const *input, unsigned int expected) {
    SVGPoints *points = svg_points_make_from_string(input);
    ASSERT_NOT_NULL(points, "Expected points to be parsed");
    ASSERT_EQ(svg_points_count(points), expected, "Unexpected point count");
    svg_points_destroy(points);
    return 0;
}


static int expect_two_points(
    char const *input,
    double x0,
    double y0,
    double x1,
    double y1
) {
    SVGPoints *points = svg_points_make_from_string(input);
    ASSERT_NOT_NULL(points, "Expected points to be parsed");
    ASSERT_EQ(svg_points_count(points), 2, "Unexpected point count");
    CALL(expect_point(points, 0, x0, y0));
    CALL(expect_point(points, 1, x1, y1));
    svg_points_destroy(points);
    return 0;
}


static int expect_fail(char const *input) {
    SVGPoints *points = svg_points_make_from_string(input);
    ASSERT_NULL(points, "Expected points parsing to fail");
    return 0;
}


static int test_001(void) {
    SVGPoints *points = svg_points_make_from_string("0 0 10 10");
    ASSERT_NOT_NULL(points, "Expected points to be parsed");
    ASSERT_EQ(svg_points_count(points), 2, "Unexpected point count");
    CALL(expect_point(points, 0, 0, 0));
    CALL(expect_point(points, 1, 10, 10));
    svg_points_destroy(points);
    return 0;
}


static int test_002(void) {
    CALL(expect_two_points("0,0 10,10", 0, 0, 10, 10));
    CALL(expect_two_points("0\t0\n10 10", 0, 0, 10, 10));
    CALL(expect_two_points("0 0,10 10", 0, 0, 10, 10));
    CALL(expect_two_points("  0 , 0 , 10 , 10  ", 0, 0, 10, 10));
    CALL(expect_two_points("0 0\r\n10 10", 0, 0, 10, 10));
    return 0;
}


static int test_003(void) {
    CALL(expect_count("", 0));
    CALL(expect_count("   ", 0));
    CALL(expect_count(",", 0));
    return 0;
}


static int test_004(void) {
    CALL(expect_two_points("0 0 10 10 20", 0, 0, 10, 10));
    CALL(expect_two_points("0 0 10 10 20 ", 0, 0, 10, 10));
    CALL(expect_count("0 0 10", 1));
    return 0;
}


static int test_005(void) {
    CALL(expect_fail("abc"));
    CALL(expect_fail("0 0 10 10 20 x"));
    CALL(expect_fail("0 x"));
    CALL(expect_fail("--1 2"));
    return 0;
}


static int test_006(void) {
    CALL(expect_two_points("1e1 2e1 -3.5 .5", 10, 20, -3.5, 0.5));
    CALL(expect_two_points("1-2-3-4", 1, -2, -3, -4));
    CALL(expect_two_points("+3 +4 5 6", 3, 4, 5, 6));
    return 0;
}


static int test_007(void) {
    SVGPoints *points = svg_points_make_from_string("10 20 30 40");
    ASSERT_NOT_NULL(points, "Expected points to be parsed");
    ASSERT_BOX_EQ(
        svg_points_get_bbox(points),
        10, 20, 20, 20,
        "Unexpected diagonal bbox"
    );
    svg_points_destroy(points);

    points = svg_points_make_from_string("10 20 30 20");
    ASSERT_NOT_NULL(points, "Expected points to be parsed");
    ASSERT_BOX_EQ(
        svg_points_get_bbox(points),
        10, 20, 20, 0,
        "Unexpected horizontal bbox"
    );
    svg_points_destroy(points);

    points = svg_points_make_from_string("10 20");
    ASSERT_NOT_NULL(points, "Expected points to be parsed");
    ASSERT_BOX_EQ(
        svg_points_get_bbox(points),
        10, 20, 0, 0,
        "Unexpected single point bbox"
    );
    svg_points_destroy(points);

    points = svg_points_make_from_string("");
    ASSERT_NOT_NULL(points, "Expected points to be parsed");
    ASSERT_BOX_EQ(
        svg_points_get_bbox(points),
        0, 0, 0, 0,
        "Unexpected empty bbox"
    );
    svg_points_destroy(points);
    return 0;
}


static int test_008(void) {
    SVGPoints *points = svg_points_make_from_string("0 0 10 10");
    ASSERT_NOT_NULL(points, "Expected points to be parsed");
    Transform *t = geo_transform_make();
    ASSERT_NOT_NULL(t, "Failed to make transform");
    geo_transform_translate(t, (Size2D){5, 5});
    svg_points_apply_transform(points, t);
    CALL(expect_point(points, 0, 5, 5));
    CALL(expect_point(points, 1, 15, 15));
    geo_transform_destroy(t);
    svg_points_destroy(points);
    return 0;
}


static int test_009(void) {
    SVGPoints *points = svg_points_make_from_string("1 2");
    ASSERT_NOT_NULL(points, "Expected points to be parsed");
    Point2D p = svg_points_get(points, 1);
    ASSERT_FLOAT_EQ(p.x, 0, "Expected zero for out of range x");
    ASSERT_FLOAT_EQ(p.y, 0, "Expected zero for out of range y");
    svg_points_destroy(points);
    return 0;
}


static Test tests[] = {
    {.name="Basic parsing", .call=&test_001},
    {.name="Separators", .call=&test_002},
    {.name="Empty input", .call=&test_003},
    {.name="Trailing odd coordinate is ignored", .call=&test_004},
    {.name="Invalid input", .call=&test_005},
    {.name="Number formats", .call=&test_006},
    {.name="Bounding box", .call=&test_007},
    {.name="Apply transform", .call=&test_008},
    {.name="Out of range point", .call=&test_009},
    {.name="", .call=NULL},
};


static TestSuite test_suite = (TestSuite) {
    .name = "Points parsing",
    .tests = tests,
};


TestSuite const * test_suite_svg_get_5(void) {
    return &test_suite;
};
