#define _DEFAULT_SOURCE

#include <dirent.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <asm-generic/errno-base.h>

#include "core.h"
#include "io.h"

// @todo: function for checking if d_type is supported

const cstr FSType_to_cstr(FSType self) {
   switch (self) {
      case FST_File: return "File";
      case FST_Dir:  return "Directory";
   }

   return "Unknown";
}

FSType FSType_from_d_type(unsigned char d_type) {
   switch (d_type) {
      case DT_REG: return FST_File;
      case DT_DIR: return FST_Dir;
      default:     return (FSType) 12345;
   }
}

bool walk_directory_impl(nullable const cstr path, nullable OnWalkDirEntry on_entry, bool* stop) {
   if (path == nullptr || on_entry == nullptr) return true;
   bool result;

   DIR* root = opendir(path);
   if (root == nullptr) return true;

   errno = 0;
   struct dirent* entry;
   String full_path = String_from((cstr) path);
   
   loop {
      entry = readdir(root);
      if (entry == nullptr) {
         String_free(&full_path);
         return errno != 0;
      }

      if (strcmp(entry->d_name, ".")  == 0) continue;
      if (strcmp(entry->d_name, "..") == 0) continue;
      if (entry->d_type != DT_DIR && entry->d_type != DT_REG)
         continue;

      String_append(&full_path, '/');
      String_append_cstr(&full_path, entry->d_name);
      WalkDirAction next = on_entry(full_path.chars, FSType_from_d_type(entry->d_type));

      switch (next) {
         case WDA_Continue: break;
         case WDA_Failure:  goto failure;
         case WDA_Skip:     goto skip;
         case WDA_Stop: {
            *stop = true;
            goto success;
         }

         default: goto failure;
      }

      switch (entry->d_type) {
         case DT_DIR: {
            if (walk_directory_impl(full_path.chars, on_entry, stop))
               goto failure;
            if (*stop) goto success;
         } break;
      }

   skip:
      String_clear(&full_path);
      String_append_cstr(&full_path, (cstr) path);
   }

success:
   result = false;
   goto cleanup;

failure:
   result = true;

cleanup:
   if (closedir(root)) result = true;
   String_free(&full_path);
   return result;
}

bool walk_directory(nullable const cstr path, nullable OnWalkDirEntry on_entry) {
   bool stop = false; 
   
   return walk_directory_impl(path, on_entry, &stop);
}

