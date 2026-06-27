#include "markforge/cli.h"

void mf_cli_init(mf_cli_app_t *app, const char *name, const char *version, const char *help) {
    if (app) { memset(app, 0, sizeof(*app)); app->name = name; app->version = version; app->help = help; }
}
void mf_cli_set_options(mf_cli_app_t *app, mf_cli_option_t *options, size_t count) { if (app) { app->options = options; app->option_count = count; } }
void mf_cli_set_commands(mf_cli_app_t *app, mf_cli_command_t *commands, size_t count) { if (app) { app->commands = commands; app->command_count = count; } }
void mf_cli_print_help(const mf_cli_app_t *app, FILE *out) {
    size_t i;
    if (!out) out = stdout;
    fprintf(out, "%s %s\n%s\n\nOptions:\n  -h, --help        Show help\n  --version         Show version\n  --verbose         Verbose output\n  --quiet           Quiet output\n  --json            JSON output\n  --color           Force color\n  --no-color        Disable color\n  --config PATH     Use config file\n", app->name ? app->name : "app", app->version ? app->version : "", app->help ? app->help : "");
    for (i = 0U; i < app->option_count; i++) fprintf(out, "  -%c, --%-12s %s\n", app->options[i].short_name ? app->options[i].short_name : ' ', app->options[i].long_name, app->options[i].help ? app->options[i].help : "");
    if (app->command_count) { fprintf(out, "\nCommands:\n"); for (i = 0U; i < app->command_count; i++) fprintf(out, "  %-16s %s\n", app->commands[i].name, app->commands[i].help ? app->commands[i].help : ""); }
}
static mf_cli_option_t *find_opt(mf_cli_app_t *app, const char *arg) {
    size_t i;
    for (i = 0U; i < app->option_count; i++) {
        char s[3];
        s[0] = '-'; s[1] = app->options[i].short_name; s[2] = '\0';
        if ((strncmp(arg, "--", 2) == 0 && strcmp(arg + 2, app->options[i].long_name) == 0) || (app->options[i].short_name && strcmp(arg, s) == 0)) return &app->options[i];
    }
    return NULL;
}
static int is_standard_flag(const char *arg, int *takes_value) {
    *takes_value = 0;
    if (strcmp(arg, "--verbose") == 0 || strcmp(arg, "--quiet") == 0 || strcmp(arg, "--json") == 0 || strcmp(arg, "--color") == 0 || strcmp(arg, "--no-color") == 0) return 1;
    if (strcmp(arg, "--config") == 0) { *takes_value = 1; return 1; }
    return 0;
}
int mf_cli_run(mf_cli_app_t *app, int argc, char **argv) {
    int i;
    if (!app) return 2;
    for (i = 1; i < argc; i++) {
        int takes_value;
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "help") == 0) { mf_cli_print_help(app, stdout); return 0; }
        if (strcmp(argv[i], "--version") == 0 || strcmp(argv[i], "version") == 0) { printf("%s\n", app->version ? app->version : ""); return 0; }
        if (is_standard_flag(argv[i], &takes_value)) { if (takes_value) { if (i + 1 >= argc) return 2; i++; } continue; }
        if (argv[i][0] == '-') {
            mf_cli_option_t *o = find_opt(app, argv[i]);
            if (!o) return 2;
            if (o->type == MF_CLI_BOOL) *(int *)o->target = 1;
            else { if (i + 1 >= argc) return 2; if (o->type == MF_CLI_INT) *(int *)o->target = atoi(argv[++i]); else *(const char **)o->target = argv[++i]; }
        } else {
            size_t j;
            for (j = 0U; j < app->command_count; j++) {
                if (strcmp(argv[i], app->commands[j].name) == 0) {
                    if (i + 1 < argc && (strcmp(argv[i + 1], "--help") == 0 || strcmp(argv[i + 1], "-h") == 0)) {
                        printf("%s - %s\n", app->commands[j].name, app->commands[j].help ? app->commands[j].help : "command");
                        puts("Options: --help --verbose --quiet --json --color --no-color --config PATH");
                        return 0;
                    }
                    return app->commands[j].run(argc - i, argv + i, app->commands[j].ctx);
                }
            }
            return 2;
        }
    }
    mf_cli_print_help(app, stdout);
    return 0;
}
