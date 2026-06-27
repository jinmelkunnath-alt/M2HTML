#ifndef MARKFORGE_THEME_H
#define MARKFORGE_THEME_H
#include "markforge/config.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct mf_theme_s { char name[64]; char version[32]; char parent[64]; mf_config_t values; } mf_theme_t;
typedef struct mf_theme_manager_s { mf_hash_map_t themes; } mf_theme_manager_t;

mf_result_t mf_theme_init(mf_theme_t *theme, const char *name);
void mf_theme_destroy(mf_theme_t *theme);
mf_result_t mf_theme_load_file(mf_theme_t *theme, const char *path);
mf_result_t mf_theme_validate(const mf_theme_t *theme);
void mf_theme_manager_init(mf_theme_manager_t *manager);
void mf_theme_manager_destroy(mf_theme_manager_t *manager);
mf_result_t mf_theme_manager_register(mf_theme_manager_t *manager, const mf_theme_t *theme);
const mf_theme_t *mf_theme_manager_find(const mf_theme_manager_t *manager, const char *name);
void mf_theme_manager_list(const mf_theme_manager_t *manager, FILE *out);

#ifdef __cplusplus
}
#endif
#endif
