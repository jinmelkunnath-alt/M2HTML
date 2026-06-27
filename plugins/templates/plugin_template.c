#include "markforge/markforge.h"
static mf_result_t plugin_init(mf_plugin_context_t *ctx) { MF_UNUSED(ctx); return MF_OK; }
static void plugin_shutdown(mf_plugin_context_t *ctx) { MF_UNUSED(ctx); }
mf_plugin_t create_plugin(void) { mf_plugin_t p; mf_plugin_metadata_init(&p.metadata, "template", "1.0", "Template plugin"); p.init = plugin_init; p.shutdown = plugin_shutdown; p.loaded = 0; return p; }
