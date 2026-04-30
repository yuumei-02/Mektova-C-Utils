#pragma once

#include "types.h"

#define KiB 1024
#define MiB (1024*1024)
#define GiB (1024*1024*1024)

/// Wrapper around malloc that [panics] on allocation failure.
/// [Bytes] must be more than [0].
void* mcu_malloc(usize bytes);

/// Wrapper around realloc that [panics] on reallocation failure.
/// [new_byte_size] must be more than [0].
/// [ptr] can't be [null].
void* mcu_realloc(void* ptr, usize new_byte_size);

/// Wrapper around free.
/// [ptr] is allowed to be [null].
void mcu_free(void* ptr);

typedef struct Arena {
   void* buffer;
   usize length;
   usize capacity;
} Arena;

// Todo: Documentation
// Todo: Verify implementation
Arena Arena_new(usize bytes);
void Arena_clear(Arena* self);
void Arena_destroy(Arena* self);

void* Arena_alloc(Arena* self, usize bytes);
void Arena_pop(Arena* self, usize bytes);

