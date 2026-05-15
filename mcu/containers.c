#include "core.h"
#include "memory.h"
#include "containers.h"

#include <string.h>

// Todo: Documentation
// Todo: Implementation validation

Vector Vector_new(usize byte_size_of_T) {
   mcu_assert(byte_size_of_T > 0, "Can't create a Vector<T> where T is 0 bytes in size");

   return (Vector) {
      .buffer = mcu_malloc(16 * byte_size_of_T),
      .length = 0,
      .capacity = 16,
      .T_size = byte_size_of_T
   };
}

Vector Vector_with_capacity(usize byte_size_of_T, usize el_capacity) {
   mcu_assert(byte_size_of_T > 0, "Can't create a Vector<T> where T is 0 bytes in size");
   mcu_assert(el_capacity > 0, "Can't create a Vector<T> with a capacity of 0 elements");

   return (Vector) {
      .buffer = mcu_malloc(el_capacity * byte_size_of_T),
      .length = 0,
      .capacity = el_capacity,
      .T_size = byte_size_of_T
   };
}

Vector Vector_dummy() {
   return (Vector) {
      .buffer = nullptr,
      .length = 0,
      .capacity = 0,
      .T_size = 0
   };
}

void Vector_clear(Vector* self) {
   mcu_assert(self != nullptr, "self can't be null");
   self->length = 0;
}

void Vector_push(Vector* self, void* data) {
   mcu_assert(self != nullptr, "Can't push to null");
   mcu_assert(data != nullptr, "Address of data can't be null");

   if (self->length >= self->capacity) {
      self->capacity *= 2;
      self->buffer = mcu_realloc(self->buffer, self->capacity * self->T_size);
   }

   memcpy(
      ((u8*) self->buffer) + (self->length * self->T_size),
      data, self->T_size);
   self->length += 1;
}

// note: May return null when there is nothing to pop
void* Vector_pop(Vector* self) {
   mcu_assert(self != nullptr, "Can't pop from null");

   if (self->length <= self->capacity / 2 && self->capacity > 16) {
      self->capacity /= 2;
      self->buffer = mcu_realloc(self->buffer, self->capacity * self->T_size);
   }

   void* ptr = nullptr;
   if (self->length > 0) {
      ptr = ((u8*) self->buffer) + ((self->length - 1) * self->T_size);
      self->length -= 1;
   }

   return ptr;
}

// note: May return null when out of bounds
void* Vector_get(Vector* self, usize index) {
   mcu_assert(self != nullptr, "Null is not a vector");

   if (index >= self->length) {
      return nullptr;
   }

   return ((u8*) self->buffer) + (index * self->T_size);
}

void Vector_remove(Vector* self, usize index) {
   mcu_assert(self != nullptr, "self can't be null");

   if (index >= self->length)
      return;

   if (index == self->length - 1) {
      Vector_pop(self);
      return;
   }

   u8* base = ((u8*) self->buffer) + (index * self->T_size);

   memmove(
      base, base + self->T_size,
      (self->length - index - 1) * self->T_size);

   self->length -= 1;

   if (self->length <= self->capacity / 2 && self->capacity > 16) {
      self->capacity /= 2;
      self->buffer = mcu_realloc(self->buffer, self->capacity * self->T_size);
   }
}

void Vector_free(Vector* self) {
   mcu_assert(self != nullptr, "Can't free a null Vector");

   mcu_free(self->buffer);
   self->buffer = nullptr;
   self->length = 0;
   self->capacity = 0;
   self->T_size = 0;
}

Array Array_new(usize length, usize T_size) {
   mcu_assert(length > 0, "Can't create an array with a length of 0");
   mcu_assert(T_size > 0, "Can't create an array where the size of T is 0 bytes");

   return (Array) {
      .buffer = mcu_malloc(length * T_size),
      .length = length,
      .T_size = T_size
   };
}

Array Array_from(void* array, usize length, usize T_size) {
   mcu_assert(array != nullptr, "Can't create an array from null");
   mcu_assert(length > 0, "Can't create an array with a length of 0");
   mcu_assert(T_size > 0, "Can't create an array where the size of T is 0 bytes");

   return (Array) {
      .buffer = array,
      .length = length,
      .T_size = T_size
   };
}

Array Vector_to_array(Vector* vector) {
   mcu_assert(vector != nullptr, "vector can't be null");
   mcu_assert(vector->length > 0, "Can't create an Array from a Vector of length 0");

   Array self = {
      .buffer = mcu_realloc(vector->buffer, vector->length * vector->T_size),
      .length = vector->length,
      .T_size = vector->T_size
   };

   return self;
}

Array Array_dummy() {
   return (Array) {
      .buffer = nullptr,
      .length = 0,
      .T_size = 0
   };
}

void Array_free(Array* self) {
   mcu_assert(self != nullptr, "Can't free a null Array");

   mcu_free(self->buffer);
   self->buffer = nullptr;
   self->length = 0;
   self->T_size = 0;
}

void* Array_get(Array* self, usize index) {
   mcu_assert(self != nullptr, "Can't index into a null Array");

   if (index >= self->length) {
      return nullptr;
   }

   return ((u8*) self->buffer) + (index * self->T_size);
}

