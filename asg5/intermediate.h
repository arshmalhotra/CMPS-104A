// CMPS104A
// ASSIGNMENT: ASG5
// NAME 1: Shlok Gharia
// EMAIL 1: sgharia@ucsc.edu
// NAME 2: Arsh Malhotra
// EMAIL 2: amalhot3@ucsc.edu

#include <stdio.h>

#include "symtable.h"
using namespace std;

extern FILE* out;

#define TAB "        "

string format_var(astree* node);
void print_function(astree* node);
void print_struct(astree* node);
void print_var(astree* node);
bool traverse(astree* node);
string traverse_block(astree* node);
string compare(astree* node);
