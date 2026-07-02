#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define cstr char*
#define null 0
#define nullable
#define out

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;
typedef size_t usize;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;
typedef ptrdiff_t isize;

#define cstr_match(str) cstr match = str;
#define ncstreq(str) if (strcmp(match, str) == 0)
#define cstreq(str) else ncstreq(str)

typedef struct String {
   cstr chars;
   usize length;
   usize capacity;
} String;

typedef struct StringView {
   cstr chars;
   usize length;
} StringView;

StringView StringView_from(nullable cstr chars, usize length);

/// Chops [self] from the left [n] amount of times.
/// Returns a new [StringView] containing what got chopped and modifies [self]
/// to be the remainder.
/// [n] can be out of bounds.
StringView SV_chop_left(StringView* self, usize n);

/// Chops [self] from the right [n] amount of times.
/// Returns a new [StringView] containing what got chopped and modifies [self]
/// to be the remainder.
/// [n] can be out of bounds.
StringView SV_chop_right(StringView* self, usize n);

/// Chops [self] from the left untill and including the delimiter.
/// Returns a new [StringView] containing what got chopped and modifies [self]
/// to be the remainder.
StringView SV_chop_left_by_delimiter(StringView* self, char delimiter);

/// Chops [self] from the right untill and including the delimiter.
/// Returns a new [StringView] containing what got chopped and modifies [self]
/// to be the remainder.
StringView SV_chop_right_by_delimiter(StringView* self, char delimiter);

/// Allocates a new [heap] allocated [String]
/// [Panics] on allocation failure.
String String_new();
String String_with_capacity(usize capacity);
String String_from_sv(StringView sv);
void String_append_sv(String* self, StringView sv);

/// Creates a new [heap] allocated [String] from a [cstr].
/// [Panics] on allocation failure.
/// [str] may be null.
String String_from(const cstr str);

/// Deep copies [original].
/// [panics] on allocation failure.
/// [self]'s capacity is that of its length, not of the [original]'s capacity.
String String_clone(String original);

/// Frees a [String's heap allocation] from memory.
void String_free(nullable String* self);

/// Clears a [String], preserving its [length].
void String_clear(nullable String* self);

/// Appends char [c] to a [String].
/// [Panics] on reallocation failure.
void String_append(String* self, char c);

/// Appends char [c] to the back of a [String].
/// [Panics] on reallocation failure.
void String_append_back(String* self, char c);

/// Pops the char at the top of the string and returns it.
/// The returning char may be a [null byte]
/// when there is nothing left to pop or when [self] is [null].
char String_pop(nullable String* self);

/// Appends cstr [other] to String [self] through a [copy] operation.
/// [Panics] on reallocation failure.
/// The memory region of [self] and [other] may overlap.
void String_append_cstr(String* self, nullable cstr other);

/// Appends cstr [other] to String [self] through a [copy] operation.
/// [Panics] on reallocation failure.
/// The memory region of [self] and [other] may overlap.
void String_appendf(String* self, const cstr format, ...);

/// Same as [String_appendf] except that this version takes in the
/// va_list instead of creating it itself.
void String_appendfv(String* self, const cstr format, va_list args);

