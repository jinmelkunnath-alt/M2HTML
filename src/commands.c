#include "markforge/commands.h"
#include "markforge/parser.h"
#include "markforge/renderers.h"
#include "markforge/renderer_manager.h"
#include "markforge/fs.h"
#include "markforge/config_engine.h"
#include "markforge/devtools.h"

static int read_arg(int argc, char **argv, char **data, size_t *len) {
    if (argc < 2) return 2;
    return mf_fs_read_file(argv[1], data, len) == MF_OK ? 0 : 1;
}

static int parse_file_arg(int argc, char **argv, mf_ast_t *ast) {
    char *data;
    size_t len;
    int rc = read_arg(argc, argv, &data, &len);
    if (rc) return rc;
    rc = mf_parse_string(argv[1], data, ast, NULL, NULL) == MF_OK ? 0 : 1;
    free(data);
    return rc;
}

static int render_cmd(int argc, char **argv, const char *name) {
    mf_ast_t ast;
    mf_render_output_t out;
    mf_renderer_manager_t m;
    int rc = parse_file_arg(argc, argv, &ast);
    if (rc) return rc;
    mf_render_output_init_stream(&out, stdout);
    mf_renderer_manager_init(&m);
    mf_renderer_manager_register_builtin(&m);
    rc = mf_renderer_manager_render(&m, name, &ast, NULL, &out) == MF_OK ? 0 : 1;
    mf_renderer_manager_destroy(&m);
    mf_ast_destroy(&ast);
    return rc;
}

int mf_cmd_parse(int argc, char **argv, void *ctx) {
    mf_ast_t ast;
    MF_UNUSED(ctx);
    if (parse_file_arg(argc, argv, &ast)) return 1;
    mf_parser_print_stats(&(mf_parse_stats_t){0U, mf_ast_statistics(&ast).node_count, mf_ast_statistics(&ast).max_depth, 0U, 0.0, mf_arena_used(&ast.arena)}, stdout);
    mf_ast_destroy(&ast);
    return 0;
}

int mf_cmd_validate(int argc, char **argv, void *ctx) {
    char *data;
    size_t len;
    mf_diagnostic_bag_t diagnostics;
    int ok;
    MF_UNUSED(ctx);
    if (read_arg(argc, argv, &data, &len)) return 1;
    MF_UNUSED(len);
    ok = mf_validate_document_full(argv[1], data, &diagnostics) == MF_OK && mf_diag_count(&diagnostics) == 0U;
    printf("%s\n", ok ? "valid" : "invalid");
    if (!ok) mf_diag_print(&diagnostics, stderr, 1);
    mf_diag_bag_destroy(&diagnostics);
    free(data);
    return ok ? 0 : 1;
}

int mf_cmd_dump_tokens(int argc, char **argv, void *ctx) {
    char *data;
    size_t len;
    mf_token_stream_t s;
    MF_UNUSED(ctx);
    if (read_arg(argc, argv, &data, &len)) return 1;
    mf_token_stream_init(&s);
    mf_token_stream_from_source(&s, argv[1], data, len, NULL);
    mf_token_stream_dump(&s, stdout);
    mf_token_stream_destroy(&s);
    free(data);
    return 0;
}

int mf_cmd_dump_ast(int argc, char **argv, void *ctx) {
    mf_ast_t ast;
    MF_UNUSED(ctx);
    if (parse_file_arg(argc, argv, &ast)) return 1;
    mf_ast_print(&ast, stdout);
    mf_ast_destroy(&ast);
    return 0;
}

int mf_cmd_render_html(int a, char **v, void *c) { MF_UNUSED(c); return render_cmd(a, v, "html"); }
int mf_cmd_render_json(int a, char **v, void *c) { MF_UNUSED(c); return render_cmd(a, v, "json"); }
int mf_cmd_render_xml(int a, char **v, void *c) { MF_UNUSED(c); return render_cmd(a, v, "xml"); }
int mf_cmd_render_text(int a, char **v, void *c) { MF_UNUSED(c); return render_cmd(a, v, "text"); }
int mf_cmd_format(int a, char **v, void *c) { MF_UNUSED(c); return render_cmd(a, v, "markdown"); }

int mf_cmd_list_renderers(int argc, char **argv, void *ctx) {
    mf_renderer_manager_t m;
    MF_UNUSED(argc); MF_UNUSED(argv); MF_UNUSED(ctx);
    mf_renderer_manager_init(&m);
    mf_renderer_manager_register_builtin(&m);
    mf_renderer_manager_list(&m, stdout);
    mf_renderer_manager_destroy(&m);
    return 0;
}

int mf_cmd_plugins(int argc, char **argv, void *ctx) { MF_UNUSED(argc); MF_UNUSED(argv); MF_UNUSED(ctx); puts("plugins: local static metadata registration only"); return 0; }
int mf_cmd_themes(int argc, char **argv, void *ctx) { MF_UNUSED(argc); MF_UNUSED(argv); MF_UNUSED(ctx); puts("themes: key-value theme records only"); return 0; }

int mf_cmd_config(int argc, char **argv, void *ctx) {
    mf_engine_config_t c;
    MF_UNUSED(ctx);
    if (argc < 2) { puts("config requires file"); return 2; }
    mf_engine_config_init(&c);
    if (mf_engine_config_load(&c, argv[1]) != MF_OK) { mf_engine_config_destroy(&c); return 1; }
    printf("config profile=%s version=%s\n", c.profile, c.version);
    mf_engine_config_destroy(&c);
    return 0;
}

int mf_cmd_validate_config(int argc, char **argv, void *ctx) {
    mf_engine_config_t c;
    mf_diagnostic_bag_t d;
    int ok;
    MF_UNUSED(ctx);
    if (argc < 2) return 2;
    mf_diag_bag_init(&d);
    mf_engine_config_init(&c);
    ok = mf_engine_config_load(&c, argv[1]) == MF_OK && mf_engine_config_validate(&c, &d) == MF_OK;
    printf("%s\n", ok ? "valid" : "invalid");
    mf_diag_print(&d, stderr, 1);
    mf_diag_bag_destroy(&d);
    mf_engine_config_destroy(&c);
    return ok ? 0 : 1;
}

mf_result_t mf_cli_register_markdown_commands(mf_cli_app_t *app, mf_cli_command_t *cmds, size_t cap, size_t *out) {
    size_t n = 0U;
    if (!app || !cmds || cap < 15U || !out) return MF_ERROR_INVALID;
    cmds[n++] = (mf_cli_command_t){"parse", "parse markdown", mf_cmd_parse, NULL};
    cmds[n++] = (mf_cli_command_t){"validate", "validate markdown", mf_cmd_validate, NULL};
    cmds[n++] = (mf_cli_command_t){"dump-tokens", "dump tokens", mf_cmd_dump_tokens, NULL};
    cmds[n++] = (mf_cli_command_t){"dump-ast", "dump ast", mf_cmd_dump_ast, NULL};
    cmds[n++] = (mf_cli_command_t){"render-html", "render html", mf_cmd_render_html, NULL};
    cmds[n++] = (mf_cli_command_t){"render-json", "render json", mf_cmd_render_json, NULL};
    cmds[n++] = (mf_cli_command_t){"render-xml", "render xml", mf_cmd_render_xml, NULL};
    cmds[n++] = (mf_cli_command_t){"render-text", "render text", mf_cmd_render_text, NULL};
    cmds[n++] = (mf_cli_command_t){"format", "format markdown", mf_cmd_format, NULL};
    cmds[n++] = (mf_cli_command_t){"list-renderers", "list renderers", mf_cmd_list_renderers, NULL};
    cmds[n++] = (mf_cli_command_t){"plugins", "plugin info", mf_cmd_plugins, NULL};
    cmds[n++] = (mf_cli_command_t){"themes", "theme info", mf_cmd_themes, NULL};
    cmds[n++] = (mf_cli_command_t){"config", "show config", mf_cmd_config, NULL};
    cmds[n++] = (mf_cli_command_t){"validate-config", "validate config", mf_cmd_validate_config, NULL};
    mf_cli_set_commands(app, cmds, n);
    *out = n;
    return MF_OK;
}
