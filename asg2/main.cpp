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
