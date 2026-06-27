#ifndef MARKFORGE_UTIL_H
#define MARKFORGE_UTIL_H
#include "markforge/common.h"
#include "markforge/platform.h"
#ifdef __cplusplus
extern "C" {
#endif

#define MF_ASSERT(expr) do { if (!(expr)) mf_assert_fail(#expr, __FILE__, __func__, __LINE__); } while (0)

typedef struct mf_timer_s { uint64_t start_ns; uint64_t end_ns; } mf_timer_t;
typedef struct mf_benchmark_s { const char *name; double elapsed_ms; size_t iterations; } mf_benchmark_t;

void mf_assert_fail(const char *expr, const char *file, const char *func, int line);
void mf_timer_start(mf_timer_t *timer);
void mf_timer_stop(mf_timer_t *timer);
double mf_timer_elapsed_ms(const mf_timer_t *timer);
void mf_random_seed(unsigned int seed);
uint32_t mf_random_u32(void);
const char *mf_version_string(void);
int mf_terminal_supports_color(FILE *stream);
const char *mf_ansi_color(const char *name);

#ifdef __cplusplus
}
#endif
#endif
