#ifndef __ENV_H__
#define __ENV_H__

#include <stdbool.h>
#include <stdint.h>

#include "ast.h"

void eval(Node* _ast);

typedef struct Env {
  const char* var;
  int64_t     value;

  struct Env* next;
} Env;

Env* mk_env();
bool insert_or_update(Env* _env, const char* _var, int64_t _value);
Env* find(Env* _env, const char* _var);
void free_env(Env* _env);

#endif  // !__ENV_H__
