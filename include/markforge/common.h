#ifndef MARKFORGE_COMMON_H
#define MARKFORGE_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <limits.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>

#define MF_VERSION_MAJOR 0
#define MF_VERSION_MINOR 5
#define MF_VERSION_PATCH 0
#define MF_VERSION_STRING "0.5.0-alpha"

#if defined(_WIN32) || defined(_WIN64)
#define MF_PLATFORM_WINDOWS 1
#else
#define MF_PLATFORM_WINDOWS 0
#endif
#if defined(__APPLE__)
#define MF_PLATFORM_MACOS 1
#else
#define MF_PLATFORM_MACOS 0
#endif
#if defined(__linux__)
#define MF_PLATFORM_LINUX 1
#else
#define MF_PLATFORM_LINUX 0
#endif

#if defined(_MSC_VER)
#define MF_INLINE __inline
#define MF_EXPORT __declspec(dllexport)
#else
#define MF_INLINE inline
#define MF_EXPORT
#endif

#define MF_UNUSED(x) ((void)(x))
#define MF_ARRAY_LEN(a) (sizeof(a) / sizeof((a)[0]))

typedef enum mf_result_e {
    MF_OK = 0,
    MF_ERROR = -1,
    MF_ERROR_NOMEM = -2,
    MF_ERROR_INVALID = -3,
    MF_ERROR_IO = -4,
    MF_ERROR_NOT_FOUND = -5,
    MF_ERROR_OVERFLOW = -6
} mf_result_t;

const char *mf_result_string(mf_result_t code);

#ifdef __cplusplus
}
#endif
#endif
