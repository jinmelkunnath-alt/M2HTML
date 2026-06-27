#ifndef MARKFORGE_CONFIG_ENGINE_H
#define MARKFORGE_CONFIG_ENGINE_H
#include "markforge/config.h"
#include "markforge/diagnostics.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct mf_engine_config_s {
    mf_config_t values;
    char version[32];
    char profile[64];
} mf_engine_config_t;

mf_result_t mf_engine_config_init(mf_engine_config_t *cfg);
void mf_engine_config_destroy(mf_engine_config_t *cfg);
mf_result_t mf_engine_config_load(mf_engine_config_t *cfg, const char *path);
mf_result_t mf_engine_config_load_text(mf_engine_config_t *cfg, const char *text, mf_config_format_t format);
mf_result_t mf_engine_config_validate(const mf_engine_config_t *cfg, mf_diagnostic_bag_t *diagnostics);
mf_result_t mf_engine_config_apply_defaults(mf_engine_config_t *cfg);
mf_result_t mf_engine_config_apply_env(mf_engine_config_t *cfg, const char *prefix);
mf_result_t mf_engine_config_migrate(mf_engine_config_t *cfg, const char *target_version);

#ifdef __cplusplus
}
#endif
#endif
