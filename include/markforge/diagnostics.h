#ifndef MARKFORGE_DIAGNOSTICS_H
#define MARKFORGE_DIAGNOSTICS_H
#include "markforge/common.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum mf_diag_severity_e { MF_DIAG_NOTE, MF_DIAG_WARNING, MF_DIAG_ERROR } mf_diag_severity_t;
typedef struct mf_source_region_s { const char *file; size_t line; size_t column; size_t length; } mf_source_region_t;
typedef struct mf_diagnostic_s { int code; mf_diag_severity_t severity; char message[256]; char suggestion[256]; mf_source_region_t region; } mf_diagnostic_t;
typedef struct mf_diagnostic_bag_s { mf_diagnostic_t *items; size_t count; size_t capacity; } mf_diagnostic_bag_t;

void mf_diag_bag_init(mf_diagnostic_bag_t *bag);
void mf_diag_bag_destroy(mf_diagnostic_bag_t *bag);
mf_result_t mf_diag_add(mf_diagnostic_bag_t *bag, mf_diag_severity_t severity, int code, mf_source_region_t region, const char *message, const char *suggestion);
size_t mf_diag_count(const mf_diagnostic_bag_t *bag);
void mf_diag_print(const mf_diagnostic_bag_t *bag, FILE *out, int color);
const char *mf_diag_severity_name(mf_diag_severity_t severity);

#ifdef __cplusplus
}
#endif
#endif
