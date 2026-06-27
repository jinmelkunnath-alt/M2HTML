#include "markforge/test.h"
static int g_failed; static size_t g_mock_allocs;
void mf_test_runner_init(mf_test_runner_t*runner){ if(runner){runner->passed=0U;runner->failed=0U;} }
int mf_test_runner_run(mf_test_runner_t*runner,const mf_test_case_t*tests,size_t count){ size_t i; if(!runner||!tests) return 1; for(i=0U;i<count;i++){ g_failed=0; tests[i].fn(); if(g_failed){ runner->failed++; printf("FAIL %s\n",tests[i].name); } else { runner->passed++; printf("PASS %s\n",tests[i].name); } } printf("tests: %lu passed, %lu failed\n",(unsigned long)runner->passed,(unsigned long)runner->failed); return runner->failed?1:0; }
void mf_test_fail(const char*expr,const char*file,int line){ fprintf(stderr,"test assertion failed: %s at %s:%d\n",expr,file,line); g_failed=1; }
void*mf_mock_alloc(size_t size){ void*p=malloc(size?size:1U); if(p) g_mock_allocs++; return p; }
void mf_mock_free(void*ptr){ if(ptr){free(ptr); if(g_mock_allocs) g_mock_allocs--; } }
size_t mf_mock_allocation_count(void){ return g_mock_allocs; }
void mf_mock_reset(void){ g_mock_allocs=0U; }
