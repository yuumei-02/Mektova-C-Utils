#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "core.h"

#ifndef CUSTOM_PANIC_HANDLER
[[noreturn]]
void panic_handler(cstr file, i32 line, const cstr func, cstr format, ...) {
   va_list args;
   va_start(args, format);

   fprintf(stderr, "%s:%d:0: [PANIC] in function \"%s\", ", file, line, func);
   vfprintf(stderr, format, args);
   fputs("\n", stderr);

   va_end(args);
   abort();
}
#endif

#ifndef CUSTOM_ASSERT_HANDLER
[[noreturn]]
void assert_handler(cstr file, i32 line, const cstr func, cstr format, ...) {
   va_list args;
   va_start(args, format);

   fprintf(stderr, "%s:%d:0: [ASSERT] in function \"%s\", ", file, line, func);
   vfprintf(stderr, format, args);
   fputs("\n", stderr);

   va_end(args);
   abort();
}
#endif

[[noreturn]]
void misc_handler(cstr format, ...) {
   va_list args;
   va_start(args, format);
   vprintf(format, args);
   va_end(args);
   abort();
}

