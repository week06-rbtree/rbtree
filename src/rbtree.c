#include "rbtree.h"

#include <stdlib.h>

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->root = NULL;
  p->nil = malloc(sizeof(node_t));
  p->nil->color = RBTREE_BLACK; // NIL 노드는 항상 블랙
  p->nil->key = NULL;
  p->nil->left = NULL;
  p->nil->right = NULL;
  p->nil->parent = NULL;
  return p;
}

void delete_rbtree(rbtree *t)
{
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // TODO: implement insert
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t)
{
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t)
{
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p)
{
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  // TODO: implement to_array
  return 0;
}

void left_rotate(rbtree *t, node_t *target)
{
  // TODO: 왼쪽 회전 구현
  node_t *Y = target->right; // 타겟 오른쪽의 노드, Y라 함
  target->right = Y->left;   // 회전하면서 타겟 오른쪽은 Y의 왼쪽 노드가 됨
  Y->left = target;          // 기존 타겟은 자신 Y의 왼쪽으로 내려감
  if (t->root == target)     // 만약 타겟이 트리의 루트였었다면
  {
    t->root = Y;      // 루트 교체
    Y->parent = NULL; // 루트의 부모는 없음 -> NULL 로 교체
  }
  else // 루트가 아니라면
  {
    Y->parent = target->parent; // 자리가 바뀌었으니 Y의 부모도 target으로 교체
  }
  target->parent = Y; // 기존 타겟의 부모 교체
}

void right_rotate(rbtree *t, node_t *target)
{
  // TODO: 오른쪽 회전 구현
  node_t *X = target->left; // 오른쪽으로 회전해야 하니까 타겟의 왼쪽을 X로 두자
  target->left = X->right;  // 타겟의 왼쪽에는 X의 오른쪽 노드가 달린다
  X->right = target;        // X의 오른쪽에는 타겟이 됨. 내려간다
  if (t->root == target)    // 만약 타겟이 루트였으면
  {
    t->root = X;      // 루트를 새로 설정해주고
    X->parent = NULL; // 루트의 부모는 없으니까 NULL로 설정
  }
  else // 루트가 아니라면
  {
    X->parent = target->parent; // X의 부모를 타겟의 부모로 바꿔주기
  }
  target->parent = X; // 타겟의 부모는 X로
}