#include <test-shared.h>
#include <test-svg.h>
#include <stdio.h>
#include <string.h>


char const * test_fail_reason = NULL;


typedef struct {
    char * tsuite_filter;
    char * t_filter;
    int is_error;
} Options;
typedef struct {
    size_t passed;
    size_t failed;
    size_t skipped;
} TestSuiteRunReport;
static Options parse_args(int, char *[]);
static void print_help(void);
static TestSuiteRunReport run_test_suite(TestSuite const *, Options const *);


int main(int argc, char * argv[]) {

    Options const options = parse_args(argc, argv);
    if (options.is_error) {
        print_help();
        return options.is_error;
    }

    printf("Running all tests...\n");

    TestSuite const * ts[] = {
        test_suite_svg_get_1(),
        test_suite_svg_get_2(),
        test_suite_svg_get_3(),
        NULL
    };

    TestSuiteRunReport report = {0};
    for (size_t i = 0; ts[i] != NULL; i++) {
        char const * filter = options.tsuite_filter;
        if (filter != NULL && strstr(ts[i]->name, filter) == NULL) {
            continue;
        }
        TestSuiteRunReport ts_report = run_test_suite(ts[i], &options);
        report.passed += ts_report.passed;
        report.failed += ts_report.failed;
        report.skipped += ts_report.skipped;
        printf("\n");
    }

    if (report.skipped > 0) {
        printf("\x1b[34m%zu tests skipped.\x1b[m\n", report.skipped);
    }
    if (report.failed > 0) {
        printf("\x1b[31m%zu tests failed! :(\x1b[m\n", report.failed);
    } else {
        printf("\x1b[32mAll %zu tests passed.\x1b[m\n", report.passed);
    }

    return 0;
}


TestSuiteRunReport run_test_suite(TestSuite const *ts, Options const *options) {
    static char const * indent = "  ";
    TestSuiteRunReport report = {0};
    printf("Running test suite '%s':\n", ts->name);
    for (size_t i = 0; 1 ; i++) {
        Test const * test = &ts->tests[i];
        if (test->call == NULL) {
            break;
        }
        char const * filter = options->t_filter;
        if (filter != NULL && strstr(test->name, filter) == NULL) {
            report.skipped++;
            continue;
        }
        printf(
            "%s%04zu%sRunning test '%s'...",
            indent, i + 1, indent, test->name
        );
        fflush(stdout);
        int err = test->call();
        if (err) {
            printf(" \x1b[31mFailed\x1b[m");
            if (test_fail_reason != NULL) {
                printf(
                    "\n\n%s%s\x1b[33m%s\x1b[m\n",
                    indent,
                    indent,
                    test_fail_reason
                );
            }
            printf("\n");
            report.failed++;
        } else {
            printf(" \x1b[32mPassed\x1b[m\n");
            report.passed++;
        }
    }
    printf(
        "Done! Runned %zu tests."
        " \x1b[32m %zu passed\x1b[m",
        report.passed + report.failed,
        report.passed
    );

    if (report.failed) {
        printf(" and \x1b[31m %zu failed\x1b[m\n", report.failed);
    } else {
        printf(".\n");
    }

    return report;
}


Options parse_args(int argc, char * argv[]) {
    Options options = {
        .tsuite_filter = NULL,
        .t_filter = NULL,
        .is_error = 0,
    };

    for (int i = 1; i < argc && options.is_error == 0; i++) {
        char const * arg = argv[i];
        if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
            options.is_error = 1;
        } else if (strcmp(arg, "-f") == 0 || strcmp(arg, "--filter") == 0) {
            i++;
            options.is_error = i >= argc;
            options.t_filter = argv[i];
        } else if (strcmp(arg, "-s") == 0 || strcmp(arg, "--test-suite") == 0) {
            i++;
            options.is_error = i >= argc;
            options.tsuite_filter = argv[i];
        } else {
            printf("Unknown option \"\x1b[31m%s\x1b[m\".\n", arg);
            options.is_error = 1;
        }
    }
    return options;
}


void print_help(void) {
    printf(
        "Usage:\n\n"
        "\t./tests [OPTIONS]\n\n"
        "-f, --filter <S>\tFilter tests by the exact match of the given\n"
        "                \tsubstring of the test's name.\n"
        "\n"
        "-s, <S>         \tFilter tests by the exact match of the given\n"
        "--test-suite    \tsubstring of the test case's name.\n"
        "\n\n"
        "Author: Daniel Torres, 2024.\n"
    );
}
