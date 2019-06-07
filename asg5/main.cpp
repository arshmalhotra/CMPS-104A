// main.cpp
// Travis Takai
// 1375886
// ttakai@ucsc.edu
// CS104a

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

// #include "auxlib.h"
#include "string_set.h"
#include "lyutils.h"
#include "intermediate.h"

using namespace std;

string targetFile;
string CPP = "/usr/bin/cpp -nostdinc";
constexpr size_t LINESIZE = 1024;
extern int yy_flex_debug;
extern int yydebug;
extern FILE* yyin;
extern FILE* outFile;
extern FILE* out;

int exit_status;
// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

void astree::print_tok (FILE* outfile, astree* tree) {
   fprintf (outfile, "\t%zd  %zd.%zd  %d  %s  (%s)\n",
      tree->lloc.filenr, tree->lloc.linenr, tree->lloc.offset,
      tree->symbol, parser::get_tname (tree->symbol),
      tree->lexinfo->c_str());
}

void cpplines (FILE* pipe, const char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy (inputname, filename);
   for (;;) {
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == NULL) break;
      chomp (buffer, '\n');
      sscanf (buffer, "# %d \"%[^\"]\"",
            &linenr, inputname);

      // Create and open .tok file
      const char* fname = (
      targetFile.substr(0, targetFile.size()-3) + ".tok").c_str();
      outFile = fopen(fname, "w");
      int yy_rc = yyparse();
      fclose(outFile);

      ++linenr;
      if(yy_rc != 0) {
         exit_status = EXIT_FAILURE;
         cout << "ERROR in yy_rc" << endl;
      }
   }
}

int main (int argc, char** argv) {
   exit_status = EXIT_SUCCESS;
   yy_flex_debug = 0;
   yydebug = 0;
   exec::execname = basename (argv[0]);

   int option;
   while ((option = getopt(argc, argv, "@:D:ly")) != -1) {
      switch (option) {
         case '@': set_debugflags(optarg); break;
         case 'l': yy_flex_debug = 1; break;
         case 'y': yydebug = 1; break;
         case 'D': CPP = CPP + " -D " + optarg; break;
         default : perror("Usage: oc [-ly] [-@ flag ...] \
      [-D string] program.oc\n"); break;
      }
   }
   if (optind  > argc) {
      errprintf ("Usage: oc [-ly] [-@ flag ...] \
      [-D string] program.oc\n");
      exit (exit_status);
   }

   targetFile = basename(argv[optind]);
   // Check that the file provided has .oc ending
   if(targetFile.find(".oc") == string::npos) {
      perror("Usage: oc [-ly] [-@ flag ...] [-D string] program.oc\n");
      exit(exit_status);
   }
   // Check if file exists
   if (FILE *file = fopen(argv[optind], "r")) {
      fclose(file);
   } else {
      perror("Please use previously created .oc file\n");
      exit(1);
   }

   const char* execname = basename (argv[0]);
   char* filename = argv[optind];
   string command = CPP + " " + filename;
   DEBUGF('c', "command=\"%s\"\n", command.c_str());
   yyin = popen (command.c_str(), "r");
   if (yyin == NULL) {
      exit_status = EXIT_FAILURE;
      fprintf (stderr, "%s: %s: %s\n",
               execname, command.c_str(), strerror (errno));
   } else {
      cpplines (yyin, filename);
      int pclose_rc = pclose (yyin);
      if (pclose_rc != 0) exit_status = EXIT_FAILURE;
   }

   // Create and open .str file
   string name = targetFile.substr(0, targetFile.size()-3) + ".str";
   const char* fname = name.c_str();
   FILE* outFile = fopen(fname, "w");
   string_set::dump(outFile);
   fclose(outFile);

   // Type checking and symbol table setup/output
   symtable();
   name = targetFile.substr(0, targetFile.size()-3) + ".sym";
   fname = name.c_str();
   outFile = fopen(fname, "w");
   bool rc = semantic_analysis(parser::root, outFile);
   if(rc == false) exit_status = EXIT_FAILURE;
   fclose(outFile);


   // Create and open .ast file
   name = targetFile.substr(0, targetFile.size()-3) + ".ast";
   fname = name.c_str();
   out = fopen(fname, "w");
   astree::print(out, parser::root);
   fclose(out);

   // Create and open .oil file
   name = targetFile.substr(0, targetFile.size()-3) + ".oil";
   fname = name.c_str();
   out = fopen(fname, "w");
   rc = traverse(parser::root);
   if(rc == false) exit_status = EXIT_FAILURE;
   fclose(out);

   return exit_status;
}
