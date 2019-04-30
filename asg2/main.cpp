// main.cpp
// Travis Takai
// 1375996
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

#include "auxlib.h"
#include "string_set.h"
#include "lyutils.h"

using namespace std;

string targetFile;
string CPP = "/usr/bin/cpp -nostdinc";
constexpr size_t LINESIZE = 1024;
extern int yy_flex_debug;
extern int yydebug;
extern FILE* yyin;
extern FILE* outFile;
int exit_status;
// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
   char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

void astree::pretty_print (FILE* outfile, astree* tree) {
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
      const char* fname = (
   targetFile.substr(0, targetFile.size()-3) + ".tok").c_str();
      outFile = fopen(fname, "w");
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
      ++linenr;
   }
}

int main (int argc, char** argv) {
   exit_status = EXIT_SUCCESS;
   yy_flex_debug = 0;
   yydebug = 0;

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

   // Set up output file and write to it
   string name = targetFile.substr(0, targetFile.size()-3) + ".str";
   const char* fname = name.c_str();
   FILE* outFile = fopen(fname, "w");
   string_set::dump(outFile);
   fclose(outFile);
   return exit_status;
}
