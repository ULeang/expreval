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

Token* tokenize(const char*_cmd);  // return the head of the token list(contains no valid token)
void   free_token_list(Token* _t);  // free the whole list, including `var`
void   print_token_list(Token* _tk);
// dup `_tk->var` instead of take it, to eaze mental burden
const char* dup_var(Token* _tk);

#endif  // !__LEXER_H__
