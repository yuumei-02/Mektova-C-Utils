#include "core.h"
#include "memory.h"
#include "unicode.h"

u32 uchar32_to_uchar8(uchar32 c, uchar8 buff[4]) {
   if (c > 0x10ffff || (c >= 0xd800 && c <= 0xdfff))
      return 0;

   if (c <= 0x7f) {
      buff[0] = (u8) c;
      return 1;
   }

   if (c <= 0x7ff) {
      buff[0] = (u8) (0xc0 | (c >> 6));
      buff[1] = (u8) (0x80 | (c & 0x3f));
      return 2;
   }

   if (c <= 0xffff) {
      buff[0] = (u8) (0xe0 | (c >> 12));
      buff[1] = (u8) (0x80 | ((c >> 6) & 0x3f));
      buff[2] = (u8) (0x80 | (c & 0x3f));
      return 3;
   }

   buff[0] = (u8) (0xf0 | (c >> 18));
   buff[1] = (u8) (0x80 | ((c >> 12) & 0x3f));
   buff[2] = (u8) (0x80 | ((c >> 6) & 0x3f));
   buff[3] = (u8) (0x80 | (c & 0x3f));
   return 4;
}

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

UString32 UString32_new() {
   UString32 self = {
      .chars = mcu_malloc(17 * sizeof(u32)),
      .length = 0,
      .capacity = 16
   };

   memset(self.chars, 0, 17 * sizeof(u32));
   return self;
}

UString32 UString32_from(nullable ustr32 chars) {
   if (chars == nullptr)
      return UString32_new();

   usize char_len = ustr32_len(chars);
   UString32 self = {
      .chars = mcu_malloc((char_len + 1) * sizeof(u32)),
      .length = char_len,
      .capacity = char_len
   };

   memcpy(self.chars, chars, char_len * sizeof(u32));
   return self;
}

void UString32_delete(nullable UString32* self) {
   if (self == nullptr) return;

   mcu_free(self->chars);
   *self = (UString32) {0};
}

[[gnu::always_inline]]
static inline void UString32_check_grow(UString32* self) {
   while (self->length > self->capacity) {
      self->capacity *= 2;
      self->chars = mcu_realloc(self->chars, (self->capacity + 1) * sizeof(u32));
      self->chars[self->capacity] = '\0';
   }

   self->chars[self->length] = '\0';
}

[[gnu::always_inline]]
static inline void UString32_check_shrink(UString32* self) {
   while (self->capacity > 16 && self->length < self->capacity / 2) {
      self->capacity /= 2;
      self->chars = mcu_realloc(self->chars, (self->capacity + 1) * sizeof(u32));
      self->chars[self->capacity] = '\0';
   }

   self->chars[self->length] = '\0';
}

void UString32_append(UString32* self, u32 c) {
   mcu_assert(self != nullptr, "self can't be null");

   self->length++;
   UString32_check_grow(self);
   self->chars[self->length - 1] = c;
}

void UString32_append_ustr32(UString32* self, ustr32 str) {
   mcu_assert(self != nullptr, "self can't be null");
   mcu_assert(str != nullptr, "str can't be null");

   usize str_len = ustr32_len(str);
   self->length += str_len;
   UString32_check_grow(self);
   memcpy(self->chars + (self->length - str_len), str, (str_len + 1) * sizeof(u32));
}

u32 UString32_pop(UString32* self) {
   mcu_assert(self != nullptr, "self can't be null");

   if (self->length == 0)
      return null;

   u32 c = self->chars[--self->length];
   UString32_check_shrink(self);
   return c;
}

u32 UString32_remove(UString32* self, usize index) {
   mcu_assert(self != nullptr, "self can't be null");

   if (self->length == 0 || index >= self->length)
      return null;

   u32 c = self->chars[index];
   memmove(self->chars + index, self->chars + index + 1, (self->length - index) * sizeof(u32));
   UString32_check_shrink(self);
   return c;
}

