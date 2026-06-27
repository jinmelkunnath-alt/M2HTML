#include "markforge/config_engine.h"
#include "markforge/fs.h"
#include "markforge/strings.h"

static mf_result_t set_view_pair(mf_engine_config_t *c, mf_string_view_t key, mf_string_view_t value) {
    char kb[128];
    char vb[512];
    size_t kn;
    size_t vn;
    if (!c) return MF_ERROR_INVALID;
    key = mf_sv_trim(key);
    value = mf_sv_trim(value);
    if (key.length == 0U) return MF_OK;
    kn = key.length < sizeof(kb) - 1U ? key.length : sizeof(kb) - 1U;
    vn = value.length < sizeof(vb) - 1U ? value.length : sizeof(vb) - 1U;
    memcpy(kb, key.data, kn); kb[kn] = '\0';
    memcpy(vb, value.data, vn); vb[vn] = '\0';
    return mf_config_set(&c->values, kb, vb);
}

static void parse_pairs(mf_engine_config_t *c, char *data) {
    char *line = data;
    char *p = data;
    for (;;) {
        if (*p == '\n' || *p == '\r' || *p == '\0') {
            char saved = *p;
            char *eq;
            *p = '\0';
            eq = strchr(line, '=');
            if (!eq) eq = strchr(line, ':');
            if (eq) {
                *eq = '\0';
                (void)set_view_pair(c, mf_sv_from_cstr(line), mf_sv_from_cstr(eq + 1));
            }
            if (saved == '\0') break;
            if (saved == '\r' && p[1] == '\n') p++;
            line = p + 1;
        }
        p++;
    }
}

mf_result_t mf_engine_config_init(mf_engine_config_t *c) {
    if (!c) return MF_ERROR_INVALID;
    memset(c, 0, sizeof(*c));
    snprintf(c->version, sizeof(c->version), "%s", MF_VERSION_STRING);
    snprintf(c->profile, sizeof(c->profile), "%s", "default");
    return mf_config_init(&c->values);
}

void mf_engine_config_destroy(mf_engine_config_t *c) {
    if (c) { mf_config_destroy(&c->values); memset(c, 0, sizeof(*c)); }
}

mf_result_t mf_engine_config_load_text(mf_engine_config_t *c, const char *text, mf_config_format_t fmt) {
    char *copy;
    size_t n;
    MF_UNUSED(fmt);
    if (!c || !text) return MF_ERROR_INVALID;
    n = strlen(text);
    copy = (char *)malloc(n + 1U);
    if (!copy) return MF_ERROR_NOMEM;
    memcpy(copy, text, n + 1U);
    parse_pairs(c, copy);
    free(copy);
    return MF_OK;
}

mf_result_t mf_engine_config_load(mf_engine_config_t *c, const char *path) {
    char *data;
    size_t len;
    mf_result_t r;
    if (!c || !path) return MF_ERROR_INVALID;
    if (mf_fs_read_file(path, &data, &len) != MF_OK) return MF_ERROR_IO;
    MF_UNUSED(len);
    r = mf_engine_config_load_text(c, data, MF_CONFIG_FORMAT_AUTO);
    free(data);
    return r;
}

mf_result_t mf_engine_config_validate(const mf_engine_config_t *c, mf_diagnostic_bag_t *d) {
    mf_source_region_t r;
    if (!c) return MF_ERROR_INVALID;
    if (!c->version[0]) {
        r.file = "config"; r.line = 1U; r.column = 1U; r.length = 1U;
        if (d) (void)mf_diag_add(d, MF_DIAG_ERROR, 3001, r, "configuration version is missing", "set version to a supported value");
        return MF_ERROR_INVALID;
    }
    return MF_OK;
}

mf_result_t mf_engine_config_apply_defaults(mf_engine_config_t *c) {
    if (!c) return MF_ERROR_INVALID;
    (void)mf_config_set(&c->values, "render.pretty", "true");
    (void)mf_config_set(&c->values, "html.safe", "true");
    (void)mf_config_set(&c->values, "indent.width", "2");
    return MF_OK;
}

mf_result_t mf_engine_config_apply_env(mf_engine_config_t *c, const char *prefix) {
    const char *v;
    if (!c) return MF_ERROR_INVALID;
    v = getenv(prefix ? prefix : "MARKFORGE_PROFILE");
    if (v && *v) snprintf(c->profile, sizeof(c->profile), "%s", v);
    return MF_OK;
}

mf_result_t mf_engine_config_migrate(mf_engine_config_t *c, const char *target) {
    if (!c || !target) return MF_ERROR_INVALID;
    snprintf(c->version, sizeof(c->version), "%s", target);
    return MF_OK;
}
