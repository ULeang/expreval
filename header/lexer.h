#ifndef __LEXER_H__
#define __LEXER_H__

#include <stdint.h>

typedef enum TokenType {
  NUM,
  LPARENT,
  RPARENT,
  ADD,
  SUB,
  MUL,
  DIV,
  MOD,
  SEMICOLON,
  VAR,
  ASSIGN,
  AT,
} TokenType;

typedef struct Token {
  TokenType   tt;
  int64_t     num;  // enable if NUM
  const char* var;  // enable if VAR

  struct Token* next;
} Token;

void   get_cmd(const char* _cmd);
Token* parse();  // return the head of the token list(contains no valid token)
void   free_token_list(Token* _t);  // free the whole list, including `var`
void   print_token_list(Token* _tk);
// sets `_tk->var` to NULL and return the previous value
const char* take_var(Token* _tk);
// if one production func takes var from a token, then the func fails,
// the var should be given back to the token
void give_var(Token* _tk, const char* _var);

#endif  // !__LEXER_H__
