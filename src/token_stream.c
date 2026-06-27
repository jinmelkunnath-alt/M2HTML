#include "markforge/token_stream.h"

static mf_result_t token_stream_push(mf_token_stream_t *s, mf_token_t tok) {
    mf_token_t *p;
    size_t nc;
    if (s->count == s->capacity) {
        nc = s->capacity ? s->capacity * 2U : 256U;
        p = (mf_token_t *)realloc(s->tokens, nc * sizeof(*p));
        if (!p) return MF_ERROR_NOMEM;
        s->tokens = p;
        s->capacity = nc;
    }
    s->tokens[s->count++] = tok;
    return MF_OK;
}

mf_result_t mf_token_stream_init(mf_token_stream_t *stream) {
    if (!stream) return MF_ERROR_INVALID;
    memset(stream, 0, sizeof(*stream));
    return MF_OK;
}

void mf_token_stream_destroy(mf_token_stream_t *stream) {
    if (stream) {
        free(stream->tokens);
        memset(stream, 0, sizeof(*stream));
    }
}

mf_result_t mf_token_stream_from_source(mf_token_stream_t *stream, const char *filename, const char *input, size_t length, mf_diagnostic_bag_t *diagnostics) {
    mf_tokenizer_t tokenizer;
    mf_token_t tok;
    mf_result_t r;
    if (!stream) return MF_ERROR_INVALID;
    mf_token_stream_destroy(stream);
    (void)mf_token_stream_init(stream);
    mf_tokenizer_init(&tokenizer, filename, input, length, diagnostics);
    do {
        tok = mf_tokenizer_next(&tokenizer);
        r = token_stream_push(stream, tok);
        if (r != MF_OK) return r;
    } while (tok.type != MF_TOKEN_EOF);
    return MF_OK;
}

const mf_token_t *mf_token_stream_peek(const mf_token_stream_t *stream) {
    if (!stream || stream->position >= stream->count) return NULL;
    return &stream->tokens[stream->position];
}

const mf_token_t *mf_token_stream_advance(mf_token_stream_t *stream) {
    const mf_token_t *tok;
    tok = mf_token_stream_peek(stream);
    if (tok && tok->type != MF_TOKEN_EOF) stream->position++;
    return tok;
}

const mf_token_t *mf_token_stream_current(const mf_token_stream_t *stream) {
    return mf_token_stream_peek(stream);
}

const mf_token_t *mf_token_stream_previous(const mf_token_stream_t *stream) {
    if (!stream || stream->position == 0U || stream->count == 0U) return NULL;
    return &stream->tokens[stream->position - 1U];
}

const mf_token_t *mf_token_stream_lookahead(const mf_token_stream_t *stream, size_t distance) {
    size_t index;
    if (!stream) return NULL;
    index = stream->position + distance;
    if (index >= stream->count) return stream->count ? &stream->tokens[stream->count - 1U] : NULL;
    return &stream->tokens[index];
}

size_t mf_token_stream_checkpoint(const mf_token_stream_t *stream) {
    return stream ? stream->position : 0U;
}

void mf_token_stream_restore(mf_token_stream_t *stream, size_t checkpoint) {
    if (stream) stream->position = checkpoint < stream->count ? checkpoint : stream->count;
}

void mf_token_stream_rewind(mf_token_stream_t *stream) {
    if (stream) stream->position = 0U;
}

int mf_token_stream_eof(const mf_token_stream_t *stream) {
    const mf_token_t *tok;
    tok = mf_token_stream_peek(stream);
    return !tok || tok->type == MF_TOKEN_EOF;
}

void mf_token_stream_dump(const mf_token_stream_t *stream, FILE *out) {
    size_t i;
    if (!stream) return;
    for (i = 0U; i < stream->count; ++i) mf_token_dump(&stream->tokens[i], out);
}
