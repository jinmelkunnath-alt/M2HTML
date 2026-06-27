#include "markforge/renderer.h"

static mf_result_t render_node(mf_renderer_t *r, const mf_ast_node_t *n) {
    const mf_ast_node_t *c;
    mf_result_t res;
    if (!r || !n || !r->vtable) return MF_ERROR_INVALID;
    if (r->vtable->enter_node) { res = r->vtable->enter_node(r, n); if (res != MF_OK) return res; }
    for (c = n->first_child; c; c = c->next_sibling) { res = render_node(r, c); if (res != MF_OK) return res; }
    if (r->vtable->leave_node) { res = r->vtable->leave_node(r, n); if (res != MF_OK) return res; }
    return MF_OK;
}

mf_result_t mf_renderer_render_ast(mf_renderer_t *renderer, const mf_ast_t *ast) {
    mf_result_t r;
    if (!renderer || !ast || !ast->root) return MF_ERROR_INVALID;
    if (renderer->vtable->begin_document) { r = renderer->vtable->begin_document(renderer, ast); if (r != MF_OK) return r; }
    r = render_node(renderer, ast->root);
    if (r != MF_OK) return r;
    if (renderer->vtable->end_document) return renderer->vtable->end_document(renderer, ast);
    return MF_OK;
}
