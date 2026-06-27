#include "markforge/markforge.h"
#include "markforge/test.h"
void test_fs_module(void){ char path[64]; char *data=NULL; size_t len=0U; snprintf(path,sizeof(path),"markforge_test_%lu.txt",(unsigned long)time(NULL)); MF_TEST_ASSERT_EQ_INT(mf_fs_write_file(path,"abc",3U),MF_OK); MF_TEST_ASSERT(mf_fs_exists(path)); MF_TEST_ASSERT_EQ_INT(mf_fs_read_file(path,&data,&len),MF_OK); MF_TEST_ASSERT_EQ_INT((int)len,3); MF_TEST_ASSERT_STR(data,"abc"); free(data); remove(path); }
