#ifndef MARKFORGE_PLATFORM_H
#define MARKFORGE_PLATFORM_H
#include "markforge/common.h"
#ifdef __cplusplus
extern "C" {
#endif

const char *mf_platform_name(void);
int mf_platform_cpu_count(void);
uint64_t mf_platform_time_ns(void);
const char *mf_platform_home_dir(void);
const char *mf_platform_env(const char *name);
int mf_platform_is_tty(FILE *stream);

#ifdef __cplusplus
}
#endif
#endif
