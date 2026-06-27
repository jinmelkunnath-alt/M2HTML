#include "markforge/common.h"

const char *mf_result_string(mf_result_t code) {
    switch (code) {
        case MF_OK: return "ok";
        case MF_ERROR: return "error";
        case MF_ERROR_NOMEM: return "out of memory";
        case MF_ERROR_INVALID: return "invalid argument";
        case MF_ERROR_IO: return "io error";
        case MF_ERROR_NOT_FOUND: return "not found";
        case MF_ERROR_OVERFLOW: return "overflow";
        default: return "unknown";
    }
}
