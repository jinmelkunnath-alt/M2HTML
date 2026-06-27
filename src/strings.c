#include "markforge/strings.h"

static mf_result_t str_reserve(mf_string_t *s,size_t need){ char *p; size_t nc; if(need<=s->capacity) return MF_OK; nc=s->capacity?s->capacity:32U; while(nc<need) nc*=2U; p=(char*)realloc(s->data,nc); if(!p) return MF_ERROR_NOMEM; s->data=p; s->capacity=nc; return MF_OK; }
mf_result_t mf_string_init(mf_string_t *s){ if(!s) return MF_ERROR_INVALID; memset(s,0,sizeof(*s)); return str_reserve(s,1U); }
mf_result_t mf_string_from_cstr(mf_string_t *s,const char*text){ mf_result_t r=mf_string_init(s); return r==MF_OK?mf_string_assign(s,text):r; }
void mf_string_destroy(mf_string_t *s){ if(s){free(s->data); memset(s,0,sizeof(*s));} }
mf_result_t mf_string_assign(mf_string_t *s,const char*text){ size_t n; if(!s) return MF_ERROR_INVALID; n=text?strlen(text):0U; if(str_reserve(s,n+1U)!=MF_OK) return MF_ERROR_NOMEM; if(n) memcpy(s->data,text,n); s->data[n]='\0'; s->length=n; return MF_OK; }
mf_result_t mf_string_append(mf_string_t *s,const char*text){ size_t n; if(!s) return MF_ERROR_INVALID; n=text?strlen(text):0U; if(str_reserve(s,s->length+n+1U)!=MF_OK) return MF_ERROR_NOMEM; if(n) memcpy(s->data+s->length,text,n); s->length+=n; s->data[s->length]='\0'; return MF_OK; }
const char *mf_string_cstr(const mf_string_t *s){ return (s&&s->data)?s->data:""; }
mf_string_view_t mf_sv(const char*data,size_t length){ mf_string_view_t v; v.data=data; v.length=length; return v; }
mf_string_view_t mf_sv_from_cstr(const char*text){ return mf_sv(text?text:"", text?strlen(text):0U); }
int mf_sv_equal(mf_string_view_t a,mf_string_view_t b){ return a.length==b.length && (a.length==0U || memcmp(a.data,b.data,a.length)==0); }
mf_string_view_t mf_sv_trim(mf_string_view_t v){ while(v.length && isspace((unsigned char)*v.data)){v.data++;v.length--;} while(v.length && isspace((unsigned char)v.data[v.length-1U])) v.length--; return v; }
size_t mf_strlcpy(char*dst,const char*src,size_t dst_size){ size_t n=src?strlen(src):0U; if(dst_size){ size_t c=(n>=dst_size)?dst_size-1U:n; if(c) memcpy(dst,src,c); dst[c]='\0'; } return n; }
size_t mf_strlcat(char*dst,const char*src,size_t dst_size){ size_t d=0U; while(d<dst_size && dst[d]) d++; if(d==dst_size) return d+(src?strlen(src):0U); return d+mf_strlcpy(dst+d,src,dst_size-d); }
int mf_utf8_decode(const char*s,size_t len,uint32_t*cp,size_t*width){ unsigned char c; uint32_t out; size_t w; if(!s||len==0U) return 0; c=(unsigned char)s[0]; if(c<0x80U){out=c;w=1U;} else if((c&0xE0U)==0xC0U && len>=2U){out=c&0x1FU;w=2U;} else if((c&0xF0U)==0xE0U && len>=3U){out=c&0x0FU;w=3U;} else if((c&0xF8U)==0xF0U && len>=4U){out=c&0x07U;w=4U;} else return 0; { size_t i; for(i=1U;i<w;i++){ unsigned char t=(unsigned char)s[i]; if((t&0xC0U)!=0x80U) return 0; out=(out<<6U)|(uint32_t)(t&0x3FU);} } if((w==2U&&out<0x80U)||(w==3U&&out<0x800U)||(w==4U&&out<0x10000U)||out>0x10FFFFU||(out>=0xD800U&&out<=0xDFFFU)) return 0; if(cp) *cp=out; if(width) *width=w; return 1; }
int mf_utf8_validate(const char*s,size_t len){ size_t off=0U,w=0U; while(off<len){ if(!mf_utf8_decode(s+off,len-off,NULL,&w)) return 0; off+=w; } return 1; }
void mf_utf8_iter_init(mf_utf8_iter_t*it,mf_string_view_t view){ if(it){it->view=view;it->offset=0U;it->codepoint=0U;it->width=0U;it->valid=0;} }
int mf_utf8_iter_next(mf_utf8_iter_t*it){ if(!it||it->offset>=it->view.length) return 0; it->valid=mf_utf8_decode(it->view.data+it->offset,it->view.length-it->offset,&it->codepoint,&it->width); if(!it->valid){it->width=1U;it->codepoint=0xFFFDU;} it->offset+=it->width; return 1; }
