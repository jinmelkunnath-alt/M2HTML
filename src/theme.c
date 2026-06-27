#include "markforge/theme.h"
mf_result_t mf_theme_init(mf_theme_t*t,const char*name){ if(!t)return MF_ERROR_INVALID; memset(t,0,sizeof(*t)); snprintf(t->name,sizeof(t->name),"%s",name?name:"default"); snprintf(t->version,sizeof(t->version),"%s","1.0"); return mf_config_init(&t->values); }
void mf_theme_destroy(mf_theme_t*t){ if(t){ mf_config_destroy(&t->values); memset(t,0,sizeof(*t)); } }
mf_result_t mf_theme_load_file(mf_theme_t*t,const char*path){ if(!t||!path)return MF_ERROR_INVALID; return mf_config_load_file(&t->values,path,MF_CONFIG_FORMAT_AUTO); }
mf_result_t mf_theme_validate(const mf_theme_t*t){ return (t&&t->name[0])?MF_OK:MF_ERROR_INVALID; }
void mf_theme_manager_init(mf_theme_manager_t*m){ if(m) (void)mf_hash_map_init(&m->themes); }
void mf_theme_manager_destroy(mf_theme_manager_t*m){ size_t i; if(!m)return; for(i=0;i<m->themes.capacity;i++) if(m->themes.entries[i].state==1){ mf_theme_destroy((mf_theme_t*)m->themes.entries[i].value); free(m->themes.entries[i].value); } mf_hash_map_destroy(&m->themes); }
mf_result_t mf_theme_manager_register(mf_theme_manager_t*m,const mf_theme_t*t){ mf_theme_t*c; if(!m||!t)return MF_ERROR_INVALID; c=(mf_theme_t*)malloc(sizeof(*c)); if(!c)return MF_ERROR_NOMEM; if(mf_theme_init(c,t->name)!=MF_OK){free(c);return MF_ERROR_NOMEM;} snprintf(c->version,sizeof(c->version),"%s",t->version); snprintf(c->parent,sizeof(c->parent),"%s",t->parent); return mf_hash_map_put(&m->themes,c->name,c); }
const mf_theme_t*mf_theme_manager_find(const mf_theme_manager_t*m,const char*name){ return (!m||!name)?NULL:(const mf_theme_t*)mf_hash_map_get(&m->themes,name); }
void mf_theme_manager_list(const mf_theme_manager_t*m,FILE*out){ size_t i; if(!out)out=stdout; if(!m)return; for(i=0;i<m->themes.capacity;i++) if(m->themes.entries[i].state==1) fprintf(out,"%s\n",m->themes.entries[i].key); }
