#include "markforge/markforge.h"
#include "markforge/test.h"
void test_platform_module(void){ MF_TEST_ASSERT(strlen(mf_platform_name())>0U); MF_TEST_ASSERT(mf_platform_cpu_count()>=1); MF_TEST_ASSERT(mf_platform_time_ns()>0U); }
