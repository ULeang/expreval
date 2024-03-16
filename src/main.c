#include <string.h>

#include "env.h"
#include "lexer.h"
#include "parser.h"
#include "repl.h"
#include "util.h"

void _usage() { usage(CMDARGERR, "PROG [(-c cmd|script)]"); }

int main(int argc, char** argv) {
  enum { CMD, SCRIPT, REPL } mode;
  const char* cmd_or_script;
  if (argc == 1)
    mode = REPL;
  else {
    for (int i = 1; i < argc; ++i) {
      if (argv[i][0] == '-') {
        if (strcmp(argv[i] + 1, "c") == 0) {
          if (i + 1 == argc - 1) {
            mode          = CMD;
            cmd_or_script = argv[i + 1];
            ++i;
          } else {
            _usage();
          }
        } else {
          _usage();
        }
      } else {
        if (i == argc - 1) {
          mode          = SCRIPT;
          cmd_or_script = argv[i];
        } else {
          _usage();
        }
      }
    }
  }
  switch (mode) {
  case CMD: {
    Token* t = tokenize(cmd_or_script);
    Node*  n = parse(t);
    eval_once(n);
    break;
  }
  case REPL: repl_launch(); break;
  case SCRIPT: scri_interpret(cmd_or_script); break;
  }
  return 0;
}
