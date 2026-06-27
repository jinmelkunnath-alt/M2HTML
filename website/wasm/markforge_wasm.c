#include <stddef.h>
#include <string.h>

static char g_output[1048576];
static const char *html_escape_char(char c) {
    switch (c) { case '<': return "&lt;"; case '>': return "&gt;"; case '&': return "&amp;"; case '"': return "&quot;"; default: return 0; }
}
static void append(const char *s) { size_t a = strlen(g_output), b = strlen(s); if (a + b + 1 < sizeof(g_output)) memcpy(g_output + a, s, b + 1); }
static void append_n(const char *s, size_t n) { size_t a = strlen(g_output); if (a + n + 1 >= sizeof(g_output)) n = sizeof(g_output) - a - 1; memcpy(g_output + a, s, n); g_output[a + n] = 0; }
static void append_escaped(const char *s, size_t n) { size_t i; for (i = 0; i < n; ++i) { const char *e = html_escape_char(s[i]); if (e) append(e); else append_n(s + i, 1); } }
int markforge_version_major(void) { return 1; }
const char *markforge_render_html(const char *markdown) {
    const char *p = markdown;
    g_output[0] = 0;
    while (*p) {
        const char *line = p; size_t n = 0;
        while (line[n] && line[n] != '\n' && line[n] != '\r') n++;
        if (n > 0 && line[0] == '#') { size_t level = 0; while (level < n && line[level] == '#' && level < 6) level++; append("<h1>"); append_escaped(line + level + (level < n && line[level] == ' ' ? 1 : 0), n - level - (level < n && line[level] == ' ' ? 1 : 0)); append("</h1>\n"); }
        else if (n > 0) { append("<p>"); append_escaped(line, n); append("</p>\n"); }
        p = line + n; if (*p == '\r') p++; if (*p == '\n') p++;
    }
    return g_output;
}
