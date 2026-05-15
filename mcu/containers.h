#pragma once

#include "types.h"

typedef struct Vector {
   void* buffer;
   usize length;
   usize capacity;
   usize T_size;
} Vector;

Vector Vector_new(usize byte_size_of_T);
Vector Vector_with_capacity(usize byte_size_of_T, usize el_capacity);
Vector Vector_dummy();
void Vector_free(Vector* self);
void Vector_clear(Vector* self);

void Vector_push(Vector* self, void* data);
void* Vector_pop(Vector* self);
void* Vector_get(Vector* self, usize index);
void Vector_remove(Vector* self, usize index);

#define Vector_push_create(vector, data) \
   do { \
      auto tmp = data; \
      Vector_push(vector, &tmp); \
   } while (0) \

#define foreach(vector, index_variable) \
   for (usize index_variable = 0; index_variable < vector.length; ++index_variable)

typedef struct Array {
   void* buffer;
   usize length;
   usize T_size;
} Array;

Array Array_new(usize length, usize T_size);
/// The created Array takes ownership of the [array] data
Array Array_from(void* array, usize length, usize T_size);
/// Converts a [vector] to an [Array] invalidating the original [vector]
Array Vector_to_array(Vector* vector);
Array Array_dummy();
void Array_free(Array* self);

void* Array_get(Array* self, usize index);

#define HashEntry(T) HashEntry_##T
#define HashMap(T)   HashMap_##T

#define HashMap_new(T)     HashMap_##T##_new
#define HashMap_free(T)    HashMap_##T##_free
#define HashMap_hash(T)    HashMap_##T##_hash
#define HashMap_put(T)     HashMap_##T##_put
#define HashMap_get(T)     HashMap_##T##_get
#define HashMap_delete(T)  HashMap_##T##_delete
#define HashMap_resize(T)  HashMap_##T##_resize
#define HashMap_foreach(T) HashMap_##T##_foreach

#define HashMap_hdr(T) \
   typedef struct HashEntry_##T {                                                      \
      struct HashEntry_##T* next;                                                      \
      cstr key;                                                                        \
      T value;                                                                         \
   } HashEntry_##T;                                                                    \
                                                                                       \
   typedef struct {                                                                    \
      HashEntry_##T** entries;                                                         \
      usize length;                                                                    \
      usize capacity;                                                                  \
   } HashMap_##T;                                                                      \
                                                                                       \
   HashMap_##T HashMap_##T##_new();                                                    \
   void HashMap_##T##_free(HashMap_##T* self);                                         \
                                                                                       \
   /* [self] can't be null. */                                                         \
   /* [str] can't be null. */                                                          \
   /* Returns the value's hash index. */                                               \
   usize HashMap_##T##_hash(HashMap_##T* self, cstr str);                              \
                                                                                       \
   /* [self] can't be null. */                                                         \
   /* [key] can't be null. */                                                          \
   /* Returns [false] on update and [true] on [insert]. */                             \
   /* [key] gets copied when inserting a new entry. */                                 \
   bool HashMap_##T##_put(HashMap_##T* self, cstr key, T value);                       \
                                                                                       \
   /* [self] can't be null. */                                                         \
   /* [key] can't be null. */                                                          \
   /* Returns a pointer to the [key]'s [value]. */                                     \
   /* Returns [null] when the key doesn't exist. */                                    \
   T* HashMap_##T##_get(HashMap_##T* self, cstr key);                                  \
                                                                                       \
   typedef void (*HashMap_##T##_ForeachFn)(cstr key, T* value, nullable void* opt);    \
   void HashMap_##T##_foreach(HashMap_##T* self, HashMap_##T##_ForeachFn on_pair, void* opt); \
                                                                                       \
   /* [self] can't be null. */                                                         \
   /* [key] can't be null. */                                                          \
   /* Returns a [bool] where [false] is [deletion] and [true] is [not found]. */       \
   bool HashMap_##T##_delete(HashMap_##T* self, cstr key);                             \
   void HashMap_##T##_resize(HashMap_##T* self, usize new_capacity);                   \

#define HashMap_impl(T) \
   HashMap_##T HashMap_##T##_new() {                                        \
      HashMap_##T self = {                                                  \
         .entries = mcu_malloc(sizeof(HashEntry_##T*) * 16),                \
         .length = 0,                                                       \
         .capacity = 16                                                     \
      };                                                                    \
                                                                            \
      memset(self.entries, 0, sizeof(HashEntry_##T*) * 16);                 \
      return self;                                                          \
   }                                                                        \
                                                                            \
   void HashMap_##T##_free(HashMap_##T* self) {                             \
      mcu_assert(self != nullptr, "self can't be null");                    \
                                                                            \
      for (usize i = 0; i < self->capacity; ++i) {                          \
         HashEntry_##T* curr = self->entries[i];                            \
         while (curr) {                                                     \
            HashEntry_##T* next = curr->next;                               \
            mcu_free(curr->key);                                            \
            mcu_free(curr);                                                 \
            curr = next;                                                    \
         }                                                                  \
      }                                                                     \
                                                                            \
      mcu_free(self->entries);                                              \
      *self = (HashMap_##T) {0};                                            \
   }                                                                        \
                                                                            \
   usize HashMap_##T##_hash(HashMap_##T* self, cstr str) {                  \
      mcu_assert(self != nullptr, "self can't be null");                    \
      mcu_assert(str != nullptr, "str can't be null");                      \
                                                                            \
      u32 hash = 5381;                                                      \
      i32 c;                                                                \
      while ((c = *str++))                                                  \
         hash = ((hash << 5) + hash) + (u32) c;                             \
      return (usize) (hash % self->capacity);                               \
   }                                                                        \
                                                                            \
   bool HashMap_##T##_put(HashMap_##T* self, cstr key, T value) {           \
      mcu_assert(self != nullptr, "self can't be null");                    \
      mcu_assert(key != nullptr, "key can't be null");                      \
                                                                            \
      if (self->length * 4 >= self->capacity * 3)                           \
         HashMap_##T##_resize(self, self->capacity * 2);                    \
                                                                            \
      usize idx = HashMap_##T##_hash(self, key);                            \
      HashEntry_##T* curr = self->entries[idx];                             \
                                                                            \
      while (curr) {                                                        \
         if (strcmp(curr->key, key) == 0) {                                 \
            curr->value = value;                                            \
            return false;                                                   \
         }                                                                  \
         curr = curr->next;                                                 \
      }                                                                     \
                                                                            \
      HashEntry_##T* new = mcu_malloc(sizeof(HashEntry_##T));               \
      *new = (HashEntry_##T) {                                              \
         .key = strdup(key),                                                \
         .value = value,                                                    \
         .next = self->entries[idx]                                         \
      };                                                                    \
      self->entries[idx] = new;                                             \
      self->length += 1;                                                    \
                                                                            \
      return true;                                                          \
   }                                                                        \
                                                                            \
   T* HashMap_##T##_get(HashMap_##T* self, cstr key) {                      \
      mcu_assert(self != nullptr, "self can't be null");                    \
      mcu_assert(key != nullptr, "key can't be null");                      \
                                                                            \
      usize idx = HashMap_##T##_hash(self, key);                            \
      HashEntry_##T* curr = self->entries[idx];                             \
                                                                            \
      while (curr) {                                                        \
         if (strcmp(curr->key, key) == 0)                                   \
            return &curr->value;                                            \
         curr = curr->next;                                                 \
      }                                                                     \
                                                                            \
      return nullptr;                                                       \
   }                                                                        \
                                                                            \
   void HashMap_##T##_foreach(HashMap_##T* self, HashMap_##T##_ForeachFn on_pair, nullable void* opt) { \
      mcu_assert(self != nullptr, "self can't be null");                    \
      mcu_assert(on_pair != nullptr, "on_pair can't be null");              \
                                                                            \
      for (usize i = 0; i < self->capacity; ++i) {                          \
         HashEntry_##T* curr = self->entries[i];                            \
         while (curr) {                                                     \
            on_pair(curr->key, &curr->value, opt);                          \
            curr = curr->next;                                              \
         }                                                                  \
      }                                                                     \
   }                                                                        \
                                                                            \
   bool HashMap_##T##_delete(HashMap_##T* self, cstr key) {                 \
      mcu_assert(self != nullptr, "self can't be null");                    \
      mcu_assert(key != nullptr, "key can't be null");                      \
                                                                            \
      usize idx = HashMap_##T##_hash(self, key);                            \
      HashEntry_##T** pp = &self->entries[idx];                             \
                                                                            \
      while (*pp) {                                                         \
         if (strcmp((*pp)->key, key) == 0) {                                \
            HashEntry_##T* del = *pp;                                       \
            *pp = del->next;                                                \
            mcu_free(del->key);                                             \
            mcu_free(del);                                                  \
            self->length -= 1;                                              \
            return false;                                                   \
         }                                                                  \
                                                                            \
         pp = &(*pp)->next;                                                 \
      }                                                                     \
                                                                            \
      return true;                                                          \
   }                                                                        \
                                                                            \
   void HashMap_##T##_resize(HashMap_##T* self, usize new_capacity) {       \
      HashEntry_##T** old_entries = self->entries;                          \
      usize old_capacity = self->capacity;                                  \
                                                                            \
      self->entries = mcu_malloc(sizeof(HashEntry_##T*) * new_capacity);    \
      self->capacity = new_capacity;                                        \
      self->length = 0;                                                     \
      memset(self->entries, 0, sizeof(HashEntry_##T*) * new_capacity);      \
                                                                            \
      for (usize i = 0; i < old_capacity; ++i) {                            \
         HashEntry_##T* curr = old_entries[i];                              \
         while (curr) {                                                     \
            HashEntry_##T* next = curr->next;                               \
            usize idx = HashMap_##T##_hash(self, curr->key);                \
            curr->next = self->entries[idx];                                \
            self->entries[idx] = curr;                                      \
            self->length += 1;                                              \
            curr = next;                                                    \
         }                                                                  \
      }                                                                     \
                                                                            \
      mcu_free(old_entries);                                                \
   }

