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
#include "emitter.h"

using namespace std;

string CPP = "/usr/bin/cpp -nostdinc";
constexpr size_t LINESIZE = 1024;
extern FILE* outFile;
int exit_status;
string cpp_command;
char* basefilename;

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

void cpplines (FILE* pipe, const char* filename) {
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
    string fname = std::string(filename);
    string tokFilename = fname.substr(0, fname.size()-3) + ".tok";
    const char* tokFile = tokFilename.c_str();
    outFile = fopen(tokFile, "w");
    int token;
    while((token = yylex()) != YYEOF) {
      if(token == -1) {
        fprintf(stderr, "%s",
          "Encountered bad set of characters\n");
        exit_status = EXIT_FAILURE;
        break;
      }
    }
    fclose(outFile);
    linenr++;
  }
}

void cpp_pclose () {
  int pclose_rc = pclose (yyin);
  eprint_status (cpp_command.c_str(), pclose_rc);
  if (pclose_rc != 0) exec::exit_status = EXIT_FAILURE;
}

void cpp_popen (const char* execname, const char* filename) {
  cpp_command = CPP + " " + filename;
  yyin = popen (cpp_command.c_str(), "r");
  if (yyin == nullptr) {
    exit_status = EXIT_FAILURE;
    fprintf (stderr, "%s: %s: %s\n",
             execname, cpp_command.c_str(), strerror (errno));
  }else {
    if (yy_flex_debug) {
      fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
               cpp_command.c_str(), fileno (yyin));
    }
    cpplines (yyin, basefilename);
    string fname = std::string(basefilename);
    string strFilename = fname.substr(0, fname.size()-3) + ".str";
    const char* strFile = strFilename.c_str();
    FILE* pipeout = fopen(strFile, "w+");
    string_set::dump (pipeout);
    fclose (pipeout);

    int parse_rc = yyparse();
    cpp_pclose();
    yylex_destroy();
    if (yydebug or yy_flex_debug) {
      fprintf (stderr, "Dumping parser::root:\n");
      if (parser::root != nullptr) parser::root->dump_tree(stderr);
      fprintf (stderr, "Dumping string_set:\n");
      string_set::dump (stderr);
    }
    if (parse_rc) {
      errprintf ("parse failed (%d)\n", parse_rc);
    } else {
      astree::print (stdout, parser::root);
      emit_sm_code (parser::root);
      delete parser::root;
    }
  }
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
  const char* execname = basename (argv[0]);
  const char* filename = optind == argc ? "-" : argv[optind];
  basefilename = basename(argv[optind]);
  cpp_popen (execname, filename);
}

int main (int argc, char** argv) {
  exit_status = EXIT_SUCCESS;

  scan_opts(argc, argv);

  return exit_status;
}
