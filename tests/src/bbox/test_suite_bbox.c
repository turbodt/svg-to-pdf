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
    {.name="", .call=NULL},
};


static TestSuite test_suite = (TestSuite) {
    .name = "Bounding boxes",
    .tests = tests,
};


TestSuite const * test_suite_bbox_get_1(void) {
    return &test_suite;
};
