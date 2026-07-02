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

typedef struct {
   void* items;
   bool* free_list;
   usize item_size;
   usize capacity;
} SlotAllocator;

/// [item_capacity] must be greater than [0]
/// [item_size] must be greater than [0]
SlotAllocator SlotAllocator_new(usize item_capacity, usize item_size);
void SlotAllocator_destroy(SlotAllocator* self);

void* SlotAllocator_alloc(SlotAllocator* self);
void* SlotAllocator_alloc_no_panic(nullable SlotAllocator* self);
void SlotAllocator_free(nullable SlotAllocator* self, nullable void* ptr);

