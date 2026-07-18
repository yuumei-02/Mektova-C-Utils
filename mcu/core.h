#pragma once

#include "types.h"

#define loop while(true)
#define unused (void)

[[noreturn]]
void panic_handler(cstr file, i32 line, const cstr func, cstr format, ...);

[[noreturn]]
void assert_handler(cstr file, i32 line, const cstr func, cstr format, ...);

[[noreturn]]
void misc_handler(cstr format, ...);

#define panic(format, ...) \
   panic_handler(__FILE__, __LINE__, __func__, format __VA_OPT__(,) __VA_ARGS__)

#ifndef NDEBUG
#define mcu_assert(condition, format, ...) \
   if (!(condition)) { \
      assert_handler(__FILE__, __LINE__, __func__, format __VA_OPT__(,) __VA_ARGS__); \
   }
#define mcu_todo(msg) \
   misc_handler("%s:%d:0: [TODO(%s)]: %s\n", __FILE__, __LINE__, __func__, msg);
#else
#define mcu_assert(condition, format, ...)
#define mcu_todo(msg)
#endif
