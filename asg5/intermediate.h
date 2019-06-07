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
