#include "core.h"
#include "memory.h"

#include <sys/mman.h>

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

Arena Arena_new_ex(usize capacity, ArenaSettings settings) {
   Arena self;
   self.length = 0;
   self.capacity = capacity;

   i32 prot = 0;
   if (settings.protection & MP_None) {
      prot = PROT_NONE;
   } else {
      prot = settings.protection & MP_Exec  ? prot | PROT_EXEC  : prot;
      prot = settings.protection & MP_Read  ? prot | PROT_READ  : prot;
      prot = settings.protection & MP_Write ? prot | PROT_WRITE : prot;
   }
   
   self.buffer = mmap(nullptr, capacity, prot, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
   if (self.buffer == MAP_FAILED) {
      panic("[!] Failed to allocate memory for arena of size \"%zu\", reason: \"%s\"", capacity, strerror(errno));
   }
   
   return self;
}

void Arena_destroy(nullable Arena* self) {
   if (self == nullptr) return;

   munmap(self->buffer, self->capacity);
   *self = (Arena) {0};
}

void* Arena_alloc(Arena* self, usize bytes) {
   mcu_assert(self != nullptr, "self can't be null");

   if (bytes > (self->capacity - self->length)) {
      panic("Allocation from arena failed with OOM");
   }

   void* data = self->buffer + self->length;
   self->length += bytes;

   return data;
}

/// Same as [Arena_alloc] except that this version returns [null] on allocation failure.
void* Arena_alloc_non_panic(Arena* self, usize bytes) {
   mcu_assert(self != nullptr, "self can't be null");

   if (bytes > (self->capacity - self->length)) {
      return nullptr;
   }

   void* data = self->buffer + self->length;
   self->length += bytes;
   
   return data;
}

/// [bytes] is allowed to be more than the [arena's length].
void Arena_free(nullable Arena* self, usize bytes) {
   if (self == nullptr) return;

   if (self->length < bytes) {
      self->length = 0;
   } else {
      self->length -= bytes;
   }
}

void Arena_clear(nullable Arena* self) {
   if (self == nullptr) return;
   self->length = 0;
}

