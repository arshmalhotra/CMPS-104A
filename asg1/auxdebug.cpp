// CMPS104A
// // ASSIGNMENT: ASG1
// // NAME 1: Shlok Gharia
// // EMAIL 1: sgharia@ucsc.edu
// // NAME 2: Arsh Malhotra
// // EMAIL 2: amalhot3@ucsc.edu

#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>

#include "auxdebug.h"

const char* debugflags = "";
bool alldebugflags = false;

void set_debugflags (const char* flags) {
   debugflags = flags;
   assert (debugflags != nullptr);
   alldebugflags = true;
   DEBUGF ('x', "Debugflags = \"%s\", all = %d\n",
           debugflags, alldebugflags);
}

bool is_debugflag (char flag) {
   return alldebugflags or strchr (debugflags, flag) != nullptr;
}

void __debugprintf (char flag, const char* file, int line,
                    const char* func, const char* format, ...) {
   va_list args;
   if (not is_debugflag (flag)) return;
   fflush (nullptr);
   va_start (args, format);
   fprintf (stderr, "DEBUGF(%c): %s[%d] %s():\n",
             flag, file, line, func);
   vfprintf (stderr, format, args);
   va_end (args);
   fflush (nullptr);
}

