#include "markforge/markforge.h"
#include "markforge/test.h"

static int hook_seen(mf_hook_event_t e, void *data, void *ctx) { int *seen = (int *)ctx; MF_UNUSED(e); MF_UNUSED(data); *seen = 1; return 0; }

void test_extensibility_module(void) {
    mf_renderer_manager_t rm;
    mf_hook_manager_t hm;
    mf_theme_t theme;
    mf_theme_manager_t tm;
    mf_plugin_manager_t pm;
    int seen = 0;
    MF_TEST_ASSERT_EQ_INT(mf_renderer_manager_init(&rm), MF_OK);
    MF_TEST_ASSERT_EQ_INT(mf_renderer_manager_register_builtin(&rm), MF_OK);
    MF_TEST_ASSERT(mf_renderer_manager_find(&rm, "html") != NULL);
    mf_hook_manager_init(&hm);
    MF_TEST_ASSERT_EQ_INT(mf_hook_register(&hm, MF_HOOK_BEFORE_RENDERING, hook_seen, &seen), MF_OK);
    MF_TEST_ASSERT_EQ_INT(mf_hook_emit(&hm, MF_HOOK_BEFORE_RENDERING, NULL), 0);
    MF_TEST_ASSERT_EQ_INT(seen, 1);
    MF_TEST_ASSERT_EQ_INT(mf_theme_init(&theme, "default"), MF_OK);
    mf_theme_manager_init(&tm);
    MF_TEST_ASSERT_EQ_INT(mf_theme_manager_register(&tm, &theme), MF_OK);
    MF_TEST_ASSERT(mf_theme_manager_find(&tm, "default") != NULL);
    mf_plugin_manager_init(&pm, &rm, &hm);
    MF_TEST_ASSERT_EQ_INT(mf_plugin_load_all(&pm), MF_OK);
    mf_plugin_manager_destroy(&pm);
    mf_theme_manager_destroy(&tm);
    mf_theme_destroy(&theme);
    mf_hook_manager_destroy(&hm);
    mf_renderer_manager_destroy(&rm);
}
