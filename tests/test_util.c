#include "markforge/markforge.h"
#include "markforge/test.h"
void test_util_module(void){ mf_timer_t t; mf_timer_start(&t); mf_timer_stop(&t); MF_TEST_ASSERT(mf_timer_elapsed_ms(&t)>=0.0); MF_TEST_ASSERT_STR(mf_version_string(),MF_VERSION_STRING); }
