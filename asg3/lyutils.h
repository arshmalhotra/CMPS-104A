// CMPS104A
// ASSIGNMENT: ASG3
// NAME 1: Shlok Gharia
// EMAIL 1: sgharia@ucsc.edu
// NAME 2: Arsh Malhotra
// EMAIL 2: amalhot3@ucsc.edu

#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>
using namespace std;

#include <stdio.h>

#include "astree.h"
#include "auxlib.h"

#define YYEOF 0

extern FILE* yyin;
extern char* yytext;
extern int yy_flex_debug;
extern int yydebug;
extern int yyleng;

int yylex();
int yylex_destroy();
int yyparse();
void yyerror (const char* message);

struct scanner {
  static bool interactive;
  static location lloc;
  static size_t last_yyleng;
  static vector<string> filenames;
  static const string* filename (int filenr);
  static void newfilename (const string& filename);
  static void advance();
  static void newline();
  static void badchar (unsigned char bad);
  static void include();
  static int token (int symbol);
  static int badtoken (int symbol);
};

struct parser {
  static astree* root;
  static const char* get_tname (int symbol);
};

#define YYSTYPE_IS_DECLARED
typedef astree* YYSTYPE;
#include "yyparse.h"

#endif
