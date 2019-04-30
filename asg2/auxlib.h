// CMPS104A
// ASSIGNMENT: ASG2
// NAME 1: Shlok Gharia
// EMAIL 1: sgharia@ucsc.edu
// NAME 2: Arsh Malhotra
// EMAIL 2: amalhot3@ucsc.edu

#ifndef __AUXLIB_H__
#define __AUXLIB_H__

#include <string>
using namespace std;

#include <stdarg.h>


struct exec {
   static string execname;
   static int exit_status;
};

void veprintf (const char* format, va_list args);

void eprintf (const char* format, ...);

void errprintf (const char* format, ...);

void syserrprintf (const char* object);

void eprint_status (const char* command, int status);


#define STUBPRINTF(...) \
        __stubprintf (__FILE__, __LINE__, __PRETTY_FUNCTION__, \
                      __VA_ARGS__)
void __stubprintf (const char* file, int line, const char* func,
                   const char* format, ...);


void set_debugflags (const char* flags);

bool is_debugflag (char flag);

#ifdef NDEBUG

#define DEBUGF(FLAG,...)
#define DEBUGSTMT(FLAG,STMTS)
#else
void __debugprintf (char flag, const char* file, int line,
                    const char* func, const char* format, ...);
#define DEBUGF(FLAG,...) \
        __debugprintf (FLAG, __FILE__, __LINE__, __PRETTY_FUNCTION__, \
                       __VA_ARGS__)
#define DEBUGSTMT(FLAG,STMTS) \
        if (is_debugflag (FLAG)) { DEBUGF (FLAG, "\n"); STMTS }
#endif

#endif
