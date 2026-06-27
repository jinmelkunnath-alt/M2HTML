#include "markforge/markforge.h"
#include "markforge/test.h"
void test_cli_module(void){ mf_cli_app_t app; int flag=0; char *argvv[]={(char*)"mf",(char*)"--flag"}; mf_cli_option_t opt={"flag",'f',MF_CLI_BOOL,"flag",&flag}; mf_cli_init(&app,"mf","0","help"); mf_cli_set_options(&app,&opt,1U); MF_TEST_ASSERT_EQ_INT(mf_cli_run(&app,2,argvv),0); MF_TEST_ASSERT_EQ_INT(flag,1); }
