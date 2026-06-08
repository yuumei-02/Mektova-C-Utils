#pragma once

#include "types.h"

typedef u32* ustr32;

typedef struct {
   ustr32 chars;
   usize length;
   usize capacity;
} UString32;

usize ustr32_len(ustr32 chars);
/// Returns a [boolean] where [true] means the strings are equal.
bool ustr32_cmp(ustr32 left, ustr32 right);

#define UString32_new(...) \
   UString32_new_impl((OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })

#define UString32_from(chars, ...) \
   UString32_from_impl(chars, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })

#define UString32_delete(self, ...) \
   UString32_delete_impl(self, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })
   
UString32 UString32_new_impl(OptArena opt);
UString32 UString32_from_impl(nullable ustr32 chars, OptArena opt);
void UString32_delete_impl(nullable UString32* self, OptArena opt);

