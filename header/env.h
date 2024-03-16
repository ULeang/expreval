#ifndef __ENV_H__
#define __ENV_H__

#include <stdbool.h>
#include <stdint.h>

#include "ast.h"

typedef struct Env {
  const char* var;
  int64_t     value;

  struct Env* next;
} Env;

// eval once, automatically free the _ast
void eval_once(Node* _ast);
// eval with env, automatically free `_ast` but not `_env`
bool eval_with(Env* _env, Node* _ast);

Env* mk_env();
bool insert_or_update(Env* _env, const char* _var, int64_t _value);
Env* find(Env* _env, const char* _var);
void free_env(Env* _env);

#endif  // !__ENV_H__
