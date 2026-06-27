#include "markforge/markforge.h"
#include "markforge/test.h"
void test_diagnostics_module(void){ mf_diagnostic_bag_t b; mf_source_region_t r; r.file="x.md"; r.line=1U; r.column=2U; r.length=3U; mf_diag_bag_init(&b); MF_TEST_ASSERT_EQ_INT(mf_diag_add(&b,MF_DIAG_ERROR,1,r,"bad","fix"),MF_OK); MF_TEST_ASSERT_EQ_INT((int)mf_diag_count(&b),1); mf_diag_bag_destroy(&b); }
