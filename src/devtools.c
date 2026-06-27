#include "markforge/devtools.h"
#if MF_PLATFORM_WINDOWS
#include <windows.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

static uint32_t checksum_bytes(const char *s, size_t n) {
    uint32_t h = 2166136261U;
    size_t i;
    for (i = 0U; i < n; ++i) { h ^= (unsigned char)s[i]; h *= 16777619U; }
    return h;
}

static size_t count_lines(const char *s) { size_t n = 0U; if (!s || !*s) return 0U; n = 1U; while (*s) if (*s++ == '\n') n++; return n; }
static size_t count_words(const char *s) { size_t n = 0U; int in_word = 0; while (s && *s) { if (isspace((unsigned char)*s)) in_word = 0; else if (!in_word) { in_word = 1; n++; } s++; } return n; }
static const char *color_code(mf_terminal_t *t, const char *name) { return (t && t->color != MF_COLOR_NEVER) ? mf_ansi_color(name) : ""; }

void mf_terminal_init(mf_terminal_t *t, FILE *out, mf_color_mode_t color) { if (t) { t->out = out ? out : stdout; t->color = color; t->unicode_icons = 1; t->width = 80; } }
void mf_terminal_set_width(mf_terminal_t *t, int width) { if (t && width > 20) t->width = width; }

mf_result_t mf_terminal_panel(mf_terminal_t *t, const char *title, const char *body) {
    int w;
    if (!t) return MF_ERROR_INVALID;
    w = t->width > 10 ? t->width : 80;
    fprintf(t->out, "%s+", color_code(t, "blue"));
    { int i; for (i = 0; i < w - 2; ++i) fputc('-', t->out); }
    fprintf(t->out, "+%s\n| %-*s |\n+", color_code(t, "reset"), w - 4, title ? title : "MarkForge");
    { int i; for (i = 0; i < w - 2; ++i) fputc('-', t->out); }
    fprintf(t->out, "+\n%s\n+", body ? body : "");
    { int i; for (i = 0; i < w - 2; ++i) fputc('-', t->out); }
    fputs("+\n", t->out);
    return MF_OK;
}

mf_result_t mf_terminal_table(mf_terminal_t *t, const char **headers, const char ***rows, size_t row_count, size_t col_count) {
    size_t r, c;
    if (!t || !headers || !rows || col_count == 0U) return MF_ERROR_INVALID;
    for (c = 0U; c < col_count; ++c) fprintf(t->out, "%-18s", headers[c]);
    fputc('\n', t->out);
    for (c = 0U; c < col_count; ++c) fprintf(t->out, "------------------");
    fputc('\n', t->out);
    for (r = 0U; r < row_count; ++r) { for (c = 0U; c < col_count; ++c) fprintf(t->out, "%-18s", rows[r][c]); fputc('\n', t->out); }
    return MF_OK;
}

static void tree_node(FILE *out, const mf_ast_node_t *n, size_t depth) { const mf_ast_node_t *c; size_t i; for (i = 0U; i < depth; ++i) fputs("  ", out); fprintf(out, "- %s #%lu\n", mf_ast_node_type_name(n->type), (unsigned long)n->id); for (c = n->first_child; c; c = c->next_sibling) tree_node(out, c, depth + 1U); }
mf_result_t mf_terminal_tree_ast(mf_terminal_t *t, const mf_ast_t *ast) { if (!t || !ast || !ast->root) return MF_ERROR_INVALID; tree_node(t->out, ast->root, 0U); return MF_OK; }

void mf_progress_start(mf_progress_t *p, mf_terminal_t *t, const char *label, size_t total) { if (p) { p->terminal = t; p->label = label; p->total = total ? total : 1U; p->current = 0U; mf_progress_update(p, 0U); } }
void mf_progress_update(mf_progress_t *p, size_t current) { size_t filled, i; if (!p || !p->terminal) return; p->current = current; filled = current * 20U / p->total; fprintf(p->terminal->out, "\r%s [", p->label ? p->label : "progress"); for (i = 0U; i < 20U; ++i) fputc(i < filled ? '#' : '-', p->terminal->out); fprintf(p->terminal->out, "] %lu/%lu", (unsigned long)current, (unsigned long)p->total); fflush(p->terminal->out); }
void mf_progress_finish(mf_progress_t *p) { if (p) { mf_progress_update(p, p->total); fputc('\n', p->terminal ? p->terminal->out : stdout); } }

void mf_profile_report_init(mf_profile_report_t *r) { if (r) memset(r, 0, sizeof(*r)); }

mf_result_t mf_profile_document(const char *filename, const char *source, const char *renderer, mf_profile_report_t *report) {
    mf_timer_t t;
    mf_ast_t ast;
    mf_render_output_t out;
    mf_ast_stats_t st;
    char *rendered;
    if (!source || !report) return MF_ERROR_INVALID;
    mf_profile_report_init(report);
    report->file_size = strlen(source);
    report->characters = report->file_size;
    report->lines = count_lines(source);
    report->words = count_words(source);
    mf_timer_start(&t);
    if (mf_parse_string(filename ? filename : "<input>", source, &ast, NULL, NULL) != MF_OK) return MF_ERROR;
    mf_timer_stop(&t);
    report->parse_ms = mf_timer_elapsed_ms(&t);
    st = mf_ast_statistics(&ast);
    report->node_count = st.node_count;
    report->maximum_nesting = st.max_depth;
    report->memory_used = mf_arena_used(&ast.arena);
    report->peak_memory = report->memory_used;
    report->token_count = 0U;
    mf_timer_start(&t);
    mf_render_output_init_memory(&out);
    if (renderer && strcmp(renderer, "json") == 0) (void)mf_render_json(&ast, NULL, &out, NULL);
    else if (renderer && strcmp(renderer, "xml") == 0) (void)mf_render_xml(&ast, NULL, &out);
    else if (renderer && strcmp(renderer, "text") == 0) (void)mf_render_text(&ast, NULL, &out);
    else (void)mf_render_html(&ast, NULL, &out);
    rendered = mf_render_output_take(&out);
    free(rendered);
    mf_timer_stop(&t);
    report->render_ms = mf_timer_elapsed_ms(&t);
    mf_timer_start(&t);
    mf_timer_stop(&t);
    report->traversal_ms = mf_timer_elapsed_ms(&t);
    mf_ast_destroy(&ast);
    return MF_OK;
}

void mf_profile_report_print(const mf_profile_report_t *r, FILE *out, int json) {
    if (!r) return;
    if (!out) out = stdout;
    if (json) fprintf(out, "{\"parse_ms\":%.3f,\"render_ms\":%.3f,\"memory\":%lu,\"nodes\":%lu,\"lines\":%lu,\"words\":%lu}\n", r->parse_ms, r->render_ms, (unsigned long)r->memory_used, (unsigned long)r->node_count, (unsigned long)r->lines, (unsigned long)r->words);
    else fprintf(out, "Parse: %.3f ms\nRender: %.3f ms\nMemory: %lu bytes\nNodes: %lu\nLines: %lu\nWords: %lu\nMax nesting: %lu\n", r->parse_ms, r->render_ms, (unsigned long)r->memory_used, (unsigned long)r->node_count, (unsigned long)r->lines, (unsigned long)r->words, (unsigned long)r->maximum_nesting);
}

mf_result_t mf_benchmark_run_document(const char *name, const char *source, const char *renderer, size_t iterations, mf_benchmark_report_t *report) {
    mf_timer_t t;
    size_t i;
    mf_profile_report_t pr;
    if (!source || !report || iterations == 0U) return MF_ERROR_INVALID;
    memset(report, 0, sizeof(*report));
    snprintf(report->name, sizeof(report->name), "%s", name ? name : "document");
    report->iterations = iterations;
    report->bytes_processed = strlen(source) * iterations;
    mf_timer_start(&t);
    for (i = 0U; i < iterations; ++i) (void)mf_profile_document("bench.md", source, renderer, &pr);
    mf_timer_stop(&t);
    report->total_ms = mf_timer_elapsed_ms(&t);
    report->average_ms = report->total_ms / (double)iterations;
    return MF_OK;
}

mf_result_t mf_benchmark_report_write_csv(const mf_benchmark_report_t *reports, size_t count, mf_render_output_t *out) { size_t i; char b[160]; if (!reports || !out) return MF_ERROR_INVALID; mf_render_output_write(out, "name,iterations,total_ms,average_ms,bytes\n"); for (i = 0U; i < count; ++i) { snprintf(b, sizeof(b), "%s,%lu,%.3f,%.6f,%lu\n", reports[i].name, (unsigned long)reports[i].iterations, reports[i].total_ms, reports[i].average_ms, (unsigned long)reports[i].bytes_processed); mf_render_output_write(out, b); } return MF_OK; }
mf_result_t mf_benchmark_report_write_json(const mf_benchmark_report_t *reports, size_t count, mf_render_output_t *out) { size_t i; char b[192]; if (!reports || !out) return MF_ERROR_INVALID; mf_render_output_write(out, "["); for (i = 0U; i < count; ++i) { snprintf(b, sizeof(b), "%s{\"name\":\"%s\",\"iterations\":%lu,\"total_ms\":%.3f,\"average_ms\":%.6f}", i ? "," : "", reports[i].name, (unsigned long)reports[i].iterations, reports[i].total_ms, reports[i].average_ms); mf_render_output_write(out, b); } return mf_render_output_write(out, "]\n"); }
mf_result_t mf_benchmark_report_write_markdown(const mf_benchmark_report_t *reports, size_t count, mf_render_output_t *out) { size_t i; char b[192]; if (!reports || !out) return MF_ERROR_INVALID; mf_render_output_write(out, "| Benchmark | Iterations | Total ms | Avg ms |\n|---|---:|---:|---:|\n"); for (i = 0U; i < count; ++i) { snprintf(b, sizeof(b), "| %s | %lu | %.3f | %.6f |\n", reports[i].name, (unsigned long)reports[i].iterations, reports[i].total_ms, reports[i].average_ms); mf_render_output_write(out, b); } return MF_OK; }

void mf_lint_result_init(mf_lint_result_t *r) { if (r) { mf_diag_bag_init(&r->diagnostics); r->issue_count = 0U; } }
void mf_lint_result_destroy(mf_lint_result_t *r) { if (r) { mf_diag_bag_destroy(&r->diagnostics); r->issue_count = 0U; } }

mf_result_t mf_lint_markdown(const char *filename, const char *source, mf_lint_result_t *result) {
    size_t line = 1U, col = 1U, i = 0U;
    mf_hash_set_t headings;
    char hbuf[256];
    if (!source || !result) return MF_ERROR_INVALID;
    mf_lint_result_init(result);
    mf_hash_set_init(&headings);
    while (source[i]) {
        size_t start = i, n;
        while (source[i] && source[i] != '\n' && source[i] != '\r') i++;
        n = i - start;
        if (n > 0U && (source[start + n - 1U] == ' ' || source[start + n - 1U] == '\t')) { mf_source_region_t r = { filename ? filename : "<input>", line, n, 1U }; mf_diag_add(&result->diagnostics, MF_DIAG_WARNING, 4001, r, "trailing whitespace", "remove trailing spaces"); result->issue_count++; }
        if (n > 1U && source[start] == '#') { size_t k = 0U; while (k < n && k < sizeof(hbuf) - 1U) { hbuf[k] = (char)tolower((unsigned char)source[start + k]); k++; } hbuf[k] = '\0'; if (mf_hash_set_contains(&headings, hbuf)) { mf_source_region_t r = { filename ? filename : "<input>", line, 1U, n }; mf_diag_add(&result->diagnostics, MF_DIAG_WARNING, 4002, r, "duplicate heading", "make the heading unique"); result->issue_count++; } else mf_hash_set_add(&headings, hbuf); }
        if (n > 0U) { size_t ticks = 0U, j; for (j = start; j < start + n; ++j) if (source[j] == '`') ticks++; if ((ticks & 1U) != 0U) { mf_source_region_t r = { filename ? filename : "<input>", line, col, n }; mf_diag_add(&result->diagnostics, MF_DIAG_WARNING, 4003, r, "unclosed inline code span", "add a closing backtick"); result->issue_count++; } }
        if (source[i] == '\r') i++;
        if (source[i] == '\n') i++;
        line++;
    }
    mf_hash_set_destroy(&headings);
    return MF_OK;
}

mf_result_t mf_validate_document_full(const char *filename, const char *source, mf_diagnostic_bag_t *diagnostics) {
    mf_ast_t ast;
    mf_lint_result_t lint;
    mf_result_t r;
    if (!source || !diagnostics) return MF_ERROR_INVALID;
    mf_diag_bag_init(diagnostics);
    r = mf_parse_string(filename, source, &ast, diagnostics, NULL);
    if (r == MF_OK) mf_ast_destroy(&ast);
    mf_lint_markdown(filename, source, &lint);
    { size_t i; for (i = 0U; i < lint.diagnostics.count; ++i) { mf_diagnostic_t *d = &lint.diagnostics.items[i]; mf_diag_add(diagnostics, d->severity, d->code, d->region, d->message, d->suggestion); } }
    mf_lint_result_destroy(&lint);
    return r;
}

void mf_incremental_cache_init(mf_incremental_cache_t *c) { if (c) memset(c, 0, sizeof(*c)); }
void mf_incremental_cache_destroy(mf_incremental_cache_t *c) { if (c) { free(c->source); if (c->has_ast) mf_ast_destroy(&c->ast); memset(c, 0, sizeof(*c)); } }
mf_result_t mf_incremental_update(mf_incremental_cache_t *c, const char *filename, const char *source, size_t length, int *changed) { uint32_t sum; char *copy; if (!c || !source) return MF_ERROR_INVALID; sum = checksum_bytes(source, length); if (changed) *changed = (sum != c->checksum); if (sum == c->checksum && c->has_ast) return MF_OK; copy = (char *)malloc(length + 1U); if (!copy) return MF_ERROR_NOMEM; memcpy(copy, source, length); copy[length] = '\0'; if (c->has_ast) mf_ast_destroy(&c->ast); if (mf_parse_string(filename, copy, &c->ast, NULL, NULL) != MF_OK) { free(copy); return MF_ERROR; } free(c->source); c->source = copy; c->length = length; c->checksum = sum; c->has_ast = 1; return MF_OK; }

mf_result_t mf_watch_run(const mf_watch_options_t *o) { char *data; size_t len; mf_incremental_cache_t cache; int changed; unsigned int cycles, maxc; if (!o || !o->input_path) return MF_ERROR_INVALID; maxc = o->max_cycles ? o->max_cycles : 1U; mf_incremental_cache_init(&cache); for (cycles = 0U; cycles < maxc; ++cycles) { if (mf_fs_read_file(o->input_path, &data, &len) != MF_OK) { mf_incremental_cache_destroy(&cache); return MF_ERROR_IO; } if (mf_incremental_update(&cache, o->input_path, data, len, &changed) == MF_OK && changed && o->output_path) { mf_render_output_t out; char *rendered; mf_render_output_init_memory(&out); if (o->renderer && strcmp(o->renderer, "json") == 0) mf_render_json(&cache.ast, NULL, &out, NULL); else mf_render_html(&cache.ast, NULL, &out); rendered = mf_render_output_take(&out); if (rendered) { (void)mf_fs_write_file(o->output_path, rendered, strlen(rendered)); free(rendered); } } free(data); } mf_incremental_cache_destroy(&cache); return MF_OK; }

void mf_preview_server_init(mf_preview_server_t *s, unsigned short port) { if (s) { memset(s, 0, sizeof(*s)); s->host = "127.0.0.1"; s->port = port ? port : 8080U; } }
void mf_preview_server_destroy(mf_preview_server_t *s) { if (s) { free(s->html); memset(s, 0, sizeof(*s)); } }
mf_result_t mf_preview_server_set_document(mf_preview_server_t *s, const mf_ast_t *ast) { mf_render_output_t out; char *html; if (!s || !ast) return MF_ERROR_INVALID; mf_render_output_init_memory(&out); if (mf_render_html(ast, NULL, &out) != MF_OK) return MF_ERROR; html = mf_render_output_take(&out); if (!html) return MF_ERROR_NOMEM; free(s->html); s->html = html; s->html_length = strlen(html); return MF_OK; }
mf_result_t mf_preview_server_serve_once(mf_preview_server_t *s) {
#if MF_PLATFORM_WINDOWS
    MF_UNUSED(s);
    return MF_ERROR;
#else
    int fd, client;
    struct sockaddr_in addr;
    char header[256];
    if (!s) return MF_ERROR_INVALID;
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) return MF_ERROR_IO;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(s->port);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) != 0) { close(fd); return MF_ERROR_IO; }
    if (listen(fd, 1) != 0) { close(fd); return MF_ERROR_IO; }
    s->running = 1;
    client = accept(fd, NULL, NULL);
    if (client >= 0) {
        snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\nCache-Control: no-store\r\nContent-Length: %lu\r\n\r\n", (unsigned long)(s->html ? s->html_length : 0U));
        (void)send(client, header, strlen(header), 0);
        if (s->html) (void)send(client, s->html, s->html_length, 0);
        close(client);
    }
    close(fd);
    s->running = 0;
    return client >= 0 ? MF_OK : MF_ERROR_IO;
#endif
}

void mf_config_manager_init(mf_config_manager_t *m) { if (m) memset(m, 0, sizeof(*m)); }
void mf_config_manager_destroy(mf_config_manager_t *m) { if (!m) return; if (m->has_global) mf_engine_config_destroy(&m->global_config); if (m->has_project) mf_engine_config_destroy(&m->project_config); if (m->has_local) mf_engine_config_destroy(&m->local_config); if (m->has_temporary) mf_engine_config_destroy(&m->temporary_config); memset(m, 0, sizeof(*m)); }
static mf_result_t load_one(mf_engine_config_t *c, const char *p, int *has) { if (!p) return MF_OK; if (mf_engine_config_init(c) != MF_OK) return MF_ERROR_NOMEM; *has = 1; return mf_engine_config_load(c, p); }
mf_result_t mf_config_manager_load(mf_config_manager_t *m, const char *g, const char *p, const char *l) { mf_result_t r; if (!m) return MF_ERROR_INVALID; mf_config_manager_init(m); if ((r = load_one(&m->global_config, g, &m->has_global)) != MF_OK) return r; if ((r = load_one(&m->project_config, p, &m->has_project)) != MF_OK) return r; return load_one(&m->local_config, l, &m->has_local); }
mf_result_t mf_config_manager_set_temporary(mf_config_manager_t *m, const char *key, const char *value) { if (!m || !key || !value) return MF_ERROR_INVALID; if (!m->has_temporary) { if (mf_engine_config_init(&m->temporary_config) != MF_OK) return MF_ERROR_NOMEM; m->has_temporary = 1; } return mf_config_set(&m->temporary_config.values, key, value); }
const char *mf_config_manager_get(const mf_config_manager_t *m, const char *key, const char *fallback) { const char *v; if (!m || !key) return fallback; if (m->has_temporary && (v = mf_config_get(&m->temporary_config.values, key, NULL)) != NULL) return v; if (m->has_local && (v = mf_config_get(&m->local_config.values, key, NULL)) != NULL) return v; if (m->has_project && (v = mf_config_get(&m->project_config.values, key, NULL)) != NULL) return v; if (m->has_global && (v = mf_config_get(&m->global_config.values, key, NULL)) != NULL) return v; return fallback; }
mf_result_t mf_config_manager_validate(const mf_config_manager_t *m, mf_diagnostic_bag_t *d) { if (!m) return MF_ERROR_INVALID; if (m->has_global) (void)mf_engine_config_validate(&m->global_config, d); if (m->has_project) (void)mf_engine_config_validate(&m->project_config, d); if (m->has_local) (void)mf_engine_config_validate(&m->local_config, d); if (m->has_temporary) (void)mf_engine_config_validate(&m->temporary_config, d); return MF_OK; }

void mf_local_plugin_manager_init(mf_local_plugin_manager_t *m, mf_renderer_manager_t *r, mf_hook_manager_t *h, const char *root) { if (m) { memset(m,0,sizeof(*m)); mf_plugin_manager_init(&m->core, r, h); mf_hash_set_init(&m->enabled); mf_hash_set_init(&m->disabled); snprintf(m->install_root, sizeof(m->install_root), "%s", root ? root : "plugins"); } }
void mf_local_plugin_manager_destroy(mf_local_plugin_manager_t *m) { if (m) { mf_plugin_manager_destroy(&m->core); mf_hash_set_destroy(&m->enabled); mf_hash_set_destroy(&m->disabled); memset(m,0,sizeof(*m)); } }
mf_result_t mf_local_plugin_install(mf_local_plugin_manager_t *m, const char *manifest_path) { if (!m || !manifest_path) return MF_ERROR_INVALID; return mf_plugin_discover_manifest(&m->core, manifest_path); }
mf_result_t mf_local_plugin_remove(mf_local_plugin_manager_t *m, const char *name) { if (!m || !name) return MF_ERROR_INVALID; (void)mf_hash_set_remove(&m->enabled, name); (void)mf_hash_set_remove(&m->disabled, name); return MF_OK; }
mf_result_t mf_local_plugin_enable(mf_local_plugin_manager_t *m, const char *name) { if (!m || !name) return MF_ERROR_INVALID; (void)mf_hash_set_remove(&m->disabled, name); return mf_hash_set_add(&m->enabled, name); }
mf_result_t mf_local_plugin_disable(mf_local_plugin_manager_t *m, const char *name) { if (!m || !name) return MF_ERROR_INVALID; (void)mf_hash_set_remove(&m->enabled, name); return mf_hash_set_add(&m->disabled, name); }
int mf_local_plugin_is_enabled(const mf_local_plugin_manager_t *m, const char *name) { return m && name && mf_hash_set_contains(&m->enabled, name) && !mf_hash_set_contains(&m->disabled, name); }

mf_result_t mf_inspect_tokens(const char *filename, const char *source, mf_inspection_report_t *r) { mf_token_stream_t s; size_t i; char b[256]; if (!source || !r) return MF_ERROR_INVALID; mf_sb_init(&r->text); mf_token_stream_init(&s); mf_token_stream_from_source(&s, filename, source, strlen(source), NULL); for (i = 0U; i < s.count; ++i) { snprintf(b, sizeof(b), "%s %lu:%lu %lu..%lu\n", mf_token_type_name(s.tokens[i].type), (unsigned long)s.tokens[i].line, (unsigned long)s.tokens[i].column, (unsigned long)s.tokens[i].start_offset, (unsigned long)s.tokens[i].end_offset); mf_sb_append(&r->text, b); } mf_token_stream_destroy(&s); return MF_OK; }
mf_result_t mf_inspect_ast(const mf_ast_t *ast, mf_inspection_report_t *r) { if (!ast || !r) return MF_ERROR_INVALID; mf_sb_init(&r->text); { mf_ast_stats_t s = mf_ast_statistics(ast); char b[128]; snprintf(b, sizeof(b), "nodes=%lu depth=%lu text=%lu errors=%lu\n", (unsigned long)s.node_count, (unsigned long)s.max_depth, (unsigned long)s.text_bytes, (unsigned long)s.error_nodes); mf_sb_append(&r->text, b); } return MF_OK; }
mf_result_t mf_inspect_performance(const mf_profile_report_t *p, mf_inspection_report_t *r) { char b[256]; if (!p || !r) return MF_ERROR_INVALID; mf_sb_init(&r->text); snprintf(b, sizeof(b), "parse=%.3f render=%.3f memory=%lu nodes=%lu\n", p->parse_ms, p->render_ms, (unsigned long)p->memory_used, (unsigned long)p->node_count); return mf_sb_append(&r->text, b); }
void mf_inspection_report_destroy(mf_inspection_report_t *r) { if (r) mf_sb_destroy(&r->text); }

mf_result_t mf_export_document(const mf_ast_t *ast, const char *kind, mf_render_output_t *out) { if (!ast || !kind || !out) return MF_ERROR_INVALID; if (strcmp(kind,"html")==0) return mf_render_html(ast,NULL,out); if (strcmp(kind,"json")==0) return mf_render_json(ast,NULL,out,NULL); if (strcmp(kind,"xml")==0) return mf_render_xml(ast,NULL,out); if (strcmp(kind,"text")==0) return mf_render_text(ast,NULL,out); if (strcmp(kind,"markdown")==0) return mf_format_markdown(ast,NULL,out); if (strcmp(kind,"stats")==0) { mf_ast_stats_t s = mf_ast_statistics(ast); char b[128]; snprintf(b,sizeof(b),"nodes=%lu depth=%lu\n",(unsigned long)s.node_count,(unsigned long)s.max_depth); return mf_render_output_write(out,b); } return MF_ERROR_NOT_FOUND; }
mf_result_t mf_release_generate_notes(const char *version, mf_render_output_t *out) { if (!out) return MF_ERROR_INVALID; mf_render_output_write(out, "# MarkForge "); mf_render_output_write(out, version ? version : MF_VERSION_STRING); return mf_render_output_write(out, "\n\nSelf-contained offline Markdown engine release.\n"); }
mf_result_t mf_release_generate_checksum(const char *path, char *buffer, size_t buffer_size) { char *data; size_t len; uint32_t sum; if (!path || !buffer) return MF_ERROR_INVALID; if (mf_fs_read_file(path, &data, &len) != MF_OK) return MF_ERROR_IO; sum = checksum_bytes(data, len); free(data); if (snprintf(buffer, buffer_size, "%08lx", (unsigned long)sum) >= (int)buffer_size) return MF_ERROR_OVERFLOW; return MF_OK; }
