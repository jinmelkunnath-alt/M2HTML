#ifndef MARKFORGE_STRINGS_H
#define MARKFORGE_STRINGS_H
#include "markforge/common.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct mf_string_s { char *data; size_t length; size_t capacity; } mf_string_t;
typedef struct mf_string_view_s { const char *data; size_t length; } mf_string_view_t;
typedef struct mf_utf8_iter_s { mf_string_view_t view; size_t offset; uint32_t codepoint; size_t width; int valid; } mf_utf8_iter_t;

mf_result_t mf_string_init(mf_string_t *s);
mf_result_t mf_string_from_cstr(mf_string_t *s, const char *text);
void mf_string_destroy(mf_string_t *s);
mf_result_t mf_string_assign(mf_string_t *s, const char *text);
mf_result_t mf_string_append(mf_string_t *s, const char *text);
const char *mf_string_cstr(const mf_string_t *s);

mf_string_view_t mf_sv(const char *data, size_t length);
mf_string_view_t mf_sv_from_cstr(const char *text);
int mf_sv_equal(mf_string_view_t a, mf_string_view_t b);
mf_string_view_t mf_sv_trim(mf_string_view_t v);

size_t mf_strlcpy(char *dst, const char *src, size_t dst_size);
size_t mf_strlcat(char *dst, const char *src, size_t dst_size);
int mf_utf8_decode(const char *s, size_t len, uint32_t *cp, size_t *width);
int mf_utf8_validate(const char *s, size_t len);
void mf_utf8_iter_init(mf_utf8_iter_t *it, mf_string_view_t view);
int mf_utf8_iter_next(mf_utf8_iter_t *it);

#ifdef __cplusplus
}
#endif
#endif
