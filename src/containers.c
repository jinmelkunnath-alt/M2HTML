#include "markforge/containers.h"

static char *mf_strdup_local(const char *s) { size_t n; char *p; if (!s) return NULL; n = strlen(s) + 1U; p = (char *)malloc(n); if (p) memcpy(p, s, n); return p; }
static mf_result_t grow_ptr_array(void ***items, size_t *cap, size_t need) {
    void **p; size_t nc = *cap ? *cap : 8U; while (nc < need) nc *= 2U; p = (void **)realloc(*items, nc * sizeof(void *)); if (!p) return MF_ERROR_NOMEM; *items = p; *cap = nc; return MF_OK;
}
mf_result_t mf_vector_init(mf_vector_t *v) { if (!v) return MF_ERROR_INVALID; memset(v,0,sizeof(*v)); return MF_OK; }
void mf_vector_destroy(mf_vector_t *v) { if (v) { free(v->items); memset(v,0,sizeof(*v)); } }
mf_result_t mf_vector_push(mf_vector_t *v, void *item) { if (!v) return MF_ERROR_INVALID; if (v->size == v->capacity && grow_ptr_array(&v->items,&v->capacity,v->size+1U)!=MF_OK) return MF_ERROR_NOMEM; v->items[v->size++] = item; return MF_OK; }
void *mf_vector_pop(mf_vector_t *v) { if (!v || v->size==0U) return NULL; return v->items[--v->size]; }
void *mf_vector_get(const mf_vector_t *v, size_t index) { return (!v || index>=v->size) ? NULL : v->items[index]; }
mf_result_t mf_vector_set(mf_vector_t *v, size_t index, void *item) { if (!v || index>=v->size) return MF_ERROR_INVALID; v->items[index]=item; return MF_OK; }
size_t mf_vector_size(const mf_vector_t *v) { return v ? v->size : 0U; }
mf_result_t mf_stack_init(mf_stack_t *s) { return mf_vector_init(s); }
void mf_stack_destroy(mf_stack_t *s) { mf_vector_destroy(s); }
mf_result_t mf_stack_push(mf_stack_t *s, void *item) { return mf_vector_push(s,item); }
void *mf_stack_pop(mf_stack_t *s) { return mf_vector_pop(s); }
void *mf_stack_peek(const mf_stack_t *s) { return (!s || s->size==0U) ? NULL : s->items[s->size-1U]; }

static mf_result_t queue_grow(mf_queue_t *q) { void **n; size_t nc,i; nc=q->capacity?q->capacity*2U:8U; n=(void**)calloc(nc,sizeof(void*)); if(!n) return MF_ERROR_NOMEM; for(i=0U;i<q->size;i++) n[i]=q->items[(q->head+i)%q->capacity]; free(q->items); q->items=n; q->capacity=nc; q->head=0U; q->tail=q->size; return MF_OK; }
mf_result_t mf_queue_init(mf_queue_t *q) { if(!q) return MF_ERROR_INVALID; memset(q,0,sizeof(*q)); return queue_grow(q); }
void mf_queue_destroy(mf_queue_t *q) { if(q){free(q->items); memset(q,0,sizeof(*q));} }
mf_result_t mf_queue_push(mf_queue_t *q, void *item) { if(!q) return MF_ERROR_INVALID; if(q->size==q->capacity && queue_grow(q)!=MF_OK) return MF_ERROR_NOMEM; q->items[q->tail]=item; q->tail=(q->tail+1U)%q->capacity; q->size++; return MF_OK; }
void *mf_queue_pop(mf_queue_t *q) { void *x; if(!q||q->size==0U) return NULL; x=q->items[q->head]; q->head=(q->head+1U)%q->capacity; q->size--; return x; }
size_t mf_queue_size(const mf_queue_t *q) { return q?q->size:0U; }

void mf_list_init(mf_list_t *list) { if(list) memset(list,0,sizeof(*list)); }
void mf_list_destroy(mf_list_t *list, void (*free_value)(void *)) { mf_list_node_t *n,*next; if(!list) return; n=list->head; while(n){ next=n->next; if(free_value) free_value(n->value); free(n); n=next; } memset(list,0,sizeof(*list)); }
mf_result_t mf_list_push_back(mf_list_t *list, void *value) { mf_list_node_t *n; if(!list) return MF_ERROR_INVALID; n=(mf_list_node_t*)calloc(1U,sizeof(*n)); if(!n) return MF_ERROR_NOMEM; n->value=value; n->prev=list->tail; if(list->tail) list->tail->next=n; else list->head=n; list->tail=n; list->size++; return MF_OK; }
void *mf_list_pop_front(mf_list_t *list) { mf_list_node_t *n; void *v; if(!list||!list->head) return NULL; n=list->head; v=n->value; list->head=n->next; if(list->head) list->head->prev=NULL; else list->tail=NULL; free(n); list->size--; return v; }
size_t mf_list_size(const mf_list_t *list) { return list?list->size:0U; }

static unsigned long hash_str(const char *s) { unsigned long h=1469598103934665603UL; while(*s){ h^=(unsigned char)*s++; h*=1099511628211UL; } return h; }
static mf_result_t map_alloc(mf_hash_map_t *m, size_t cap) { m->entries=(mf_hash_entry_t*)calloc(cap,sizeof(mf_hash_entry_t)); if(!m->entries) return MF_ERROR_NOMEM; m->capacity=cap; m->size=0U; return MF_OK; }
mf_result_t mf_hash_map_init(mf_hash_map_t *map) { if(!map) return MF_ERROR_INVALID; memset(map,0,sizeof(*map)); return map_alloc(map,32U); }
void mf_hash_map_destroy(mf_hash_map_t *map) { size_t i; if(!map) return; for(i=0U;i<map->capacity;i++) if(map->entries[i].state==1) free(map->entries[i].key); free(map->entries); memset(map,0,sizeof(*map)); }
static mf_result_t map_rehash(mf_hash_map_t *m) { mf_hash_entry_t *old; size_t oc,i; old=m->entries; oc=m->capacity; m->entries=NULL; if(map_alloc(m,oc*2U)!=MF_OK){m->entries=old;m->capacity=oc;return MF_ERROR_NOMEM;} for(i=0U;i<oc;i++){ if(old[i].state==1){ (void)mf_hash_map_put(m,old[i].key,old[i].value); free(old[i].key);} } free(old); return MF_OK; }
static size_t map_find_slot(const mf_hash_map_t *m,const char*key,int *found){ size_t idx,first_del,i; idx=(size_t)(hash_str(key)%m->capacity); first_del=(size_t)-1; for(i=0U;i<m->capacity;i++){ size_t s=(idx+i)%m->capacity; if(m->entries[s].state==1 && strcmp(m->entries[s].key,key)==0){*found=1;return s;} if(m->entries[s].state==2 && first_del==(size_t)-1) first_del=s; if(m->entries[s].state==0){*found=0;return first_del!=(size_t)-1?first_del:s;} } *found=0; return first_del!=(size_t)-1?first_del:0U; }
mf_result_t mf_hash_map_put(mf_hash_map_t *map, const char *key, void *value) { int found; size_t s; if(!map||!key) return MF_ERROR_INVALID; if((map->size+1U)*100U/map->capacity>70U && map_rehash(map)!=MF_OK) return MF_ERROR_NOMEM; s=map_find_slot(map,key,&found); if(found){ map->entries[s].value=value; return MF_OK; } map->entries[s].key=mf_strdup_local(key); if(!map->entries[s].key) return MF_ERROR_NOMEM; map->entries[s].value=value; map->entries[s].state=1; map->size++; return MF_OK; }
void *mf_hash_map_get(const mf_hash_map_t *map, const char *key) { int f; size_t s; if(!map||!key||!map->entries) return NULL; s=map_find_slot(map,key,&f); return f?map->entries[s].value:NULL; }
int mf_hash_map_contains(const mf_hash_map_t *map, const char *key) { int f; if(!map||!key||!map->entries) return 0; (void)map_find_slot(map,key,&f); return f; }
mf_result_t mf_hash_map_remove(mf_hash_map_t *map, const char *key) { int f; size_t s; if(!map||!key) return MF_ERROR_INVALID; s=map_find_slot(map,key,&f); if(!f) return MF_ERROR_NOT_FOUND; free(map->entries[s].key); map->entries[s].key=NULL; map->entries[s].value=NULL; map->entries[s].state=2; map->size--; return MF_OK; }
size_t mf_hash_map_size(const mf_hash_map_t *map) { return map?map->size:0U; }
mf_result_t mf_hash_set_init(mf_hash_set_t *set){return mf_hash_map_init(set);} void mf_hash_set_destroy(mf_hash_set_t *set){mf_hash_map_destroy(set);} mf_result_t mf_hash_set_add(mf_hash_set_t *set,const char *key){return mf_hash_map_put(set,key,(void*)set);} int mf_hash_set_contains(const mf_hash_set_t *set,const char *key){return mf_hash_map_contains(set,key);} mf_result_t mf_hash_set_remove(mf_hash_set_t *set,const char *key){return mf_hash_map_remove(set,key);}

static mf_result_t sb_reserve(mf_string_builder_t *sb,size_t need){ char *p; size_t nc; if(need<=sb->capacity) return MF_OK; nc=sb->capacity?sb->capacity:64U; while(nc<need) nc*=2U; p=(char*)realloc(sb->data,nc); if(!p) return MF_ERROR_NOMEM; sb->data=p; sb->capacity=nc; return MF_OK; }
mf_result_t mf_sb_init(mf_string_builder_t *sb){ if(!sb) return MF_ERROR_INVALID; memset(sb,0,sizeof(*sb)); return sb_reserve(sb,1U); }
void mf_sb_destroy(mf_string_builder_t *sb){ if(sb){free(sb->data); memset(sb,0,sizeof(*sb));} }
mf_result_t mf_sb_append_n(mf_string_builder_t *sb,const char*text,size_t n){ if(!sb||(!text&&n)) return MF_ERROR_INVALID; if(sb_reserve(sb,sb->length+n+1U)!=MF_OK) return MF_ERROR_NOMEM; if(n) memcpy(sb->data+sb->length,text,n); sb->length+=n; sb->data[sb->length]='\0'; return MF_OK; }
mf_result_t mf_sb_append(mf_string_builder_t *sb,const char*text){ return mf_sb_append_n(sb,text, text?strlen(text):0U); }
mf_result_t mf_sb_append_char(mf_string_builder_t *sb,char c){ return mf_sb_append_n(sb,&c,1U); }
const char *mf_sb_cstr(mf_string_builder_t *sb){ if(!sb) return ""; if(!sb->data && mf_sb_init(sb)!=MF_OK) return ""; sb->data[sb->length]='\0'; return sb->data; }
char *mf_sb_take(mf_string_builder_t *sb){ char *p; if(!sb) return NULL; p=sb->data; memset(sb,0,sizeof(*sb)); return p; }
