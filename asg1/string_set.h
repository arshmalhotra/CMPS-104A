// CMPS104A
// // ASSIGNMENT: ASG1
// // NAME 1: Shlok Gharia
// // EMAIL 1: sgharia@ucsc.edu
// // NAME 2: Arsh Malhotra
// // EMAIL 2: amalhot3@ucsc.edu

#ifndef __STRING_SET__
#define __STRING_SET__

#include <string>
#include <unordered_set>

#include <stdio.h>

struct string_set {
   string_set();
   static std::unordered_set<std::string> set;
   static const std::string* intern (const char*);
   static void dump (FILE*);
};

#endif

