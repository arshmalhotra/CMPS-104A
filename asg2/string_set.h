// CMPS104A
// ASSIGNMENT: ASG2
// NAME 1: Shlok Gharia
// EMAIL 1: sgharia@ucsc.edu
// NAME 2: Arsh Malhotra
// EMAIL 2: amalhot3@ucsc.edu

#ifndef __STRING_SET__
#define __STRING_SET__

#include <string>
#include <unordered_set>
using namespace std;

#include <stdio.h>

struct string_set {
  string_set();
  static unordered_set<string> set;
  static const string* intern (const char*);
  static void dump (FILE*);
};

#endif
