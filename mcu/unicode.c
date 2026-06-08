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

