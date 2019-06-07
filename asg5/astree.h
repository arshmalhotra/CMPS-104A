// CMPS104A
// ASSIGNMENT: ASG4
// NAME 1: Shlok Gharia
// EMAIL 1: sgharia@ucsc.edu
// NAME 2: Arsh Malhotra
// EMAIL 2: amalhot3@ucsc.edu

#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
using namespace std;

#include "symtable.h"
#include "auxlib.h"

struct location {
  size_t filenr;
  size_t linenr;
  size_t offset;
};

struct astree {
  int symbol;
  location lloc;
  const string* lexinfo;
  vector<astree*> children;
  astree* parent;
  attr_bitset attributes;
  string struct_name = NULL;
  int blocknr;

  astree (int symbol, const location&, const char* lexinfo);
  ~astree();
  astree* adopt (astree* child1, astree* child2 = nullptr,
                 astree* child3 = nullptr);
  astree* adopt_sym (astree* child, int symbol);
  void swap (int token);
  void dump_node (FILE*);
  void dump_tree (FILE*, int depth = 0);
  static void dump (FILE* outfile, astree* tree);
  static void print (FILE* outfile, astree* tree, int depth = 0);
  void print_tok (FILE* outfile, astree* tree);
  void fix_function (astree* node);
};

extern astree* asroot;
astree* new_root();
void destroy (astree* tree1, astree* tree2 = nullptr,
   astree* tree3 = nullptr, astree* tree4 = nullptr);

void errllocprintf (const location&, const char* format, const char*);

#endif
