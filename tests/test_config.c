#include "markforge/markforge.h"
#include "markforge/test.h"
void test_config_module(void){ mf_config_t c; mf_parser_options_t o; mf_config_init(&c); MF_TEST_ASSERT_EQ_INT(mf_config_set(&c,"a","b"),MF_OK); MF_TEST_ASSERT_STR(mf_config_get(&c,"a","x"),"b"); mf_parser_options_default(&o); MF_TEST_ASSERT_EQ_INT(o.safe_mode,1); mf_config_destroy(&c); }
