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

typedef enum : i32 {
   MP_Exec  = 1,
   MP_Read  = 2,
   MP_Write = 4,
   MP_None  = 0,
} MemoryProtection;

typedef struct Arena {
   u8* buffer;
   usize length;
   usize capacity;
} Arena;

typedef struct {
   MemoryProtection protection;
} ArenaSettings;

#define Arena_new(capacity, ...) \
   Arena_new_ex(capacity, (ArenaSettings) { \
      .protection = MP_Read | MP_Write \
      __VA_OPT__(,) __VA_ARGS__ \
   })

Arena Arena_new_ex(usize capacity, ArenaSettings settings);
void Arena_delete(nullable Arena* self);

void* Arena_alloc(Arena* self, usize bytes);
/// Same as [Arena_alloc] except that this version returns [null] on allocation failure.
void* Arena_alloc_non_panic(Arena* self, usize bytes);

/// [bytes] is allowed to be more than the [arena's length].
void Arena_free(nullable Arena* self, usize bytes);
void Arena_clear(nullable Arena* self);

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

