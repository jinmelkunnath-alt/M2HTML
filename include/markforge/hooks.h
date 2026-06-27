#ifndef MARKFORGE_HOOKS_H
#define MARKFORGE_HOOKS_H
#include "markforge/common.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum mf_hook_event_e { MF_HOOK_BEFORE_PARSING, MF_HOOK_AFTER_PARSING, MF_HOOK_BEFORE_RENDERING, MF_HOOK_AFTER_RENDERING, MF_HOOK_CONFIGURATION_LOADED, MF_HOOK_PLUGIN_LOADED, MF_HOOK_PLUGIN_UNLOADED, MF_HOOK_RENDERER_SELECTED, MF_HOOK_FORMATTER_STARTED, MF_HOOK_FORMATTER_FINISHED } mf_hook_event_t;
typedef int (*mf_hook_fn)(mf_hook_event_t event, void *event_data, void *ctx);
typedef struct mf_hook_entry_s { mf_hook_event_t event; mf_hook_fn fn; void *ctx; } mf_hook_entry_t;
typedef struct mf_hook_manager_s { mf_hook_entry_t *entries; size_t count; size_t capacity; } mf_hook_manager_t;

void mf_hook_manager_init(mf_hook_manager_t *manager);
void mf_hook_manager_destroy(mf_hook_manager_t *manager);
mf_result_t mf_hook_register(mf_hook_manager_t *manager, mf_hook_event_t event, mf_hook_fn fn, void *ctx);
int mf_hook_emit(mf_hook_manager_t *manager, mf_hook_event_t event, void *event_data);
const char *mf_hook_event_name(mf_hook_event_t event);

#ifdef __cplusplus
}
#endif
#endif
