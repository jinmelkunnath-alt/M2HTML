#include "markforge/devtools.h"
#include "markforge/commands.h"
#include "markforge/fs.h"

static int read_file_arg(int argc, char **argv, char **data, size_t *len) { if (argc < 2) return 2; return mf_fs_read_file(argv[1], data, len) == MF_OK ? 0 : 1; }
static int cmd_about(int argc, char **argv, void *ctx) { MF_UNUSED(argc); MF_UNUSED(argv); MF_UNUSED(ctx); puts("MarkForge - self-contained Markdown processing engine"); return 0; }
static int cmd_license(int argc, char **argv, void *ctx) { MF_UNUSED(argc); MF_UNUSED(argv); MF_UNUSED(ctx); puts("MIT License"); return 0; }
static int cmd_examples(int argc, char **argv, void *ctx) { MF_UNUSED(argc); MF_UNUSED(argv); MF_UNUSED(ctx); puts("examples/basic_usage.c\nexamples/render_document.c"); return 0; }
static int cmd_doctor(int argc, char **argv, void *ctx) { MF_UNUSED(argc); MF_UNUSED(argv); MF_UNUSED(ctx); printf("version=%s\nplatform=%s\nstatus=ok\n", mf_version_string(), mf_platform_name()); return 0; }
static int cmd_init(int argc, char **argv, void *ctx) { const char *path; MF_UNUSED(ctx); path = argc > 1 ? argv[1] : "markforge.conf"; return mf_fs_write_file(path, "render.pretty=true\nhtml.safe=true\n", 34U) == MF_OK ? 0 : 1; }
static int cmd_lint(int argc, char **argv, void *ctx) { char *data; size_t len; mf_lint_result_t r; int rc; MF_UNUSED(ctx); rc = read_file_arg(argc, argv, &data, &len); if (rc) return rc; MF_UNUSED(len); mf_lint_markdown(argv[1], data, &r); mf_diag_print(&r.diagnostics, stderr, 1); rc = r.issue_count ? 1 : 0; mf_lint_result_destroy(&r); free(data); return rc; }
static int cmd_profile(int argc, char **argv, void *ctx) { char *data; size_t len; mf_profile_report_t r; int rc; MF_UNUSED(ctx); rc = read_file_arg(argc, argv, &data, &len); if (rc) return rc; MF_UNUSED(len); rc = mf_profile_document(argv[1], data, "html", &r) == MF_OK ? 0 : 1; mf_profile_report_print(&r, stdout, 0); free(data); return rc; }
static int cmd_benchmark(int argc, char **argv, void *ctx) { char *data; size_t len; mf_benchmark_report_t r; mf_render_output_t out; int rc; MF_UNUSED(ctx); rc = read_file_arg(argc, argv, &data, &len); if (rc) return rc; MF_UNUSED(len); mf_benchmark_run_document("cli", data, "html", 10U, &r); mf_render_output_init_stream(&out, stdout); mf_benchmark_report_write_markdown(&r, 1U, &out); free(data); return 0; }
static int cmd_stats(int argc, char **argv, void *ctx) { mf_ast_t ast; mf_ast_stats_t s; MF_UNUSED(ctx); if (argc < 2 || mf_parse_file(argv[1], &ast, NULL, NULL) != MF_OK) return 1; s = mf_ast_statistics(&ast); printf("nodes=%lu depth=%lu text=%lu errors=%lu\n", (unsigned long)s.node_count, (unsigned long)s.max_depth, (unsigned long)s.text_bytes, (unsigned long)s.error_nodes); mf_ast_destroy(&ast); return 0; }
static int cmd_watch(int argc, char **argv, void *ctx) { mf_watch_options_t o; MF_UNUSED(ctx); if (argc < 2) return 2; memset(&o, 0, sizeof(o)); o.input_path = argv[1]; o.output_path = argc > 2 ? argv[2] : NULL; o.renderer = "html"; o.max_cycles = 1U; return mf_watch_run(&o) == MF_OK ? 0 : 1; }
static int cmd_serve(int argc, char **argv, void *ctx) { mf_ast_t ast; mf_preview_server_t s; MF_UNUSED(ctx); if (argc < 2 || mf_parse_file(argv[1], &ast, NULL, NULL) != MF_OK) return 1; mf_preview_server_init(&s, 8080U); mf_preview_server_set_document(&s, &ast); puts("serving one request on http://127.0.0.1:8080"); (void)mf_preview_server_serve_once(&s); mf_preview_server_destroy(&s); mf_ast_destroy(&ast); return 0; }
static int cmd_shell(int argc, char **argv, void *ctx) { char line[256]; MF_UNUSED(argc); MF_UNUSED(argv); MF_UNUSED(ctx); puts("MarkForge shell. Type 'exit' to leave."); while (fgets(line, sizeof(line), stdin)) { if (strncmp(line, "exit", 4) == 0) break; if (strncmp(line, "version", 7) == 0) puts(MF_VERSION_STRING); else puts("commands: parse render-html lint profile benchmark stats watch serve exit"); } return 0; }

int mf_cli_main(int argc, char **argv) {
    mf_cli_app_t app;
    mf_cli_command_t commands[40];
    size_t n = 0U;
    mf_cli_init(&app, "markforge", MF_VERSION_STRING, "Self-contained Markdown parser, renderer, and developer toolkit");
    (void)mf_cli_register_markdown_commands(&app, commands, 40U, &n);
    commands[n++] = (mf_cli_command_t){ "render", "render as html", mf_cmd_render_html, NULL };
    commands[n++] = (mf_cli_command_t){ "lint", "lint markdown", cmd_lint, NULL };
    commands[n++] = (mf_cli_command_t){ "benchmark", "run benchmark", cmd_benchmark, NULL };
    commands[n++] = (mf_cli_command_t){ "profile", "profile document", cmd_profile, NULL };
    commands[n++] = (mf_cli_command_t){ "stats", "document statistics", cmd_stats, NULL };
    commands[n++] = (mf_cli_command_t){ "watch", "watch document", cmd_watch, NULL };
    commands[n++] = (mf_cli_command_t){ "serve", "serve local preview", cmd_serve, NULL };
    commands[n++] = (mf_cli_command_t){ "shell", "interactive shell", cmd_shell, NULL };
    commands[n++] = (mf_cli_command_t){ "theme", "theme info", mf_cmd_themes, NULL };
    commands[n++] = (mf_cli_command_t){ "list-themes", "list themes", mf_cmd_themes, NULL };
    commands[n++] = (mf_cli_command_t){ "renderer", "list renderers", mf_cmd_list_renderers, NULL };
    commands[n++] = (mf_cli_command_t){ "plugin", "plugin info", mf_cmd_plugins, NULL };
    commands[n++] = (mf_cli_command_t){ "diagnostics", "validate diagnostics", mf_cmd_validate, NULL };
    commands[n++] = (mf_cli_command_t){ "dump-tree", "dump tree", mf_cmd_dump_ast, NULL };
    commands[n++] = (mf_cli_command_t){ "dump-json", "dump json", mf_cmd_render_json, NULL };
    commands[n++] = (mf_cli_command_t){ "license", "show license", cmd_license, NULL };
    commands[n++] = (mf_cli_command_t){ "about", "about MarkForge", cmd_about, NULL };
    commands[n++] = (mf_cli_command_t){ "examples", "show examples", cmd_examples, NULL };
    commands[n++] = (mf_cli_command_t){ "doctor", "check installation", cmd_doctor, NULL };
    commands[n++] = (mf_cli_command_t){ "init", "create config", cmd_init, NULL };
    mf_cli_set_commands(&app, commands, n);
    return mf_cli_run(&app, argc, argv);
}
