#include "parser.h"

#include <stdbool.h>
#include <stdlib.h>

#include "ast.h"
#include "lexer.h"
#include "util.h"

static Token* token;
static void   next() { token = token->next; }
static bool   expect(TokenType _tt) {
  if (token == NULL || token->tt != _tt) return false;
  return true;
}
static bool expect_next(TokenType _tt) {
  if (!expect(_tt)) return false;
  next();
  return true;
}

static Node* p_printexpr();
static Node* p_semiexpr();
static Node* p_commonexpr();
static Node* p_semi_(Node* _n);
static Node* p_semi__(Node* _n);
static Node* p_assignexpr();
static Node* p_addexpr();
static Node* p_VAR();
static Node* p_mulexpr();
static Node* p_add_(Node* _n);
static Node* p_parentexpr();
static Node* p_mul_(Node* _n);
static Node* p_primitive();
static Node* p_NUM();

static Node* p_NUM() {
  if (!expect(NUM)) return NULL;
  Node* ret = mk_num(token->num);
  next();
  return ret;
}
static Node* p_VAR() {
  if (!expect(VAR)) return NULL;
  Node* ret = mk_var(take_var(token));
  next();
  return ret;
}
static Node* p_primitive() {
  Node* ret = p_NUM();
  if (ret != NULL) return ret;
  ret = p_VAR();
  return ret;
}
static Node* p_parentexpr() {
  Token* backtrack = token;
  if (!expect_next(LPARENT)) return p_primitive();
  Node* ret = p_semiexpr();
  if (ret != NULL && expect_next(RPARENT)) return ret;
  free_ast(ret);
  token = backtrack;
  return NULL;
}
static Node* p_mul_(Node* _n) {
  Token* backtrack = token;
  if (expect_next(MUL)) {
    Node* parentexpr = p_parentexpr();
    if (parentexpr != NULL) {
      Node* local = mk_with_2(NMUL, _n, parentexpr);
      Node* mul_  = p_mul_(local);
      if (mul_ != NULL) return mul_;
      return local;
    }
  } else if (expect_next(DIV)) {
    Node* parentexpr = p_parentexpr();
    if (parentexpr != NULL) {
      Node* local = mk_with_2(NDIV, _n, parentexpr);
      Node* mul_  = p_mul_(local);
      if (mul_ != NULL) return mul_;
      return local;
    }
  } else if (expect_next(MOD)) {
    Node* parentexpr = p_parentexpr();
    if (parentexpr != NULL) {
      Node* local = mk_with_2(NMOD, _n, parentexpr);
      Node* mul_  = p_mul_(local);
      if (mul_ != NULL) return mul_;
      return local;
    }
  }
  token = backtrack;
  return NULL;
}

static Node* p_mulexpr() {
  Node* parentexpr = p_parentexpr();
  if (parentexpr == NULL) return NULL;

  Node* mul_ = p_mul_(parentexpr);
  if (mul_ != NULL) return mul_;

  return parentexpr;
}

static Node* p_add_(Node* _n) {
  Token* backtrack = token;
  if (expect_next(ADD)) {
    Node* mulexpr = p_mulexpr();
    if (mulexpr != NULL) {
      Node* local = mk_with_2(NADD, _n, mulexpr);
      Node* add_  = p_add_(local);
      if (add_ != NULL) return add_;
      return local;
    }
  } else if (expect_next(SUB)) {
    Node* mulexpr = p_mulexpr();
    if (mulexpr != NULL) {
      Node* local = mk_with_2(NSUB, _n, mulexpr);
      Node* add_  = p_add_(local);
      if (add_ != NULL) return add_;
      return local;
    }
  }
  token = backtrack;
  return NULL;
}
static Node* p_addexpr() {
  Node* mulexpr = p_mulexpr();
  if (mulexpr == NULL) return NULL;

  Node* add_ = p_add_(mulexpr);
  if (add_ != NULL) return add_;

  return mulexpr;
}

static Node* p_assignexpr() {
  Token* backtrack = token;
  Node*  var       = p_VAR();
  if (var == NULL) return NULL;
  if (!expect_next(ASSIGN)) {
    token = backtrack;
    give_var(token, take_node_var(var));
    free_ast(var);
    return NULL;
  }
  Node* commonexpr = p_commonexpr();
  if (commonexpr == NULL) {
    free_ast(var);
    token = backtrack;
    return NULL;
  }
  return mk_with_2(NASSIGN, var, commonexpr);
}

static Node* p_commonexpr() {
  Node* assignexpr = p_assignexpr();
  if (assignexpr != NULL) return assignexpr;

  Node* addexpr = p_addexpr();
  if (addexpr != NULL) return addexpr;

  return NULL;
}

static Node* p_semi__(Node* _n) {
  Node* commonexpr = p_commonexpr();
  if (commonexpr == NULL) return NULL;

  Node* local  = mk_with_2(NSEMICOLON, _n, commonexpr);
  Node* semi_ = p_semi_(local);
  if (semi_ != NULL) return semi_;

  return local;
}

static Node* p_semi_(Node* _n) {
  if (!expect_next(SEMICOLON)) return NULL;
  return p_semi__(_n);
}

static Node* p_semiexpr() {
  Node* commonexpr = p_commonexpr();
  if (commonexpr == NULL) return NULL;
  Node* semi_ = p_semi_(commonexpr);
  if (semi_ == NULL) return commonexpr;
  return semi_;
}

static Node* p_printexpr() {
  Token* backtrack = token;
  if (expect_next(AT)) {
    Node* semiexpr = p_semiexpr();
    if (semiexpr != NULL) return mk_with_1(NNOPRINT, semiexpr);

    token = backtrack;
    return NULL;
  }

  Node* semiexpr = p_semiexpr();
  if (semiexpr != NULL) return mk_with_1(NPRINT, semiexpr);

  return NULL;
}

Node* parse_to_ast(Token* _tk) {
  token           = _tk->next;
  Node* line_prog = p_printexpr();
  if (token != NULL) {
    warn("unexpected tokens");
    print_token_list(token);
    free_ast(line_prog);
    free_token_list(_tk);
    return NULL;
  }
  free_token_list(_tk);
  return line_prog;
}
