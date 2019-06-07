// CMPS104A
// ASSIGNMENT: ASG5
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
  parent = nullptr;
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

astree* astree::adopt (astree* child1, astree* child2, astree* child3){
  if (child1 != nullptr) {
    child1->parent = this;
    children.push_back (child1);
  }
  if (child2 != nullptr) {
    child2->parent = this;
    children.push_back (child2);
  }
  if (child3 != nullptr) {
    child3->parent = this;
    children.push_back (child3);
  }
  return this;
}

astree* astree::adopt_sym (astree* child, int symbol_) {
  symbol = symbol_;
  return adopt (child);
}

void astree::dump_node (FILE* outfile) {
  fprintf (outfile, "%p->{%s %zd.%zd.%zd \"%s\":",
          this, parser::get_tname (symbol),
          lloc.filenr, lloc.linenr, lloc.offset,
          lexinfo->c_str());
  for (size_t child = 0; child < children.size(); ++child) {
    fprintf (outfile, " %p", children.at(child));
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

void astree::print (FILE* out, astree* tree, int depth) {
   for(int i = 0; i < depth; ++i) {
      fprintf (out, "|  ");
   }

   attr_bitset attr = tree->attributes;
   string list;

   if (attr[ATTR_field] == 0) list += "{"+to_string(tree->blocknr)+"} ";

   if (attr[ATTR_field] == 1) {
      list += "field {" + tree->struct_name + "} ";
   } if (attr[ATTR_struct] == 1) {
      list += "struct \"" + tree->struct_name + "\" ";
   } if (attr[ATTR_int] == 1) {
      list += "int ";
   } if (attr[ATTR_string] == 1) {
      list += "string ";
   } if (attr[ATTR_variable] == 1) {
      list += "variable ";
   } if (attr[ATTR_null] == 1) {
      list += "null ";
   } if (attr[ATTR_function] == 1) {
      list += "function ";
   } if (attr[ATTR_lval] == 1) {
      list += "lval ";
   } if (attr[ATTR_param] == 1) {
      list += "param ";
   } if (attr[ATTR_const] == 1) {
      list += "const ";
   } if (attr[ATTR_vreg] == 1) {
      list += "vreg ";
   } if (attr[ATTR_vaddr] == 1) {
      list += "vaddr ";
   } if (attr[ATTR_void] == 1) {
      list += "void ";
   } if (attr[ATTR_array] == 1) {
      list += "array ";
   } if (attr[ATTR_prototype] == 1) {
      list += "prototype ";
   }

   const char *tname = parser::get_tname (tree->symbol);
   if (strstr (tname, "TOK_") == tname) tname += 4;
   fprintf (out, "%s \"%s\" %zd.%zd.%zd %s\n",
         tname, tree->lexinfo->c_str(),
         tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset,
         list.c_str());

   for (astree* child: tree->children) {
      astree::print (out, child, depth + 1);
   }
}

void astree::swap (int token) {
   symbol = token;
}

void astree::fix_function (astree* node) {
   for(uint i = 1; i < node->children.size(); ++i) {
      this->children.push_back(node->children[i]);
   }

   for(uint i = 0; i < node->children.size()+i-1; ++i) {
      node->children.erase(node->children.begin()+1);
   }
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

astree* new_root () {
   return new astree (ROOT, {0, 0, 0}, "<<ROOT>>");
}
