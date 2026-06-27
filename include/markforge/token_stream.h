#ifndef MARKFORGE_TOKEN_STREAM_H
#define MARKFORGE_TOKEN_STREAM_H
#include "markforge/tokenizer.h"
#include "markforge/containers.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mf_token_stream_s {
    mf_token_t *tokens;
    size_t count;
    size_t capacity;
    size_t position;
} mf_token_stream_t;

mf_result_t mf_token_stream_init(mf_token_stream_t *stream);
void mf_token_stream_destroy(mf_token_stream_t *stream);
mf_result_t mf_token_stream_from_source(mf_token_stream_t *stream, const char *filename, const char *input, size_t length, mf_diagnostic_bag_t *diagnostics);
const mf_token_t *mf_token_stream_peek(const mf_token_stream_t *stream);
const mf_token_t *mf_token_stream_advance(mf_token_stream_t *stream);
const mf_token_t *mf_token_stream_current(const mf_token_stream_t *stream);
const mf_token_t *mf_token_stream_previous(const mf_token_stream_t *stream);
const mf_token_t *mf_token_stream_lookahead(const mf_token_stream_t *stream, size_t distance);
size_t mf_token_stream_checkpoint(const mf_token_stream_t *stream);
void mf_token_stream_restore(mf_token_stream_t *stream, size_t checkpoint);
void mf_token_stream_rewind(mf_token_stream_t *stream);
int mf_token_stream_eof(const mf_token_stream_t *stream);
void mf_token_stream_dump(const mf_token_stream_t *stream, FILE *out);

#ifdef __cplusplus
}
#endif
#endif
