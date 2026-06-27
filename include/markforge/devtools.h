#ifndef MARKFORGE_DEVTOOLS_H
#define MARKFORGE_DEVTOOLS_H
#include "markforge/parser.h"
#include "markforge/renderers.h"
#include "markforge/renderer_manager.h"
#include "markforge/config_engine.h"
#include "markforge/theme.h"
#include "markforge/plugin.h"
#include "markforge/hooks.h"
#include "markforge/fs.h"
#include "markforge/platform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mf_color_mode_e { MF_COLOR_AUTO = 0, MF_COLOR_ALWAYS, MF_COLOR_NEVER } mf_color_mode_t;

typedef struct mf_cli_global_options_s {
    int verbose;
    int quiet;
    int json;
    mf_color_mode_t color;
    const char *config_path;
} mf_cli_global_options_t;

typedef struct mf_terminal_s {
    FILE *out;
    mf_color_mode_t color;
    int unicode_icons;
    int width;
} mf_terminal_t;

typedef struct mf_progress_s {
    mf_terminal_t *terminal;
    const char *label;
    size_t total;
    size_t current;
} mf_progress_t;

typedef struct mf_profile_report_s {
    double parse_ms;
    double render_ms;
    double format_ms;
    size_t memory_used;
    size_t peak_memory;
    size_t allocation_count;
    size_t node_count;
    size_t token_count;
    size_t file_size;
    size_t characters;
    size_t lines;
    size_t words;
    size_t maximum_nesting;
    double traversal_ms;
} mf_profile_report_t;

typedef struct mf_benchmark_report_s {
    char name[64];
    size_t iterations;
    double total_ms;
    double average_ms;
    size_t bytes_processed;
} mf_benchmark_report_t;

typedef enum mf_lint_code_e {
    MF_LINT_TRAILING_SPACE = 1,
    MF_LINT_DUPLICATE_HEADING,
    MF_LINT_UNCLOSED_INLINE_CODE,
    MF_LINT_MALFORMED_TABLE,
    MF_LINT_WHITESPACE
} mf_lint_code_t;

typedef struct mf_lint_result_s {
    mf_diagnostic_bag_t diagnostics;
    size_t issue_count;
} mf_lint_result_t;

typedef struct mf_incremental_cache_s {
    char *source;
    size_t length;
    uint32_t checksum;
    mf_ast_t ast;
    int has_ast;
} mf_incremental_cache_t;

typedef struct mf_watch_options_s {
    const char *input_path;
    const char *output_path;
    const char *renderer;
    unsigned int interval_ms;
    unsigned int max_cycles;
} mf_watch_options_t;

typedef struct mf_preview_server_s {
    const char *host;
    unsigned short port;
    char *html;
    size_t html_length;
    int running;
} mf_preview_server_t;

typedef struct mf_config_manager_s {
    mf_engine_config_t global_config;
    mf_engine_config_t project_config;
    mf_engine_config_t local_config;
    mf_engine_config_t temporary_config;
    int has_global;
    int has_project;
    int has_local;
    int has_temporary;
} mf_config_manager_t;

typedef struct mf_local_plugin_manager_s {
    mf_plugin_manager_t core;
    mf_hash_set_t enabled;
    mf_hash_set_t disabled;
    char install_root[260];
} mf_local_plugin_manager_t;

typedef struct mf_inspection_report_s {
    mf_string_builder_t text;
} mf_inspection_report_t;

void mf_terminal_init(mf_terminal_t *terminal, FILE *out, mf_color_mode_t color);
void mf_terminal_set_width(mf_terminal_t *terminal, int width);
mf_result_t mf_terminal_panel(mf_terminal_t *terminal, const char *title, const char *body);
mf_result_t mf_terminal_table(mf_terminal_t *terminal, const char **headers, const char ***rows, size_t row_count, size_t col_count);
mf_result_t mf_terminal_tree_ast(mf_terminal_t *terminal, const mf_ast_t *ast);
void mf_progress_start(mf_progress_t *progress, mf_terminal_t *terminal, const char *label, size_t total);
void mf_progress_update(mf_progress_t *progress, size_t current);
void mf_progress_finish(mf_progress_t *progress);

void mf_profile_report_init(mf_profile_report_t *report);
mf_result_t mf_profile_document(const char *filename, const char *source, const char *renderer, mf_profile_report_t *report);
void mf_profile_report_print(const mf_profile_report_t *report, FILE *out, int json);

mf_result_t mf_benchmark_run_document(const char *name, const char *source, const char *renderer, size_t iterations, mf_benchmark_report_t *report);
mf_result_t mf_benchmark_report_write_csv(const mf_benchmark_report_t *reports, size_t count, mf_render_output_t *out);
mf_result_t mf_benchmark_report_write_json(const mf_benchmark_report_t *reports, size_t count, mf_render_output_t *out);
mf_result_t mf_benchmark_report_write_markdown(const mf_benchmark_report_t *reports, size_t count, mf_render_output_t *out);

void mf_lint_result_init(mf_lint_result_t *result);
void mf_lint_result_destroy(mf_lint_result_t *result);
mf_result_t mf_lint_markdown(const char *filename, const char *source, mf_lint_result_t *result);
mf_result_t mf_validate_document_full(const char *filename, const char *source, mf_diagnostic_bag_t *diagnostics);

void mf_incremental_cache_init(mf_incremental_cache_t *cache);
void mf_incremental_cache_destroy(mf_incremental_cache_t *cache);
mf_result_t mf_incremental_update(mf_incremental_cache_t *cache, const char *filename, const char *source, size_t length, int *changed);

mf_result_t mf_watch_run(const mf_watch_options_t *options);
void mf_preview_server_init(mf_preview_server_t *server, unsigned short port);
void mf_preview_server_destroy(mf_preview_server_t *server);
mf_result_t mf_preview_server_set_document(mf_preview_server_t *server, const mf_ast_t *ast);
mf_result_t mf_preview_server_serve_once(mf_preview_server_t *server);

void mf_config_manager_init(mf_config_manager_t *manager);
void mf_config_manager_destroy(mf_config_manager_t *manager);
mf_result_t mf_config_manager_load(mf_config_manager_t *manager, const char *global_path, const char *project_path, const char *local_path);
mf_result_t mf_config_manager_set_temporary(mf_config_manager_t *manager, const char *key, const char *value);
const char *mf_config_manager_get(const mf_config_manager_t *manager, const char *key, const char *fallback);
mf_result_t mf_config_manager_validate(const mf_config_manager_t *manager, mf_diagnostic_bag_t *diagnostics);

void mf_local_plugin_manager_init(mf_local_plugin_manager_t *manager, mf_renderer_manager_t *renderers, mf_hook_manager_t *hooks, const char *root);
void mf_local_plugin_manager_destroy(mf_local_plugin_manager_t *manager);
mf_result_t mf_local_plugin_install(mf_local_plugin_manager_t *manager, const char *manifest_path);
mf_result_t mf_local_plugin_remove(mf_local_plugin_manager_t *manager, const char *name);
mf_result_t mf_local_plugin_enable(mf_local_plugin_manager_t *manager, const char *name);
mf_result_t mf_local_plugin_disable(mf_local_plugin_manager_t *manager, const char *name);
int mf_local_plugin_is_enabled(const mf_local_plugin_manager_t *manager, const char *name);

mf_result_t mf_inspect_tokens(const char *filename, const char *source, mf_inspection_report_t *report);
mf_result_t mf_inspect_ast(const mf_ast_t *ast, mf_inspection_report_t *report);
mf_result_t mf_inspect_performance(const mf_profile_report_t *profile, mf_inspection_report_t *report);
void mf_inspection_report_destroy(mf_inspection_report_t *report);

mf_result_t mf_export_document(const mf_ast_t *ast, const char *kind, mf_render_output_t *out);
mf_result_t mf_release_generate_notes(const char *version, mf_render_output_t *out);
mf_result_t mf_release_generate_checksum(const char *path, char *buffer, size_t buffer_size);

int mf_cli_main(int argc, char **argv);

#ifdef __cplusplus
}
#endif
#endif
