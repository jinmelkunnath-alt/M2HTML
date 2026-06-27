#ifndef MARKFORGE_TEST_H
#define MARKFORGE_TEST_H
#include "markforge/common.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef void (*mf_test_fn)(void);
typedef struct mf_test_case_s { const char *name; mf_test_fn fn; } mf_test_case_t;
typedef struct mf_test_runner_s { size_t passed; size_t failed; } mf_test_runner_t;

void mf_test_runner_init(mf_test_runner_t *runner);
int mf_test_runner_run(mf_test_runner_t *runner, const mf_test_case_t *tests, size_t count);
void mf_test_fail(const char *expr, const char *file, int line);
void *mf_mock_alloc(size_t size);
void mf_mock_free(void *ptr);
size_t mf_mock_allocation_count(void);
void mf_mock_reset(void);

#define MF_TEST_ASSERT(expr) do { if (!(expr)) mf_test_fail(#expr, __FILE__, __LINE__); } while (0)
#define MF_TEST_ASSERT_EQ_INT(a,b) MF_TEST_ASSERT((a)==(b))
#define MF_TEST_ASSERT_STR(a,b) MF_TEST_ASSERT(strcmp((a),(b))==0)

#ifdef __cplusplus
}
#endif
#endif
