#ifndef MARKFORGE_TOKENIZER_H
#define MARKFORGE_TOKENIZER_H
#include "markforge/common.h"
#include "markforge/strings.h"
#include "markforge/diagnostics.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum mf_token_type_e {
    MF_TOKEN_TEXT = 0,
    MF_TOKEN_WHITESPACE,
    MF_TOKEN_NEWLINE,
    MF_TOKEN_ESCAPE,
    MF_TOKEN_BACKTICK,
    MF_TOKEN_ASTERISK,
    MF_TOKEN_UNDERSCORE,
    MF_TOKEN_HASH,
    MF_TOKEN_GREATER_THAN,
    MF_TOKEN_DASH,
    MF_TOKEN_PLUS,
    MF_TOKEN_MINUS,
    MF_TOKEN_NUMBER,
    MF_TOKEN_PIPE,
    MF_TOKEN_BRACKET_OPEN,
    MF_TOKEN_BRACKET_CLOSE,
    MF_TOKEN_PAREN_OPEN,
    MF_TOKEN_PAREN_CLOSE,
    MF_TOKEN_IMAGE_MARKER,
    MF_TOKEN_LINK_MARKER,
    MF_TOKEN_COLON,
    MF_TOKEN_SEMICOLON,
    MF_TOKEN_DOT,
    MF_TOKEN_COMMA,
    MF_TOKEN_SPECIAL,
    MF_TOKEN_EOF
} mf_token_type_t;

typedef struct mf_token_s {
    mf_token_type_t type;
    mf_string_view_t raw;
    size_t start_offset;
    size_t end_offset;
    size_t char_offset;
    size_t char_length;
    size_t line;
    size_t column;
    size_t length;
} mf_token_t;

typedef struct mf_tokenizer_s {
    const char *filename;
    mf_string_view_t input;
    size_t offset;
    size_t char_offset;
    size_t line;
    size_t column;
    mf_diagnostic_bag_t *diagnostics;
} mf_tokenizer_t;

void mf_tokenizer_init(mf_tokenizer_t *tokenizer, const char *filename, const char *input, size_t length, mf_diagnostic_bag_t *diagnostics);
mf_token_t mf_tokenizer_next(mf_tokenizer_t *tokenizer);
mf_token_t mf_tokenizer_peek(mf_tokenizer_t *tokenizer);
size_t mf_tokenizer_checkpoint(const mf_tokenizer_t *tokenizer);
void mf_tokenizer_restore(mf_tokenizer_t *tokenizer, size_t checkpoint);
const char *mf_token_type_name(mf_token_type_t type);
void mf_token_dump(const mf_token_t *token, FILE *out);

#ifdef __cplusplus
}
#endif
#endif
