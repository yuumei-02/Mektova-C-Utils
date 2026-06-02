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

typedef struct Arena Arena;

/// Procedures that optionally use arenas and can potentionally
/// grow and or shrink the allocated data's size,
/// will assume that it's the last allocated item unless documented otherwise.
typedef struct {
   Arena* arena;
} OptArena;

/// Allocates a new [heap] allocated [String]
/// [Panics] on allocation failure.
#define String_new(...) \
   String_new_ex((OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })
String String_new_ex(OptArena opt);

#define String_with_capacity(capacity, ...) \
   String_with_capacity_ex(capacity, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })
String String_with_capacity_ex(usize capacity, OptArena opt);

static inline String String_dummy() { return (String) {0}; }

/// Creates a new [heap] allocated [String] from a [cstr].
/// [Panics] on allocation failure.
/// [str] may be null.
#define String_from(str, ...) \
   String_from_ex(str, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })
String String_from_ex(const cstr str, OptArena opt);

/// Deep copies [original].
/// [panics] on allocation failure.
/// [self]'s capacity is that of its length, not of the [original]'s capacity.
#define String_clone(original, ...) \
   String_clone_ex(original, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })
String String_clone_ex(String original, OptArena opt);

/// Frees a [String's heap allocation] from memory.
#define String_free(self, ...) \
  String_free_ex(self, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })
void String_free_ex(nullable String* self, OptArena opt);

/// Clears a [String], preserving its [length].
void String_clear(nullable String* self);

/// Appends char [c] to a [String].
/// [Panics] on reallocation failure.
#define String_append(self, c, ...) \
   String_append_ex(self, c, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })
void String_append_ex(String* self, char c, OptArena opt);

/// Appends char [c] to the back of a [String].
/// [Panics] on reallocation failure.
#define String_append_back(self, c, ...) \
   String_append_back_ex(self, c, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })
void String_append_back_ex(String* self, char c, OptArena opt);

/// Pops the char at the top of the string and returns it.
/// The returning char may be a [null byte]
/// when there is nothing left to pop or when [self] is [null].
#define String_pop(self, ...) \
   String_pop_ex(self, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })
char String_pop_ex(nullable String* self, OptArena opt);

/// Appends cstr [other] to String [self] through a [copy] operation.
/// [Panics] on reallocation failure.
/// The memory region of [self] and [other] may overlap.
#define String_append_cstr(self, other, ...) \
   String_append_cstr_ex(self, other, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })
void String_append_cstr_ex(String* self, nullable cstr other, OptArena opt);

/// Appends cstr [other] to String [self] through a [copy] operation.
/// [Panics] on reallocation failure.
/// The memory region of [self] and [other] may overlap.
void String_appendf(String* self, const cstr format, ...);
void String_appendf_arena(nullable Arena* arena, String* self, const cstr format, ...);

/// Same as [String_appendf] except that this version takes in the
/// va_list instead of creating it itself.
#define String_appendfv(self, format, args, ...) \
   String_appendfv_ex(self, format, args, (OptArena) { .arena = nullptr __VA_OPT__(,) __VA_ARGS__ })
void String_appendfv_ex(String* self, const cstr format, va_list args, OptArena opt);

