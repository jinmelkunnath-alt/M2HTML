#include "markforge/markforge.h"
#include "markforge/test.h"

void test_devtools_module(void) {
    const char *doc = "# A\n\nText with `code`  \n# A\n";
    mf_profile_report_t profile;
    mf_benchmark_report_t bench;
    mf_lint_result_t lint;
    mf_incremental_cache_t cache;
    mf_config_manager_t cfg;
    mf_renderer_manager_t renderers;
    mf_hook_manager_t hooks;
    mf_local_plugin_manager_t plugins;
    mf_ast_t ast;
    mf_render_output_t out;
    mf_inspection_report_t report;
    int changed = 0;
    char *text;
    MF_TEST_ASSERT_EQ_INT(mf_profile_document("dev.md", doc, "html", &profile), MF_OK);
    MF_TEST_ASSERT(profile.node_count > 0U);
    MF_TEST_ASSERT_EQ_INT(mf_benchmark_run_document("small", doc, "html", 2U, &bench), MF_OK);
    MF_TEST_ASSERT_EQ_INT((int)bench.iterations, 2);
    MF_TEST_ASSERT_EQ_INT(mf_lint_markdown("dev.md", doc, &lint), MF_OK);
    MF_TEST_ASSERT(lint.issue_count >= 1U);
    mf_lint_result_destroy(&lint);
    mf_incremental_cache_init(&cache);
    MF_TEST_ASSERT_EQ_INT(mf_incremental_update(&cache, "dev.md", doc, strlen(doc), &changed), MF_OK);
    MF_TEST_ASSERT_EQ_INT(changed, 1);
    MF_TEST_ASSERT_EQ_INT(mf_incremental_update(&cache, "dev.md", doc, strlen(doc), &changed), MF_OK);
    MF_TEST_ASSERT_EQ_INT(changed, 0);
    mf_incremental_cache_destroy(&cache);
    mf_config_manager_init(&cfg);
    MF_TEST_ASSERT_EQ_INT(mf_config_manager_set_temporary(&cfg, "render.pretty", "false"), MF_OK);
    MF_TEST_ASSERT_STR(mf_config_manager_get(&cfg, "render.pretty", "true"), "false");
    mf_config_manager_destroy(&cfg);
    mf_renderer_manager_init(&renderers);
    mf_renderer_manager_register_builtin(&renderers);
    mf_hook_manager_init(&hooks);
    mf_local_plugin_manager_init(&plugins, &renderers, &hooks, "plugins");
    MF_TEST_ASSERT_EQ_INT(mf_local_plugin_enable(&plugins, "example"), MF_OK);
    MF_TEST_ASSERT(mf_local_plugin_is_enabled(&plugins, "example"));
    mf_local_plugin_manager_destroy(&plugins);
    mf_hook_manager_destroy(&hooks);
    mf_renderer_manager_destroy(&renderers);
    MF_TEST_ASSERT_EQ_INT(mf_parse_string("dev.md", doc, &ast, NULL, NULL), MF_OK);
    mf_render_output_init_memory(&out);
    MF_TEST_ASSERT_EQ_INT(mf_export_document(&ast, "json", &out), MF_OK);
    text = mf_render_output_take(&out);
    MF_TEST_ASSERT(strstr(text, "document") != NULL);
    free(text);
    MF_TEST_ASSERT_EQ_INT(mf_inspect_ast(&ast, &report), MF_OK);
    MF_TEST_ASSERT(strstr(mf_sb_cstr(&report.text), "nodes=") != NULL);
    mf_inspection_report_destroy(&report);
    mf_ast_destroy(&ast);
}
