#include "markforge/markforge.h"
#include "markforge/test.h"

static void parse_case(const char *name, const char *doc) {
    mf_ast_t ast;
    mf_render_output_t out;
    char *html;
    MF_TEST_ASSERT_EQ_INT(mf_parse_string(name, doc, &ast, NULL, NULL), MF_OK);
    MF_TEST_ASSERT(mf_ast_validate(&ast));
    MF_TEST_ASSERT_EQ_INT(mf_render_output_init_memory(&out), MF_OK);
    MF_TEST_ASSERT_EQ_INT(mf_render_html(&ast, NULL, &out), MF_OK);
    html = mf_render_output_take(&out);
    MF_TEST_ASSERT(html != NULL);
    MF_TEST_ASSERT(strlen(html) > 0U || strlen(doc) == 0U);
    free(html);
    mf_ast_destroy(&ast);
}

void test_release_quality_module(void) {
    mf_diagnostic_bag_t diagnostics;
    mf_lint_result_t lint;
    mf_benchmark_report_t bench;
    mf_render_output_t out;
    mf_ast_t ast;
    char *report;
    char checksum[32];
    MF_TEST_ASSERT_STR(mf_version_string(), "0.5.0-alpha");
    parse_case("unicode.md", "# Unicode\n\nHello π 世界 😀\n");
    parse_case("malformed.md", "# Bad\n\n`unterminated\n\n[link](x)\n");
    parse_case("large.md", "# Large\n\nParagraph\n\n- item\n- item\n");
    MF_TEST_ASSERT_EQ_INT(mf_validate_document_full("bad.md", "# A  \n# A\n", &diagnostics), MF_OK);
    MF_TEST_ASSERT(mf_diag_count(&diagnostics) >= 1U);
    mf_diag_bag_destroy(&diagnostics);
    MF_TEST_ASSERT_EQ_INT(mf_lint_markdown("lint.md", "# A  \n# A\n", &lint), MF_OK);
    MF_TEST_ASSERT(lint.issue_count >= 1U);
    mf_lint_result_destroy(&lint);
    MF_TEST_ASSERT_EQ_INT(mf_benchmark_run_document("release", "# Bench\n\nText\n", "html", 1U, &bench), MF_OK);
    mf_render_output_init_memory(&out);
    MF_TEST_ASSERT_EQ_INT(mf_benchmark_report_write_json(&bench, 1U, &out), MF_OK);
    report = mf_render_output_take(&out);
    MF_TEST_ASSERT(strstr(report, "release") != NULL);
    free(report);
    MF_TEST_ASSERT_EQ_INT(mf_release_generate_checksum("README.md", checksum, sizeof(checksum)), MF_OK);
    MF_TEST_ASSERT(strlen(checksum) > 0U);
    MF_TEST_ASSERT_EQ_INT(mf_fs_write_file("mf_release_link.md", "[x](javascript:alert(1))\n", 25U), MF_OK);
    MF_TEST_ASSERT_EQ_INT(mf_parse_file("mf_release_link.md", &ast, NULL, NULL), MF_OK);
    mf_render_output_init_memory(&out);
    MF_TEST_ASSERT_EQ_INT(mf_render_html(&ast, NULL, &out), MF_OK);
    report = mf_render_output_take(&out);
    MF_TEST_ASSERT(strstr(report, "#unsafe-url") != NULL);
    free(report);
    mf_ast_destroy(&ast);
    remove("mf_release_link.md");
}
