#include "markforge/platform.h"
#if MF_PLATFORM_WINDOWS
#include <windows.h>
#include <io.h>
#else
#include <unistd.h>
#include <sys/time.h>
extern int fileno(FILE *stream);
#endif

const char *mf_platform_name(void) {
#if MF_PLATFORM_WINDOWS
    return "windows";
#elif MF_PLATFORM_MACOS
    return "macos";
#elif MF_PLATFORM_LINUX
    return "linux";
#else
    return "unknown";
#endif
}

int mf_platform_cpu_count(void) {
#if MF_PLATFORM_WINDOWS
    SYSTEM_INFO si;
    GetSystemInfo(&si);
    return (int)si.dwNumberOfProcessors;
#else
    long n;
    n = sysconf(_SC_NPROCESSORS_ONLN);
    return n > 0 ? (int)n : 1;
#endif
}

uint64_t mf_platform_time_ns(void) {
#if MF_PLATFORM_WINDOWS
    LARGE_INTEGER f;
    LARGE_INTEGER c;
    QueryPerformanceFrequency(&f);
    QueryPerformanceCounter(&c);
    return (uint64_t)((c.QuadPart * 1000000000ULL) / f.QuadPart);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000000ULL + (uint64_t)tv.tv_usec * 1000ULL;
#endif
}

const char *mf_platform_home_dir(void) {
    const char *h;
    h = getenv("HOME");
#if MF_PLATFORM_WINDOWS
    if (!h) h = getenv("USERPROFILE");
#endif
    return h ? h : "";
}

const char *mf_platform_env(const char *name) {
    const char *v;
    v = name ? getenv(name) : NULL;
    return v ? v : "";
}

int mf_platform_is_tty(FILE *stream) {
#if MF_PLATFORM_WINDOWS
    return _isatty(_fileno(stream));
#else
    return isatty(fileno(stream));
#endif
}
