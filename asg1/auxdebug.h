// CMPS104A
// // ASSIGNMENT: ASG1
// // NAME 1: Shlok Gharia
// // EMAIL 1: sgharia@ucsc.edu
// // NAME 2: Arsh Malhotra
// // EMAIL 2: amalhot3@ucsc.edu

#ifndef __AUXDEBUG_H__
#define __AUXDEBUG_H__

#include <string>
using namespace std;

#include <stdarg.h>

void set_debugflags (const char* flags);
bool is_debugflag (char flag);

#ifdef NDEBUG
#define DEBUGF(FLAG,...)   /**/
#define DEBUGSTMT(FLAG,STMTS) /**/
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

