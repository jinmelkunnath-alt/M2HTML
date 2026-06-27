#include "markforge/renderer_manager.h"

static char *dup_s(const char *s) { size_t n; char *p; if(!s) return NULL; n=strlen(s)+1U; p=(char*)malloc(n); if(p) memcpy(p,s,n); return p; }
static mf_result_t call_html(const mf_ast_t*a,const mf_render_options_t*o,mf_render_output_t*out){ return mf_render_html(a,o,out); }
static mf_result_t call_json(const mf_ast_t*a,const mf_render_options_t*o,mf_render_output_t*out){ return mf_render_json(a,o,out,NULL); }
static mf_result_t call_xml(const mf_ast_t*a,const mf_render_options_t*o,mf_render_output_t*out){ return mf_render_xml(a,o,out); }
static mf_result_t call_text(const mf_ast_t*a,const mf_render_options_t*o,mf_render_output_t*out){ return mf_render_text(a,o,out); }
static mf_result_t call_md(const mf_ast_t*a,const mf_render_options_t*o,mf_render_output_t*out){ return mf_format_markdown(a,o,out); }

mf_result_t mf_renderer_manager_init(mf_renderer_manager_t *m){ if(!m)return MF_ERROR_INVALID; return mf_vector_init(&m->entries); }
void mf_renderer_manager_destroy(mf_renderer_manager_t *m){ size_t i; if(!m)return; for(i=0;i<m->entries.size;i++){ mf_renderer_entry_t*e=(mf_renderer_entry_t*)m->entries.items[i]; if(e){ free((char*)e->name); free(e);} } mf_vector_destroy(&m->entries); }
mf_result_t mf_renderer_manager_register(mf_renderer_manager_t*m,const char*name,mf_render_format_t fmt,int pri,mf_registered_renderer_fn fn){ mf_renderer_entry_t*e; if(!m||!name||!fn)return MF_ERROR_INVALID; e=(mf_renderer_entry_t*)calloc(1U,sizeof(*e)); if(!e)return MF_ERROR_NOMEM; e->name=dup_s(name); e->format=fmt; e->priority=pri; e->render=fn; if(!e->name){free(e);return MF_ERROR_NOMEM;} return mf_vector_push(&m->entries,e); }
const mf_renderer_entry_t *mf_renderer_manager_find(const mf_renderer_manager_t*m,const char*name){ size_t i; const mf_renderer_entry_t*best=NULL; if(!m||!name)return NULL; for(i=0;i<m->entries.size;i++){ const mf_renderer_entry_t*e=(const mf_renderer_entry_t*)m->entries.items[i]; if(strcmp(e->name,name)==0 && (!best||e->priority>best->priority)) best=e; } return best; }
mf_result_t mf_renderer_manager_render(const mf_renderer_manager_t*m,const char*name,const mf_ast_t*ast,const mf_render_options_t*o,mf_render_output_t*out){ const mf_renderer_entry_t*e=mf_renderer_manager_find(m,name); if(!e)return MF_ERROR_NOT_FOUND; return e->render(ast,o,out); }
mf_result_t mf_renderer_manager_register_builtin(mf_renderer_manager_t*m){ mf_result_t r; if((r=mf_renderer_manager_register(m,"html",MF_RENDER_HTML,100,call_html))!=MF_OK)return r; if((r=mf_renderer_manager_register(m,"json",MF_RENDER_JSON,100,call_json))!=MF_OK)return r; if((r=mf_renderer_manager_register(m,"xml",MF_RENDER_XML,100,call_xml))!=MF_OK)return r; if((r=mf_renderer_manager_register(m,"text",MF_RENDER_TEXT,100,call_text))!=MF_OK)return r; return mf_renderer_manager_register(m,"markdown",MF_RENDER_MARKDOWN,100,call_md); }
void mf_renderer_manager_list(const mf_renderer_manager_t*m,FILE*out){ size_t i; if(!out)out=stdout; if(!m)return; for(i=0;i<m->entries.size;i++){ const mf_renderer_entry_t*e=(const mf_renderer_entry_t*)m->entries.items[i]; fprintf(out,"%s priority=%d\n",e->name,e->priority); } }
