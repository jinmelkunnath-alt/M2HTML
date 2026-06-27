#ifndef MARKFORGE_FS_H
#define MARKFORGE_FS_H
#include "markforge/common.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef int (*mf_dir_visit_fn)(const char *path, int is_dir, void *ctx);
typedef struct mf_buffered_reader_s { FILE *fp; unsigned char *buf; size_t cap; size_t pos; size_t len; } mf_buffered_reader_t;
typedef struct mf_buffered_writer_s { FILE *fp; unsigned char *buf; size_t cap; size_t len; } mf_buffered_writer_t;

mf_result_t mf_fs_read_file(const char *path, char **out_data, size_t *out_len);
mf_result_t mf_fs_write_file(const char *path, const void *data, size_t len);
mf_result_t mf_fs_append_file(const char *path, const void *data, size_t len);
int mf_fs_exists(const char *path);
mf_result_t mf_fs_temp_file(char *buffer, size_t buffer_size, const char *prefix);
mf_result_t mf_path_join(char *buffer, size_t buffer_size, const char *a, const char *b);
const char *mf_path_basename(const char *path);
mf_result_t mf_dir_traverse(const char *path, mf_dir_visit_fn visit, void *ctx);
mf_result_t mf_dir_search_recursive(const char *path, const char *name, mf_dir_visit_fn visit, void *ctx);
mf_result_t mf_buffered_reader_open(mf_buffered_reader_t *br, const char *path, size_t capacity);
int mf_buffered_reader_getc(mf_buffered_reader_t *br);
void mf_buffered_reader_close(mf_buffered_reader_t *br);
mf_result_t mf_buffered_writer_open(mf_buffered_writer_t *bw, const char *path, size_t capacity);
mf_result_t mf_buffered_writer_write(mf_buffered_writer_t *bw, const void *data, size_t len);
mf_result_t mf_buffered_writer_flush(mf_buffered_writer_t *bw);
void mf_buffered_writer_close(mf_buffered_writer_t *bw);

#ifdef __cplusplus
}
#endif
#endif
