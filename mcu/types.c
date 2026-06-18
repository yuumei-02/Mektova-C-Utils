#define _GNU_SOURCE

#include "types.h"
#include "core.h"
#include "memory.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

StringView StringView_from(nullable cstr chars, usize length) {
   if (chars == nullptr || length == 0) {
      return (StringView) {0};
   }

   return (StringView) {
      .chars = chars,
      .length = length
   };
}

StringView SV_chop_left(StringView* self, usize n) {
   mcu_assert(self != nullptr, "self can't be null");

   StringView new = *self;

   if (n >= self->length) {
      *self = (StringView) {0};
      return new;
   }

   self->chars += n;
   self->length -= n;
   new.length = n;
   return new;
}

StringView SV_chop_right(StringView* self, usize n) {
   mcu_assert(self != nullptr, "self can't be null");

   StringView new = *self;

   if (n >= self->length) {
      *self = (StringView) {0};
      return new;
   }

   self->length -= n;
   new.chars += self->length;
   new.length = n;
   return new;
}

StringView SV_chop_left_by_delimiter(StringView* self, char delimiter) {
   mcu_assert(self != nullptr, "self can't be null");

   StringView new = *self;
   usize chopped = 0;

   for (; chopped < self->length && self->chars[chopped] != delimiter; ++chopped) {}

   self->chars += chopped;
   self->length -= chopped;
   new.length = chopped;

   if (self->length == 0) {
      *self = (StringView) {0};
      return new;
   }

   // Chop the delimiter
   self->chars++;
   self->length--;

   if (self->length == 0) {
      *self = (StringView) {0};
      return new;
   }

   return new;
}

StringView SV_chop_right_by_delimiter(StringView* self, char delimiter) {
   mcu_assert(self != nullptr, "self can't be null");

   StringView new = *self;
   usize chopped = 0;

   while (self->length > 0 && self->chars[self->length - 1] != delimiter) {
      self->length--;
      chopped++;
   }

   new.chars += new.length - chopped;
   new.length = chopped;

   if (self->length == 0) {
      *self = (StringView) {0};
      return new;
   }

   // Chop the delimiter
   self->length--;

   if (self->length == 0) {
      *self = (StringView) {0};
      return new;
   }

   return new;
}

String String_new_ex(OptArena opt) {
   String string = {
      .chars = opt.arena == nullptr
         ? mcu_malloc(17)
         : Arena_alloc(opt.arena, 17),
      .length = 0,
      .capacity = 16
   };

   memset(string.chars, 0, 17);
   return string;
}

String String_with_capacity_ex(usize capacity, OptArena opt) {
   mcu_assert(capacity > 0, "Can't create a String with a capacity of 0");

   String string = {
      .chars = opt.arena == nullptr
         ? mcu_malloc(capacity + 1)
         : Arena_alloc(opt.arena, capacity + 1),
      .length = 0,
      .capacity = capacity
   };

   string.chars[0] = '\0';
   string.chars[capacity] = '\0';
   return string;
}

String String_from_sv_ex(StringView sv, OptArena opt) {
   if (sv.chars == nullptr || sv.length < 1)
      return String_new_ex(opt);

   String self = {
      .chars = opt.arena == nullptr
         ? mcu_malloc(sv.length + 1)
         : Arena_alloc(opt.arena, sv.length + 1),
      .length = sv.length,
      .capacity = sv.length
   };

   memcpy(self.chars, sv.chars, sv.length);
   self.chars[self.capacity] = '\0';
   return self;
}

void String_append_sv_ex(String* self, StringView sv, OptArena opt) {
   mcu_assert(self != nullptr, "self can't be null");

   if (sv.length < 1 || sv.chars == nullptr) return;

   self->length += sv.length;
   while (self->length >= self->capacity) {
      if (opt.arena == nullptr) {
         self->capacity *= 2;
         self->chars = mcu_realloc(self->chars, self->capacity + 1);
      } else {
         Arena_free(opt.arena, self->capacity + 1);
         self->capacity *= 2;
         self->chars = Arena_alloc(opt.arena, self->capacity + 1);
      }
   }

   void* base = self->chars + (self->length - sv.length);
   memcpy(base, sv.chars, sv.length);
   self->chars[self->length] = '\0';
}

String String_from_ex(const cstr str, OptArena opt) {
   mcu_assert(str != nullptr, "Can't create a String from null");

   usize str_length = strlen(str);
   String string = {
      .chars = opt.arena == nullptr
         ? mcu_malloc(str_length + 1)
         : Arena_alloc(opt.arena, str_length + 1),
      .length = str_length,
      .capacity = str_length
   };

   memcpy(string.chars, str, str_length);
   string.chars[str_length] = '\0';

   return string;
}

String String_clone_ex(String original, OptArena opt) {
   String self = {
      .length = original.length,
      .chars = opt.arena == nullptr
         ? mcu_malloc(original.length + 1)
         : Arena_alloc(opt.arena, original.length + 1),
      .capacity = original.length
   };

   memcpy(self.chars, original.chars, original.length);
   self.chars[self.length] = '\0';

   return self;
}

void String_free_ex(nullable String* self, OptArena opt) {
   if (self == nullptr) return;

   if (opt.arena == nullptr)
      mcu_free(self->chars);
   else
      Arena_free(opt.arena, self->capacity + 1);
   *self = (String) {0};
}

void String_clear(nullable String* self) {
   if (self == nullptr) return;

   self->length = 0;
   self->chars[0] = '\0';
}

void String_append_ex(String* self, char c, OptArena opt) {
   mcu_assert(self != nullptr, "Can't append a character to null");

   while (self->length >= self->capacity) {
      if (opt.arena == nullptr) {
         self->capacity *= 2;
         self->chars = mcu_realloc(self->chars, self->capacity + 1);
      } else {
         Arena_free(opt.arena, self->capacity + 1);
         self->capacity *= 2;
         self->chars = Arena_alloc(opt.arena, self->capacity + 1);
      }
   }

   self->chars[self->length] = c;
   self->length += 1;
   self->chars[self->length] = '\0';
}

void String_append_back_ex(String* self, char c, OptArena opt) {
   mcu_assert(self != nullptr, "Can't append a character to null");

   while (self->length >= self->capacity) {
      if (opt.arena == nullptr) {
         self->capacity *= 2;
         self->chars = mcu_realloc(self->chars, self->capacity + 1);
      } else {
         Arena_free(opt.arena, self->capacity + 1);
         self->capacity *= 2;
         self->chars = Arena_alloc(opt.arena, self->capacity + 1);
      }
   }

   for (isize i = self->length - 1; i >= 0; --i) {
      self->chars[i + 1] = self->chars[i];
   }

   // @todo: validate correctness
   self->chars[0] = c;
   self->length += 1;
   self->chars[self->length] = '\0';
   return;
}

char String_pop_ex(nullable String* self, OptArena opt) {
   if (self == nullptr)   return '\0';
   if (self->length == 0) return '\0';

   if (self->length >= self->capacity / 2 && self->capacity > 16) {
      if (opt.arena == nullptr) {
         self->capacity /= 2;
         self->chars = mcu_realloc(self->chars, self->capacity + 1);
      } else {
         Arena_free(opt.arena, self->capacity + 1);
         self->capacity /= 2;
         self->chars = Arena_alloc(opt.arena, self->capacity + 1);
      }
   }

   self->length -= 1;
   char c = self->chars[self->length];
   self->chars[self->length] = '\0';

   return c;
}

void String_append_cstr_ex(String* self, nullable cstr other, OptArena opt) {
   mcu_assert(self != nullptr, "Can't append to null");

   if (other == nullptr) return;

   usize other_length = strlen(other);

   while (self->length + other_length >= self->capacity) {
      if (opt.arena == nullptr) {
         self->capacity *= 2;
         self->chars = mcu_realloc(self->chars, self->capacity + 1);
      } else {
         Arena_free(opt.arena, self->capacity + 1);
         self->capacity *= 2;
         self->chars = Arena_alloc(opt.arena, self->capacity + 1);
      }
   }

   memmove(self->chars + self->length, other, other_length);
   self->length += other_length;
   self->chars[self->length] = '\0';
}

void String_appendf(String* self, const cstr format, ...) {
   mcu_assert(self != nullptr, "Can't append to null");
   mcu_assert(format != nullptr, "Can't append a format of null to String");

   va_list args;
   va_start(args, format);

   cstr tmp_str = nullptr;
   vasprintf(&tmp_str, format, args);
   usize other_length = strlen(tmp_str);

   while (self->length + other_length >= self->capacity) {
      self->capacity *= 2;
      self->chars = mcu_realloc(self->chars, self->capacity + 1);
   }

   memmove(self->chars + self->length, tmp_str, other_length);
   self->length += other_length;
   self->chars[self->length] = '\0';

   free(tmp_str);

   va_end(args);
}

void String_appendf_arena(nullable Arena* arena, String* self, const cstr format, ...) {
   mcu_assert(self != nullptr, "Can't append to null");
   mcu_assert(format != nullptr, "Can't append a format of null to String");

   va_list args;
   va_start(args, format);

   cstr tmp_str = nullptr;
   vasprintf(&tmp_str, format, args);
   usize other_length = strlen(tmp_str);

   while (self->length + other_length >= self->capacity) {
      if (arena == nullptr) {
         self->capacity *= 2;
         self->chars = mcu_realloc(self->chars, self->capacity + 1);
      } else {
         Arena_free(arena, self->capacity + 1);
         self->capacity *= 2;
         self->chars = Arena_alloc(arena, self->capacity + 1);
      }
   }

   memmove(self->chars + self->length, tmp_str, other_length);
   self->length += other_length;
   self->chars[self->length] = '\0';

   free(tmp_str);

   va_end(args);
}

void String_appendfv_ex(String* self, const cstr format, va_list args, OptArena opt) {
   mcu_assert(self != nullptr, "Can't append to null");
   mcu_assert(format != nullptr, "Can't append a format of null to String");

   cstr tmp_str = nullptr;
   vasprintf(&tmp_str, format, args);
   usize other_length = strlen(tmp_str);

   while (self->length + other_length >= self->capacity) {
      if (opt.arena == nullptr) {
         self->capacity *= 2;
         self->chars = mcu_realloc(self->chars, self->capacity + 1);
      } else {
         Arena_free(opt.arena, self->capacity + 1);
         self->capacity *= 2;
         self->chars = Arena_alloc(opt.arena, self->capacity + 1);
      }
   }

   memmove(self->chars + self->length, tmp_str, other_length);
   self->length += other_length;
   self->chars[self->length] = '\0';

   free(tmp_str);
}

