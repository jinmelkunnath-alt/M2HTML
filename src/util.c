#include "markforge/util.h"
void mf_assert_fail(const char*expr,const char*file,const char*func,int line){ fprintf(stderr,"assertion failed: %s at %s:%s:%d\n",expr,file,func,line); abort(); }
void mf_timer_start(mf_timer_t*timer){ if(timer){timer->start_ns=mf_platform_time_ns();timer->end_ns=timer->start_ns;} }
void mf_timer_stop(mf_timer_t*timer){ if(timer) timer->end_ns=mf_platform_time_ns(); }
double mf_timer_elapsed_ms(const mf_timer_t*timer){ return timer?(double)(timer->end_ns-timer->start_ns)/1000000.0:0.0; }
void mf_random_seed(unsigned int seed){ srand(seed); }
uint32_t mf_random_u32(void){ return ((uint32_t)rand()<<16U) ^ (uint32_t)rand(); }
const char*mf_version_string(void){ return MF_VERSION_STRING; }
int mf_terminal_supports_color(FILE*stream){ const char*no=getenv("NO_COLOR"); return !no && mf_platform_is_tty(stream); }
const char*mf_ansi_color(const char*name){ if(!name) return ""; if(strcmp(name,"red")==0) return "\033[31m"; if(strcmp(name,"green")==0) return "\033[32m"; if(strcmp(name,"yellow")==0) return "\033[33m"; if(strcmp(name,"blue")==0) return "\033[34m"; if(strcmp(name,"reset")==0) return "\033[0m"; return ""; }
