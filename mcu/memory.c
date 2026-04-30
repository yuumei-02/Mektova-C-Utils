#include "core.h"
#include "memory.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

void* mcu_malloc(usize bytes) {
   mcu_assert(bytes > 0, "Cannot allocate 0 bytes of memory");

   void* ptr = malloc(bytes);
   if (ptr == nullptr) {
      panic("Failed to allocate %zu amount of bytes, reason: %s", bytes, strerror(errno));
   }

   return ptr;
}

void* mcu_realloc(void* ptr, usize new_byte_size) {
   mcu_assert(new_byte_size > 0, "Cannot resize a pointer to be of size 0");
   
   if (ptr == nullptr) {
      panic("Unable to reallocate a nullptr");
   }

   void* new_ptr = realloc(ptr, new_byte_size);
   if (new_ptr == nullptr) {
      panic("Failed to reallocate ptr to a size of %zu bytes, reason: %s", new_byte_size, strerror(errno));
   }

   return new_ptr;
}

void mcu_free(void* ptr) {
   free(ptr);
}

Arena Arena_new(usize byte_capacity) {
   mcu_assert(byte_capacity > 0, "Can't allocate an arena of size 0");

   return (Arena) {
      .buffer = mcu_malloc(byte_capacity),
      .length = 0,
      .capacity = byte_capacity
   };
}

void Arena_clear(Arena* self) {
   mcu_assert(self != nullptr, "self can't be null");

   self->length = 0;
}

void Arena_destroy(Arena* self) {
   mcu_assert(self != nullptr, "self can't be null")

   mcu_free(self->buffer);
   self->buffer = nullptr;
   self->length = 0;
   self->capacity = 0;
}

void* Arena_alloc(Arena* self, usize bytes) {
   mcu_assert(self != nullptr, "self can't be null");
   mcu_assert(bytes > 0, "Can't allocate 0 or less bytes");
   mcu_assert(bytes <= self->capacity, "Can't allocate more bytes than the capacity of self");

   if (bytes >= self->capacity - self->length) {
      return nullptr;
   }

   void* ptr = ((u8*) self->buffer) + self->length;
   self->length += bytes;

   return ptr;
}

void Arena_pop(Arena* self, usize bytes) {
   mcu_assert(self != nullptr, "self can't be null");
   mcu_assert(bytes <= self->capacity, "Can't pop more bytes than self's byte capacity");

   self->length -= bytes > self->length
      ? self->length
      : bytes; 
}

