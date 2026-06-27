#ifndef MARKFORGE_LOGGING_H
#define MARKFORGE_LOGGING_H
#include "markforge/common.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef enum mf_log_level_e { MF_LOG_TRACE, MF_LOG_DEBUG, MF_LOG_INFO, MF_LOG_WARN, MF_LOG_ERROR, MF_LOG_FATAL } mf_log_level_t;
typedef struct mf_logger_s { FILE *file; int console_enabled; int color_enabled; mf_log_level_t min_level; } mf_logger_t;

void mf_log_init(mf_logger_t *logger);
mf_result_t mf_log_set_file(mf_logger_t *logger, const char *path);
void mf_log_close(mf_logger_t *logger);
void mf_log_write(mf_logger_t *logger, mf_log_level_t level, const char *file, const char *func, int line, const char *fmt, ...);
const char *mf_log_level_name(mf_log_level_t level);

#define MF_LOG_TRACE(l, ...) mf_log_write((l), MF_LOG_TRACE, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define MF_LOG_DEBUG(l, ...) mf_log_write((l), MF_LOG_DEBUG, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define MF_LOG_INFO(l, ...) mf_log_write((l), MF_LOG_INFO, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define MF_LOG_WARN(l, ...) mf_log_write((l), MF_LOG_WARN, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define MF_LOG_ERROR(l, ...) mf_log_write((l), MF_LOG_ERROR, __FILE__, __func__, __LINE__, __VA_ARGS__)
#define MF_LOG_FATAL(l, ...) mf_log_write((l), MF_LOG_FATAL, __FILE__, __func__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif
#endif
