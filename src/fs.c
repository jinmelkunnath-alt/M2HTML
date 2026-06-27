#include "markforge/fs.h"
#if MF_PLATFORM_WINDOWS
#include <windows.h>
#include <direct.h>
#define MF_SEP '\\'
#else
#include <dirent.h>
#include <sys/stat.h>
#define MF_SEP '/'
#endif

mf_result_t mf_fs_read_file(const char *path,char **out_data,size_t *out_len){ FILE *f; long n; char *buf; if(!path||!out_data) return MF_ERROR_INVALID; f=fopen(path,"rb"); if(!f) return MF_ERROR_IO; if(fseek(f,0,SEEK_END)!=0){fclose(f);return MF_ERROR_IO;} n=ftell(f); if(n<0){fclose(f);return MF_ERROR_IO;} rewind(f); buf=(char*)malloc((size_t)n+1U); if(!buf){fclose(f);return MF_ERROR_NOMEM;} if((size_t)n && fread(buf,1U,(size_t)n,f)!=(size_t)n){free(buf);fclose(f);return MF_ERROR_IO;} buf[n]='\0'; fclose(f); *out_data=buf; if(out_len) *out_len=(size_t)n; return MF_OK; }
mf_result_t mf_fs_write_file(const char*path,const void*data,size_t len){ FILE*f; if(!path||(!data&&len)) return MF_ERROR_INVALID; f=fopen(path,"wb"); if(!f) return MF_ERROR_IO; if(len && fwrite(data,1U,len,f)!=len){fclose(f);return MF_ERROR_IO;} return fclose(f)==0?MF_OK:MF_ERROR_IO; }
mf_result_t mf_fs_append_file(const char*path,const void*data,size_t len){ FILE*f; if(!path||(!data&&len)) return MF_ERROR_INVALID; f=fopen(path,"ab"); if(!f) return MF_ERROR_IO; if(len && fwrite(data,1U,len,f)!=len){fclose(f);return MF_ERROR_IO;} return fclose(f)==0?MF_OK:MF_ERROR_IO; }
int mf_fs_exists(const char*path){ FILE*f=fopen(path,"rb"); if(f){fclose(f);return 1;} return 0; }
mf_result_t mf_fs_temp_file(char*buffer,size_t buffer_size,const char*prefix){ unsigned long r=(unsigned long)time(NULL)^(unsigned long)rand(); if(!buffer||buffer_size==0U) return MF_ERROR_INVALID; if(snprintf(buffer,buffer_size,"%s_%lu.tmp",prefix?prefix:"markforge",r)>=(int)buffer_size) return MF_ERROR_OVERFLOW; return MF_OK; }
mf_result_t mf_path_join(char*buffer,size_t buffer_size,const char*a,const char*b){ size_t la; if(!buffer||!a||!b) return MF_ERROR_INVALID; la=strlen(a); if(snprintf(buffer,buffer_size,"%s%c%s",a,(la&&a[la-1U]==MF_SEP)?'\0':MF_SEP,b)>=(int)buffer_size) return MF_ERROR_OVERFLOW; if(la&&a[la-1U]==MF_SEP){ memmove(buffer+la, b, strlen(b)+1U); } return MF_OK; }
const char*mf_path_basename(const char*path){ const char*p; const char*last; if(!path) return ""; last=path; for(p=path;*p;p++) if(*p=='/'||*p=='\\') last=p+1; return last; }
#if MF_PLATFORM_WINDOWS
mf_result_t mf_dir_traverse(const char*path,mf_dir_visit_fn visit,void*ctx){ char pattern[MAX_PATH]; WIN32_FIND_DATAA fd; HANDLE h; if(!path||!visit) return MF_ERROR_INVALID; snprintf(pattern,sizeof(pattern),"%s\\*",path); h=FindFirstFileA(pattern,&fd); if(h==INVALID_HANDLE_VALUE) return MF_ERROR_IO; do{ char full[MAX_PATH]; if(strcmp(fd.cFileName,".")==0||strcmp(fd.cFileName,"..")==0) continue; snprintf(full,sizeof(full),"%s\\%s",path,fd.cFileName); if(visit(full,(fd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0,ctx)) break; }while(FindNextFileA(h,&fd)); FindClose(h); return MF_OK; }
#else
mf_result_t mf_dir_traverse(const char*path,mf_dir_visit_fn visit,void*ctx){ DIR*d; struct dirent*e; if(!path||!visit) return MF_ERROR_INVALID; d=opendir(path); if(!d) return MF_ERROR_IO; while((e=readdir(d))!=NULL){ char full[4096]; struct stat st; if(strcmp(e->d_name,".")==0||strcmp(e->d_name,"..")==0) continue; if(snprintf(full,sizeof(full),"%s/%s",path,e->d_name)>=(int)sizeof(full)) continue; if(stat(full,&st)!=0) continue; if(visit(full,S_ISDIR(st.st_mode),ctx)) break; } closedir(d); return MF_OK; }
#endif
typedef struct search_ctx_s{ const char*name; mf_dir_visit_fn visit; void*ctx; } search_ctx_t;
static int search_visit(const char*path,int is_dir,void*ctx){ search_ctx_t*s=(search_ctx_t*)ctx; if(strcmp(mf_path_basename(path),s->name)==0) return s->visit(path,is_dir,s->ctx); if(is_dir) (void)mf_dir_search_recursive(path,s->name,s->visit,s->ctx); return 0; }
mf_result_t mf_dir_search_recursive(const char*path,const char*name,mf_dir_visit_fn visit,void*ctx){ search_ctx_t s; if(!path||!name||!visit) return MF_ERROR_INVALID; s.name=name; s.visit=visit; s.ctx=ctx; return mf_dir_traverse(path,search_visit,&s); }
mf_result_t mf_buffered_reader_open(mf_buffered_reader_t*br,const char*path,size_t cap){ if(!br||!path) return MF_ERROR_INVALID; memset(br,0,sizeof(*br)); br->fp=fopen(path,"rb"); if(!br->fp) return MF_ERROR_IO; br->cap=cap?cap:4096U; br->buf=(unsigned char*)malloc(br->cap); if(!br->buf){fclose(br->fp);return MF_ERROR_NOMEM;} return MF_OK; }
int mf_buffered_reader_getc(mf_buffered_reader_t*br){ if(!br||!br->fp) return EOF; if(br->pos>=br->len){ br->len=fread(br->buf,1U,br->cap,br->fp); br->pos=0U; if(br->len==0U) return EOF; } return br->buf[br->pos++]; }
void mf_buffered_reader_close(mf_buffered_reader_t*br){ if(br){ if(br->fp) fclose(br->fp); free(br->buf); memset(br,0,sizeof(*br)); } }
mf_result_t mf_buffered_writer_open(mf_buffered_writer_t*bw,const char*path,size_t cap){ if(!bw||!path) return MF_ERROR_INVALID; memset(bw,0,sizeof(*bw)); bw->fp=fopen(path,"wb"); if(!bw->fp) return MF_ERROR_IO; bw->cap=cap?cap:4096U; bw->buf=(unsigned char*)malloc(bw->cap); if(!bw->buf){fclose(bw->fp);return MF_ERROR_NOMEM;} return MF_OK; }
mf_result_t mf_buffered_writer_flush(mf_buffered_writer_t*bw){ if(!bw||!bw->fp) return MF_ERROR_INVALID; if(bw->len && fwrite(bw->buf,1U,bw->len,bw->fp)!=bw->len) return MF_ERROR_IO; bw->len=0U; return MF_OK; }
mf_result_t mf_buffered_writer_write(mf_buffered_writer_t*bw,const void*data,size_t len){ const unsigned char*p=(const unsigned char*)data; if(!bw||(!data&&len)) return MF_ERROR_INVALID; while(len){ size_t room=bw->cap-bw->len; size_t n=len<room?len:room; memcpy(bw->buf+bw->len,p,n); bw->len+=n; p+=n; len-=n; if(bw->len==bw->cap && mf_buffered_writer_flush(bw)!=MF_OK) return MF_ERROR_IO; } return MF_OK; }
void mf_buffered_writer_close(mf_buffered_writer_t*bw){ if(bw){ (void)mf_buffered_writer_flush(bw); if(bw->fp) fclose(bw->fp); free(bw->buf); memset(bw,0,sizeof(*bw)); } }
