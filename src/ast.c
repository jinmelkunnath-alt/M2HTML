#include "markforge/ast.h"

static char *arena_copy(mf_ast_t *ast, const char *data, size_t len) {
    char *p;
    if (!ast || !data) return NULL;
    p = (char *)mf_arena_alloc(&ast->arena, len + 1U, 1U);
    if (!p) return NULL;
    if (len) memcpy(p, data, len);
    p[len] = '\0';
    return p;
}

mf_result_t mf_ast_init(mf_ast_t *ast, size_t arena_capacity) {
    if (!ast) return MF_ERROR_INVALID;
    memset(ast, 0, sizeof(*ast));
    if (arena_capacity == 0U) arena_capacity = 1024U * 1024U;
    if (mf_arena_init(&ast->arena, arena_capacity, mf_allocator_system()) != MF_OK) return MF_ERROR_NOMEM;
    ast->next_id = 1U;
    ast->root = mf_ast_create_node(ast, MF_AST_DOCUMENT, mf_sv("", 0U), 1U, 1U);
    return ast->root ? MF_OK : MF_ERROR_NOMEM;
}

void mf_ast_destroy(mf_ast_t *ast) {
    if (!ast) return;
    mf_arena_destroy(&ast->arena);
    free(ast->source);
    memset(ast, 0, sizeof(*ast));
}

mf_ast_node_t *mf_ast_create_node(mf_ast_t *ast, mf_ast_node_type_t type, mf_string_view_t text, size_t line, size_t column) {
    mf_ast_node_t *n;
    char *copy;
    if (!ast) return NULL;
    n = (mf_ast_node_t *)mf_arena_alloc(&ast->arena, sizeof(*n), sizeof(void *));
    if (!n) return NULL;
    memset(n, 0, sizeof(*n));
    n->id = ast->next_id++;
    n->type = type;
    if (text.data && text.length) {
        copy = arena_copy(ast, text.data, text.length);
        if (!copy) return NULL;
        n->text = mf_sv(copy, text.length);
    } else {
        n->text = mf_sv("", 0U);
    }
    n->line = line;
    n->column = column;
    ast->node_count++;
    return n;
}

mf_result_t mf_ast_append_child(mf_ast_node_t *parent, mf_ast_node_t *child) {
    if (!parent || !child) return MF_ERROR_INVALID;
    child->parent = parent;
    child->prev_sibling = parent->last_child;
    child->next_sibling = NULL;
    if (parent->last_child) parent->last_child->next_sibling = child;
    else parent->first_child = child;
    parent->last_child = child;
    parent->child_count++;
    return MF_OK;
}

mf_ast_node_t *mf_ast_first_child(const mf_ast_node_t *node) { return node ? node->first_child : NULL; }
mf_ast_node_t *mf_ast_next_sibling(const mf_ast_node_t *node) { return node ? node->next_sibling : NULL; }

static int validate_node(const mf_ast_node_t *n) {
    const mf_ast_node_t *c;
    if (!n) return 0;
    for (c = n->first_child; c; c = c->next_sibling) if (c->parent != n || !validate_node(c)) return 0;
    return 1;
}
int mf_ast_validate(const mf_ast_t *ast) { return ast && ast->root && validate_node(ast->root); }

static void traverse_node(mf_ast_node_t *node, mf_ast_visitor_t *visitor) {
    mf_ast_node_t *c;
    if (!node || !visitor) return;
    if (visitor->enter && visitor->enter(node, visitor->ctx)) return;
    for (c = node->first_child; c; c = c->next_sibling) traverse_node(c, visitor);
    if (visitor->leave) (void)visitor->leave(node, visitor->ctx);
}
void mf_ast_traverse(mf_ast_node_t *node, mf_ast_visitor_t *visitor) { traverse_node(node, visitor); }

const char *mf_ast_node_type_name(mf_ast_node_type_t t) {
    switch (t) {
        case MF_AST_DOCUMENT: return "document"; case MF_AST_HEADING: return "heading"; case MF_AST_PARAGRAPH: return "paragraph";
        case MF_AST_TEXT: return "text"; case MF_AST_BOLD: return "bold"; case MF_AST_ITALIC: return "italic"; case MF_AST_UNDERLINE: return "underline";
        case MF_AST_STRIKE: return "strike"; case MF_AST_INLINE_CODE: return "inline_code"; case MF_AST_CODE_BLOCK: return "code_block";
        case MF_AST_HORIZONTAL_RULE: return "horizontal_rule"; case MF_AST_BLOCKQUOTE: return "blockquote"; case MF_AST_ORDERED_LIST: return "ordered_list";
        case MF_AST_UNORDERED_LIST: return "unordered_list"; case MF_AST_LIST_ITEM: return "list_item"; case MF_AST_TASK_LIST_ITEM: return "task_list_item";
        case MF_AST_TABLE: return "table"; case MF_AST_TABLE_ROW: return "table_row"; case MF_AST_TABLE_CELL: return "table_cell"; case MF_AST_LINK: return "link";
        case MF_AST_IMAGE: return "image"; case MF_AST_FOOTNOTE: return "footnote"; case MF_AST_HTML_BLOCK: return "html_block"; case MF_AST_LINE_BREAK: return "line_break";
        case MF_AST_SOFT_BREAK: return "soft_break"; case MF_AST_HARD_BREAK: return "hard_break"; case MF_AST_EMOJI: return "emoji"; case MF_AST_CALLOUT: return "callout";
        case MF_AST_SPOILER: return "spoiler"; case MF_AST_HIGHLIGHT: return "highlight"; case MF_AST_SUPERSCRIPT: return "superscript"; case MF_AST_SUBSCRIPT: return "subscript";
        case MF_AST_INLINE_MATH: return "inline_math"; case MF_AST_MATH_BLOCK: return "math_block"; case MF_AST_DEFINITION_LIST: return "definition_list";
        case MF_AST_DEFINITION_TERM: return "definition_term"; case MF_AST_DEFINITION_DESCRIPTION: return "definition_description"; case MF_AST_ERROR: return "error";
        default: return "unknown";
    }
}

static void print_node(const mf_ast_node_t *n, FILE *out, size_t depth) {
    size_t i;
    const mf_ast_node_t *c;
    for (i = 0U; i < depth; ++i) fputs("  ", out);
    fprintf(out, "%s#%lu", mf_ast_node_type_name(n->type), (unsigned long)n->id);
    if (n->level) fprintf(out, " level=%d", n->level);
    if (n->text.length) fprintf(out, " text=\"%.*s\"", (int)n->text.length, n->text.data);
    fputc('\n', out);
    for (c = n->first_child; c; c = c->next_sibling) print_node(c, out, depth + 1U);
}
void mf_ast_print(const mf_ast_t *ast, FILE *out) { if (ast && ast->root) print_node(ast->root, out ? out : stdout, 0U); }

static void stats_node(const mf_ast_node_t *n, size_t depth, mf_ast_stats_t *s) {
    const mf_ast_node_t *c;
    if (!n) return;
    s->node_count++;
    if (depth > s->max_depth) s->max_depth = depth;
    s->text_bytes += n->text.length;
    if (n->type == MF_AST_ERROR) s->error_nodes++;
    for (c = n->first_child; c; c = c->next_sibling) stats_node(c, depth + 1U, s);
}
mf_ast_stats_t mf_ast_statistics(const mf_ast_t *ast) {
    mf_ast_stats_t s;
    memset(&s, 0, sizeof(s));
    if (ast && ast->root) stats_node(ast->root, 1U, &s);
    return s;
}
