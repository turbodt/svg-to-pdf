#include "./bounding.h"
#include <math.h>


#define DOUBLE_ALMOST_EQ_TOL 1e-10


typedef struct {double min; double max;} Interval;


static Box2D cmd_linear_get_bbox(SVGPathCommand const *command);
static Box2D cmd_qbezier_get_bbox(SVGPathCommand const *command);
static Box2D cmd_cbezier_get_bbox(SVGPathCommand const *command);
static Interval cmd_linear_get_x_bound(SVGPathCommand const *command);
static Interval cmd_linear_get_y_bound(SVGPathCommand const *command);
static Interval cmd_qbezier_get_x_bound(SVGPathCommand const *command);
static Interval cmd_qbezier_get_y_bound(SVGPathCommand const *command);
static Interval cmd_cbezier_get_x_bound(SVGPathCommand const *command);
static Interval cmd_cbezier_get_y_bound(SVGPathCommand const *command);
static Interval cmd_linear_get_dim_bound(double p0, double p1);
static Interval cmd_qbezier_get_dim_bound(double p0, double p1, double p2);
static Interval cmd_cbezier_get_dim_bound(double p0, double p1, double p2, double p3);
static Interval polynomial_deg3_get_dim_bound_01(double D, double C, double B, double A);
static Interval polynomial_deg2_get_dim_bound_01(double C, double B, double A);
static Interval polynomial_deg1_get_dim_bound_01(double B, double A);
static Box2D create_box(Interval Ix, Interval Iy);
static double min2(double const [2]);
static double max2(double const [2]);
static double min3(double const [3]);
static double max3(double const [3]);
static double min4(double const [4]);
static double max4(double const [4]);
static int almost_eq_rel(double a, double b);


Box2D svg_path_get_bbox(SVGPath const *path) {
    Box2D bbox = {0};
    if (!path) {
        return bbox;
    }
    unsigned int const cmd_count = svg_path_command_count(path);
    if (cmd_count == 0) {
        return bbox;
    }
    SVGPathCommand const *command;
    command = svg_path_get_command(path, 0);
    bbox = svg_path_command_get_bbox(command);

    for (unsigned int i = 1; i < cmd_count; i++) {
        command = svg_path_get_command(path, i);
        Box2D cmd_bbox = svg_path_command_get_bbox(command);
        bbox = geo_boxes_compose(bbox, cmd_bbox);
    }

    return bbox;
};


Box2D svg_path_command_get_bbox(SVGPathCommand const *command) {
    switch (command->type) {
        case SVG_PATH_CMD_LINE:
            return cmd_linear_get_bbox(command);
        case SVG_PATH_CMD_QBEZIER:
            return cmd_qbezier_get_bbox(command);
        case SVG_PATH_CMD_CBEZIER:
            return cmd_cbezier_get_bbox(command);
        default: break;
    }
    return (Box2D){0};
};


inline Box2D cmd_linear_get_bbox(SVGPathCommand const *command) {
    Interval Ix, Iy;
    Ix = cmd_linear_get_x_bound(command);
    Iy = cmd_linear_get_y_bound(command);
    return create_box(Ix, Iy);
};


inline Box2D cmd_qbezier_get_bbox(SVGPathCommand const *command) {
    Interval Ix, Iy;
    Ix = cmd_qbezier_get_x_bound(command);
    Iy = cmd_qbezier_get_y_bound(command);
    return create_box(Ix, Iy);
};


inline Box2D cmd_cbezier_get_bbox(SVGPathCommand const *command) {
    Interval Ix, Iy;
    Ix = cmd_cbezier_get_x_bound(command);
    Iy = cmd_cbezier_get_y_bound(command);
    return create_box(Ix, Iy);
}


inline Interval cmd_linear_get_x_bound(SVGPathCommand const *command) {
    return cmd_linear_get_dim_bound(command->start.x, command->end.x);
};


inline Interval cmd_linear_get_y_bound(SVGPathCommand const *command) {
    return cmd_linear_get_dim_bound(command->start.y, command->end.y);
};


inline Interval cmd_qbezier_get_x_bound(SVGPathCommand const *command) {
    return cmd_qbezier_get_dim_bound(
        command->start.x,
        command->ctrl[0].x,
        command->end.x
    );
};


inline Interval cmd_qbezier_get_y_bound(SVGPathCommand const *command) {
    return cmd_qbezier_get_dim_bound(
        command->start.y,
        command->ctrl[0].y,
        command->end.y
    );
};


inline Interval cmd_cbezier_get_x_bound(SVGPathCommand const *command) {
    return cmd_cbezier_get_dim_bound(
        command->start.x,
        command->ctrl[0].x,
        command->ctrl[1].x,
        command->end.x
    );
};


inline Interval cmd_cbezier_get_y_bound(SVGPathCommand const *command) {
    return cmd_cbezier_get_dim_bound(
        command->start.y,
        command->ctrl[0].y,
        command->ctrl[1].y,
        command->end.y
    );
};


inline Interval cmd_linear_get_dim_bound(double p0, double p1) {
    if (p0 < p1) {
        return (Interval) {p0, p1};
    }
    return (Interval) {p1, p0};
};


Interval cmd_qbezier_get_dim_bound(double p0, double p1, double p2) {
    double const A = p2 - 2*p1 + p0;
    double const B = 2*(p1 - p0);
    return polynomial_deg2_get_dim_bound_01(p0, B, A);
}


Interval cmd_cbezier_get_dim_bound(double p0, double p1, double p2, double p3) {
    double const b1 = p3 - 2*p2 + p1;
    double const b0 = p2 - 2*p1 + p0;
    double const A = b1 - b0;
    double const B = 3*b0;
    double const C = 3*(p1 - p0);

    return polynomial_deg3_get_dim_bound_01(p0, C, B, A);
}

static Interval polynomial_deg3_get_dim_bound_01(double D, double C, double B, double A) {
    // P(t) = D + C t + B t^2 + A t^3
    // P'(t) = C + 2 B t + 3 A t^2
    if (almost_eq_rel(A, 0.0)) {
        return polynomial_deg2_get_dim_bound_01(D, C, B);
    }

    double const R = B*B - 3*A*C;
    if (R < 0) {
        return polynomial_deg1_get_dim_bound_01(D, C + B + A);
    }
    double t1 = (- B + sqrt(R))/(3*A);
    double t2 = (- B - sqrt(R))/(3*A);
    t1 = t1 < 0 ? 0 : t1 > 1 ? 1 : t1;
    t2 = t2 < 0 ? 0 : t2 > 1 ? 1 : t2;

    double p[4] = {
        D, D + C + B + A,
        ((A*t1 + B)*t1 + C)*t1 + D,
        ((A*t2 + B)*t2 + C)*t2 + D,
    };

    return (Interval){min4(p),max4(p)};
}

static Interval polynomial_deg2_get_dim_bound_01(double C, double B, double A) {
    if (almost_eq_rel(A, 0.0)) {
        return polynomial_deg1_get_dim_bound_01(C, B);
    }
    double t = -B/(2*A);
    t = t < 0 ? 0 : t > 1 ? 1 : t;

    double p[3] = {C, C + B + A, (A*t + B)*t + C};

    return (Interval){.min = min3(p), .max=max3(p)};
}

static Interval polynomial_deg1_get_dim_bound_01(double B, double A) {
    Interval I = {.min=B, .max=B};
    if (A < 0) {
        I.min += A;
    } else {
        I.max += A;
    }
    return I;
};


inline Box2D create_box(Interval Ix, Interval Iy){
    return (Box2D){
        .tl = {.x=Ix.min, .y=Iy.min},
        .size = {.width=Ix.max - Ix.min, .height=Iy.max - Iy.min},
    };
}


inline double min2(double const values[2]) {
    double m = values[0];
    if (m > values[1]) {
        m = values[1];
    }
    return m;
};


inline double max2(double const  values[2]) {
    double m = values[0];
    if (m < values[1]) {
        m = values[1];
    }
    return m;
};


inline double min3(double const values[3]) {
    double m = min2(values+1);
    if (m > values[0]) {
        m = values[0];
    }
    return m;
};


inline double max3(double const  values[3]) {
    double m = max2(values+1);
    if (m < values[0]) {
        m = values[0];
    }
    return m;
};


inline double min4(double const  values[4]) {
    double m = min3(values+1);
    if (m > values[0]) {
        m = values[0];
    }
    return m;
};

inline double max4(double const  values[4]) {
    double m = max3(values+1);
    if (m < values[0]) {
        m = values[0];
    }
    return m;
};


inline int almost_eq_rel(double A, double B) {
    double diff = fabs(A - B);
    A = fabs(A);
    B = fabs(B);
    double largest = A < B ? B : A;

    if (diff <= largest * DOUBLE_ALMOST_EQ_TOL) {
        return 1;
    }
    return 0;
}
