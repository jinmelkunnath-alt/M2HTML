#include "markforge/markforge.h"
static mf_result_t example_init(mf_plugin_context_t *ctx) { MF_UNUSED(ctx); return MF_OK; }
static void example_shutdown(mf_plugin_context_t *ctx) { MF_UNUSED(ctx); }
mf_plugin_t markforge_example_plugin(void) { mf_plugin_t p; mf_plugin_metadata_init(&p.metadata, "example", "1.0", "Example static MarkForge plugin"); p.init = example_init; p.shutdown = example_shutdown; p.loaded = 0; return p; }
