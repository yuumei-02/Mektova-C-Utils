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

