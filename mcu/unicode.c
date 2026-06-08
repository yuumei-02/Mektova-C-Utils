#include "core.h"
#include "memory.h"
#include "unicode.h"

bool ustr32_cmp(ustr32 left, ustr32 right) {
   mcu_assert(left != nullptr, "left can't be null");
   mcu_assert(right != nullptr, "right can't be null");

   loop {
      u32 l = *left++;
      u32 r = *right++;

      if (l != r)
         return false;

      if (l == U'\0' && r == U'\0')
         return true;

      if (l == U'\0' || r == U'\0')
         return false;
   }
}

usize ustr32_len(ustr32 chars) {
   mcu_assert(chars != nullptr, "chars can't be null");

   usize length = 0;
   while (*chars++ != U'\0')
      length++;

   return length;
}

UString32 UString32_new_impl(OptArena opt) {
   UString32 self = {
      .chars = opt.arena == nullptr
         ? mcu_malloc(17 * sizeof(u32))
         : Arena_alloc(opt.arena, 17 * sizeof(u32)),
      .length = 0,
      .capacity = 16
   };

   memset(self.chars, 0, 17 * sizeof(u32));
   return self;
}

UString32 UString32_from_impl(nullable ustr32 chars, OptArena opt) {
   if (chars == nullptr)
      return UString32_new_impl(opt);

   usize char_len = ustr32_len(chars);
   UString32 self = {
      .chars = opt.arena == nullptr
         ? mcu_malloc((char_len + 1) * sizeof(u32))
         : Arena_alloc(opt.arena, (char_len + 1) * sizeof(u32)),
      .length = char_len,
      .capacity = char_len
   };

   memcpy(self.chars, chars, char_len * sizeof(u32));
   return self;
}

void UString32_delete_impl(nullable UString32* self, OptArena opt) {
   if (self == nullptr)
      return;

   if (opt.arena == nullptr)
      mcu_free(self->chars);
   else
      Arena_free(opt.arena, self->capacity);

   *self = (UString32) {0};
}

[[gnu::always_inline]]
static inline void UString32_check_grow(UString32* self, OptArena opt) {
   while (self->length > self->capacity) {
      if (opt.arena == nullptr) {
         self->capacity *= 2;
         self->chars = mcu_realloc(self->chars, (self->capacity + 1) * sizeof(u32));
      } else {
         Arena_free(opt.arena, (self->capacity + 1) * sizeof(u32));
         self->capacity *= 2;
         self->chars = Arena_alloc(opt.arena, (self->capacity + 1) * sizeof(u32));
      }

      self->chars[self->capacity] = '\0';
   }

   self->chars[self->length] = '\0';
}

[[gnu::always_inline]]
static inline void UString32_check_shrink(UString32* self, OptArena opt) {
   while (self->capacity > 16 && self->length < self->capacity / 2) {
      if (opt.arena == nullptr) {
         self->capacity /= 2;
         self->chars = mcu_realloc(self->chars, (self->capacity + 1) * sizeof(u32));
      } else {
         Arena_free(opt.arena, (self->capacity + 1) * sizeof(u32));
         self->capacity /= 2;
         self->chars = Arena_alloc(opt.arena, (self->capacity + 1) * sizeof(u32));
      }

      self->chars[self->capacity] = '\0';
   }

   self->chars[self->length] = '\0';
}

void UString32_append_impl(UString32* self, u32 c, OptArena opt) {
   mcu_assert(self != nullptr, "self can't be null");

   self->length++;
   UString32_check_grow(self, opt);
   self->chars[self->length - 1] = c;
}

void UString32_append_ustr32_impl(UString32* self, ustr32 str, OptArena opt) {
   mcu_assert(self != nullptr, "self can't be null");
   mcu_assert(str != nullptr, "str can't be null");

   usize str_len = ustr32_len(str);
   self->length += str_len;
   UString32_check_grow(self, opt);
   memcpy(self->chars + (self->length - str_len), str, (str_len + 1) * sizeof(u32));
}

u32 UString32_pop_impl(UString32* self, OptArena opt) {
   mcu_assert(self != nullptr, "self can't be null");

   if (self->length == 0)
      return null;

   u32 c = self->chars[--self->length];
   UString32_check_shrink(self, opt);
   return c;
}

u32 UString32_remove_impl(UString32* self, usize index, OptArena opt) {
   mcu_assert(self != nullptr, "self can't be null");

   if (self->length == 0 || index >= self->length)
      return null;

   u32 c = self->chars[index];
   memmove(self->chars + index, self->chars + index + 1, (self->length - index) * sizeof(u32));
   UString32_check_shrink(self, opt);
   return c;
}

