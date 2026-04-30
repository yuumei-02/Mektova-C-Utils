#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <asm-generic/errno-base.h>

#include "core.h"
#include "io.h"

// @todo: documentation and validation

Result(File, IOError) File_open(cstr file_path, const cstr mode) {
   mcu_assert(file_path != nullptr, "file_path can't be null");
   mcu_assert(mode != nullptr, "mode can't be null");

   FILE* handle = fopen(file_path, mode);
   if (handle == nullptr) {
      switch (errno) {
         case EACCES: return Err(File, IOError, IOE_PermissionDenied);
         case EINTR:  return Err(File, IOError, IOE_Interupted); 
         case EBADF:  return Err(File, IOError, IOE_FileNotFound);
         case ENOMEM: {
            panic("[!] OOM, reason: \"%s\"", strerror(errno));
         } break;
         default: {
            return Err(File, IOError, IOE_Unknown);
         }
      }
   }

   File self = {
      .handle = handle,
      .path = Str_copy_from(file_path, strlen(file_path))
   };

   return Ok(File, IOError, self);
}

Option(IOError) File_close(File* file) {
   mcu_assert(file != nullptr, "File can't be null");

   if (fclose(file->handle) == EOF) {
      switch (errno) {
         case EBADF: return Some(IOError, IOE_InvalidFileDescriptor);
         case EINTR: return Some(IOError, IOE_Interupted);
         case ENOMEM: {
            panic("[!] OOM, reason: \"%s\"", strerror(errno));
         } break;
         default:    return Some(IOError, IOE_Unknown);
      }
   }

   Str_free(&file->path);
   *file = (File) {0};
   return None(IOError);
}

Result(usize, IOError) File_size(File* file) {
   mcu_assert(file != nullptr, "file can't be null");

   struct stat st;
   if (stat(file->path.chars, &st) != 0) {
      switch (errno) {
         case EACCES:   return Err(usize, IOError, IOE_PermissionDenied);
         case EBADF:    return Err(usize, IOError, IOE_InvalidFileDescriptor);
         case ENOTDIR:  return Err(usize, IOError, IOE_NotADirectory);
         case ENOMEM: {
            panic("[!] OOM, reason: \"%s\"", strerror(errno));
         } break;
         default:       return Err(usize, IOError, IOE_Unknown);
      }
   }

   return Ok(usize, IOError, st.st_size);
}

DirResult Directory_open(cstr dir_path) {
   mcu_assert(dir_path != nullptr, "dir_path can't be null");

   Directory self = {
      .path = Str_copy_from(dir_path, strlen(dir_path)),
      .dirent_dir = nullptr
   };
   IOError error;

   DIR* dir = opendir(dir_path);
   if (dir == nullptr) {
      // @todo: handle the remaining errors
      switch (errno) {
         case EACCES:  error = IOE_PermissionDenied;  break;
         case ENOENT:  error = IOE_DirectoryNotFound; break;
         case ENOTDIR: error = IOE_NotADirectory;     break;
         case ENOMEM: {
            panic("[!] OOM, reason: \"%s\"", strerror(errno));
         }
         default: error = IOE_Unknown;
      }
      goto failure;
   }

   self.dirent_dir = dir;

   Vector Str_files = Vector_new(sizeof(Str));
   struct dirent* entry;
   while ((entry = readdir(dir)) != nullptr) {
      struct stat entry_stat;
      char path[4096];
      snprintf(path, 4096, "%s/%s", dir_path, entry->d_name);

      if (stat(path, &entry_stat) == 0) {
         if (S_ISDIR(entry_stat.st_mode)) continue;
      } else {
         switch (errno) {
            case EACCES:  error = IOE_PermissionDenied; break;
            case EBADF:   error = IOE_FileNotFound;     break;
            case ENOTDIR: error = IOE_NotADirectory;    break;
            case ENOMEM: {
               panic("[!] OOM, reason: \"%s\"", strerror(errno));
            }
            default: error = IOE_Unknown;
         }
         goto failure;
      }

      Str file = Str_copy_from(entry->d_name, strlen(entry->d_name));
      if (strcmp(file.chars, ".") == 0 || strcmp(file.chars, "..") == 0) {
         continue;
      }

      Vector_push(&Str_files, &file);
   }

   if (Str_files.length > 0)
      self.Str_files = Vector_to_array(&Str_files);
   else
      self.Str_files = Array_dummy();

   return (DirResult) {
      .ok = self,
      .is_err = false
   };

failure:
   return (DirResult) {
      .err = error,
      .is_err = true
   };
}

void Directory_close(Directory* self, bool* failed) {
   mcu_assert(self != nullptr, "Can't close a null Directory");

   Str_free(&self->path);
   for (usize i = 0; i < self->Str_files.length; ++i) {
      Str_free(Array_get(&self->Str_files, i));
   }
   Array_free(&self->Str_files);

   if (closedir(self->dirent_dir) != 0) {
      if (failed == nullptr) {
         panic("Failed to close directory \"%s\", reason: %s", self->path.chars, strerror(errno));
      }

      *failed = true;
      return;
   }

   self->dirent_dir = nullptr;
   if (failed != nullptr) {
      *failed = false;
   }
}

bool shell_command(const cstr command) {
   if (command == nullptr) {
      return false;
   }

   return system(command) != 0;
}

