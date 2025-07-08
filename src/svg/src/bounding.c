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
static Box2D create_box(Interval Ix, Interval Iy);
static double min2(double const [2]);
static double max2(double const [2]);
static double min4(double const [4]);
static double max4(double const [4]);
static int almost_eq_rel(double a, double b);


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
    if (almost_eq_rel(p2 - p1, p1 - p0)) {
        return cmd_linear_get_dim_bound(p0, p2);
    }

    double const A = p2 - 2*p1 + p0;
    double const B = 2*(p1 - p0);
    double const t = - B / 2 /A;
    if (t <= 0 || t >= 1) {
        return cmd_linear_get_dim_bound(p0, p2);
    }

    double const p4 = (A *t + B)*t + p0;
    Interval I = {p0, p2};
    if (p2 < p0) {
        I = (Interval){p2, p0};
    }
    if (p4 < I.min) {
        I.min = p4;
    } else if (p4 > I.max) {
        I.max = p4;
    }
    return I;
}


Interval cmd_cbezier_get_dim_bound(double p0, double p1, double p2, double p3) {
    double const b1 = p3 - 2*p2 + p1;
    double const b0 = p2 - 2*p1 + p0;
    if (almost_eq_rel(b1, b0)) {
        return cmd_linear_get_dim_bound(p0, p3);
    }

    double const A = b1 - b0;
    double const B = 3*b0;
    double const C = 3*(p1 - p0);

    double const R = b0*b0 - A*C/3;
    if (R < 0) {
        return cmd_linear_get_dim_bound(p0, p3);
    }
    double t1 = - B/3/A + sqrt(R)/A;
    double t2 = - B/3/A - sqrt(R)/A;
    t1 = t1 < 0 ? 0 : t1 > 1 ? 1 : t1;
    t2 = t2 < 0 ? 0 : t2 > 1 ? 1 : t2;

    double p[4] = {
        p0, p3,
        ((A*t1 + B)*t1 + C)*t1 + p0,
        ((A*t2 + B)*t2 + C)*t2 + p0
    };

    return (Interval){min4(p),max4(p)};
}


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


inline double min4(double const  values[4]) {
    double m = values[0];
    if (m > values[1]) {
        m = values[1];
    }
    if (m > values[2]) {
        m = values[2];
    }
    if (m > values[3]) {
        m = values[3];
    }
    return m;
};

inline double max4(double const  values[4]) {
    double m = values[0];
    if (m < values[1]) {
        m = values[1];
    }
    if (m < values[2]) {
        m = values[2];
    }
    if (m < values[3]) {
        m = values[3];
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
