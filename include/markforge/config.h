#ifndef MARKFORGE_CONFIG_H
#define MARKFORGE_CONFIG_H
#include "markforge/common.h"
#include "markforge/containers.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum mf_config_format_e { MF_CONFIG_FORMAT_AUTO, MF_CONFIG_FORMAT_INI, MF_CONFIG_FORMAT_JSON, MF_CONFIG_FORMAT_YAML } mf_config_format_t;
typedef struct mf_config_s { mf_hash_map_t values; char source[260]; } mf_config_t;
typedef struct mf_config_cache_s { mf_hash_map_t configs; } mf_config_cache_t;
typedef struct mf_parser_options_s { int extensions; int safe_mode; int max_nesting; } mf_parser_options_t;

mf_result_t mf_config_init(mf_config_t *cfg);
void mf_config_destroy(mf_config_t *cfg);
mf_result_t mf_config_load_file(mf_config_t *cfg, const char *path, mf_config_format_t format);
mf_result_t mf_config_set(mf_config_t *cfg, const char *key, const char *value);
const char *mf_config_get(const mf_config_t *cfg, const char *key, const char *fallback);
mf_result_t mf_config_validate(const mf_config_t *cfg);
void mf_config_cache_init(mf_config_cache_t *cache);
void mf_config_cache_destroy(mf_config_cache_t *cache);
mf_result_t mf_config_cache_load(mf_config_cache_t *cache, const char *path, mf_config_t **out_config);
void mf_parser_options_default(mf_parser_options_t *options);

#ifdef __cplusplus
}
#endif
#endif
