#include "markforge/renderers.h"

void mf_render_options_default(mf_render_options_t *o) {
    if (o) { memset(o, 0, sizeof(*o)); o->pretty = 1; o->safe_html = 1; o->indent_width = 2; o->preserve_formatting = 1; }
}

mf_result_t mf_render_output_init_memory(mf_render_output_t *out) { if (!out) return MF_ERROR_INVALID; memset(out, 0, sizeof(*out)); return mf_sb_init(&out->builder); }
void mf_render_output_init_stream(mf_render_output_t *out, FILE *stream) { if (out) { memset(out, 0, sizeof(*out)); out->stream = stream ? stream : stdout; } }
void mf_render_output_destroy(mf_render_output_t *out) { if (out && !out->stream) mf_sb_destroy(&out->builder); }

mf_result_t mf_render_output_write_n(mf_render_output_t *out, const char *text, size_t len) {
    if (!out || (!text && len)) return MF_ERROR_INVALID;
    if (out->stream) return (!len || fwrite(text, 1U, len, out->stream) == len) ? MF_OK : MF_ERROR_IO;
    return mf_sb_append_n(&out->builder, text, len);
}
mf_result_t mf_render_output_write(mf_render_output_t *out, const char *text) { return mf_render_output_write_n(out, text, text ? strlen(text) : 0U); }
char *mf_render_output_take(mf_render_output_t *out) { return out && !out->stream ? mf_sb_take(&out->builder) : NULL; }

static mf_result_t write_html_escaped(mf_render_output_t *out, const char *s, size_t n, int attr) {
    size_t i;
    MF_UNUSED(attr);
    for (i = 0U; i < n; ++i) {
        switch (s[i]) {
            case '&': if (mf_render_output_write(out, "&amp;") != MF_OK) return MF_ERROR_IO; break;
            case '<': if (mf_render_output_write(out, "&lt;") != MF_OK) return MF_ERROR_IO; break;
            case '>': if (mf_render_output_write(out, "&gt;") != MF_OK) return MF_ERROR_IO; break;
            case '"': if (mf_render_output_write(out, "&quot;") != MF_OK) return MF_ERROR_IO; break;
            default: if (mf_render_output_write_n(out, s + i, 1U) != MF_OK) return MF_ERROR_IO; break;
        }
    }
    return MF_OK;
}

static mf_result_t write_json_escaped(mf_render_output_t *out, const char *s, size_t n) {
    size_t i;
    char buf[8];
    for (i = 0U; i < n; ++i) {
        unsigned char c = (unsigned char)s[i];
        switch (c) {
            case '\\': if (mf_render_output_write(out, "\\\\") != MF_OK) return MF_ERROR_IO; break;
            case '"': if (mf_render_output_write(out, "\\\"") != MF_OK) return MF_ERROR_IO; break;
            case '\n': if (mf_render_output_write(out, "\\n") != MF_OK) return MF_ERROR_IO; break;
            case '\r': if (mf_render_output_write(out, "\\r") != MF_OK) return MF_ERROR_IO; break;
            case '\t': if (mf_render_output_write(out, "\\t") != MF_OK) return MF_ERROR_IO; break;
            default:
                if (c < 0x20U) { snprintf(buf, sizeof(buf), "\\u%04x", (unsigned)c); if (mf_render_output_write(out, buf) != MF_OK) return MF_ERROR_IO; }
                else if (mf_render_output_write_n(out, s + i, 1U) != MF_OK) return MF_ERROR_IO;
                break;
        }
    }
    return MF_OK;
}

static int starts_case(const char *s, size_t n, const char *prefix) {
    size_t i;
    size_t pn = strlen(prefix);
    if (n < pn) return 0;
    for (i = 0U; i < pn; ++i) if (tolower((unsigned char)s[i]) != tolower((unsigned char)prefix[i])) return 0;
    return 1;
}

static int safe_url(mf_string_view_t url) {
    if (!url.data || url.length == 0U) return 1;
    if (starts_case(url.data, url.length, "http://") || starts_case(url.data, url.length, "https://") || starts_case(url.data, url.length, "mailto:")) return 1;
    if (url.data[0] == '/' || url.data[0] == '#' || url.data[0] == '.') return 1;
    if (memchr(url.data, ':', url.length)) return 0;
    return 1;
}

static mf_result_t newline_if_pretty(mf_render_output_t *out, const mf_render_options_t *o) { return (!o || o->pretty) ? mf_render_output_write(out, "\n") : MF_OK; }

static mf_result_t html_node(const mf_ast_node_t *n, const mf_render_options_t *o, mf_render_output_t *out) {
    const mf_ast_node_t *c;
    char tag[16];
    if (!n) return MF_OK;
    switch (n->type) {
        case MF_AST_DOCUMENT:
            for (c = n->first_child; c; c = c->next_sibling) if (html_node(c, o, out) != MF_OK) return MF_ERROR_IO;
            break;
        case MF_AST_HEADING:
            snprintf(tag, sizeof(tag), "h%d", n->level ? n->level : 1);
            mf_render_output_write(out, "<"); mf_render_output_write(out, tag); mf_render_output_write(out, ">");
            for (c = n->first_child; c; c = c->next_sibling) html_node(c, o, out);
            mf_render_output_write(out, "</"); mf_render_output_write(out, tag); mf_render_output_write(out, ">");
            newline_if_pretty(out, o);
            break;
        case MF_AST_PARAGRAPH:
            mf_render_output_write(out, "<p>");
            for (c = n->first_child; c; c = c->next_sibling) html_node(c, o, out);
            mf_render_output_write(out, "</p>"); newline_if_pretty(out, o); break;
        case MF_AST_TEXT: return write_html_escaped(out, n->text.data, n->text.length, 0);
        case MF_AST_BOLD: mf_render_output_write(out, "<strong>"); write_html_escaped(out, n->text.data, n->text.length, 0); mf_render_output_write(out, "</strong>"); break;
        case MF_AST_ITALIC: mf_render_output_write(out, "<em>"); write_html_escaped(out, n->text.data, n->text.length, 0); mf_render_output_write(out, "</em>"); break;
        case MF_AST_STRIKE: mf_render_output_write(out, "<del>"); write_html_escaped(out, n->text.data, n->text.length, 0); mf_render_output_write(out, "</del>"); break;
        case MF_AST_HIGHLIGHT: mf_render_output_write(out, "<mark>"); write_html_escaped(out, n->text.data, n->text.length, 0); mf_render_output_write(out, "</mark>"); break;
        case MF_AST_INLINE_CODE: mf_render_output_write(out, "<code>"); write_html_escaped(out, n->text.data, n->text.length, 0); mf_render_output_write(out, "</code>"); break;
        case MF_AST_INLINE_MATH: mf_render_output_write(out, "<span class=\"math\">"); write_html_escaped(out, n->text.data, n->text.length, 0); mf_render_output_write(out, "</span>"); break;
        case MF_AST_CODE_BLOCK: mf_render_output_write(out, "<pre><code>"); write_html_escaped(out, n->text.data, n->text.length, 0); mf_render_output_write(out, "</code></pre>"); newline_if_pretty(out, o); break;
        case MF_AST_MATH_BLOCK: mf_render_output_write(out, "<pre class=\"math\">"); write_html_escaped(out, n->text.data, n->text.length, 0); mf_render_output_write(out, "</pre>"); newline_if_pretty(out, o); break;
        case MF_AST_HORIZONTAL_RULE: mf_render_output_write(out, "<hr>"); newline_if_pretty(out, o); break;
        case MF_AST_BLOCKQUOTE: mf_render_output_write(out, "<blockquote>"); for (c = n->first_child; c; c = c->next_sibling) html_node(c, o, out); if (!n->first_child) write_html_escaped(out, n->text.data, n->text.length, 0); mf_render_output_write(out, "</blockquote>"); newline_if_pretty(out, o); break;
        case MF_AST_UNORDERED_LIST: mf_render_output_write(out, "<ul>"); for (c = n->first_child; c; c = c->next_sibling) html_node(c, o, out); mf_render_output_write(out, "</ul>"); newline_if_pretty(out, o); break;
        case MF_AST_ORDERED_LIST: mf_render_output_write(out, "<ol>"); for (c = n->first_child; c; c = c->next_sibling) html_node(c, o, out); mf_render_output_write(out, "</ol>"); newline_if_pretty(out, o); break;
        case MF_AST_LIST_ITEM: mf_render_output_write(out, "<li>"); for (c = n->first_child; c; c = c->next_sibling) html_node(c, o, out); if (!n->first_child) write_html_escaped(out, n->text.data, n->text.length, 0); mf_render_output_write(out, "</li>"); break;
        case MF_AST_LINK:
            mf_render_output_write(out, "<a href=\"");
            if (safe_url(n->meta)) write_html_escaped(out, n->meta.data, n->meta.length, 1);
            else mf_render_output_write(out, "#unsafe-url");
            mf_render_output_write(out, "\">"); write_html_escaped(out, n->text.data, n->text.length, 0); mf_render_output_write(out, "</a>"); break;
        case MF_AST_IMAGE:
            mf_render_output_write(out, "<img src=\"");
            if (safe_url(n->meta)) write_html_escaped(out, n->meta.data, n->meta.length, 1);
            mf_render_output_write(out, "\" alt=\""); write_html_escaped(out, n->text.data, n->text.length, 1); mf_render_output_write(out, "\">"); break;
        case MF_AST_HTML_BLOCK:
            if (o && o->raw_html) mf_render_output_write_n(out, n->text.data, n->text.length);
            else write_html_escaped(out, n->text.data, n->text.length, 0);
            newline_if_pretty(out, o); break;
        default:
            write_html_escaped(out, n->text.data, n->text.length, 0); break;
    }
    return MF_OK;
}

mf_result_t mf_render_html(const mf_ast_t *ast, const mf_render_options_t *options, mf_render_output_t *out) { return ast && ast->root && out ? html_node(ast->root, options, out) : MF_ERROR_INVALID; }

static mf_result_t json_node(const mf_ast_node_t *n, const mf_render_options_t *o, mf_render_output_t *out, size_t depth) {
    const mf_ast_node_t *c;
    size_t i;
    char b[64];
    if (o && o->pretty) for (i = 0U; i < depth; ++i) mf_render_output_write(out, "  ");
    mf_render_output_write(out, "{\"id\":"); snprintf(b, sizeof(b), "%lu", (unsigned long)n->id); mf_render_output_write(out, b);
    mf_render_output_write(out, ",\"type\":\""); write_json_escaped(out, mf_ast_node_type_name(n->type), strlen(mf_ast_node_type_name(n->type))); mf_render_output_write(out, "\"");
    mf_render_output_write(out, ",\"text\":\""); write_json_escaped(out, n->text.data, n->text.length); mf_render_output_write(out, "\"");
    if (n->meta.length) { mf_render_output_write(out, ",\"meta\":\""); write_json_escaped(out, n->meta.data, n->meta.length); mf_render_output_write(out, "\""); }
    mf_render_output_write(out, ",\"line\":"); snprintf(b, sizeof(b), "%lu", (unsigned long)n->line); mf_render_output_write(out, b);
    mf_render_output_write(out, ",\"column\":"); snprintf(b, sizeof(b), "%lu", (unsigned long)n->column); mf_render_output_write(out, b);
    mf_render_output_write(out, ",\"children\":["); if (o && o->pretty) mf_render_output_write(out, "\n");
    for (c = n->first_child; c; c = c->next_sibling) { json_node(c, o, out, depth + 1U); if (c->next_sibling) mf_render_output_write(out, ","); if (o && o->pretty) mf_render_output_write(out, "\n"); }
    if (o && o->pretty) for (i = 0U; i < depth; ++i) mf_render_output_write(out, "  ");
    mf_render_output_write(out, "]}");
    return MF_OK;
}

mf_result_t mf_render_json(const mf_ast_t *ast, const mf_render_options_t *options, mf_render_output_t *out, const mf_diagnostic_bag_t *diagnostics) {
    MF_UNUSED(diagnostics);
    return ast && ast->root && out ? json_node(ast->root, options, out, 0U) : MF_ERROR_INVALID;
}

static mf_result_t xml_node(const mf_ast_node_t *n, const mf_render_options_t *o, mf_render_output_t *out, size_t depth) {
    const mf_ast_node_t *c;
    size_t i;
    char b[64];
    if (o && o->pretty) for (i = 0U; i < depth; ++i) mf_render_output_write(out, "  ");
    mf_render_output_write(out, "<node type=\""); write_html_escaped(out, mf_ast_node_type_name(n->type), strlen(mf_ast_node_type_name(n->type)), 1);
    mf_render_output_write(out, "\" id=\""); snprintf(b, sizeof(b), "%lu", (unsigned long)n->id); mf_render_output_write(out, b); mf_render_output_write(out, "\">");
    if (n->text.length) write_html_escaped(out, n->text.data, n->text.length, 0);
    if (n->first_child && o && o->pretty) mf_render_output_write(out, "\n");
    for (c = n->first_child; c; c = c->next_sibling) xml_node(c, o, out, depth + 1U);
    if (n->first_child && o && o->pretty) for (i = 0U; i < depth; ++i) mf_render_output_write(out, "  ");
    mf_render_output_write(out, "</node>"); if (o && o->pretty) mf_render_output_write(out, "\n");
    return MF_OK;
}

mf_result_t mf_render_xml(const mf_ast_t *ast, const mf_render_options_t *options, mf_render_output_t *out) {
    if (!ast || !ast->root || !out) return MF_ERROR_INVALID;
    mf_render_output_write(out, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"); newline_if_pretty(out, options);
    return xml_node(ast->root, options, out, 0U);
}

static mf_result_t text_node(const mf_ast_node_t *n, mf_render_output_t *out) {
    const mf_ast_node_t *c;
    if (!n) return MF_OK;
    if (n->text.length && n->type != MF_AST_DOCUMENT) mf_render_output_write_n(out, n->text.data, n->text.length);
    for (c = n->first_child; c; c = c->next_sibling) text_node(c, out);
    if (n->type == MF_AST_PARAGRAPH || n->type == MF_AST_HEADING || n->type == MF_AST_LIST_ITEM) mf_render_output_write(out, "\n");
    return MF_OK;
}

mf_result_t mf_render_text(const mf_ast_t *ast, const mf_render_options_t *options, mf_render_output_t *out) { MF_UNUSED(options); return ast && ast->root && out ? text_node(ast->root, out) : MF_ERROR_INVALID; }

mf_result_t mf_format_markdown(const mf_ast_t *ast, const mf_render_options_t *options, mf_render_output_t *out) {
    const mf_ast_node_t *n;
    MF_UNUSED(options);
    if (!ast || !ast->root || !out) return MF_ERROR_INVALID;
    for (n = ast->root->first_child; n; n = n->next_sibling) {
        if (n->type == MF_AST_HEADING) { int i; for (i = 0; i < (n->level ? n->level : 1); ++i) mf_render_output_write(out, "#"); mf_render_output_write(out, " "); mf_render_output_write_n(out, n->text.data, n->text.length); mf_render_output_write(out, "\n\n"); }
        else if (n->type == MF_AST_PARAGRAPH) { mf_render_output_write_n(out, n->text.data, n->text.length); mf_render_output_write(out, "\n\n"); }
        else if (n->type == MF_AST_CODE_BLOCK) { mf_render_output_write(out, "```\n"); mf_render_output_write_n(out, n->text.data, n->text.length); mf_render_output_write(out, "\n```\n\n"); }
        else if (n->type == MF_AST_HORIZONTAL_RULE) mf_render_output_write(out, "---\n\n");
        else { mf_render_output_write_n(out, n->text.data, n->text.length); mf_render_output_write(out, "\n"); }
    }
    return MF_OK;
}
