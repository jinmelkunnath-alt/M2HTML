#ifndef MARKFORGE_PARSER_H
#define MARKFORGE_PARSER_H
#include "markforge/ast.h"
#include "markforge/token_stream.h"
#include "markforge/config.h"
#include "markforge/util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mf_parse_options_s {
    mf_parser_options_t base;
    int trace;
    int recover;
    size_t arena_capacity;
} mf_parse_options_t;

typedef struct mf_parse_stats_s {
    size_t token_count;
    size_t node_count;
    size_t max_depth;
    size_t diagnostic_count;
    double parse_time_ms;
    size_t memory_used;
} mf_parse_stats_t;

typedef struct mf_parser_s {
    const char *filename;
    const char *source;
    size_t length;
    mf_token_stream_t tokens;
    mf_diagnostic_bag_t diagnostics;
    mf_parse_options_t options;
    mf_parse_stats_t stats;
    FILE *trace_out;
} mf_parser_t;

void mf_parse_options_default(mf_parse_options_t *options);
mf_result_t mf_parser_init(mf_parser_t *parser, const char *filename, const char *source, size_t length, const mf_parse_options_t *options);
void mf_parser_destroy(mf_parser_t *parser);
mf_result_t mf_parser_parse(mf_parser_t *parser, mf_ast_t *ast);
mf_result_t mf_parse_string(const char *filename, const char *source, mf_ast_t *ast, mf_diagnostic_bag_t *diagnostics, const mf_parse_options_t *options);
mf_result_t mf_parse_file(const char *path, mf_ast_t *ast, mf_diagnostic_bag_t *diagnostics, const mf_parse_options_t *options);
const mf_parse_stats_t *mf_parser_stats(const mf_parser_t *parser);
void mf_parser_trace_set(mf_parser_t *parser, FILE *out);
void mf_parser_print_stats(const mf_parse_stats_t *stats, FILE *out);

#ifdef __cplusplus
}
#endif
#endif
