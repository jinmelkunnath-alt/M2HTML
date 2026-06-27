#include "markforge/memory.h"

typedef struct mf_heap_record_s {
    void *ptr;
    size_t size;
    char tag[32];
    struct mf_heap_record_s *next;
} mf_heap_record_t;

struct mf_heap_s {
    mf_heap_record_t *records;
    mf_memory_stats_t stats;
    mf_leak_hook_fn leak_hook;
    void *leak_ctx;
};

static void *system_alloc(void *ctx, size_t size, size_t alignment) {
    MF_UNUSED(ctx);
    MF_UNUSED(alignment);
    return malloc(size ? size : 1U);
}
static void system_free(void *ctx, void *ptr) { MF_UNUSED(ctx); free(ptr); }

mf_allocator_t mf_allocator_system(void) {
    mf_allocator_t a;
    a.alloc = system_alloc;
    a.free = system_free;
    a.ctx = NULL;
    return a;
}

void *mf_alloc(size_t size) { return malloc(size ? size : 1U); }
void *mf_calloc(size_t count, size_t size) { return calloc(count ? count : 1U, size ? size : 1U); }
void *mf_realloc(void *ptr, size_t size) { return realloc(ptr, size ? size : 1U); }
void mf_free(void *ptr) { free(ptr); }

int mf_is_power_of_two(size_t value) { return value != 0U && (value & (value - 1U)) == 0U; }
size_t mf_align_forward(size_t value, size_t alignment) {
    size_t mask;
    if (!mf_is_power_of_two(alignment)) return value;
    mask = alignment - 1U;
    return (value + mask) & ~mask;
}

void *mf_aligned_alloc(size_t size, size_t alignment) {
    void *raw;
    uintptr_t aligned;
    size_t total;
    if (!mf_is_power_of_two(alignment) || alignment < sizeof(void *)) alignment = sizeof(void *);
    total = size + alignment - 1U + sizeof(void *);
    raw = malloc(total ? total : 1U);
    if (!raw) return NULL;
    aligned = ((uintptr_t)raw + sizeof(void *) + alignment - 1U) & ~((uintptr_t)alignment - 1U);
    ((void **)aligned)[-1] = raw;
    return (void *)aligned;
}
void mf_aligned_free(void *ptr) { if (ptr) free(((void **)ptr)[-1]); }

mf_result_t mf_arena_init(mf_arena_t *arena, size_t capacity, mf_allocator_t backing) {
    if (!arena || capacity == 0U) return MF_ERROR_INVALID;
    if (!backing.alloc) backing = mf_allocator_system();
    arena->buffer = (unsigned char *)backing.alloc(backing.ctx, capacity, sizeof(void *));
    if (!arena->buffer) return MF_ERROR_NOMEM;
    arena->capacity = capacity;
    arena->offset = 0U;
    arena->backing = backing;
    return MF_OK;
}
void mf_arena_destroy(mf_arena_t *arena) {
    if (arena && arena->buffer) {
        arena->backing.free(arena->backing.ctx, arena->buffer);
        arena->buffer = NULL;
        arena->capacity = 0U;
        arena->offset = 0U;
    }
}
void *mf_arena_alloc(mf_arena_t *arena, size_t size, size_t alignment) {
    size_t next;
    size_t aligned;
    if (!arena || !arena->buffer) return NULL;
    if (alignment == 0U) alignment = sizeof(void *);
    aligned = mf_align_forward(arena->offset, alignment);
    next = aligned + size;
    if (next < aligned || next > arena->capacity) return NULL;
    arena->offset = next;
    return arena->buffer + aligned;
}
void mf_arena_reset(mf_arena_t *arena) { if (arena) arena->offset = 0U; }
size_t mf_arena_used(const mf_arena_t *arena) { return arena ? arena->offset : 0U; }

mf_result_t mf_pool_init(mf_pool_t *pool, size_t block_size, size_t block_count, mf_allocator_t backing) {
    size_t i;
    unsigned char *p;
    if (!pool || block_size < sizeof(void *) || block_count == 0U) return MF_ERROR_INVALID;
    if (!backing.alloc) backing = mf_allocator_system();
    block_size = mf_align_forward(block_size, sizeof(void *));
    if (block_size != 0U && block_count > ((size_t)-1) / block_size) return MF_ERROR_OVERFLOW;
    pool->buffer = (unsigned char *)backing.alloc(backing.ctx, block_size * block_count, sizeof(void *));
    if (!pool->buffer) return MF_ERROR_NOMEM;
    pool->block_size = block_size;
    pool->block_count = block_count;
    pool->used_count = 0U;
    pool->backing = backing;
    pool->free_list = pool->buffer;
    for (i = 0U; i < block_count; ++i) {
        p = pool->buffer + i * block_size;
        *((void **)p) = (i + 1U < block_count) ? (void *)(p + block_size) : NULL;
    }
    return MF_OK;
}
void mf_pool_destroy(mf_pool_t *pool) {
    if (pool && pool->buffer) {
        pool->backing.free(pool->backing.ctx, pool->buffer);
        memset(pool, 0, sizeof(*pool));
    }
}
void *mf_pool_alloc(mf_pool_t *pool) {
    void *p;
    if (!pool || !pool->free_list) return NULL;
    p = pool->free_list;
    pool->free_list = *((void **)p);
    pool->used_count++;
    return p;
}
void mf_pool_free(mf_pool_t *pool, void *ptr) {
    uintptr_t start;
    uintptr_t end;
    uintptr_t p;
    if (!pool || !ptr || !pool->buffer) return;
    start = (uintptr_t)pool->buffer;
    end = start + pool->block_size * pool->block_count;
    p = (uintptr_t)ptr;
    if (p < start || p >= end || ((p - start) % pool->block_size) != 0U) return;
    *((void **)ptr) = pool->free_list;
    pool->free_list = ptr;
    if (pool->used_count > 0U) pool->used_count--;
}
size_t mf_pool_used(const mf_pool_t *pool) { return pool ? pool->used_count : 0U; }

mf_result_t mf_heap_init(mf_heap_t **heap) {
    if (!heap) return MF_ERROR_INVALID;
    *heap = (mf_heap_t *)calloc(1U, sizeof(mf_heap_t));
    return *heap ? MF_OK : MF_ERROR_NOMEM;
}
static mf_heap_record_t *find_record(mf_heap_t *heap, void *ptr, mf_heap_record_t **prev) {
    mf_heap_record_t *r;
    if (prev) *prev = NULL;
    for (r = heap ? heap->records : NULL; r; r = r->next) {
        if (r->ptr == ptr) return r;
        if (prev) *prev = r;
    }
    return NULL;
}
void *mf_heap_alloc(mf_heap_t *heap, size_t size, const char *tag) {
    mf_heap_record_t *r;
    void *p;
    if (!heap) return NULL;
    p = malloc(size ? size : 1U);
    if (!p) return NULL;
    r = (mf_heap_record_t *)malloc(sizeof(*r));
    if (!r) { free(p); return NULL; }
    r->ptr = p;
    r->size = size ? size : 1U;
    strncpy(r->tag, tag ? tag : "untagged", sizeof(r->tag)-1U);
    r->tag[sizeof(r->tag)-1U] = '\0';
    r->next = heap->records;
    heap->records = r;
    heap->stats.alloc_count++;
    heap->stats.bytes_current += r->size;
    heap->stats.bytes_total += r->size;
    if (heap->stats.bytes_current > heap->stats.bytes_peak) heap->stats.bytes_peak = heap->stats.bytes_current;
    return p;
}
void *mf_heap_calloc(mf_heap_t *heap, size_t count, size_t size, const char *tag) {
    void *p;
    if (size != 0U && count > ((size_t)-1) / size) return NULL;
    p = mf_heap_alloc(heap, count * size, tag);
    if (p) memset(p, 0, count * size);
    return p;
}
void mf_heap_free(mf_heap_t *heap, void *ptr) {
    mf_heap_record_t *r;
    mf_heap_record_t *prev;
    if (!heap || !ptr) return;
    r = find_record(heap, ptr, &prev);
    if (!r) return;
    if (prev) prev->next = r->next; else heap->records = r->next;
    heap->stats.free_count++;
    if (heap->stats.bytes_current >= r->size) heap->stats.bytes_current -= r->size;
    free(r->ptr);
    free(r);
}
mf_memory_stats_t mf_heap_stats(const mf_heap_t *heap) {
    mf_memory_stats_t z;
    memset(&z, 0, sizeof(z));
    return heap ? heap->stats : z;
}
void mf_heap_set_leak_hook(mf_heap_t *heap, mf_leak_hook_fn hook, void *ctx) { if (heap) { heap->leak_hook = hook; heap->leak_ctx = ctx; } }
size_t mf_heap_report_leaks(mf_heap_t *heap) {
    size_t count = 0U;
    mf_heap_record_t *r;
    for (r = heap ? heap->records : NULL; r; r = r->next) {
        count++;
        if (heap->leak_hook) heap->leak_hook(r->ptr, r->size, r->tag, heap->leak_ctx);
    }
    return count;
}
void mf_heap_destroy(mf_heap_t *heap) {
    mf_heap_record_t *r;
    mf_heap_record_t *n;
    if (!heap) return;
    (void)mf_heap_report_leaks(heap);
    r = heap->records;
    while (r) { n = r->next; free(r->ptr); free(r); r = n; }
    free(heap);
}
