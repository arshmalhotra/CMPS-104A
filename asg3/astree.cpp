// CMPS104A
// ASSIGNMENT: ASG3
// NAME 1: Shlok Gharia
// EMAIL 1: sgharia@ucsc.edu
// NAME 2: Arsh Malhotra
// EMAIL 2: amalhot3@ucsc.edu

#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "astree.h"
#include "string_set.h"
#include "lyutils.h"

astree::astree (int symbol_, const location& lloc_, const char* info) {
  symbol = symbol_;
  lloc = lloc_;
  lexinfo = string_set::intern (info);
}

astree::~astree() {
  while (not children.empty()) {
    astree* child = children.back();
    children.pop_back();
    delete child;
  }
  if (yydebug) {
    fprintf (stderr, "Deleting astree (");
    astree::dump (stderr, this);
    fprintf (stderr, ")\n");
  }
}

astree* astree::adopt (astree* child1, astree* child2, astree* child3) {
  if (child1 != nullptr) children.push_back (child1);
  if (child2 != nullptr) children.push_back (child2);
  if (child3 != nullptr) children.push_back (child3);
  return this;
}

astree* astree::adopt_sym (astree* child, int symbol_) {
  symbol = symbol_;
  return adopt (child);
}

void astree::dump_node (FILE* outfile) {
  fprintf (outfile, "%p->{%s %zd.%zd.%zd \"%s\":",
          static_cast<const void*> (this),
          parser::get_tname (symbol),
          lloc.filenr, lloc.linenr, lloc.offset,
          lexinfo->c_str());
  for (size_t child = 0; child < children.size(); ++child) {
    fprintf (outfile, " %p",
             static_cast<const void*> (children.at(child)));
  }
}

void astree::dump_tree (FILE* outfile, int depth) {
  fprintf (outfile, "%*s", depth * 3, "");
  dump_node (outfile);
  fprintf (outfile, "\n");
  for (astree* child: children) child->dump_tree (outfile, depth + 1);
  fflush (nullptr);
}

void astree::dump (FILE* outfile, astree* tree) {
  if (tree == nullptr) fprintf (outfile, "nullptr");
                 else tree->dump_node (outfile);
}

void astree::print (FILE* outfile, astree* tree, int depth) {
  fprintf (outfile, "; %*s", depth * 3, "");
  fprintf (outfile, "%s \"%s\" (%zd.%zd.%zd)\n",
          parser::get_tname (tree->symbol), tree->lexinfo->c_str(),
          tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset);
  for (astree* child: tree->children) {
    astree::print (outfile, child, depth + 1);
  }
}

void astree::swap (int token) {
   symbol = token;
}

void destroy (astree* tree1, astree* tree2, astree* tree3, astree* tree4) {
  if (tree1 != nullptr) delete tree1;
  if (tree2 != nullptr) delete tree2;
  if (tree3 != nullptr) delete tree3;
  if (tree4 != nullptr) delete tree4;
}

void errllocprintf (const location& lloc, const char* format,
                    const char* arg) {
  static char buffer[0x1000];
  assert (sizeof buffer > strlen (format) + strlen (arg));
  snprintf (buffer, sizeof buffer, format, arg);
  errprintf ("%s:%zd.%zd: %s",
            scanner::filename (lloc.filenr), lloc.linenr, lloc.offset,
            buffer);
}
