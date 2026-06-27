#include "markforge/markforge.h"
#include "markforge/test.h"

void test_rendering_module(void) {
    mf_ast_t ast;
    mf_render_output_t out;
    char *html;
    MF_TEST_ASSERT_EQ_INT(mf_parse_string("test.md", "# Title\n\nHello **world**\n", &ast, NULL, NULL), MF_OK);
    MF_TEST_ASSERT(mf_ast_validate(&ast));
    MF_TEST_ASSERT_EQ_INT(mf_render_output_init_memory(&out), MF_OK);
    MF_TEST_ASSERT_EQ_INT(mf_render_html(&ast, NULL, &out), MF_OK);
    html = mf_render_output_take(&out);
    MF_TEST_ASSERT(strstr(html, "<h1>") != NULL);
    MF_TEST_ASSERT(strstr(html, "<strong>world</strong>") != NULL);
    free(html);
    mf_ast_destroy(&ast);
}
