#include "env.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "util.h"

Env* mk_env() {
  Env* ret = calloc(sizeof(Env), 1);
  return ret;
}
bool insert_or_update(Env* _env, const char* _var, int64_t _value) {
  Env* cur = _env;
  while (cur->next != NULL && strcmp(_var, cur->next->var) != 0) {
    cur = cur->next;
  }
  if (cur->next == NULL) {
    cur               = _env->next;
    _env->next        = mk_env();
    _env->next->var   = _var;
    _env->next->value = _value;
    _env->next->next  = cur;
    return false;
  }
  cur->next->value = _value;
  return true;
}
Env* find(Env* _env, const char* _var) {
  while (_env->next != NULL && strcmp(_env->next->var, _var) != 0) {
    _env = _env->next;
  }
  return _env->next;
}
void free_env(Env* _env) {
  Env* cur = _env;
  while (cur != NULL) {
    _env = cur->next;
    free((void*)cur->var);
    free(cur);
    cur = _env;
  }
}

static bool eval_error;

static int64_t eval_impl(Env* _env, Node* _ast) {
  switch (_ast->nt) {
  case NPRINT: {
    int64_t v = eval_impl(_env, _ast->lc);
    if (eval_error) return 0;
    printf("%ld\n", v);
    return v;
  }
  case NNOPRINT: return eval_impl(_env, _ast->lc);
  case NNUM: return _ast->num;
  case NVAR: {
    Env* var = find(_env, _ast->var);
    if (var != NULL) return var->value;
    warn("`%s` not defined!", _ast->var);
    eval_error = true;
    return 0;
  }
  case NADD: {
    int64_t l = eval_impl(_env, _ast->lc);
    if (eval_error) return 0;
    int64_t r = eval_impl(_env, _ast->rc);
    if (eval_error) return 0;
    return l + r;
  }
  case NSUB: {
    int64_t l = eval_impl(_env, _ast->lc);
    if (eval_error) return 0;
    int64_t r = eval_impl(_env, _ast->rc);
    if (eval_error) return 0;
    return l - r;
  }
  case NMUL: {
    int64_t l = eval_impl(_env, _ast->lc);
    if (eval_error) return 0;
    int64_t r = eval_impl(_env, _ast->rc);
    if (eval_error) return 0;
    return l * r;
  }
  case NDIV: {
    int64_t l = eval_impl(_env, _ast->lc);
    if (eval_error) return 0;
    int64_t r = eval_impl(_env, _ast->rc);
    if (eval_error) return 0;
    if (r == 0) {
      warn("divided by 0!");
      eval_error = true;
      return 0;
    }
    return l / r;
  }
  case NMOD: {
    int64_t l = eval_impl(_env, _ast->lc);
    if (eval_error) return 0;
    int64_t r = eval_impl(_env, _ast->rc);
    if (eval_error) return 0;
    if (r == 0) {
      warn("modded by 0!");
      eval_error = true;
      return 0;
    }
    return l % r;
  }
  case NASSIGN: {
    int64_t r = eval_impl(_env, _ast->rc);
    if (eval_error) return 0;
    insert_or_update(_env, dup_node_var(_ast->lc), r);
    return r;
  }
  case NSEMICOLON: {
    eval_impl(_env, _ast->lc);
    if (eval_error) return 0;
    return eval_impl(_env, _ast->rc);
  }
  case NNEG: {
    int64_t v = eval_impl(_env, _ast->lc);
    if (eval_error) return 0;
    return -v;
  }
  }
  return 0;
}

void eval_once(Node* _ast) {
  if (_ast == NULL) return;
  Env* env   = mk_env();
  eval_error = false;
  eval_impl(env, _ast);
  fflush(stdout);
  free_ast(_ast);
  free_env(env);
}

void eval_with(Env* _env, Node* _ast) {
  if (_ast == NULL) return;
  eval_error = false;
  eval_impl(_env, _ast);
  fflush(stdout);
  free_ast(_ast);
}
