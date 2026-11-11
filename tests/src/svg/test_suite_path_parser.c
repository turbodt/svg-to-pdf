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

    ASSERT_PATH_IS_LINE(*command, 0, 0, 1, 1);

    svg_path_destroy(path);
    return 0;
}



static int test_004(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 L 1 1 1 0 Z");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 3, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1 , "Unexpected first command");

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 1,1, 1,0, "Unexpected second command");

    command = svg_path_get_command(path, 2);
    ASSERT_NOT_NULL(command, "Expected third command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 1,0, 0,0, "Unexpected third command");

    svg_path_destroy(path);
    return 0;
}


static int test_005(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 L 1 1 M 2 2 L 4 4");
    ASSERT_NOT_NULL(path);
    ASSERT_EQ(svg_path_command_count(path), 2);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command);
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command);
    ASSERT_PATH_IS_LINE(*command, 2,2, 4,4);

    svg_path_destroy(path);
    return 0;
}


static int test_006(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 L 1 1 m 2 2 L 4 4");
    ASSERT_NOT_NULL(path);
    ASSERT_EQ(svg_path_command_count(path), 2);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command);
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command);
    ASSERT_PATH_IS_LINE(*command, 3,3, 4,4);

    svg_path_destroy(path);
    return 0;
}

static int test_007(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("m 0 0 L 1 1 m 2 2 L 4 4");
    ASSERT_NOT_NULL(path);
    ASSERT_EQ(svg_path_command_count(path), 2);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command);
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command);
    ASSERT_PATH_IS_LINE(*command, 3,3, 4,4);

    svg_path_destroy(path);
    return 0;
}


static int test_101(void) {
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


static int test_008(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string(
        "M 0 0 C 1 1 3 1 4 0 S 7 -1 8 0"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 0,0, 1,1, 3,1, 4,0);

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_EQ(command->type, SVG_PATH_CMD_CBEZIER, "Unexpected second command type");
    ASSERT_FLOAT_EQ(command->start.x, 4, "Unexpected second command start x");
    ASSERT_FLOAT_EQ(command->start.y, 0, "Unexpected second command start y");
    ASSERT_FLOAT_EQ(command->end.x, 8, "Unexpected second command end x");
    ASSERT_FLOAT_EQ(command->end.y, 0, "Unexpected second command end y");
    ASSERT_FLOAT_EQ(command->ctrl[1].x, 7, "Unexpected second command control 2 x");
    ASSERT_FLOAT_EQ(command->ctrl[1].y, -1, "Unexpected second command control 2 y");

    svg_path_destroy(path);
    return 0;
}


static int test_009(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string(
        "M 0 0 C 1 1 3 1 4 0 S 7 -1 8 0 10 1 11 0"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 3, "Unexpected command count");

    command = svg_path_get_command(path, 2);
    ASSERT_NOT_NULL(command, "Expected third command to be not NULL");
    ASSERT_EQ(command->type, SVG_PATH_CMD_CBEZIER, "Unexpected third command type");
    ASSERT_FLOAT_EQ(command->start.x, 8, "Unexpected third command start x");
    ASSERT_FLOAT_EQ(command->start.y, 0, "Unexpected third command start y");
    ASSERT_FLOAT_EQ(command->end.x, 11, "Unexpected third command end x");
    ASSERT_FLOAT_EQ(command->end.y, 0, "Unexpected third command end y");
    ASSERT_FLOAT_EQ(command->ctrl[1].x, 10, "Unexpected third command control 2 x");
    ASSERT_FLOAT_EQ(command->ctrl[1].y, 1, "Unexpected third command control 2 y");

    svg_path_destroy(path);
    return 0;
}


static int test_010(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string(
        "M 0 0 C 1 1 3 1 4 0 s 3 -1 4 0"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_EQ(command->type, SVG_PATH_CMD_CBEZIER, "Unexpected second command type");
    ASSERT_FLOAT_EQ(command->start.x, 4, "Unexpected second command start x");
    ASSERT_FLOAT_EQ(command->start.y, 0, "Unexpected second command start y");
    ASSERT_FLOAT_EQ(command->end.x, 8, "Unexpected second command end x");
    ASSERT_FLOAT_EQ(command->end.y, 0, "Unexpected second command end y");
    ASSERT_FLOAT_EQ(command->ctrl[1].x, 7, "Unexpected second command control 2 x");
    ASSERT_FLOAT_EQ(command->ctrl[1].y, -1, "Unexpected second command control 2 y");

    svg_path_destroy(path);
    return 0;
}


static int test_011(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 5 5");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 5,5);

    svg_path_destroy(path);
    return 0;
}


static int test_012(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 5 5 10 10");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 5,5);

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 5,5, 10,10);

    svg_path_destroy(path);
    return 0;
}


static int test_013(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("m 10 10 5 5");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 10,10, 15,15);

    svg_path_destroy(path);
    return 0;
}


static int test_014(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 5 5 L 10 10");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 5,5);

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 5,5, 10,10);

    svg_path_destroy(path);
    return 0;
}


static int test_015(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string(
        "M 0 0 C 1 1 3 1 4 0 S 7 -1 8 0"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 4,0, 5,-1, 7,-1, 8,0);

    svg_path_destroy(path);
    return 0;
}


static int test_016(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string(
        "M 0 0 C 1 1 3 1 4 0 S 6 -2 8 0 S 10 1 11 0"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 3, "Unexpected command count");

    command = svg_path_get_command(path, 2);
    ASSERT_NOT_NULL(command, "Expected third command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 8,0, 10,2, 10,1, 11,0);

    svg_path_destroy(path);
    return 0;
}


static int test_017(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string(
        "M 0 0 C 1 1 3 1 4 0 L 5 5 S 7 6 8 5"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 3, "Unexpected command count");

    command = svg_path_get_command(path, 2);
    ASSERT_NOT_NULL(command, "Expected third command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 5,5, 5,5, 7,6, 8,5);

    svg_path_destroy(path);
    return 0;
}


static int test_018(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 Q 1 1 2 0 T 4 -1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_QBEZIER(*command, 2,0, 3,-1, 4,-1);

    svg_path_destroy(path);
    return 0;
}


static int test_019(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string(
        "M 0 0 Q 1 1 2 0 T 4 -1 T 6 1"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 3, "Unexpected command count");

    command = svg_path_get_command(path, 2);
    ASSERT_NOT_NULL(command, "Expected third command to be not NULL");
    ASSERT_PATH_IS_QBEZIER(*command, 4,-1, 5,-1, 6,1);

    svg_path_destroy(path);
    return 0;
}


static int test_020(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string(
        "M 0 0 Q 1 1 2 0 L 3 3 T 5 5"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 3, "Unexpected command count");

    command = svg_path_get_command(path, 2);
    ASSERT_NOT_NULL(command, "Expected third command to be not NULL");
    ASSERT_PATH_IS_QBEZIER(*command, 3,3, 3,3, 5,5);

    svg_path_destroy(path);
    return 0;
}


static int test_021(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string(
        "M 0 0 C 1 1 3 1 4 0 s 3 -1 4 0"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 4,0, 5,-1, 7,-1, 8,0);

    svg_path_destroy(path);
    return 0;
}


static int test_022(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 q 1 1 2 0 t 2 2");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_QBEZIER(*command, 2,0, 3,-1, 4,2);

    svg_path_destroy(path);
    return 0;
}


static int test_023(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 Q 1 1 2 0 S 4 1 5 0");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 2,0, 2,0, 4,1, 5,0);

    svg_path_destroy(path);
    return 0;
}


static int test_024(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 C 1 1 3 1 4 0 T 6 1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_QBEZIER(*command, 4,0, 4,0, 6,1);

    svg_path_destroy(path);
    return 0;
}


static int test_025(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 1 0 A 1 1 0 0 1 0 1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    double const k = 4.0/3.0*tan(M_PI/8.0);
    ASSERT_PATH_IS_CBEZIER(*command, 1,0, 1,k, k,1, 0,1);

    svg_path_destroy(path);
    return 0;
}


static int test_026(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 1 0 A 1 1 0 0 1 -1 0");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    double const k = 4.0/3.0*tan(M_PI/8.0);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 1,0, 1,k, k,1, 0,1);

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 0,1, -k,1, -1,k, -1,0);

    svg_path_destroy(path);
    return 0;
}


static int test_027(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 1 0 A 1 1 0 0 0 -1 0");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    double const k = 4.0/3.0*tan(M_PI/8.0);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 1,0, 1,-k, k,-1, 0,-1);

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 0,-1, -k,-1, -1,-k, -1,0);

    svg_path_destroy(path);
    return 0;
}


static int test_028(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 1 0 A 1 1 0 1 1 0 -1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 3, "Unexpected command count");

    double const k = 4.0/3.0*tan(M_PI/8.0);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 1,0, 1,k, k,1, 0,1);

    command = svg_path_get_command(path, 2);
    ASSERT_NOT_NULL(command, "Expected third command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, -1,0, -1,-k, -k,-1, 0,-1);

    svg_path_destroy(path);
    return 0;
}


static int test_029(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 1 0 a 1 1 0 0 1 -1 1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    double const k = 4.0/3.0*tan(M_PI/8.0);
    ASSERT_PATH_IS_CBEZIER(*command, 1,0, 1,k, k,1, 0,1);

    svg_path_destroy(path);
    return 0;
}


static int test_030(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 A 0 5 30 0 1 10 10");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 10,10);

    svg_path_destroy(path);
    return 0;
}


static int test_031(void) {
    SVGPath * path = svg_path_make_from_string("M 0 0 A 5 5 30 0 1 0 0");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 0, "Unexpected command count");
    svg_path_destroy(path);

    SVGPathCommand const *command = NULL;
    path = svg_path_make_from_string("M 0 0 A 5 5 30 0 1 0 0 L 3 3");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 3,3);

    svg_path_destroy(path);
    return 0;
}


static int test_032(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string(
        "M 1 0 A 1 1 0 0 1 0 1 1 1 0 0 1 0 -1"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 3, "Unexpected command count");

    command = svg_path_get_command(path, 2);
    ASSERT_NOT_NULL(command, "Expected third command to be not NULL");
    ASSERT_EQ(command->type, SVG_PATH_CMD_CBEZIER, "Unexpected third command type");
    ASSERT_FLOAT_EQ(command->end.x, 0, "Unexpected third command end x");
    ASSERT_FLOAT_EQ(command->end.y, -1, "Unexpected third command end y");

    svg_path_destroy(path);

    path = svg_path_make_from_string(
        "M 0 0 A 5 5 30 0 1 10 0 5 5 30 0 1 0 0"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 4, "Unexpected command count");

    command = svg_path_get_command(path, 3);
    ASSERT_NOT_NULL(command, "Expected fourth command to be not NULL");
    ASSERT_FLOAT_EQ(command->end.x, 0, "Unexpected fourth command end x");
    ASSERT_FLOAT_EQ(command->end.y, 0, "Unexpected fourth command end y");

    svg_path_destroy(path);
    return 0;
}


static int test_033(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 A 1 1 0 0 1 4 0");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    double const k = 4.0/3.0*tan(M_PI/8.0);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 0,0, 0,-2*k, 2-2*k,-2, 2,-2);

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 2,-2, 2+2*k,-2, 4,-2*k, 4,0);

    svg_path_destroy(path);
    return 0;
}


static int test_034(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 A 2 1 90 0 1 4 0");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    double const k = 4.0/3.0*tan(M_PI/8.0);

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 0,0, 0,-4*k, 2-2*k,-4, 2,-4);

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 2,-4, 2+2*k,-4, 4,-4*k, 4,0);

    svg_path_destroy(path);
    return 0;
}


static int test_035(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 1 0 A -1 -1 0 0 1 0 1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    double const k = 4.0/3.0*tan(M_PI/8.0);
    ASSERT_PATH_IS_CBEZIER(*command, 1,0, 1,k, k,1, 0,1);

    svg_path_destroy(path);
    return 0;
}


static int test_036(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string(
        "M 1 0 A 1 1 0 0 1 0 1 S 2 2 3 3"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_CBEZIER(*command, 0,1, 0,1, 2,2, 3,3);

    svg_path_destroy(path);
    return 0;
}


static int test_037(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string(
        "M 1 0 A 1 1 0 0 1 0 1 T 2 2"
    );
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 2, "Unexpected command count");

    command = svg_path_get_command(path, 1);
    ASSERT_NOT_NULL(command, "Expected second command to be not NULL");
    ASSERT_PATH_IS_QBEZIER(*command, 0,1, 0,1, 2,2);

    svg_path_destroy(path);
    return 0;
}


static int test_038(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 1 0 A 1 1 0 010 1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    double const k = 4.0/3.0*tan(M_PI/8.0);
    ASSERT_PATH_IS_CBEZIER(*command, 1,0, 1,k, k,1, 0,1);

    svg_path_destroy(path);
    return 0;
}


static int test_039(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0\nL 1 1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    svg_path_destroy(path);

    path = svg_path_make_from_string("M 0 0\n5 5");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 5,5);

    svg_path_destroy(path);
    return 0;
}


static int test_040(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0\r\nL 1 1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    svg_path_destroy(path);

    path = svg_path_make_from_string("M 0 0\tL 1 1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    svg_path_destroy(path);
    return 0;
}


static int test_041(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0 0 L 1 1 ");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    svg_path_destroy(path);

    path = svg_path_make_from_string("M 0 0 L 1 1\n");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    svg_path_destroy(path);

    path = svg_path_make_from_string("M 0 0 L 1 1\r\n");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    svg_path_destroy(path);
    return 0;
}


static int test_042(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 0\n0 L 1\n1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    svg_path_destroy(path);
    return 0;
}


static int test_043(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("M 1 0\nA 1 1 0 0\n1 0 1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    double const k = 4.0/3.0*tan(M_PI/8.0);
    ASSERT_PATH_IS_CBEZIER(*command, 1,0, 1,k, k,1, 0,1);

    svg_path_destroy(path);
    return 0;
}


static int test_044(void) {
    SVGPathCommand const *command = NULL;
    SVGPath * path = svg_path_make_from_string("  M 0 0 L 1 1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    svg_path_destroy(path);

    path = svg_path_make_from_string("\n\tM 0 0\nL 1 1");
    ASSERT_NOT_NULL(path, "Expected path to be parsed");
    ASSERT_EQ(svg_path_command_count(path), 1, "Unexpected command count");

    command = svg_path_get_command(path, 0);
    ASSERT_NOT_NULL(command, "Expected first command to be not NULL");
    ASSERT_PATH_IS_LINE(*command, 0,0, 1,1);

    svg_path_destroy(path);
    return 0;
}


static Test tests[] = {
    {.name="Basic parsing 001", .call=&test_001},
    {.name="Basic parsing 002", .call=&test_002},
    {.name="Basic parsing 003", .call=&test_003},
    {.name="Basic parsing 004", .call=&test_004},
    {.name="Multiple M commands 005", .call=&test_005},
    {.name="Relative M commands 006", .call=&test_006},
    {.name="Relative M commands 007", .call=&test_007},
    {.name="S command parsing 008", .call=&test_008},
    {.name="S command repetition 009", .call=&test_009},
    {.name="Relative s command 010", .call=&test_010},
    {.name="Moveto implicit lineto 011", .call=&test_011},
    {.name="Moveto implicit linetos 012", .call=&test_012},
    {.name="Relative moveto implicit lineto 013", .call=&test_013},
    {.name="Moveto implicit and explicit linetos 014", .call=&test_014},
    {.name="S reflection after C 015", .call=&test_015},
    {.name="S reflection chain 016", .call=&test_016},
    {.name="S reflection after lineto 017", .call=&test_017},
    {.name="T reflection after Q 018", .call=&test_018},
    {.name="T reflection chain 019", .call=&test_019},
    {.name="T reflection after lineto 020", .call=&test_020},
    {.name="Relative s reflection 021", .call=&test_021},
    {.name="Relative t reflection 022", .call=&test_022},
    {.name="S control after Q 023", .call=&test_023},
    {.name="T control after C 024", .call=&test_024},
    {.name="Quarter arc 025", .call=&test_025},
    {.name="Semicircle arc 026", .call=&test_026},
    {.name="Arc with sweep 0 027", .call=&test_027},
    {.name="Large arc 028", .call=&test_028},
    {.name="Relative arc 029", .call=&test_029},
    {.name="Arc with zero radii 030", .call=&test_030},
    {.name="Arc with identical endpoints 031", .call=&test_031},
    {.name="Arc implicit repetition 032", .call=&test_032},
    {.name="Arc radii scaling 033", .call=&test_033},
    {.name="Rotated arc 034", .call=&test_034},
    {.name="Negative arc radii 035", .call=&test_035},
    {.name="S after arc 036", .call=&test_036},
    {.name="T after arc 037", .call=&test_037},
    {.name="Arc flags without separators 038", .call=&test_038},
    {.name="Newline separated commands 039", .call=&test_039},
    {.name="Tab and carriage return separators 040", .call=&test_040},
    {.name="Trailing whitespace 041", .call=&test_041},
    {.name="Newline between coordinates 042", .call=&test_042},
    {.name="Newline separated arc 043", .call=&test_043},
    {.name="Leading whitespace 044", .call=&test_044},
    {.name="Real case 101", .call=&test_101},
    {.name="", .call=NULL},
};


static TestSuite test_suite = (TestSuite) {
    .name = "Path parsing",
    .tests = tests,
};


TestSuite const * test_suite_svg_get_2(void) {
    return &test_suite;
};
