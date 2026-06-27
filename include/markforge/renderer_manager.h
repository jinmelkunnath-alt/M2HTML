#ifndef MARKFORGE_RENDERER_MANAGER_H
#define MARKFORGE_RENDERER_MANAGER_H
#include "markforge/renderers.h"
#include "markforge/containers.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef mf_result_t (*mf_registered_renderer_fn)(const mf_ast_t *ast, const mf_render_options_t *options, mf_render_output_t *output);

typedef struct mf_renderer_entry_s {
    const char *name;
    mf_render_format_t format;
    int priority;
    mf_registered_renderer_fn render;
} mf_renderer_entry_t;

typedef struct mf_renderer_manager_s {
    mf_vector_t entries;
} mf_renderer_manager_t;

mf_result_t mf_renderer_manager_init(mf_renderer_manager_t *manager);
void mf_renderer_manager_destroy(mf_renderer_manager_t *manager);
mf_result_t mf_renderer_manager_register(mf_renderer_manager_t *manager, const char *name, mf_render_format_t format, int priority, mf_registered_renderer_fn render);
const mf_renderer_entry_t *mf_renderer_manager_find(const mf_renderer_manager_t *manager, const char *name);
mf_result_t mf_renderer_manager_render(const mf_renderer_manager_t *manager, const char *name, const mf_ast_t *ast, const mf_render_options_t *options, mf_render_output_t *output);
mf_result_t mf_renderer_manager_register_builtin(mf_renderer_manager_t *manager);
void mf_renderer_manager_list(const mf_renderer_manager_t *manager, FILE *out);

#ifdef __cplusplus
}
#endif
#endif
