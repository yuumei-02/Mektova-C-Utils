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

UString32 UString32_new();

/// The initial [capacity] is equals to that of the [UString32]'s [length].
UString32 UString32_from(nullable ustr32 chars);
void UString32_delete(nullable UString32* self);

void UString32_append(UString32* self, u32 c);
void UString32_append_ustr32(UString32* self, ustr32 str);

/// Returns the [popped] character.
/// May return [null] when there is nothing left to pop.
u32 UString32_pop(UString32* self);

/// Returns the [removed] character.
/// May return [null] when there is nothing to remove.
/// [index] may be out of bounds.
u32 UString32_remove(UString32* self, usize index);

