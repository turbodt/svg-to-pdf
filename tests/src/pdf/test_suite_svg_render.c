#include "./shared.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <pdf-renderer.h>
#include <string.h>


static int render_svg(char const *svg, PdfPage *page) {
    xmlDoc *doc = xmlReadMemory(
        svg,
        (int) strlen(svg),
        "input.svg",
        NULL,
        0
    );
    if (!doc) {
        return 1;
    }
    int err = pdf_render_svg_doc_to_page(doc, NULL, page);
    xmlFreeDoc(doc);
    return err;
}


static int assert_content(PdfPage const *page, char const *expected) {
    char const *content = (char const *) page->data;
    if (!content || !strstr(content, expected)) {
        LOG("Expected content: %s\n", expected);
        LOG("Actual content:\n%s\n", content ? content : "(null)");
        test_fail_reason = "Unexpected content stream";
        return 1;
    }
    return 0;
}


static int test_001(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<line x1=\"10\" y1=\"20\" x2=\"30\" y2=\"40\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "10.000000 80.000000 m\n30.000000 60.000000 l\n"
    ));
    ASSERT_ZERO(assert_content(&page, "S\n"));
    pdf_page_destroy(&page);
    return 0;
}


static int test_002(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\"><line/></svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "0.000000 100.000000 m\n0.000000 100.000000 l\n"
    ));
    ASSERT_ZERO(assert_content(&page, "f\n"));
    pdf_page_destroy(&page);
    return 0;
}


static int test_003(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<line x1=\"0\" y1=\"0\" x2=\"10\" y2=\"0\""
        " transform=\"translate(5 5)\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "5.000000 95.000000 m\n15.000000 95.000000 l\n"
    ));
    pdf_page_destroy(&page);
    return 0;
}


static int test_004(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<line x1=\"0\" y1=\"0\" x2=\"10\" y2=\"10\""
        " fill=\"none\" stroke=\"#0000ff\" stroke-width=\"2\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "0.000000 0.000000 1.000000 RG\n2.000000 w\n0 J\n[] 0 d\nS\n"
    ));
    pdf_page_destroy(&page);
    return 0;
}


static int test_005(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<line x1=\"0\" y1=\"0\" x2=\"10\" y2=\"10\" fill=\"none\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "0.000000 100.000000 m\n10.000000 90.000000 l\nn\n"
    ));
    pdf_page_destroy(&page);
    return 0;
}


static int test_006(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<line x1=\"0\" y1=\"0\" x2=\"10\" y2=\"0\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "<line x1=\"0\" y1=\"20\" x2=\"10\" y2=\"20\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "0.000000 100.000000 m\n10.000000 100.000000 l\n"
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "0.000000 80.000000 m\n10.000000 80.000000 l\n"
    ));
    pdf_page_destroy(&page);
    return 0;
}


static int test_007(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<g transform=\"translate(5 5)\">"
        "<line x1=\"0\" y1=\"0\" x2=\"10\" y2=\"0\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</g>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "5.000000 95.000000 m\n15.000000 95.000000 l\n"
    ));
    pdf_page_destroy(&page);
    return 0;
}


static int test_008(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<polyline points=\"10,20 30,40 50,20\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "10.000000 80.000000 m\n"
        "30.000000 60.000000 l\n"
        "50.000000 80.000000 l\n"
    ));
    ASSERT_ZERO(assert_content(&page, "S\n"));
    pdf_page_destroy(&page);
    return 0;
}


static int test_009(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<polyline points=\"10 20, 30 40, 50\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "10.000000 80.000000 m\n30.000000 60.000000 l\n"
    ));
    ASSERT(
        strstr((char const *) page.data, "50.000000") == NULL,
        "Expected trailing odd coordinate to be ignored"
    );
    pdf_page_destroy(&page);
    return 0;
}


static int test_010(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<polyline points=\"0,0 10,0\" transform=\"translate(5 5)\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "5.000000 95.000000 m\n15.000000 95.000000 l\n"
    ));
    pdf_page_destroy(&page);
    return 0;
}


static int test_011(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<polyline points=\"10 10\" fill=\"none\" stroke=\"#000000\"/>"
        "<polyline points=\"\" fill=\"none\" stroke=\"#000000\"/>"
        "<polyline fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT(
        strstr((char const *) page.data, " m\n") == NULL,
        "Expected no path for polylines with less than two points"
    );
    pdf_page_destroy(&page);
    return 0;
}


static int test_012(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<polygon points=\"10,20 30,40 50,20\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "10.000000 80.000000 m\n"
        "30.000000 60.000000 l\n"
        "50.000000 80.000000 l\n"
        "h\n"
    ));
    ASSERT_ZERO(assert_content(&page, "S\n"));
    pdf_page_destroy(&page);
    return 0;
}


static int test_013(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<polygon points=\"10 20, 30 40, 50\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "10.000000 80.000000 m\n30.000000 60.000000 l\nh\n"
    ));
    ASSERT(
        strstr((char const *) page.data, "50.000000") == NULL,
        "Expected trailing odd coordinate to be ignored"
    );
    pdf_page_destroy(&page);
    return 0;
}


static int test_014(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<polygon points=\"0,0 10,0\" transform=\"translate(5 5)\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "5.000000 95.000000 m\n15.000000 95.000000 l\nh\n"
    ));
    pdf_page_destroy(&page);
    return 0;
}


static int test_015(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<polygon points=\"10 10\" fill=\"none\" stroke=\"#000000\"/>"
        "<polygon points=\"\" fill=\"none\" stroke=\"#000000\"/>"
        "<polygon fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT(
        strstr((char const *) page.data, " m\n") == NULL,
        "Expected no path for polygons with less than two points"
    );
    pdf_page_destroy(&page);
    return 0;
}


static int count_occurrences(char const *haystack, char const *needle) {
    int count = 0;
    char const *p = haystack;
    while ((p = strstr(p, needle)) != NULL) {
        count++;
        p += strlen(needle);
    }
    return count;
}


static int test_016(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<circle cx=\"50\" cy=\"50\" r=\"10\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(&page, "60.000000 50.000000 m\n"));
    ASSERT_ZERO(assert_content(
        &page,
        "60.000000 44.477153 55.522847 40.000000 50.000000 40.000000 c\n"
    ));
    ASSERT_EQ(
        count_occurrences(page.data, " c\n"),
        4,
        "Expected four cubic curves"
    );
    ASSERT_ZERO(assert_content(&page, "h\n"));
    ASSERT_ZERO(assert_content(&page, "S\n"));
    pdf_page_destroy(&page);
    return 0;
}


static int test_017(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<circle cx=\"50\" cy=\"50\" r=\"10\" transform=\"translate(5 5)\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(&page, "65.000000 45.000000 m\n"));
    pdf_page_destroy(&page);
    return 0;
}


static int test_018(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<circle cx=\"50\" cy=\"50\" r=\"0\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "<circle cx=\"50\" cy=\"50\" fill=\"none\" stroke=\"#000000\"/>"
        "<circle cx=\"50\" cy=\"50\" r=\"-10\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT(
        strstr((char const *) page.data, " m\n") == NULL,
        "Expected no path for circles without a positive radius"
    );
    pdf_page_destroy(&page);
    return 0;
}


static int test_019(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<ellipse cx=\"50\" cy=\"50\" rx=\"20\" ry=\"10\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(&page, "70.000000 50.000000 m\n"));
    ASSERT_ZERO(assert_content(
        &page,
        "70.000000 44.477153 61.045695 40.000000 50.000000 40.000000 c\n"
    ));
    ASSERT_EQ(
        count_occurrences(page.data, " c\n"),
        4,
        "Expected four cubic curves"
    );
    ASSERT_ZERO(assert_content(&page, "h\n"));
    ASSERT_ZERO(assert_content(&page, "S\n"));
    pdf_page_destroy(&page);
    return 0;
}


static int test_020(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<ellipse cx=\"50\" cy=\"50\" rx=\"20\" ry=\"10\""
        " transform=\"translate(5 5)\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(&page, "75.000000 45.000000 m\n"));
    pdf_page_destroy(&page);
    return 0;
}


static int test_021(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<ellipse cx=\"50\" cy=\"50\" rx=\"0\" ry=\"10\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "<ellipse cx=\"50\" cy=\"50\" rx=\"20\" ry=\"0\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "<ellipse cx=\"50\" cy=\"50\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "<ellipse cx=\"50\" cy=\"50\" rx=\"-20\" ry=\"-10\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT(
        strstr((char const *) page.data, " m\n") == NULL,
        "Expected no path for ellipses without positive radii"
    );
    pdf_page_destroy(&page);
    return 0;
}


static int test_022(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<rect x=\"10\" y=\"10\" width=\"80\" height=\"60\" rx=\"10\" ry=\"10\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "20.000000 90.000000 m\n"
        "80.000000 90.000000 l\n"
        "85.522847 90.000000 90.000000 85.522847 90.000000 80.000000 c\n"
    ));
    ASSERT_EQ(
        count_occurrences(page.data, " c\n"),
        4,
        "Expected four rounded corners"
    );
    ASSERT_ZERO(assert_content(&page, "h\n"));
    ASSERT_ZERO(assert_content(&page, "S\n"));
    pdf_page_destroy(&page);
    return 0;
}


static int test_023(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<rect x=\"0\" y=\"0\" width=\"100\" height=\"100\" ry=\"10\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "10.000000 100.000000 m\n"
        "90.000000 100.000000 l\n"
        "95.522847 100.000000 100.000000 95.522847 100.000000 90.000000 c\n"
    ));
    ASSERT_EQ(
        count_occurrences(page.data, " c\n"),
        4,
        "Expected four rounded corners"
    );
    pdf_page_destroy(&page);
    return 0;
}


static int test_024(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<rect x=\"0\" y=\"0\" width=\"20\" height=\"10\" rx=\"100\" ry=\"100\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(
        &page,
        "10.000000 100.000000 m\n"
        "10.000000 100.000000 l\n"
        "15.522847 100.000000 20.000000 97.761424 20.000000 95.000000 c\n"
    ));
    ASSERT_EQ(
        count_occurrences(page.data, " c\n"),
        4,
        "Expected four rounded corners"
    );
    pdf_page_destroy(&page);
    return 0;
}


static int test_025(void) {
    PdfPage page = {0};
    ASSERT_ZERO(render_svg(
        "<svg width=\"100\" height=\"100\">"
        "<rect x=\"0\" y=\"0\" width=\"100\" height=\"100\" rx=\"0\" ry=\"10\""
        " fill=\"none\" stroke=\"#000000\"/>"
        "</svg>",
        &page
    ));
    ASSERT_ZERO(assert_content(&page, "0.000000 100.000000 m\n"));
    ASSERT_EQ(
        count_occurrences(page.data, " c\n"),
        0,
        "Expected square corners when rx is zero"
    );
    ASSERT_ZERO(assert_content(&page, "h\n"));
    pdf_page_destroy(&page);
    return 0;
}


static Test tests[] = {
    {.name="Line renders its path and stroke", .call=&test_001},
    {.name="Line attributes default to zero", .call=&test_002},
    {.name="Line applies its transform", .call=&test_003},
    {.name="Line stroke style", .call=&test_004},
    {.name="Line without fill nor stroke is not painted", .call=&test_005},
    {.name="Multiple lines", .call=&test_006},
    {.name="Line inherits group transform", .call=&test_007},
    {.name="Polyline renders its path and stroke", .call=&test_008},
    {.name="Polyline ignores a trailing odd coordinate", .call=&test_009},
    {.name="Polyline applies its transform", .call=&test_010},
    {.name="Polyline needs at least two points", .call=&test_011},
    {.name="Polygon renders its path and closes it", .call=&test_012},
    {.name="Polygon ignores a trailing odd coordinate", .call=&test_013},
    {.name="Polygon applies its transform", .call=&test_014},
    {.name="Polygon needs at least two points", .call=&test_015},
    {.name="Circle renders four cubic curves", .call=&test_016},
    {.name="Circle applies its transform", .call=&test_017},
    {.name="Circle needs a positive radius", .call=&test_018},
    {.name="Ellipse renders four cubic curves", .call=&test_019},
    {.name="Ellipse applies its transform", .call=&test_020},
    {.name="Ellipse needs positive radii", .call=&test_021},
    {.name="Rounded rect renders four corners", .call=&test_022},
    {.name="Rounded rect defaults rx from ry", .call=&test_023},
    {.name="Rounded rect clamps its radii", .call=&test_024},
    {.name="Rounded rect with zero rx stays square", .call=&test_025},
    {.name="", .call=NULL},
};


static TestSuite test_suite = (TestSuite) {
    .name = "SVG rendering",
    .tests = tests,
};


TestSuite const * test_suite_pdf_get_1(void) {
    return &test_suite;
};
