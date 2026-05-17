#define _GNU_SOURCE

#include "types.h"
#include "core.h"
#include "memory.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

Str Str_new(usize length) {
   mcu_assert(length > 0, "Can't create a StringView with a length of 0");

   Str self = {
      .chars = mcu_malloc(length + 1),
      .length = length
   };

   self.chars[self.length] = '\0';
   return self;
}

Str Str_from(cstr chars, usize length) {
   mcu_assert(chars != nullptr, "Can't create a StringView from a null cstr");
   mcu_assert(length > 0, "Can't create a StringView with a length of 0");

   return (Str) {
      .chars = chars,
      .length = length
   };
}

Str Str_copy_from(cstr chars, usize length) {
   mcu_assert(chars != nullptr, "Can't create a StringView from a null cstr");
   mcu_assert(length > 0, "Can't create a StringView with a length of 0");

   Str self = {
      .chars = mcu_malloc(length + 1),
      .length = length
   };

   memcpy(self.chars, chars, length);
   self.chars[self.length] = '\0';

   return self;
}

void Str_free(Str* str) {
   mcu_assert(str != nullptr, "Cannot free a null Str");

   mcu_free(str->chars);
   *str = (Str) {0};
}

StringView StringView_from(cstr chars, usize length) {
   mcu_assert(chars != nullptr, "Can't create a StringView from a null cstr");
   mcu_assert(length > 0, "Can't create a StringView with a length of 0");

   return (StringView) {
      .chars = chars,
      .length = length
   };
}

String String_new() {
   String string = {
      .chars = mcu_malloc(17),
      .length = 0,
      .capacity = 16
   };

   memset(string.chars, 0, 17);
   return string;
}

String String_with_capacity(usize capacity) {
   mcu_assert(capacity > 0, "Can't create a String with a capacity of 0");

   String string = {
      .chars = mcu_malloc(capacity + 1),
      .length = 0,
      .capacity = capacity
   };

   string.chars[0] = '\0';
   string.chars[capacity] = '\0';
   return string;
}

String String_from(const cstr str) {
   mcu_assert(str != nullptr, "Can't create a String from null");

   usize str_length = strlen(str);
   String string = {
      .chars = mcu_malloc(str_length + 1),
      .length = str_length,
      .capacity = str_length
   };

   memcpy(string.chars, str, str_length);
   string.chars[str_length] = '\0';

   return string;
}

String String_clone(String original) {
   String self = {
      .length = original.length,
      .chars = mcu_malloc(original.length + 1),
      .capacity = original.length
   };

   memcpy(self.chars, original.chars, original.length);
   self.chars[self.length] = '\0';

   return self;
}

void String_free(nullable String* self) {
   if (self == nullptr) return;

   mcu_free(self->chars);
   *self = (String) {0};
}

void String_clear(nullable String* self) {
   if (self == nullptr) return;

   self->length = 0;
   self->chars[0] = '\0';
}

void String_append(String* self, char c) {
   mcu_assert(self != nullptr, "Can't append a character to null");

   while (self->length >= self->capacity) {
      self->capacity *= 2;
      self->chars = mcu_realloc(self->chars, self->capacity + 1);
   }

   self->chars[self->length] = c;
   self->length += 1;
   self->chars[self->length] = '\0';
}

void String_append_back(String* self, char c) {
   mcu_assert(self != nullptr, "Can't append a character to null");

   while (self->length >= self->capacity) {
      self->capacity *= 2;
      self->chars = mcu_realloc(self->chars, self->capacity + 1);
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

char String_pop(nullable String* self) {
   if (self == nullptr)   return '\0';
   if (self->length == 0) return '\0';

   if (self->length >= self->capacity / 2 && self->capacity > 16) {
      self->capacity /= 2;
      self->chars = mcu_realloc(self->chars, self->capacity + 1);
   }

   self->length -= 1;
   char c = self->chars[self->length];
   self->chars[self->length] = '\0';

   return c;
}

void String_append_cstr(String* self, nullable cstr other) {
   mcu_assert(self != nullptr, "Can't append to null");

   if (other == nullptr) return;

   usize other_length = strlen(other);

   while (self->length + other_length >= self->capacity) {
      self->capacity *= 2;
      self->chars = mcu_realloc(self->chars, self->capacity + 1);
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

void String_appendfv(String* self, const cstr format, va_list args) {
   mcu_assert(self != nullptr, "Can't append to null");
   mcu_assert(format != nullptr, "Can't append a format of null to String");

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
}

void String_append_String(String* self, nullable String* other) {
   mcu_assert(self != nullptr, "Can't append to null");

   if (other == nullptr) return;

   while (self->length + other->length >= self->capacity) {
      self->capacity *= 2;
      self->chars = mcu_realloc(self->chars, self->capacity + 1);
   }

   memmove(self->chars + self->length, other->chars, other->length);
   self->length += other->length;
   self->chars[self->length] = '\0';
}

