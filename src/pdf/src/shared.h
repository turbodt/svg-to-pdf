#ifndef APP_PDF_SHARED_H
#define APP_PDF_SHARED_H


#include "./pdf-buffer.h"
#include <custom-svg.h>
#include <libxml/tree.h>


typedef struct {
    int enabled;
    double r;
    double g;
    double b;
} PdfColor;


typedef struct {
    Transform *transform;
    PdfColor fill;
    PdfColor stroke;
    double stroke_width;
    int linecap;
    char dash[64];
    Box2D viewbox;
    double page_width;
    double page_height;
    double scale;
    double offset_x;
    double offset_y;
} RenderState;


RenderState render_state_child(xmlNode *node, RenderState const *parent);
void render_state_destroy(RenderState *state);

void apply_style(RenderState *state, char const *name, char const *value);
int parse_url_id(char const *value, char *out, size_t out_size);
double attr_double(xmlNode *node, char const *name, double fallback);
char *attr_string(xmlNode *node, char const *name);

void render_path(xmlNode *node, PdfBuf *out, RenderState const *state);
void render_rect(xmlNode *node, PdfBuf *out, RenderState const *state);
void render_text(xmlNode *node, PdfBuf *out, RenderState const *state);
int emit_clip(xmlNode *node, PdfBuf *out, RenderState const *state, xmlDoc *doc);

void emit_paint(PdfBuf *out, RenderState const *state);
void emit_point(PdfBuf *out, RenderState const *state, Point2D p);
Point2D map_point(RenderState const *state, Point2D p);


#endif
