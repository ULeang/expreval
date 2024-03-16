#include <stdlib.h>

#include "env.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char** argv) {
  if (argc != 2) exit(-1);
  Token* tk = tokenize(argv[1]);
  // print_token_list(tk->next);
  Node* nd = parse(tk);
  // print_ast(nd);
  eval(nd);
  return 0;
}
