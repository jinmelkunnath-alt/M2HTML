#ifndef MARKFORGE_AST_H
#define MARKFORGE_AST_H
#include "markforge/common.h"
#include "markforge/memory.h"
#include "markforge/strings.h"
#include "markforge/containers.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mf_ast_node_type_e {
    MF_AST_DOCUMENT = 0,
    MF_AST_HEADING,
    MF_AST_PARAGRAPH,
    MF_AST_TEXT,
    MF_AST_BOLD,
    MF_AST_ITALIC,
    MF_AST_UNDERLINE,
    MF_AST_STRIKE,
    MF_AST_INLINE_CODE,
    MF_AST_CODE_BLOCK,
    MF_AST_HORIZONTAL_RULE,
    MF_AST_BLOCKQUOTE,
    MF_AST_ORDERED_LIST,
    MF_AST_UNORDERED_LIST,
    MF_AST_LIST_ITEM,
    MF_AST_TASK_LIST_ITEM,
    MF_AST_TABLE,
    MF_AST_TABLE_ROW,
    MF_AST_TABLE_CELL,
    MF_AST_LINK,
    MF_AST_IMAGE,
    MF_AST_FOOTNOTE,
    MF_AST_HTML_BLOCK,
    MF_AST_LINE_BREAK,
    MF_AST_SOFT_BREAK,
    MF_AST_HARD_BREAK,
    MF_AST_EMOJI,
    MF_AST_CALLOUT,
    MF_AST_SPOILER,
    MF_AST_HIGHLIGHT,
    MF_AST_SUPERSCRIPT,
    MF_AST_SUBSCRIPT,
    MF_AST_INLINE_MATH,
    MF_AST_MATH_BLOCK,
    MF_AST_DEFINITION_LIST,
    MF_AST_DEFINITION_TERM,
    MF_AST_DEFINITION_DESCRIPTION,
    MF_AST_ERROR
} mf_ast_node_type_t;

typedef struct mf_ast_node_s mf_ast_node_t;

struct mf_ast_node_s {
    size_t id;
    mf_ast_node_type_t type;
    mf_string_view_t text;
    mf_string_view_t meta;
    size_t line;
    size_t column;
    size_t start_offset;
    size_t end_offset;
    int level;
    int flags;
    mf_ast_node_t *parent;
    mf_ast_node_t *first_child;
    mf_ast_node_t *last_child;
    mf_ast_node_t *prev_sibling;
    mf_ast_node_t *next_sibling;
    size_t child_count;
};

typedef struct mf_ast_s {
    mf_arena_t arena;
    mf_ast_node_t *root;
    char *source;
    size_t source_length;
    size_t next_id;
    size_t node_count;
    size_t max_depth;
} mf_ast_t;

typedef struct mf_ast_stats_s {
    size_t node_count;
    size_t max_depth;
    size_t text_bytes;
    size_t error_nodes;
} mf_ast_stats_t;

typedef int (*mf_ast_visit_fn)(mf_ast_node_t *node, void *ctx);

typedef struct mf_ast_visitor_s {
    mf_ast_visit_fn enter;
    mf_ast_visit_fn leave;
    void *ctx;
} mf_ast_visitor_t;

mf_result_t mf_ast_init(mf_ast_t *ast, size_t arena_capacity);
void mf_ast_destroy(mf_ast_t *ast);
mf_ast_node_t *mf_ast_create_node(mf_ast_t *ast, mf_ast_node_type_t type, mf_string_view_t text, size_t line, size_t column);
mf_result_t mf_ast_append_child(mf_ast_node_t *parent, mf_ast_node_t *child);
mf_ast_node_t *mf_ast_first_child(const mf_ast_node_t *node);
mf_ast_node_t *mf_ast_next_sibling(const mf_ast_node_t *node);
int mf_ast_validate(const mf_ast_t *ast);
void mf_ast_traverse(mf_ast_node_t *node, mf_ast_visitor_t *visitor);
void mf_ast_print(const mf_ast_t *ast, FILE *out);
mf_ast_stats_t mf_ast_statistics(const mf_ast_t *ast);
const char *mf_ast_node_type_name(mf_ast_node_type_t type);

#ifdef __cplusplus
}
#endif
#endif
