#include "markforge/markforge.h"
#include "markforge/test.h"
void test_logging_module(void){ mf_logger_t l; mf_log_init(&l); l.console_enabled=0; l.min_level=MF_LOG_TRACE; MF_LOG_INFO(&l,"%s","ok"); MF_TEST_ASSERT_STR(mf_log_level_name(MF_LOG_WARN),"WARN"); mf_log_close(&l); }
