#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

#define buffer_size 4096

static char   buffer[buffer_size];
static size_t cur = 0;

static char peek() { return buffer[cur]; }
static bool next() {
  if (buffer[cur] != '\0') {
    ++cur;
    return true;
  }
  return false;
}
static char next_peek() {
  next();
  return peek();
}

void get_cmd(const char* _cmd) {
  strncpy(buffer, _cmd, buffer_size);

  // guarantee that buffer is a null-terminated string, see `strncpy`
  buffer[buffer_size - 1] = '\0';
}

static int64_t parse_dec() {
  int64_t sum = peek() & 0b1111;

  char c = next_peek();
  while (isdigit(c)) {
    sum *= 10;
    sum += c & 0b1111;
    c = next_peek();
  }

  return sum;
}

static int64_t hex2num(char _c) {
  if (isdigit(_c)) return _c & 0b1111;
  return (_c & 0b01011111) - 55;
}

static int64_t parse_hex() {
  int64_t sum = 0;
  char    c   = peek();
  if (!isxdigit(c)) fatal(NUMERR, "invalid hex number");
  do {
    sum <<= 4;
    sum += hex2num(c);
    c = next_peek();
  } while (isxdigit(c));
  return sum;
}

static bool isodigit(char _c) {
  switch (_c) {
  case '0' ... '7': return true;
  default: return false;
  }
}
static int64_t parse_oct() {
  int64_t sum = 0;
  char    c   = peek();
  if (!isodigit(c)) fatal(NUMERR, "invalid oct number");
  do {
    sum <<= 3;
    sum += c & 0b111;
    c = next_peek();
  } while (isodigit(c));
  return sum;
}

static bool isbdigit(char _c) {
  switch (_c) {
  case '0' ... '1': return true;
  default: return false;
  }
}
static int64_t parse_bin() {
  int64_t sum = 0;
  char    c   = peek();
  if (!isbdigit(c)) fatal(NUMERR, "invalid bin number");
  do {
    sum <<= 1;
    sum += c & 0b1;
    c = next_peek();
  } while (isbdigit(c));
  return sum;
}
static int64_t parse_num() {
  switch (peek()) {
  case '0':
    switch (next_peek()) {
    case 'x':
    case 'X': next(); return parse_hex();
    case 'b':
    case 'B': next(); return parse_bin();
    case 'o':
    case 'O': next(); return parse_oct();
    default: fatal(NUMPREFIXERR, "invalid digit prefix");
    }
  default: return parse_dec();
  }
}

static const char* parse_var() {
  char   c     = peek();
  size_t size  = 0;
  size_t start = cur;
  do {
    ++size;
    c = next_peek();
  } while (isalnum(c) || c == '_');

  char* var = malloc((size + 1) * sizeof(char));
  strncpy(var, buffer + start, size);
  var[size] = '\0';
  return var;
}

static Token* mk_plain_token(TokenType _tt) {
  Token* tk = malloc(sizeof(Token));
  tk->tt    = _tt;
  tk->next  = NULL;
  return tk;
}
static Token* mk_num_token(int64_t _num) {
  Token* tk = mk_plain_token(NUM);
  tk->num   = _num;
  return tk;
}
static Token* mk_var_token(const char* _var) {
  Token* tk = mk_plain_token(VAR);
  tk->var   = _var;
  return tk;
}

Token* parse() {
  Token* head = malloc(sizeof(Token));
  head->next  = NULL;
  Token* cur  = head;

  char c;
  while (true) {
    c = peek();
    if (isspace(c)) {
      next();
      continue;
    }
    if (isdigit(c)) {
      cur = cur->next = mk_num_token(parse_num());
      continue;
    }
    if (isalpha(c) || c == '_') {
      cur = cur->next = mk_var_token(parse_var());
      continue;
    }
    if (c == '\0') break;
    switch (c) {
    case '(': cur = cur->next = mk_plain_token(LPARENT); break;
    case ')': cur = cur->next = mk_plain_token(RPARENT); break;
    case '+': cur = cur->next = mk_plain_token(ADD); break;
    case '-': cur = cur->next = mk_plain_token(SUB); break;
    case '*': cur = cur->next = mk_plain_token(MUL); break;
    case '/': cur = cur->next = mk_plain_token(DIV); break;
    case '%': cur = cur->next = mk_plain_token(MOD); break;
    case '=': cur = cur->next = mk_plain_token(ASSIGN); break;
    case ';': cur = cur->next = mk_plain_token(SEMICOLON); break;
    case '@': cur = cur->next = mk_plain_token(AT); break;
    default: fatal(UNRECOGCHARERR, "unrecognized char '%c'", c);
    }
    next();
  }
  return head;
}
void free_token_list(Token* _t) {
  Token* cur = _t;
  while (cur != NULL) {
    _t = cur->next;
    free((void*)cur->var);
    free(cur);
    cur = _t;
  }
}
void print_token_list(Token* _tk) {
  while (_tk != NULL) {
    switch (_tk->tt) {
    case NUM: printf("NUM: %lu\n", _tk->num); break;
    case VAR: printf("VAR: %s\n", _tk->var); break;
    case LPARENT: printf("LITERAL: %c\n", '('); break;
    case RPARENT: printf("LITERAL: %c\n", ')'); break;
    case ADD: printf("LITERAL: %c\n", '+'); break;
    case SUB: printf("LITERAL: %c\n", '-'); break;
    case MUL: printf("LITERAL: %c\n", '*'); break;
    case DIV: printf("LITERAL: %c\n", '/'); break;
    case MOD: printf("LITERAL: %c\n", '%'); break;
    case ASSIGN: printf("LITERAL: %c\n", '='); break;
    case SEMICOLON: printf("LITERAL: %c\n", ';'); break;
    case AT: printf("LITERAL: %c\n", '@'); break;
    }
    _tk = _tk->next;
  }
}
const char* take_var(Token* _tk) {
  const char* ret = _tk->var;
  _tk->var        = NULL;
  return ret;
}
void give_var(Token* _tk, const char* _var) { _tk->var = _var; }
