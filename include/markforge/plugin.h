#ifndef MARKFORGE_PLUGIN_H
#define MARKFORGE_PLUGIN_H
#include "markforge/renderer_manager.h"
#include "markforge/hooks.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct mf_plugin_metadata_s { char name[64]; char version[32]; char api_version[32]; char description[160]; char dependencies[256]; } mf_plugin_metadata_t;
typedef struct mf_plugin_context_s { mf_renderer_manager_t *renderers; mf_hook_manager_t *hooks; int sandbox_mode; } mf_plugin_context_t;
typedef mf_result_t (*mf_plugin_init_fn)(mf_plugin_context_t *ctx);
typedef void (*mf_plugin_shutdown_fn)(mf_plugin_context_t *ctx);
typedef struct mf_plugin_s { mf_plugin_metadata_t metadata; mf_plugin_init_fn init; mf_plugin_shutdown_fn shutdown; int loaded; } mf_plugin_t;
typedef struct mf_plugin_manager_s { mf_vector_t plugins; mf_plugin_context_t context; } mf_plugin_manager_t;

void mf_plugin_metadata_init(mf_plugin_metadata_t *metadata, const char *name, const char *version, const char *description);
int mf_plugin_is_compatible(const mf_plugin_metadata_t *metadata);
void mf_plugin_manager_init(mf_plugin_manager_t *manager, mf_renderer_manager_t *renderers, mf_hook_manager_t *hooks);
void mf_plugin_manager_destroy(mf_plugin_manager_t *manager);
mf_result_t mf_plugin_register(mf_plugin_manager_t *manager, const mf_plugin_t *plugin);
mf_result_t mf_plugin_load_all(mf_plugin_manager_t *manager);
void mf_plugin_unload_all(mf_plugin_manager_t *manager);
void mf_plugin_manager_list(const mf_plugin_manager_t *manager, FILE *out);
mf_result_t mf_plugin_discover_manifest(mf_plugin_manager_t *manager, const char *path);

#ifdef __cplusplus
}
#endif
#endif
