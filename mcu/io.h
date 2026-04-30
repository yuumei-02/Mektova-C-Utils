#pragma once

#include <dirent.h>
#include <stdio.h> // IWYU pragma: keep

#include "types.h"
#include "containers.h"

#define println(format, ...) \
   printf(format"\n" __VA_OPT__(,) __VA_ARGS__)

#define eprintln(format, ...) \
   fprintf(stderr, format"\n" __VA_OPT__(,) __VA_ARGS__)

#define fprintln(fd, format, ...) \
   fprintf(fd, format"\n" __VA_OPT__(,) __VA_ARGS__)

// @todo: remove and redo the entire file system api as its kinda shit

// @todo: expand to include more error types
typedef enum {
   // File operation only
   IOE_FileNotFound,
   IOE_NotAFile,
   IOE_InvalidFileDescriptor,
   
   // Directory operation only
   IOE_DirectoryNotFound,
   IOE_NotADirectory,

   IOE_PermissionDenied,
   IOE_Interupted,
   IOE_ReadFailure,

   IOE_Unknown
} IOError;

typedef struct File {
   FILE* handle;
   Str path;
} File;

Result_decl(File, IOError);
Result_decl(usize, IOError);
Option_decl(IOError);

/// [copies] file_path
Result(File, IOError) File_open(cstr file_path, const cstr mode);
Option(IOError) File_close(File* file);

Result(usize, IOError) File_size(File* file);

typedef struct Directory {
   Str path;
   Array Str_files;
   DIR* dirent_dir;
} Directory;

typedef struct {
   union {
      Directory ok;
      IOError err;
   };
   bool is_err;
} DirResult;

DirResult Directory_open(cstr dir_path);
void Directory_close(Directory* self, bool* failed);

bool shell_command(const cstr command);

