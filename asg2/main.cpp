#include <string>
#include <vector>
using namespace std;

#include <assert.h>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "astree.h"
#include "auxlib.h"
#include "emitter.h"
#include "lyutils.h"
#include "string_set.h"

const string cpp_name = "/usr/bin/cpp";
string cpp_command;

void cpp_popen (const char* filename) {
  cpp_command = cpp_name + " " + filename;
  yyin = popen (cpp_command.c_str(), "r");
  if (yyin == nullptr) {
    syserrprintf (cpp_command.c_str());
    exit (exec::exit_status);
  }else {
    if (yy_flex_debug) {
      fprintf (stderr, "-- popen (%s), fileno(yyin) = %d\n",
               cpp_command.c_str(), fileno (yyin));
    }
    lexer::newfilename (cpp_command);
  }
}

void cpp_close () {
  int pclose_rc = pclose (yyin);
  eprint_status (cpp_command.c_str(), pclose_rc);
  if (pclose_rc != 0) exec::exit_status = EXIT_FAILURE;
}

void scan_opts (int argc, char** argv) {
  opterr = 0;
  yy_flex_debug = 0;
  yydebug = 0;
  lexer::interactive = isatty (fileno (stdin))
                   and isatty (fileno (stdout));
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
        print_usage();
        break;
    }
  }

  if (optind > argc) {
    errprintf ("Usage: %s [-ly] [filename]\n",
               exec::execname.c_str());
    exit (exec::exit_status);
  }
  const char* filename = optind == argc ? "-" : argv[optind];
  cpp_popen (filename);
}

int main (int argc, char** argv) {
  exec::execname = basename (argv[0]);
  if (yydebug or yy_flex_debug) {
    fprintf (stderr, "Command:");
    for (char** arg = &argv[0]; arg < &argv[argc]; ++arg) {
      fprintf (stderr, " %s", *arg);
    }
    fprintf (stderr, "\n");
  }
  scan_opts (argc, argv);
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
  return exec::exit_status;
}
