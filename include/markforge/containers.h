#ifndef MARKFORGE_CONTAINERS_H
#define MARKFORGE_CONTAINERS_H
#include "markforge/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct mf_vector_s { void **items; size_t size; size_t capacity; } mf_vector_t;
typedef mf_vector_t mf_stack_t;
typedef struct mf_queue_s { void **items; size_t capacity; size_t head; size_t tail; size_t size; } mf_queue_t;
typedef struct mf_list_node_s { void *value; struct mf_list_node_s *next; struct mf_list_node_s *prev; } mf_list_node_t;
typedef struct mf_list_s { mf_list_node_t *head; mf_list_node_t *tail; size_t size; } mf_list_t;
typedef struct mf_hash_entry_s { char *key; void *value; int state; } mf_hash_entry_t;
typedef struct mf_hash_map_s { mf_hash_entry_t *entries; size_t capacity; size_t size; } mf_hash_map_t;
typedef mf_hash_map_t mf_hash_set_t;
typedef struct mf_string_builder_s { char *data; size_t length; size_t capacity; } mf_string_builder_t;

mf_result_t mf_vector_init(mf_vector_t *v);
void mf_vector_destroy(mf_vector_t *v);
mf_result_t mf_vector_push(mf_vector_t *v, void *item);
void *mf_vector_pop(mf_vector_t *v);
void *mf_vector_get(const mf_vector_t *v, size_t index);
mf_result_t mf_vector_set(mf_vector_t *v, size_t index, void *item);
size_t mf_vector_size(const mf_vector_t *v);

mf_result_t mf_stack_init(mf_stack_t *s);
void mf_stack_destroy(mf_stack_t *s);
mf_result_t mf_stack_push(mf_stack_t *s, void *item);
void *mf_stack_pop(mf_stack_t *s);
void *mf_stack_peek(const mf_stack_t *s);

mf_result_t mf_queue_init(mf_queue_t *q);
void mf_queue_destroy(mf_queue_t *q);
mf_result_t mf_queue_push(mf_queue_t *q, void *item);
void *mf_queue_pop(mf_queue_t *q);
size_t mf_queue_size(const mf_queue_t *q);

void mf_list_init(mf_list_t *list);
void mf_list_destroy(mf_list_t *list, void (*free_value)(void *));
mf_result_t mf_list_push_back(mf_list_t *list, void *value);
void *mf_list_pop_front(mf_list_t *list);
size_t mf_list_size(const mf_list_t *list);

mf_result_t mf_hash_map_init(mf_hash_map_t *map);
void mf_hash_map_destroy(mf_hash_map_t *map);
mf_result_t mf_hash_map_put(mf_hash_map_t *map, const char *key, void *value);
void *mf_hash_map_get(const mf_hash_map_t *map, const char *key);
int mf_hash_map_contains(const mf_hash_map_t *map, const char *key);
mf_result_t mf_hash_map_remove(mf_hash_map_t *map, const char *key);
size_t mf_hash_map_size(const mf_hash_map_t *map);

mf_result_t mf_hash_set_init(mf_hash_set_t *set);
void mf_hash_set_destroy(mf_hash_set_t *set);
mf_result_t mf_hash_set_add(mf_hash_set_t *set, const char *key);
int mf_hash_set_contains(const mf_hash_set_t *set, const char *key);
mf_result_t mf_hash_set_remove(mf_hash_set_t *set, const char *key);

mf_result_t mf_sb_init(mf_string_builder_t *sb);
void mf_sb_destroy(mf_string_builder_t *sb);
mf_result_t mf_sb_append(mf_string_builder_t *sb, const char *text);
mf_result_t mf_sb_append_n(mf_string_builder_t *sb, const char *text, size_t n);
mf_result_t mf_sb_append_char(mf_string_builder_t *sb, char c);
const char *mf_sb_cstr(mf_string_builder_t *sb);
char *mf_sb_take(mf_string_builder_t *sb);

#ifdef __cplusplus
}
#endif
#endif
