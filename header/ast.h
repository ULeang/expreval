#ifndef __AST_H__
#define __AST_H__

#include <stdint.h>

typedef enum NodeType {
  NVAR,
  NNUM,
  NADD,
  NSUB,
  NMUL,
  NDIV,
  NMOD,
  NNEG,
  NASSIGN,
  NSEMICOLON,
  NPRINT,
  NNOPRINT,
} NodeType;

typedef struct Node {
  NodeType     nt;
  struct Node* lc;
  struct Node* rc;

  int64_t     num;
  const char* var;
} Node;

Node*       mk_plain(NodeType _nt);
Node*       mk_num(int64_t _n);
Node*       mk_var(const char* _v);
Node*       mk_with_1(NodeType _nt, Node* _lc);
Node*       mk_with_2(NodeType _nt, Node* _lc, Node* _rc);
void        free_ast(Node* _nd);  // free the whole ast, including `var` field
const char* dup_node_var(Node* _nd);

void print_ast(Node* _nd);

#endif  // !__AST_H__
