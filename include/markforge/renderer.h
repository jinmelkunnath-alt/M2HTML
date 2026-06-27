#ifndef MARKFORGE_RENDERER_H
#define MARKFORGE_RENDERER_H
#include "markforge/ast.h"
#include "markforge/diagnostics.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mf_renderer_s mf_renderer_t;

typedef struct mf_renderer_vtable_s {
    const char *name;
    mf_result_t (*begin_document)(mf_renderer_t *renderer, const mf_ast_t *ast);
    mf_result_t (*enter_node)(mf_renderer_t *renderer, const mf_ast_node_t *node);
    mf_result_t (*leave_node)(mf_renderer_t *renderer, const mf_ast_node_t *node);
    mf_result_t (*end_document)(mf_renderer_t *renderer, const mf_ast_t *ast);
} mf_renderer_vtable_t;

struct mf_renderer_s {
    const mf_renderer_vtable_t *vtable;
    void *ctx;
    mf_diagnostic_bag_t *diagnostics;
};

mf_result_t mf_renderer_render_ast(mf_renderer_t *renderer, const mf_ast_t *ast);

#ifdef __cplusplus
}
#endif
#endif
