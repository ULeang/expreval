#include <stdlib.h>

#include "env.h"
#include "lexer.h"
#include "parser.h"
#include "repl.h"

int main(int argc, char** argv) {
  repl_launch();
  // if (argc != 2) exit(-1);
  // Token* tk = tokenize(argv[1]);
  // // print_token_list(tk->next);
  // Node* nd = parse(tk);
  // // print_ast(nd);
  // eval_once(nd);
  return 0;
}
