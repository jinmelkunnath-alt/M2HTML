#include "markforge/markforge.h"
int main(void){ mf_logger_t log; mf_log_init(&log); MF_LOG_INFO(&log,"MarkForge foundation %s",mf_version_string()); return 0; }
