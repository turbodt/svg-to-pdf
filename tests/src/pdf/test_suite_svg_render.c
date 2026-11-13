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


static Test tests[] = {
    {.name="Line renders its path and stroke", .call=&test_001},
    {.name="Line attributes default to zero", .call=&test_002},
    {.name="Line applies its transform", .call=&test_003},
    {.name="Line stroke style", .call=&test_004},
    {.name="Line without fill nor stroke is not painted", .call=&test_005},
    {.name="Multiple lines", .call=&test_006},
    {.name="Line inherits group transform", .call=&test_007},
    {.name="", .call=NULL},
};


static TestSuite test_suite = (TestSuite) {
    .name = "SVG rendering",
    .tests = tests,
};


TestSuite const * test_suite_pdf_get_1(void) {
    return &test_suite;
};
