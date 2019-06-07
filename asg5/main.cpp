// CMPS104A
// ASSIGNMENT: ASG4
// NAME 1: Shlok Gharia
// EMAIL 1: sgharia@ucsc.edu
// NAME 2: Arsh Malhotra
// EMAIL 2: amalhot3@ucsc.edu

#include <string>
#include <iostream>
#include <fstream>

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <ctype.h>
#include <unistd.h>

#include "auxlib.h"
#include "string_set.h"
#include "lyutils.h"
#include "intermediate.h"

using namespace std;

string CPP = "/usr/bin/cpp -nostdinc";
constexpr size_t LINESIZE = 1024;
extern FILE* outFile;
int exit_status;
string cpp_command;
string basefilename;

void print_usage() {
  errprintf ("Usage: %s [-ly] [-@(flags)...] [-D(string)] filename\n",
             exec::execname.c_str());
  exit (EXIT_FAILURE);
}

void chomp (char* string, char delim) {
  size_t len = strlen (string);
  if (len == 0) return;
  char* nlpos = string + len - 1;
  if (*nlpos == delim) *nlpos = '\0';
}

void astree::print_tok (FILE* outfile, astree* tree) {
   fprintf (outfile, "%zd  %zd.%zd  %d  %s  (%s)\n",
      tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset,
      tree->symbol, parser::get_tname (tree->symbol),
      tree->lexinfo->c_str());
}

void cpplines (FILE* pipe) {
  int linenr = 1;
  for (;;) {
    char buffer[LINESIZE];
    const char* fgets_rc = fgets (buffer, LINESIZE, pipe);
    if (fgets_rc == nullptr) break;
    chomp (buffer, '\n');
    char inputname[LINESIZE];
    int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                            &linenr, inputname);
    if (sscanf_rc == 2) {
      continue;
    }
    char* savepos = nullptr;
    char* bufptr = buffer;
    for (int tokenct = 1;; tokenct++) {
      char* token = strtok_r (bufptr, " \t\n", &savepos);
      bufptr = nullptr;
      if (token == nullptr) break;
      string_set::intern (token);
    }
    // string basefilename = std::string(filename);
    string tokFilename = basefilename.substr(0, basefilename.size()-3) + ".tok";
    const char* tokFile = tokFilename.c_str();
    outFile = fopen(tokFile, "w");
    int yy_rc = yyparse();
    fclose(outFile);
    linenr++;
    if(yy_rc != 0) {
       exit_status = EXIT_FAILURE;
       cout << "ERROR in yy_rc" << endl;
    }
  }
}

void cpp_pclose () {
  int pclose_rc = pclose (yyin);
  if (pclose_rc != 0) exec::exit_status = EXIT_FAILURE;
}

void cpp_popen (const char* filename) {
  printf("does");
  cpp_command = CPP + " " + filename;
  DEBUGF('c', "command=\"%s\"\n", cpp_command.c_str());
  yyin = popen (cpp_command.c_str(), "r");
  printf("it");
  if (yyin == nullptr) {
    exit_status = EXIT_FAILURE;
    fprintf (stderr, "%s: %s: %s\n",
             exec::execname.c_str(), cpp_command.c_str(), strerror (errno));
  }else {
    printf("get");
    cpplines (yyin);
    printf("here\n");
    cpp_pclose();
    printf("does it get here\n");
  }

  // string basefilename = std::string(basefilename);
  printf("where");
  string strFilename = basefilename.substr(0, basefilename.size()-3) + ".str";
  printf("is");
  const char* strFile = strFilename.c_str();
  printf("the");
  FILE* pipeout = fopen(strFile, "w");
  printf("problem");
  string_set::dump (pipeout);
  printf("here");
  fclose (pipeout);

  symtable();
  string symFilename = basefilename.substr(0, basefilename.size()-3) + ".sym";
  const char* symFile = symFilename.c_str();
  outFile = fopen(symFile, "w");
  bool rc = semantic_analysis(parser::root, outFile);
  if(rc == false) exit_status = EXIT_FAILURE;
  fclose(outFile);

  string astFilename = basefilename.substr(0, basefilename.size()-3) + ".ast";
  const char* astFile = astFilename.c_str();
  outFile = fopen(astFile, "w");
  astree::print(outFile, parser::root);
  fclose(outFile);

  string oilFilename = basefilename.substr(0, basefilename.size()-3) + ".oil";
  const char* oilFile = oilFilename.c_str();
  outFile = fopen(oilFile, "w");
  rc = traverse(parser::root);
  if(rc == false) exit_status = EXIT_FAILURE;
  fclose(outFile);
}

void scan_opts (int argc, char** argv) {
  opterr = 0;
  yy_flex_debug = 0;
  yydebug = 0;
  int option;
  while((option = getopt (argc, argv, "@:D:ly")) != EOF) {
    switch (option) {
      case 'l':
        yy_flex_debug = 1;
        break;
      case 'y':
        yydebug = 1;
        break;
      case 'D':
        CPP = CPP + " -D " + optarg;
        break;
      case '@':
        set_debugflags(optarg);
        break;
      default:
        errprintf ("bad option (%c)\n", optopt);
        break;
    }
  }
  if (optind > argc) {
    print_usage();
  }
  exec::execname = basename (argv[0]);
  const char* filename = optind == argc ? "-" : argv[optind];
  basefilename = basename(argv[optind]);
  cpp_popen (filename);
}

int main (int argc, char** argv) {
  exit_status = EXIT_SUCCESS;
  scan_opts(argc, argv);

  return exit_status;
}
