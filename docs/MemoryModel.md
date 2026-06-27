# Memory Model

MarkForge APIs use explicit ownership. Callers that receive heap memory from APIs such as `mf_fs_read_file` own it and release it with `free`. Containers store pointers and do not own pointed-to values unless a destroy callback is provided. Arena and pool allocations are owned by their allocator object. The tracked heap records allocations, statistics, and leak hooks for diagnostics and tests.
