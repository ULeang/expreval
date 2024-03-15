#include "ast.h"

#include <stdio.h>
#include <stdlib.h>

#include "util.h"

Node* mk_num(int64_t _n) {
  Node* ret = mk_plain(NNUM);
  ret->num  = _n;
  return ret;
}
Node* mk_var(const char* _v) {
  Node* ret = mk_plain(NVAR);
  ret->var  = _v;
  return ret;
}
Node* mk_plain(NodeType _nt) {
  Node* ret = malloc(sizeof(Node));
  ret->nt   = _nt;
  return ret;
}
Node* mk_with_1(NodeType _nt, Node* _lc) {
  Node* ret = mk_plain(_nt);
  ret->lc   = _lc;
  return ret;
}
Node* mk_with_2(NodeType _nt, Node* _lc, Node* _rc) {
  Node* ret = mk_plain(_nt);
  ret->lc   = _lc;
  ret->rc   = _rc;
  return ret;
}

#define free_stack_size 4096
static Node*  free_stack[free_stack_size];
static size_t top = 0;
static void   push(Node* _n) {
  if (top >= free_stack_size) fatal(FREESTACKERR, "free stack overflow!");
  free_stack[top] = _n;
  ++top;
}
static Node* pop() {
  Node* ret       = free_stack[top];
  free_stack[top] = NULL;
  if (top > 0) --top;
  return ret;
}

// well, maybe recursion is enough a good choice, no need to use handmade stack
void free_ast(Node* _nd) {
  while (_nd != NULL) {
    if (_nd->lc != NULL) {
      push(_nd->lc);
    }
    if (_nd->rc != NULL) {
      push(_nd->rc);
    }
    free((void*)_nd->var);
    free(_nd);
    _nd = pop();
  }
}

const char* take_node_var(Node* _nd) {
  const char* ret = _nd->var;
  _nd->var        = NULL;
  return ret;
}

void print_ast(Node* _nd) {
  switch (_nd->nt) {
  case NVAR: printf("(VAR: %s)", _nd->var); break;
  case NNUM: printf("(NUM: %ld)", _nd->num); break;
  case NADD:
    printf("(ADD: ");
    print_ast(_nd->lc);
    print_ast(_nd->rc);
    putchar(')');
    break;
  case NSUB:
    printf("(SUB: ");
    print_ast(_nd->lc);
    print_ast(_nd->rc);
    putchar(')');
    break;
  case NMUL:
    printf("(MUL: ");
    print_ast(_nd->lc);
    print_ast(_nd->rc);
    putchar(')');
    break;
  case NDIV:
    printf("(DIV: ");
    print_ast(_nd->lc);
    print_ast(_nd->rc);
    putchar(')');
    break;
  case NMOD:
    printf("(MOD: ");
    print_ast(_nd->lc);
    print_ast(_nd->rc);
    putchar(')');
    break;
  case NNEG: break;
  case NASSIGN:
    printf("(ASSIGN: ");
    print_ast(_nd->lc);
    print_ast(_nd->rc);
    putchar(')');
    break;
  case NSEMICOLON:
    printf("(SEMI: ");
    print_ast(_nd->lc);
    print_ast(_nd->rc);
    putchar(')');
    break;
  case NPRINT:
    printf("(PRINT: ");
    print_ast(_nd->lc);
    break;
  case NNOPRINT:
    printf("(NOPRINT: ");
    print_ast(_nd->lc);
    break;
  }
  putchar('\n');
}
