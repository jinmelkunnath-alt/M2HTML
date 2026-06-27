#include "markforge/config.h"
#include "markforge/fs.h"
#include "markforge/strings.h"

static char *dup_local(const char *s) {
    size_t n;
    char *p;
    n = strlen(s) + 1U;
    p = (char *)malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

mf_result_t mf_config_init(mf_config_t *cfg) {
    if (!cfg) return MF_ERROR_INVALID;
    memset(cfg, 0, sizeof(*cfg));
    return mf_hash_map_init(&cfg->values);
}

void mf_config_destroy(mf_config_t *cfg) {
    size_t i;
    if (!cfg) return;
    for (i = 0U; i < cfg->values.capacity; i++) {
        if (cfg->values.entries[i].state == 1) free(cfg->values.entries[i].value);
    }
    mf_hash_map_destroy(&cfg->values);
    memset(cfg, 0, sizeof(*cfg));
}

mf_result_t mf_config_set(mf_config_t *cfg, const char *key, const char *value) {
    char *v;
    if (!cfg || !key || !value) return MF_ERROR_INVALID;
    if (mf_hash_map_contains(&cfg->values, key)) free(mf_hash_map_get(&cfg->values, key));
    v = dup_local(value);
    if (!v) return MF_ERROR_NOMEM;
    return mf_hash_map_put(&cfg->values, key, v);
}

const char *mf_config_get(const mf_config_t *cfg, const char *key, const char *fallback) {
    const char *v;
    if (!cfg || !key) return fallback;
    v = (const char *)mf_hash_map_get(&cfg->values, key);
    return v ? v : fallback;
}

mf_result_t mf_config_validate(const mf_config_t *cfg) {
    return cfg ? MF_OK : MF_ERROR_INVALID;
}

static void load_config_line(mf_config_t *cfg, char *line) {
    char *eq;
    eq = strchr(line, '=');
    if (eq) {
        mf_string_view_t k;
        mf_string_view_t v;
        char kb[128];
        char vb[512];
        size_t kn;
        size_t vn;
        *eq = '\0';
        k = mf_sv_trim(mf_sv_from_cstr(line));
        v = mf_sv_trim(mf_sv_from_cstr(eq + 1));
        kn = k.length < sizeof(kb) - 1U ? k.length : sizeof(kb) - 1U;
        vn = v.length < sizeof(vb) - 1U ? v.length : sizeof(vb) - 1U;
        memcpy(kb, k.data, kn);
        kb[kn] = '\0';
        memcpy(vb, v.data, vn);
        vb[vn] = '\0';
        (void)mf_config_set(cfg, kb, vb);
    }
}

mf_result_t mf_config_load_file(mf_config_t *cfg, const char *path, mf_config_format_t format) {
    char *data;
    char *start;
    char *p;
    size_t len;
    MF_UNUSED(format);
    if (!cfg || !path) return MF_ERROR_INVALID;
    if (mf_fs_read_file(path, &data, &len) != MF_OK) return MF_ERROR_IO;
    snprintf(cfg->source, sizeof(cfg->source), "%s", path);
    start = data;
    for (p = data; ; p++) {
        if (*p == '\n' || *p == '\r' || *p == '\0') {
            char saved;
            saved = *p;
            *p = '\0';
            load_config_line(cfg, start);
            if (saved == '\0') break;
            if (saved == '\r' && p[1] == '\n') p++;
            start = p + 1;
        }
    }
    free(data);
    return MF_OK;
}

void mf_config_cache_init(mf_config_cache_t *cache) {
    if (cache) (void)mf_hash_map_init(&cache->configs);
}

void mf_config_cache_destroy(mf_config_cache_t *cache) {
    size_t i;
    if (!cache) return;
    for (i = 0U; i < cache->configs.capacity; i++) {
        if (cache->configs.entries[i].state == 1) {
            mf_config_destroy((mf_config_t *)cache->configs.entries[i].value);
            free(cache->configs.entries[i].value);
        }
    }
    mf_hash_map_destroy(&cache->configs);
}

mf_result_t mf_config_cache_load(mf_config_cache_t *cache, const char *path, mf_config_t **out_config) {
    mf_config_t *cfg;
    if (!cache || !path || !out_config) return MF_ERROR_INVALID;
    cfg = (mf_config_t *)mf_hash_map_get(&cache->configs, path);
    if (cfg) {
        *out_config = cfg;
        return MF_OK;
    }
    cfg = (mf_config_t *)malloc(sizeof(*cfg));
    if (!cfg) return MF_ERROR_NOMEM;
    if (mf_config_init(cfg) != MF_OK) {
        free(cfg);
        return MF_ERROR_NOMEM;
    }
    if (mf_config_load_file(cfg, path, MF_CONFIG_FORMAT_AUTO) != MF_OK) {
        mf_config_destroy(cfg);
        free(cfg);
        return MF_ERROR_IO;
    }
    if (mf_hash_map_put(&cache->configs, path, cfg) != MF_OK) {
        mf_config_destroy(cfg);
        free(cfg);
        return MF_ERROR_NOMEM;
    }
    *out_config = cfg;
    return MF_OK;
}

void mf_parser_options_default(mf_parser_options_t *options) {
    if (options) {
        options->extensions = 0;
        options->safe_mode = 1;
        options->max_nesting = 128;
    }
}
