// CMPS104A
// ASSIGNMENT: ASG4
// NAME 1: Shlok Gharia
// EMAIL 1: sgharia@ucsc.edu
// NAME 2: Arsh Malhotra
// EMAIL 2: amalhot3@ucsc.edu

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auxlib.h"
#include "lyutils.h"

bool scanner::interactive = true;
location scanner::lloc = {0, 1, 0};
size_t scanner::last_yyleng = 0;
vector<string> scanner::filenames;
FILE* outFile;

astree* parser::root = nullptr;

const string* scanner::filename (int filenr) {
  return &scanner::filenames.at(filenr);
}

void scanner::newfilename (const string& filename) {
  scanner::lloc.filenr = scanner::filenames.size();
  scanner::filenames.push_back (filename);
}

void scanner::advance() {
  if (not interactive) {
    if (scanner::lloc.offset == 0) {
       printf (";%2zd.%3zd: ",
               scanner::lloc.filenr, scanner::lloc.linenr);
    }
    printf ("%s", yytext);
  }
  scanner::lloc.offset += last_yyleng;
  last_yyleng = yyleng;
}

void scanner::newline() {
  ++scanner::lloc.linenr;
  scanner::lloc.offset = 0;
}

void scanner::badchar (unsigned char bad) {
  char buffer[16];
  snprintf (buffer, sizeof buffer,
           isgraph (bad) ? "%c" : "\\%03o", bad);
  errllocprintf (scanner::lloc, "invalid source character (%s)\n",
                buffer);
}


void scanner::include() {
  size_t linenr;
  static char filename[0x1000];
  assert (sizeof filename > strlen (yytext));
  int scan_rc = sscanf (yytext, "# %zu \"%[^\"]\"", &linenr, filename);
  if (scan_rc != 2) {
    errprintf ("%s: invalid directive, ignored\n", yytext);
  }else {
    if (yy_flex_debug) {
       fprintf (stderr, "--included # %zd \"%s\"\n",
                linenr, filename);
    }
    scanner::lloc.linenr = linenr - 1;
    scanner::newfilename (filename);
  }
}

int scanner::token (int symbol) {
  yylval = new astree (symbol, scanner::lloc, yytext);
  return symbol;
}

int scanner::badtoken (int symbol) {
  errllocprintf (scanner::lloc, "invalid token (%s)\n", yytext);
  return scanner::token (symbol);
}

void yyerror (const char* message) {
  assert (not scanner::filenames.empty());
  errllocprintf (scanner::lloc, "%s\n", message);
}
