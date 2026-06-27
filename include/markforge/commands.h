#ifndef MARKFORGE_COMMANDS_H
#define MARKFORGE_COMMANDS_H
#include "markforge/cli.h"
#ifdef __cplusplus
extern "C" {
#endif

mf_result_t mf_cli_register_markdown_commands(mf_cli_app_t *app, mf_cli_command_t *commands, size_t capacity, size_t *out_count);
int mf_cmd_parse(int argc, char **argv, void *ctx);
int mf_cmd_validate(int argc, char **argv, void *ctx);
int mf_cmd_dump_tokens(int argc, char **argv, void *ctx);
int mf_cmd_dump_ast(int argc, char **argv, void *ctx);
int mf_cmd_render_html(int argc, char **argv, void *ctx);
int mf_cmd_render_json(int argc, char **argv, void *ctx);
int mf_cmd_render_xml(int argc, char **argv, void *ctx);
int mf_cmd_render_text(int argc, char **argv, void *ctx);
int mf_cmd_format(int argc, char **argv, void *ctx);
int mf_cmd_list_renderers(int argc, char **argv, void *ctx);
int mf_cmd_plugins(int argc, char **argv, void *ctx);
int mf_cmd_themes(int argc, char **argv, void *ctx);
int mf_cmd_config(int argc, char **argv, void *ctx);
int mf_cmd_validate_config(int argc, char **argv, void *ctx);

#ifdef __cplusplus
}
#endif
#endif
