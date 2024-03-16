#include "repl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "env.h"
#include "lexer.h"
#include "parser.h"
#include "util.h"

static void starting() {
  printf(
      "Simple Eval Env\n"
      "Use #quit to quit\n"
      "Default input buffer size is 4096 chars\n");
}
static void prompt(const char* _p) {
  printf("\033[3;4;32m %s \033[m", _p);
  fflush(stdout);
}

static char* buffer;

void repl_launch() {
  starting();
  buffer = malloc(4096);
  Env* e = mk_env();
  while (true) {
    prompt(">>");
    fgets(buffer, 4096, stdin);
    if (buffer[0] == '#') {
      if (strcmp(buffer + 1, "quit\n") == 0)
        break;
      else
        continue;
    }
    Token* t = tokenize(buffer);
    Node*  n = parse(t);
    eval_with(e, n);
  }
  free_env(e);
  free(buffer);
}

void scri_interpret(const char* _s) {
  FILE* fp = fopen(_s, "r");
  if (!fp) fatal(FILEERR, "`%s` not exists!", _s);
  Env* e = mk_env();
  buffer = malloc(4096);
  while (fgets(buffer, 4096, fp)) {
    Token* t = tokenize(buffer);
    Node*  n = parse(t);
    if (!eval_with(e, n)) break;
  }
  fclose(fp);
  free_env(e);
  free(buffer);
}
