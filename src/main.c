#include <stdlib.h>

#include "env.h"
#include "lexer.h"
#include "parser.h"

int main(int argc, char** argv) {
  if (argc != 2) exit(-1);
  // get_cmd(" _var1 = 10; _var1 + 1");
  get_cmd(argv[1]);
  Token* tk = parse();
  // print_token_list(tk->next);
  Node* nd = parse_to_ast(tk);
  // print_ast(nd);
  eval(nd);
  return 0;
}
