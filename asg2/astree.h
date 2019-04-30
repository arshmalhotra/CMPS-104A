#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <string>
#include <vector>
using namespace std;

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

  astree (int symbol, const location&, const char* lexinfo);
  ~astree();
  astree* adopt (astree* child1, astree* child2 = nullptr);
  astree* adopt_sym (astree* child, int symbol);
  void dump_node (FILE*);
  void dump_tree (FILE*, int depth = 0);
  static void dump (FILE* outfile, astree* tree);
  static void print (FILE* outfile, astree* tree, int depth = 0);
  void print_tok (FILE* outfile, astree* tree);
};

void destroy (astree* tree1, astree* tree2 = nullptr);

void errllocprintf (const location&, const char* format, const char*);

#endif
