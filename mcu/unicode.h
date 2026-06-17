#pragma once

#include "types.h"

typedef u32* ustr32;
typedef u8* ustr8;

typedef u32 uchar32;
typedef u8 uchar8;

typedef struct {
   ustr32 chars;
   usize length;
   usize capacity;
} UString32;

/// Returns the amount of bytes written to [buff].
/// [0] bytes written means that [c] holds an invalid codepoint.
u32 uchar32_to_uchar8(uchar32 c, uchar8 buff[4]);

usize ustr32_len(ustr32 chars);
/// Returns a [boolean] where [true] means the strings are equal.
bool ustr32_cmp(ustr32 left, ustr32 right);

#define UString32_new(...) \
   UString32_new_impl((OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })

#define UString32_from(chars, ...) \
   UString32_from_impl(chars, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })

#define UString32_delete(self, ...) \
   UString32_delete_impl(self, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })

#define UString32_append(self, c, ...) \
   UString32_append_impl(self, c, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })

#define UString32_append_ustr32(self, str, ...) \
   UString32_append_ustr32_impl(self, str, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })

#define UString32_pop(self, ...) \
   UString32_pop_impl(self, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })

#define UString32_remove(self, index, ...) \
   UString32_remove_impl(self, index, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })

UString32 UString32_new_impl(OptArena opt);

/// The initial [capacity] is equals to that of the [UString32]'s [length].
UString32 UString32_from_impl(nullable ustr32 chars, OptArena opt);
void UString32_delete_impl(nullable UString32* self, OptArena opt);

void UString32_append_impl(UString32* self, u32 c, OptArena opt);
void UString32_append_ustr32_impl(UString32* self, ustr32 str, OptArena opt);

/// Returns the [popped] character.
/// May return [null] when there is nothing left to pop.
u32 UString32_pop_impl(UString32* self, OptArena opt);

/// Returns the [removed] character.
/// May return [null] when there is nothing to remove.
/// [index] may be out of bounds.
u32 UString32_remove_impl(UString32* self, usize index, OptArena opt);

