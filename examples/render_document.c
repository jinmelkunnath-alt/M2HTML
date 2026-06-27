#include "markforge/markforge.h"
int main(void){ mf_ast_t ast; mf_render_output_t out; if(mf_parse_string("example.md","# Hello\n\nThis is **MarkForge**.\n",&ast,NULL,NULL)!=MF_OK) return 1; mf_render_output_init_stream(&out,stdout); (void)mf_render_html(&ast,NULL,&out); mf_ast_destroy(&ast); return 0; }
