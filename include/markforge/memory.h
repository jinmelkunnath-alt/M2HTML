#ifndef MARKFORGE_MEMORY_H
#define MARKFORGE_MEMORY_H
#include "markforge/common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void *(*mf_alloc_fn)(void *ctx, size_t size, size_t alignment);
typedef void (*mf_free_fn)(void *ctx, void *ptr);
typedef void (*mf_leak_hook_fn)(void *ptr, size_t size, const char *tag, void *ctx);

typedef struct mf_allocator_s {
    mf_alloc_fn alloc;
    mf_free_fn free;
    void *ctx;
} mf_allocator_t;

typedef struct mf_memory_stats_s {
    size_t alloc_count;
    size_t free_count;
    size_t bytes_current;
    size_t bytes_peak;
    size_t bytes_total;
} mf_memory_stats_t;

typedef struct mf_arena_s {
    unsigned char *buffer;
    size_t capacity;
    size_t offset;
    mf_allocator_t backing;
} mf_arena_t;

typedef struct mf_pool_s {
    unsigned char *buffer;
    void *free_list;
    size_t block_size;
    size_t block_count;
    size_t used_count;
    mf_allocator_t backing;
} mf_pool_t;

typedef struct mf_heap_s mf_heap_t;

mf_allocator_t mf_allocator_system(void);
void *mf_alloc(size_t size);
void *mf_calloc(size_t count, size_t size);
void *mf_realloc(void *ptr, size_t size);
void mf_free(void *ptr);
void *mf_aligned_alloc(size_t size, size_t alignment);
void mf_aligned_free(void *ptr);
int mf_is_power_of_two(size_t value);
size_t mf_align_forward(size_t value, size_t alignment);

mf_result_t mf_arena_init(mf_arena_t *arena, size_t capacity, mf_allocator_t backing);
void mf_arena_destroy(mf_arena_t *arena);
void *mf_arena_alloc(mf_arena_t *arena, size_t size, size_t alignment);
void mf_arena_reset(mf_arena_t *arena);
size_t mf_arena_used(const mf_arena_t *arena);

mf_result_t mf_pool_init(mf_pool_t *pool, size_t block_size, size_t block_count, mf_allocator_t backing);
void mf_pool_destroy(mf_pool_t *pool);
void *mf_pool_alloc(mf_pool_t *pool);
void mf_pool_free(mf_pool_t *pool, void *ptr);
size_t mf_pool_used(const mf_pool_t *pool);

mf_result_t mf_heap_init(mf_heap_t **heap);
void mf_heap_destroy(mf_heap_t *heap);
void *mf_heap_alloc(mf_heap_t *heap, size_t size, const char *tag);
void *mf_heap_calloc(mf_heap_t *heap, size_t count, size_t size, const char *tag);
void mf_heap_free(mf_heap_t *heap, void *ptr);
mf_memory_stats_t mf_heap_stats(const mf_heap_t *heap);
void mf_heap_set_leak_hook(mf_heap_t *heap, mf_leak_hook_fn hook, void *ctx);
size_t mf_heap_report_leaks(mf_heap_t *heap);

#ifdef __cplusplus
}
#endif
#endif
