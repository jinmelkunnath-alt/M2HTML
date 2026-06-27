#ifndef MARKFORGE_RENDERERS_H
#define MARKFORGE_RENDERERS_H
#include "markforge/renderer.h"
#include "markforge/containers.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mf_render_format_e { MF_RENDER_HTML, MF_RENDER_JSON, MF_RENDER_XML, MF_RENDER_TEXT, MF_RENDER_MARKDOWN } mf_render_format_t;

typedef struct mf_render_options_s {
    int pretty;
    int compact;
    int minify;
    int safe_html;
    int raw_html;
    int normalize_whitespace;
    int collapse_whitespace;
    int preserve_formatting;
    int indent_width;
    const char *css_prefix;
} mf_render_options_t;

typedef struct mf_render_output_s {
    mf_string_builder_t builder;
    FILE *stream;
} mf_render_output_t;

void mf_render_options_default(mf_render_options_t *options);
mf_result_t mf_render_output_init_memory(mf_render_output_t *output);
void mf_render_output_init_stream(mf_render_output_t *output, FILE *stream);
void mf_render_output_destroy(mf_render_output_t *output);
mf_result_t mf_render_output_write(mf_render_output_t *output, const char *text);
mf_result_t mf_render_output_write_n(mf_render_output_t *output, const char *text, size_t len);
char *mf_render_output_take(mf_render_output_t *output);

mf_result_t mf_render_html(const mf_ast_t *ast, const mf_render_options_t *options, mf_render_output_t *output);
mf_result_t mf_render_json(const mf_ast_t *ast, const mf_render_options_t *options, mf_render_output_t *output, const mf_diagnostic_bag_t *diagnostics);
mf_result_t mf_render_xml(const mf_ast_t *ast, const mf_render_options_t *options, mf_render_output_t *output);
mf_result_t mf_render_text(const mf_ast_t *ast, const mf_render_options_t *options, mf_render_output_t *output);
mf_result_t mf_format_markdown(const mf_ast_t *ast, const mf_render_options_t *options, mf_render_output_t *output);

#ifdef __cplusplus
}
#endif
#endif
