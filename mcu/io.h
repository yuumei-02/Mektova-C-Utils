#pragma once

#include <stdio.h> // IWYU pragma: keep

#include "types.h"
#include "containers.h"

#define println(format, ...) \
   printf(format"\n" __VA_OPT__(,) __VA_ARGS__)

#define eprintln(format, ...) \
   fprintf(stderr, format"\n" __VA_OPT__(,) __VA_ARGS__)

#define fprintln(fd, format, ...) \
   fprintf(fd, format"\n" __VA_OPT__(,) __VA_ARGS__)

typedef enum {
   WDA_Continue,
   WDA_Failure,
   WDA_Stop,
   WDA_Skip
} WalkDirAction;

typedef enum {
   FST_File,
   FST_Dir
} FSType;

const cstr FSType_to_cstr(FSType self);

typedef WalkDirAction (*OnWalkDirEntry)(const cstr path, FSType fs_type);
bool walk_directory(nullable const cstr path, nullable OnWalkDirEntry on_entry);

bool shell_command(const cstr command);

