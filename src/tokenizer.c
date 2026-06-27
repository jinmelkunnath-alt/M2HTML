#include "markforge/tokenizer.h"

static int is_text_stop(unsigned char c) {
    return c == '\0' || c == '\n' || c == '\r' || c == ' ' || c == '\t' || c == '\\' || c == '`' || c == '*' || c == '_' || c == '#' || c == '>' || c == '-' || c == '+' || c == '|' || c == '[' || c == ']' || c == '(' || c == ')' || c == ':' || c == ';' || c == '.' || c == ',' || c == '!' || isdigit(c);
}

static mf_token_t make_token(mf_tokenizer_t *t, mf_token_type_t type, size_t start, size_t end, size_t char_start, size_t line, size_t column) {
    mf_token_t tok;
    tok.type = type;
    tok.raw = mf_sv(t->input.data + start, end - start);
    tok.start_offset = start;
    tok.end_offset = end;
    tok.char_offset = char_start;
    tok.char_length = t->char_offset - char_start;
    tok.line = line;
    tok.column = column;
    tok.length = end - start;
    return tok;
}

void mf_tokenizer_init(mf_tokenizer_t *tokenizer, const char *filename, const char *input, size_t length, mf_diagnostic_bag_t *diagnostics) {
    if (!tokenizer) return;
    tokenizer->filename = filename ? filename : "<input>";
    tokenizer->input = mf_sv(input ? input : "", input ? length : 0U);
    tokenizer->offset = 0U;
    tokenizer->char_offset = 0U;
    tokenizer->line = 1U;
    tokenizer->column = 1U;
    tokenizer->diagnostics = diagnostics;
}

size_t mf_tokenizer_checkpoint(const mf_tokenizer_t *tokenizer) {
    return tokenizer ? tokenizer->offset : 0U;
}

void mf_tokenizer_restore(mf_tokenizer_t *tokenizer, size_t checkpoint) {
    size_t i;
    if (!tokenizer) return;
    tokenizer->offset = 0U;
    tokenizer->char_offset = 0U;
    tokenizer->line = 1U;
    tokenizer->column = 1U;
    for (i = 0U; i < checkpoint && tokenizer->offset < tokenizer->input.length;) {
        (void)mf_tokenizer_next(tokenizer);
        i = tokenizer->offset;
    }
}

static void advance_bytes(mf_tokenizer_t *t, size_t n) {
    t->offset += n;
    t->char_offset += 1U;
    t->column += 1U;
}

mf_token_t mf_tokenizer_next(mf_tokenizer_t *t) {
    size_t start;
    size_t cstart;
    size_t line;
    size_t column;
    unsigned char c;
    if (!t || t->offset >= t->input.length) {
        mf_token_t eof;
        memset(&eof, 0, sizeof(eof));
        eof.type = MF_TOKEN_EOF;
        if (t) {
            eof.raw = mf_sv(t->input.data + t->input.length, 0U);
            eof.start_offset = t->input.length;
            eof.end_offset = t->input.length;
            eof.char_offset = t->char_offset;
            eof.line = t->line;
            eof.column = t->column;
        }
        return eof;
    }
    start = t->offset;
    cstart = t->char_offset;
    line = t->line;
    column = t->column;
    c = (unsigned char)t->input.data[t->offset];

    if (c == '\r' || c == '\n') {
        if (c == '\r' && t->offset + 1U < t->input.length && t->input.data[t->offset + 1U] == '\n') t->offset += 2U;
        else t->offset += 1U;
        t->char_offset += 1U;
        t->line += 1U;
        t->column = 1U;
        return make_token(t, MF_TOKEN_NEWLINE, start, t->offset, cstart, line, column);
    }
    if (c == ' ' || c == '\t') {
        while (t->offset < t->input.length && (t->input.data[t->offset] == ' ' || t->input.data[t->offset] == '\t')) advance_bytes(t, 1U);
        return make_token(t, MF_TOKEN_WHITESPACE, start, t->offset, cstart, line, column);
    }
    if (c == '\\') {
        advance_bytes(t, 1U);
        if (t->offset < t->input.length && t->input.data[t->offset] != '\n' && t->input.data[t->offset] != '\r') advance_bytes(t, 1U);
        return make_token(t, MF_TOKEN_ESCAPE, start, t->offset, cstart, line, column);
    }
    if (isdigit(c)) {
        while (t->offset < t->input.length && isdigit((unsigned char)t->input.data[t->offset])) advance_bytes(t, 1U);
        return make_token(t, MF_TOKEN_NUMBER, start, t->offset, cstart, line, column);
    }
    if (c == '!' && t->offset + 1U < t->input.length && t->input.data[t->offset + 1U] == '[') {
        advance_bytes(t, 1U);
        advance_bytes(t, 1U);
        return make_token(t, MF_TOKEN_IMAGE_MARKER, start, t->offset, cstart, line, column);
    }
    switch (c) {
        case '`': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_BACKTICK, start, t->offset, cstart, line, column);
        case '*': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_ASTERISK, start, t->offset, cstart, line, column);
        case '_': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_UNDERSCORE, start, t->offset, cstart, line, column);
        case '#': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_HASH, start, t->offset, cstart, line, column);
        case '>': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_GREATER_THAN, start, t->offset, cstart, line, column);
        case '-': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_DASH, start, t->offset, cstart, line, column);
        case '+': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_PLUS, start, t->offset, cstart, line, column);
        case '|': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_PIPE, start, t->offset, cstart, line, column);
        case '[': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_BRACKET_OPEN, start, t->offset, cstart, line, column);
        case ']': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_BRACKET_CLOSE, start, t->offset, cstart, line, column);
        case '(': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_PAREN_OPEN, start, t->offset, cstart, line, column);
        case ')': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_PAREN_CLOSE, start, t->offset, cstart, line, column);
        case ':': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_COLON, start, t->offset, cstart, line, column);
        case ';': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_SEMICOLON, start, t->offset, cstart, line, column);
        case '.': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_DOT, start, t->offset, cstart, line, column);
        case ',': advance_bytes(t, 1U); return make_token(t, MF_TOKEN_COMMA, start, t->offset, cstart, line, column);
        default: break;
    }
    if ((c & 0x80U) != 0U) {
        uint32_t cp;
        size_t width;
        if (!mf_utf8_decode(t->input.data + t->offset, t->input.length - t->offset, &cp, &width)) {
            mf_source_region_t r;
            r.file = t->filename;
            r.line = line;
            r.column = column;
            r.length = 1U;
            if (t->diagnostics) (void)mf_diag_add(t->diagnostics, MF_DIAG_ERROR, 2001, r, "invalid UTF-8 sequence", "replace the invalid byte sequence with valid UTF-8");
            advance_bytes(t, 1U);
            return make_token(t, MF_TOKEN_SPECIAL, start, t->offset, cstart, line, column);
        }
        MF_UNUSED(cp);
        t->offset += width;
        t->char_offset += 1U;
        t->column += 1U;
        return make_token(t, MF_TOKEN_TEXT, start, t->offset, cstart, line, column);
    }
    while (t->offset < t->input.length && !is_text_stop((unsigned char)t->input.data[t->offset])) advance_bytes(t, 1U);
    if (t->offset == start) advance_bytes(t, 1U);
    return make_token(t, MF_TOKEN_TEXT, start, t->offset, cstart, line, column);
}

mf_token_t mf_tokenizer_peek(mf_tokenizer_t *tokenizer) {
    mf_tokenizer_t copy;
    if (!tokenizer) return mf_tokenizer_next(tokenizer);
    copy = *tokenizer;
    return mf_tokenizer_next(&copy);
}

const char *mf_token_type_name(mf_token_type_t type) {
    switch (type) {
        case MF_TOKEN_TEXT: return "Text";
        case MF_TOKEN_WHITESPACE: return "Whitespace";
        case MF_TOKEN_NEWLINE: return "Newline";
        case MF_TOKEN_ESCAPE: return "Escape";
        case MF_TOKEN_BACKTICK: return "Backtick";
        case MF_TOKEN_ASTERISK: return "Asterisk";
        case MF_TOKEN_UNDERSCORE: return "Underscore";
        case MF_TOKEN_HASH: return "Hash";
        case MF_TOKEN_GREATER_THAN: return "GreaterThan";
        case MF_TOKEN_DASH: return "Dash";
        case MF_TOKEN_PLUS: return "Plus";
        case MF_TOKEN_MINUS: return "Minus";
        case MF_TOKEN_NUMBER: return "Number";
        case MF_TOKEN_PIPE: return "Pipe";
        case MF_TOKEN_BRACKET_OPEN: return "BracketOpen";
        case MF_TOKEN_BRACKET_CLOSE: return "BracketClose";
        case MF_TOKEN_PAREN_OPEN: return "ParenOpen";
        case MF_TOKEN_PAREN_CLOSE: return "ParenClose";
        case MF_TOKEN_IMAGE_MARKER: return "ImageMarker";
        case MF_TOKEN_LINK_MARKER: return "LinkMarker";
        case MF_TOKEN_COLON: return "Colon";
        case MF_TOKEN_SEMICOLON: return "Semicolon";
        case MF_TOKEN_DOT: return "Dot";
        case MF_TOKEN_COMMA: return "Comma";
        case MF_TOKEN_SPECIAL: return "Special";
        case MF_TOKEN_EOF: return "EOF";
        default: return "Unknown";
    }
}

void mf_token_dump(const mf_token_t *token, FILE *out) {
    if (!token) return;
    if (!out) out = stdout;
    fprintf(out, "%s line=%lu col=%lu bytes=%lu..%lu text=\"%.*s\"\n", mf_token_type_name(token->type), (unsigned long)token->line, (unsigned long)token->column, (unsigned long)token->start_offset, (unsigned long)token->end_offset, (int)token->raw.length, token->raw.data ? token->raw.data : "");
}
