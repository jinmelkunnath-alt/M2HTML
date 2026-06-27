#include "markforge/parser.h"
#include "markforge/fs.h"

static int starts_with(const char *s, const char *p) { return strncmp(s, p, strlen(p)) == 0; }
static size_t line_len(const char *s) { size_t n = 0U; while (s[n] && s[n] != '\n' && s[n] != '\r') n++; return n; }
static mf_string_view_t trim_view(const char *s, size_t n) { return mf_sv_trim(mf_sv(s, n)); }

static mf_result_t ast_set_meta(mf_ast_t *ast, mf_ast_node_t *node, const char *data, size_t len) {
    char *copy;
    if (!ast || !node) return MF_ERROR_INVALID;
    if (!data || len == 0U) { node->meta = mf_sv("", 0U); return MF_OK; }
    copy = (char *)mf_arena_alloc(&ast->arena, len + 1U, 1U);
    if (!copy) return MF_ERROR_NOMEM;
    memcpy(copy, data, len);
    copy[len] = '\0';
    node->meta = mf_sv(copy, len);
    return MF_OK;
}

static void parser_validate_token_stream(mf_parser_t *p) {
    size_t i;
    int backtick_open = 0;
    const mf_token_t *open = NULL;
    for (i = 0U; p && i < p->tokens.count; ++i) {
        const mf_token_t *tok = &p->tokens.tokens[i];
        if (tok->type == MF_TOKEN_BACKTICK) {
            if (!backtick_open) { backtick_open = 1; open = tok; }
            else { backtick_open = 0; open = NULL; }
        }
        if (tok->type == MF_TOKEN_NEWLINE && backtick_open && open) {
            mf_source_region_t r;
            r.file = p->filename;
            r.line = open->line;
            r.column = open->column;
            r.length = 1U;
            (void)mf_diag_add(&p->diagnostics, MF_DIAG_WARNING, 2101, r, "unclosed inline code span", "add a closing backtick before the end of the line");
            backtick_open = 0;
            open = NULL;
        }
    }
    if (backtick_open && open) {
        mf_source_region_t r;
        r.file = p->filename;
        r.line = open->line;
        r.column = open->column;
        r.length = 1U;
        (void)mf_diag_add(&p->diagnostics, MF_DIAG_WARNING, 2101, r, "unclosed inline code span", "add a closing backtick");
    }
}

void mf_parse_options_default(mf_parse_options_t *options) {
    if (!options) return;
    mf_parser_options_default(&options->base);
    options->trace = 0;
    options->recover = 1;
    options->arena_capacity = 1024U * 1024U;
}

mf_result_t mf_parser_init(mf_parser_t *p, const char *filename, const char *source, size_t length, const mf_parse_options_t *options) {
    mf_parse_options_t def;
    if (!p || !source) return MF_ERROR_INVALID;
    memset(p, 0, sizeof(*p));
    p->filename = filename ? filename : "<input>";
    p->source = source;
    p->length = length;
    mf_diag_bag_init(&p->diagnostics);
    mf_parse_options_default(&def);
    p->options = options ? *options : def;
    if (mf_token_stream_init(&p->tokens) != MF_OK) return MF_ERROR_NOMEM;
    if (mf_token_stream_from_source(&p->tokens, p->filename, p->source, p->length, &p->diagnostics) != MF_OK) return MF_ERROR_NOMEM;
    parser_validate_token_stream(p);
    p->stats.token_count = p->tokens.count;
    return MF_OK;
}

void mf_parser_destroy(mf_parser_t *p) {
    if (!p) return;
    mf_token_stream_destroy(&p->tokens);
    mf_diag_bag_destroy(&p->diagnostics);
    memset(p, 0, sizeof(*p));
}

void mf_parser_trace_set(mf_parser_t *p, FILE *out) { if (p) p->trace_out = out; }
const mf_parse_stats_t *mf_parser_stats(const mf_parser_t *p) { return p ? &p->stats : NULL; }

static mf_ast_node_t *add_text_child(mf_ast_t *ast, mf_ast_node_t *parent, mf_ast_node_type_t type, const char *s, size_t n, size_t line) {
    mf_ast_node_t *node = mf_ast_create_node(ast, type, trim_view(s, n), line, 1U);
    if (node) (void)mf_ast_append_child(parent, node);
    return node;
}

static void parse_inline(mf_ast_t *ast, mf_ast_node_t *parent, const char *s, size_t n, size_t line) {
    size_t i = 0U;
    while (i < n) {
        size_t start;
        if (i + 1U < n && s[i] == '*' && s[i + 1U] == '*') {
            size_t j = i + 2U;
            while (j + 1U < n && !(s[j] == '*' && s[j + 1U] == '*')) j++;
            if (j + 1U < n) { add_text_child(ast, parent, MF_AST_BOLD, s + i + 2U, j - i - 2U, line); i = j + 2U; continue; }
        }
        if (s[i] == '*' || s[i] == '_') {
            char m = s[i];
            size_t j = i + 1U;
            while (j < n && s[j] != m) j++;
            if (j < n) { add_text_child(ast, parent, MF_AST_ITALIC, s + i + 1U, j - i - 1U, line); i = j + 1U; continue; }
        }
        if (i + 1U < n && s[i] == '~' && s[i + 1U] == '~') {
            size_t j = i + 2U;
            while (j + 1U < n && !(s[j] == '~' && s[j + 1U] == '~')) j++;
            if (j + 1U < n) { add_text_child(ast, parent, MF_AST_STRIKE, s + i + 2U, j - i - 2U, line); i = j + 2U; continue; }
        }
        if (i + 1U < n && s[i] == '=' && s[i + 1U] == '=') {
            size_t j = i + 2U;
            while (j + 1U < n && !(s[j] == '=' && s[j + 1U] == '=')) j++;
            if (j + 1U < n) { add_text_child(ast, parent, MF_AST_HIGHLIGHT, s + i + 2U, j - i - 2U, line); i = j + 2U; continue; }
        }
        if (s[i] == '`') {
            size_t j = i + 1U;
            while (j < n && s[j] != '`') j++;
            if (j < n) { add_text_child(ast, parent, MF_AST_INLINE_CODE, s + i + 1U, j - i - 1U, line); i = j + 1U; continue; }
        }
        if (s[i] == '$') {
            size_t j = i + 1U;
            while (j < n && s[j] != '$') j++;
            if (j < n) { add_text_child(ast, parent, MF_AST_INLINE_MATH, s + i + 1U, j - i - 1U, line); i = j + 1U; continue; }
        }
        if (s[i] == '[') {
            size_t j = i + 1U;
            while (j < n && s[j] != ']') j++;
            if (j + 1U < n && s[j + 1U] == '(') {
                size_t k = j + 2U;
                while (k < n && s[k] != ')') k++;
                if (k < n) {
                    mf_ast_node_t *ln = add_text_child(ast, parent, MF_AST_LINK, s + i + 1U, j - i - 1U, line);
                    if (ln) (void)ast_set_meta(ast, ln, s + j + 2U, k - j - 2U);
                    i = k + 1U;
                    continue;
                }
            }
        }
        if (i + 1U < n && s[i] == '!' && s[i + 1U] == '[') {
            size_t j = i + 2U;
            while (j < n && s[j] != ']') j++;
            if (j + 1U < n && s[j + 1U] == '(') {
                size_t k = j + 2U;
                while (k < n && s[k] != ')') k++;
                if (k < n) {
                    mf_ast_node_t *im = add_text_child(ast, parent, MF_AST_IMAGE, s + i + 2U, j - i - 2U, line);
                    if (im) (void)ast_set_meta(ast, im, s + j + 2U, k - j - 2U);
                    i = k + 1U;
                    continue;
                }
            }
        }
        start = i;
        while (i < n && s[i] != '*' && s[i] != '_' && s[i] != '`' && s[i] != '[' && s[i] != '!' && s[i] != '$' && s[i] != '~' && s[i] != '=') i++;
        if (i == start) i++;
        add_text_child(ast, parent, MF_AST_TEXT, s + start, i - start, line);
    }
}

static int is_hr(const char *s, size_t n) {
    mf_string_view_t v = trim_view(s, n);
    return v.length >= 3U && ((v.data[0] == '-' && v.data[1] == '-' && v.data[2] == '-') || (v.data[0] == '*' && v.data[1] == '*' && v.data[2] == '*'));
}

mf_result_t mf_parser_parse(mf_parser_t *p, mf_ast_t *ast) {
    const char *cur;
    size_t line = 1U;
    mf_timer_t timer;
    if (!p || !ast) return MF_ERROR_INVALID;
    mf_timer_start(&timer);
    if (mf_ast_init(ast, p->options.arena_capacity) != MF_OK) return MF_ERROR_NOMEM;
    ast->source = (char *)malloc(p->length + 1U);
    if (!ast->source) { mf_ast_destroy(ast); return MF_ERROR_NOMEM; }
    memcpy(ast->source, p->source, p->length);
    ast->source[p->length] = '\0';
    ast->source_length = p->length;
    cur = p->source;
    while (*cur) {
        size_t n = line_len(cur);
        mf_string_view_t tv = trim_view(cur, n);
        mf_ast_node_t *node;
        if (p->options.trace && p->trace_out) fprintf(p->trace_out, "parse line %lu: %.*s\n", (unsigned long)line, (int)n, cur);
        if (tv.length == 0U) { }
        else if (starts_with(tv.data, "```") || starts_with(tv.data, "~~~")) {
            const char *start = cur + n;
            size_t total = 0U;
            line++;
            if (*start == '\r') start++;
            if (*start == '\n') start++;
            cur = start;
            while (*cur && !starts_with(cur, "```") && !starts_with(cur, "~~~")) {
                size_t ln = line_len(cur);
                total += ln + 1U;
                cur += ln;
                if (*cur == '\r') cur++;
                if (*cur == '\n') cur++;
                line++;
            }
            node = add_text_child(ast, ast->root, MF_AST_CODE_BLOCK, start, total ? total - 1U : 0U, line);
            MF_UNUSED(node);
            if (*cur) n = line_len(cur);
        }
        else if (starts_with(tv.data, "$$")) {
            node = add_text_child(ast, ast->root, MF_AST_MATH_BLOCK, tv.data + 2U, tv.length > 4U ? tv.length - 4U : 0U, line);
            MF_UNUSED(node);
        }
        else if (tv.data[0] == '#') {
            int level = 0;
            while ((size_t)level < tv.length && tv.data[level] == '#' && level < 6) level++;
            node = mf_ast_create_node(ast, MF_AST_HEADING, trim_view(tv.data + level, tv.length - (size_t)level), line, 1U);
            if (node) { node->level = level; parse_inline(ast, node, node->text.data, node->text.length, line); (void)mf_ast_append_child(ast->root, node); }
        }
        else if (is_hr(tv.data, tv.length)) {
            node = mf_ast_create_node(ast, MF_AST_HORIZONTAL_RULE, mf_sv("", 0U), line, 1U);
            if (node) (void)mf_ast_append_child(ast->root, node);
        }
        else if (tv.data[0] == '>') {
            node = mf_ast_create_node(ast, MF_AST_BLOCKQUOTE, trim_view(tv.data + 1U, tv.length - 1U), line, 1U);
            if (node) { parse_inline(ast, node, node->text.data, node->text.length, line); (void)mf_ast_append_child(ast->root, node); }
        }
        else if ((tv.data[0] == '-' || tv.data[0] == '+' || tv.data[0] == '*') && tv.length > 1U && isspace((unsigned char)tv.data[1])) {
            node = mf_ast_create_node(ast, MF_AST_UNORDERED_LIST, mf_sv("", 0U), line, 1U);
            if (node) {
                mf_ast_node_t *it = mf_ast_create_node(ast, MF_AST_LIST_ITEM, trim_view(tv.data + 2U, tv.length - 2U), line, 3U);
                if (it) { parse_inline(ast, it, it->text.data, it->text.length, line); (void)mf_ast_append_child(node, it); }
                (void)mf_ast_append_child(ast->root, node);
            }
        }
        else if (isdigit((unsigned char)tv.data[0])) {
            size_t j = 0U;
            while (j < tv.length && isdigit((unsigned char)tv.data[j])) j++;
            if (j < tv.length && tv.data[j] == '.') {
                node = mf_ast_create_node(ast, MF_AST_ORDERED_LIST, mf_sv("", 0U), line, 1U);
                if (node) {
                    mf_ast_node_t *it = mf_ast_create_node(ast, MF_AST_LIST_ITEM, trim_view(tv.data + j + 1U, tv.length - j - 1U), line, j + 2U);
                    if (it) { parse_inline(ast, it, it->text.data, it->text.length, line); (void)mf_ast_append_child(node, it); }
                    (void)mf_ast_append_child(ast->root, node);
                }
            } else {
                node = mf_ast_create_node(ast, MF_AST_PARAGRAPH, tv, line, 1U);
                if (node) { parse_inline(ast, node, node->text.data, node->text.length, line); (void)mf_ast_append_child(ast->root, node); }
            }
        }
        else if (tv.data[0] == '|') {
            node = mf_ast_create_node(ast, MF_AST_TABLE, tv, line, 1U);
            if (node) (void)mf_ast_append_child(ast->root, node);
        }
        else if (tv.data[0] == '<') {
            node = mf_ast_create_node(ast, MF_AST_HTML_BLOCK, tv, line, 1U);
            if (node) (void)mf_ast_append_child(ast->root, node);
        }
        else {
            node = mf_ast_create_node(ast, MF_AST_PARAGRAPH, tv, line, 1U);
            if (node) { parse_inline(ast, node, node->text.data, node->text.length, line); (void)mf_ast_append_child(ast->root, node); }
        }
        cur += n;
        if (*cur == '\r') cur++;
        if (*cur == '\n') cur++;
        line++;
    }
    {
        mf_ast_stats_t as = mf_ast_statistics(ast);
        mf_timer_stop(&timer);
        p->stats.node_count = as.node_count;
        p->stats.max_depth = as.max_depth;
        p->stats.diagnostic_count = mf_diag_count(&p->diagnostics);
        p->stats.parse_time_ms = mf_timer_elapsed_ms(&timer);
        p->stats.memory_used = mf_arena_used(&ast->arena);
    }
    return MF_OK;
}

mf_result_t mf_parse_string(const char *filename, const char *source, mf_ast_t *ast, mf_diagnostic_bag_t *diagnostics, const mf_parse_options_t *options) {
    mf_parser_t p;
    mf_result_t r;
    if (!source || !ast) return MF_ERROR_INVALID;
    r = mf_parser_init(&p, filename, source, strlen(source), options);
    if (r != MF_OK) return r;
    r = mf_parser_parse(&p, ast);
    if (diagnostics) { *diagnostics = p.diagnostics; memset(&p.diagnostics, 0, sizeof(p.diagnostics)); }
    mf_parser_destroy(&p);
    return r;
}

mf_result_t mf_parse_file(const char *path, mf_ast_t *ast, mf_diagnostic_bag_t *diagnostics, const mf_parse_options_t *options) {
    char *data;
    size_t len;
    mf_result_t r;
    if (mf_fs_read_file(path, &data, &len) != MF_OK) return MF_ERROR_IO;
    r = mf_parse_string(path, data, ast, diagnostics, options);
    free(data);
    return r;
}

void mf_parser_print_stats(const mf_parse_stats_t *s, FILE *out) {
    if (!s) return;
    if (!out) out = stdout;
    fprintf(out, "tokens=%lu nodes=%lu depth=%lu diagnostics=%lu parse_ms=%.3f memory=%lu\n", (unsigned long)s->token_count, (unsigned long)s->node_count, (unsigned long)s->max_depth, (unsigned long)s->diagnostic_count, s->parse_time_ms, (unsigned long)s->memory_used);
}
