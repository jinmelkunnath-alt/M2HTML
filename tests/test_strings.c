#include "markforge/markforge.h"
#include "markforge/test.h"
void test_strings_module(void){ mf_string_t s; mf_utf8_iter_t it; MF_TEST_ASSERT_EQ_INT(mf_string_from_cstr(&s,"hi"),MF_OK); mf_string_append(&s,"!"); MF_TEST_ASSERT_STR(mf_string_cstr(&s),"hi!"); MF_TEST_ASSERT(mf_utf8_validate("hello",5U)); mf_utf8_iter_init(&it,mf_sv_from_cstr("A")); MF_TEST_ASSERT(mf_utf8_iter_next(&it)); MF_TEST_ASSERT_EQ_INT((int)it.codepoint,65); mf_string_destroy(&s); }
