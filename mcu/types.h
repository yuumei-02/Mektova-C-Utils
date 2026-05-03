#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define cstr char*
#define null 0
#define nullable

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

#define Option_decl(T) \
   typedef struct Option_##T { \
      union { \
         u8 none; \
         T some; \
      }; \
      bool is_some; \
   } Option_##T

#define Result_decl(T, E) \
   typedef struct Result_##T##_##E { \
      union { \
         T ok; \
         E err; \
      }; \
      bool is_err; \
   } Result_##T##_##E

#define Option(T) Option_##T
#define None(T) (Option_##T) { .none = 0, .is_some = false }
#define Some(T, v) (Option_##T) { .some = v, .is_some = true }

#define Result(T, E) Result_##T##_##E
#define Ok(T, E, v) (Result_##T##_##E) { .ok = v, .is_err = false }
#define Err(T, E, v) (Result_##T##_##E) { .err = v, .is_err = true }

typedef struct String {
   cstr chars;
   usize length;
   usize capacity;
} String;

typedef struct StringView {
   cstr chars;
   usize length;
} StringView;

typedef struct Str {
   cstr chars;
   usize length;
} Str;

Str Str_new(usize length);
Str Str_from(cstr chars, usize length);
Str Str_copy_from(cstr chars, usize length);
void Str_free(Str* str);

StringView StringView_from(cstr chars, usize length);

/// Allocates a new [heap] allocated [String]
/// [Panics] on allocation failure.
String String_new();

String String_with_capacity(usize capacity);

static inline String String_dummy() { return (String) {0}; }

/// Creates a new [heap] allocated [String] from a [cstr].
/// [Panics] on allocation failure.
/// [str] may be null.
String String_from(cstr str);

/// Deep copies [original].
/// [panics] on allocation failure.
/// [self]'s capacity is that of its length, not of the [original]'s capacity.
String String_clone(String original);

/// Frees a [String's heap allocation] from memory.
/// [self] may be null.
void String_free(String* self);

/// Clears a [String], preserving its [length]
/// [self] may be null.
void String_clear(String* self);

/// Appends char [c] to a [String].
/// [Panics] on reallocation failure.
/// [self] may be null.
void String_append(String* self, char c);

/// Appends char [c] to the back of a [String].
/// [Panics] on reallocation failure.
/// [self] may be null.
void String_append_back(String* self, char c);

void String_pop(String* self);

/// Appends cstr [other] to String [self] through a [copy] operation.
/// [Panics] on reallocation failure.
/// [self] may not be null.
/// [other] may not be null.
/// The memory region of [self] and [other] may overlap.
void String_append_cstr(String* self, cstr other);

/// Appends cstr [other] to String [self] through a [copy] operation.
/// [Panics] on reallocation failure.
/// [self] may not be null.
/// [other] may not be null.
/// The memory region of [self] and [other] may overlap.
void String_appendf(String* self, cstr format, ...);

/// Appends String [other] to String [self] through a [copy] operation.
/// [Panics] on reallocation failure.
/// [self] may be null.
/// [other] may be null.
/// The memory region of both Strings may overlap.
/// e.g by appending a String to itself
void String_append_String(String* self, String* other);

