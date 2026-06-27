#ifndef MARKFORGE_CLI_H
#define MARKFORGE_CLI_H
#include "markforge/common.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum mf_cli_option_type_e { MF_CLI_BOOL, MF_CLI_INT, MF_CLI_STRING } mf_cli_option_type_t;
typedef struct mf_cli_option_s { const char *long_name; char short_name; mf_cli_option_type_t type; const char *help; void *target; } mf_cli_option_t;
typedef int (*mf_cli_command_fn)(int argc, char **argv, void *ctx);
typedef struct mf_cli_command_s { const char *name; const char *help; mf_cli_command_fn run; void *ctx; } mf_cli_command_t;
typedef struct mf_cli_app_s { const char *name; const char *version; const char *help; mf_cli_option_t *options; size_t option_count; mf_cli_command_t *commands; size_t command_count; } mf_cli_app_t;

void mf_cli_init(mf_cli_app_t *app, const char *name, const char *version, const char *help);
void mf_cli_set_options(mf_cli_app_t *app, mf_cli_option_t *options, size_t count);
void mf_cli_set_commands(mf_cli_app_t *app, mf_cli_command_t *commands, size_t count);
void mf_cli_print_help(const mf_cli_app_t *app, FILE *out);
int mf_cli_run(mf_cli_app_t *app, int argc, char **argv);

#ifdef __cplusplus
}
#endif
#endif
