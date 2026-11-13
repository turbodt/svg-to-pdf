#include "./shared.h"
#include <bounding-box.h>
#include <string.h>


static SvgDocument * make_doc(char const *svg) {
    return bbox_svg_doc_make_from_memory(
        svg,
        (unsigned int) strlen(svg)
    );
}


static int test_001(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<line x1=\"10\" y1=\"20\" x2=\"30\" y2=\"40\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 10, 20, 30, 40, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_002(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<line x1=\"30\" y1=\"40\" x2=\"10\" y2=\"20\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 10, 20, 30, 40, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_003(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<line x1=\"10\" y1=\"20\" x2=\"30\" y2=\"20\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 10, 20, 30, 20, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_004(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<line x1=\"10\" y1=\"20\" x2=\"10\" y2=\"60\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 10, 20, 10, 60, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_005(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<line/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 0, "Unexpected item count");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_006(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<line x1=\"0\" y1=\"0\" x2=\"10\" y2=\"10\""
        " transform=\"translate(5 5)\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 5, 5, 15, 15, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_007(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<g transform=\"translate(5 5)\">"
        "<line x1=\"0\" y1=\"0\" x2=\"10\" y2=\"10\"/>"
        "</g>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 5, 5, 15, 15, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_008(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<path d=\"M 10 20 L 30 20\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 10, 20, 30, 20, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_009(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<line x1=\"0\" y1=\"0\" x2=\"10\" y2=\"10\"/>"
        "<line x1=\"20\" y1=\"20\" x2=\"30\" y2=\"30\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 2, "Unexpected item count");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_010(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<polyline points=\"10,20 30,40 20,60\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 10, 20, 30, 60, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_011(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<polyline points=\"10,20 30,20 50,20\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 10, 20, 50, 20, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_012(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<polyline points=\"0,0 10,10\" transform=\"translate(5 5)\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 5, 5, 15, 15, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_013(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<polyline points=\"10,20 30,40 50\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 10, 20, 30, 40, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_014(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<polyline points=\"10 10\"/>"
        "<polyline/>"
        "<polyline points=\"abc\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 0, "Unexpected item count");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_015(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<polygon points=\"10,20 30,40 20,60\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 10, 20, 30, 60, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_016(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<polygon points=\"10,20 30,20 50,20\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 10, 20, 50, 20, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_017(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<polygon points=\"0,0 10,10\" transform=\"translate(5 5)\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 5, 5, 15, 15, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_018(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<polygon points=\"10 10\"/>"
        "<polygon/>"
        "<polygon points=\"abc\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 0, "Unexpected item count");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_019(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<circle cx=\"50\" cy=\"50\" r=\"10\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 40, 40, 60, 60, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_020(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<circle cx=\"50\" cy=\"50\" r=\"10\" transform=\"translate(5 5)\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 45, 45, 65, 65, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_021(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<circle cx=\"50\" cy=\"50\" r=\"10\" transform=\"rotate(45 50 50)\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 40, 40, 60, 60, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_022(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<circle cx=\"50\" cy=\"50\" r=\"0\"/>"
        "<circle cx=\"50\" cy=\"50\"/>"
        "<circle cx=\"50\" cy=\"50\" r=\"-10\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 0, "Unexpected item count");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_023(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<ellipse cx=\"50\" cy=\"50\" rx=\"20\" ry=\"10\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 30, 40, 70, 60, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_024(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<ellipse cx=\"50\" cy=\"50\" rx=\"20\" ry=\"10\""
        " transform=\"translate(5 5)\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(item->bbox, 35, 45, 75, 65, "Unexpected bbox");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_025(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<ellipse cx=\"50\" cy=\"50\" rx=\"20\" ry=\"10\""
        " transform=\"rotate(45 50 50)\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 1, "Unexpected item count");
    BboxItem const *item = bbox_collection_getc(coll, 0);
    ASSERT_NOT_NULL(item, "Missing bbox item");
    ASSERT_BOX_COORDS_EQ(
        item->bbox,
        34.188611699158104,
        34.188611699158104,
        65.8113883008419,
        65.8113883008419,
        "Unexpected bbox"
    );
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static int test_026(void) {
    char const *svg = "<svg width=\"100\" height=\"100\">"
        "<ellipse cx=\"50\" cy=\"50\" rx=\"0\" ry=\"10\"/>"
        "<ellipse cx=\"50\" cy=\"50\" rx=\"20\" ry=\"0\"/>"
        "<ellipse cx=\"50\" cy=\"50\"/>"
        "<ellipse cx=\"50\" cy=\"50\" rx=\"-20\" ry=\"-10\"/>"
        "</svg>";
    SvgDocument *doc = make_doc(svg);
    ASSERT_NOT_NULL(doc, "Failed to parse SVG document");
    BboxCollection *coll = bbox_collection_make_from_doc(doc);
    ASSERT_NOT_NULL(coll, "Failed to make bbox collection");
    ASSERT_EQ(bbox_collection_get_count(coll), 0, "Unexpected item count");
    bbox_collection_destroy(coll);
    bbox_svg_doc_destroy(doc);
    return 0;
}


static Test tests[] = {
    {.name="Diagonal line bbox", .call=&test_001},
    {.name="Line bbox normalizes endpoints", .call=&test_002},
    {.name="Horizontal line bbox", .call=&test_003},
    {.name="Vertical line bbox", .call=&test_004},
    {.name="Empty line is not collected", .call=&test_005},
    {.name="Line bbox applies its transform", .call=&test_006},
    {.name="Line bbox inherits group transform", .call=&test_007},
    {.name="Horizontal path bbox is collected", .call=&test_008},
    {.name="Multiple lines are collected", .call=&test_009},
    {.name="Polyline bbox", .call=&test_010},
    {.name="Horizontal polyline bbox", .call=&test_011},
    {.name="Polyline bbox applies its transform", .call=&test_012},
    {.name="Polyline bbox ignores a trailing odd coordinate", .call=&test_013},
    {.name="Polylines without two points are not collected", .call=&test_014},
    {.name="Polygon bbox", .call=&test_015},
    {.name="Horizontal polygon bbox", .call=&test_016},
    {.name="Polygon bbox applies its transform", .call=&test_017},
    {.name="Polygons without two points are not collected", .call=&test_018},
    {.name="Circle bbox", .call=&test_019},
    {.name="Circle bbox applies its transform", .call=&test_020},
    {.name="Rotated circle bbox stays exact", .call=&test_021},
    {.name="Circles without a positive radius are not collected", .call=&test_022},
    {.name="Ellipse bbox", .call=&test_023},
    {.name="Ellipse bbox applies its transform", .call=&test_024},
    {.name="Rotated ellipse bbox stays exact", .call=&test_025},
    {.name="Ellipses without positive radii are not collected", .call=&test_026},
    {.name="", .call=NULL},
};


static TestSuite test_suite = (TestSuite) {
    .name = "Bounding boxes",
    .tests = tests,
};


TestSuite const * test_suite_bbox_get_1(void) {
    return &test_suite;
};
