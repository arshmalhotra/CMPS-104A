// CMPS104A
// // ASSIGNMENT: ASG1
// // NAME 1: Shlok Gharia
// // EMAIL 1: sgharia@ucsc.edu
// // NAME 2: Arsh Malhotra
// // EMAIL 2: amalhot3@ucsc.edu

#include <string>
using namespace std;

#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>

#include "string_set.h"
#include "auxdebug.h"

string CPP = "/usr/bin/cpp -nostdinc";
constexpr size_t LINESIZE = 1024;
int yy_flex_debug = 0;
int yydebug = 0;

void print_usage() {
  printf("Usage: [-ly] [-@(flags)...] [-D(string)] (program).oc\n");
  exit(2);
}

void chomp (char* string, char delim) {
  size_t len = strlen (string);
  if (len == 0) return;
  char* nlpos = string + len - 1;
  if (*nlpos == delim) *nlpos = '\0';
}

static void eprint_signal (const char* kind, int signal) {
  fprintf (stderr, ", %s %d", kind, signal);
  const char* sigstr = strsignal (signal);
  if (sigstr != nullptr) fprintf (stderr, " %s", sigstr);
}

void eprint_status (const char* command, int status) {
   if (status == 0) return; 
   fprintf (stderr, "%s: status 0x%04X", command, status);
   if (WIFEXITED (status)) {
      fprintf (stderr, ", exit %d", WEXITSTATUS (status));
   }
   if (WIFSIGNALED (status)) {
      eprint_signal ("Terminated", WTERMSIG (status));
      #ifdef WCOREDUMP
      if (WCOREDUMP (status)) fprintf (stderr, ", core dumped");
      #endif
   }
   if (WIFSTOPPED (status)) {
      eprint_signal ("Stopped", WSTOPSIG (status));
   }
   if (WIFCONTINUED (status)) {
      fprintf (stderr, ", Continued");
   }
   fprintf (stderr, "\n");
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
    linenr++;
  }
}

int main (int argc, char** argv) {
  opterr = 0;
  int option;
  while ((option = getopt (argc, argv, "@:D:ly")) != -1) {
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

  const char* execname = basename (argv[0]);
  char* filename = argv[argc-1];
  char* program = basename (argv[argc-1]);
  char prog[LINESIZE];
  strcpy(prog, strtok(program, "."));
  strcat(program, ".oc");
  string command = CPP + " " + filename;
  int exit_status = EXIT_SUCCESS;
  
  FILE* pipe = popen (command.c_str(), "r");
  if (pipe == nullptr) {
    exit_status = EXIT_FAILURE;
    fprintf (stderr, "%s: %s: %s\n",
             execname, command.c_str(), strerror (errno));
  } else {
    cpplines (pipe);
    int pclose_rc = pclose (pipe);
    eprint_status (command.c_str(), pclose_rc);
    if (pclose_rc != 0) exit_status = EXIT_FAILURE;
    strcat(prog, ".str");
    FILE* pipeout = fopen(prog, "w+");
    string_set::dump (pipeout);
    fclose (pipeout);
  }
  return exit_status;
}

