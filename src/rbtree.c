#define FIND_BROTHER(node, parent) ((node) == (parent)->left ? (parent)->right : (parent)->left)
#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

void postorder(rbtree *t, node_t *node);
void inorder(const rbtree *t, node_t *node, key_t *arr, int *index, const size_t n);
void right_rotate(rbtree *t, node_t *target);
void left_rotate(rbtree *t, node_t *target);
void insert_fixup(rbtree *t, node_t *new);
void rbtree_transplant(rbtree *t, node_t *u, node_t *v);
void rbtree_erase_fixup(rbtree *t, node_t *x);
node_t *find_insert_location(rbtree *tree, node_t *node, key_t key);
node_t *find_successor(node_t *cur_node, rbtree *tree);
void red_red_violation_check(rbtree *tree, node_t *node);
void double_black_check(node_t *cur, rbtree *tree);
int is_left(node_t *cur);

void postorder(rbtree *t, node_t *node) {
  /*
  후위순회 -> 노드 삭제 시 이용
  */
  if (node == t->nil) return;
  postorder(t, node->left);
  postorder(t, node->right);
  free(node);
}

void inorder(const rbtree *t, node_t *node, key_t *arr, int *index, const size_t n) {
  /*
  중위순회
  */
  if (node == t->nil || *index >= (int)n) return;
  inorder(t, node->left, arr, index, n);
  if (*index < (int)n) {
    arr[*index] = node->key;
    (*index)++;
  }
  inorder(t, node->right, arr, index, n);
}

void right_rotate(rbtree *t, node_t *x) {
  /*
  우회전
  */
  node_t *y = x->left; // x의 왼쪽 자식을 y로
  x->left = y->right; // x의 왼쪽 자식자리에 y의 오른쪽 자식을 입양보냄
  if (y->right != t->nil) { // 데려온 y의 오른쪽 자식이 nil이 아니라면
    y->right->parent = x; // y의 오른쪽 자식의 부모를 x로 연결에게 입양보냄
  }
  y->parent = x->parent; // y의 부모를 x의 기존 부모로 설정
  if (x->parent == t->nil) { // x의 부모가 없었다면
    t->root = y; // 트리의 루트를 y로 설정
  } else if (x == x->parent->left) { // x가 왼쪽 자식이었다면
    x->parent->left = y; // y도 x의 왼쪽 자식으로 설정
  } else {  // x가 오른쪽 자식이었다면 
    x->parent->right = y; // y도 x의 오른쪽 자식으로 설정
  } 
  y->right = x; // y의 오른쪽 자식을 x로
  x->parent = y; // x의 부모를 y로
}

void left_rotate(rbtree *t, node_t *x) {
  /*
  좌회전
  */
  node_t *y = x->right; // x의 오른쪽 자식을 y로
  x->right = y->left; // x의 오른쪽 자식 자리에 y의 왼쪽 자식을 입양보냄
  if (y->left != t->nil) { // 데려온 y의 왼쪽 자식이 nil이 아니라면 
    y->left->parent = x; // y의 왼쪽 자식의 부모로 x를 설정
  }
  y->parent = x->parent; // y의 부모를 x의 기존 부모로 설정
  if (x->parent == t->nil) { // x의 부모가 없었다면
    t->root = y; // 트리의 루트를 y로 설정
  } else if (x == x->parent->left) { // x가 왼쪽 자식이었다면
    x->parent->left = y; // y도 왼쪽 자식으로 설정 
  } else { // x가 오른쪽 자식이었다면
    x->parent->right = y; // y도 오른쪽 자식으로 설정  
  }
  y->left = x; // y의 왼쪽 자식을 x로
  x->parent = y; // x의 부모를 y로
}

void insert_fixup(rbtree *t, node_t *new) {
  /*
  삽입 수정
  */ 
  while (new->parent->color == RBTREE_RED) { // red-red violoation 처리 
    if (new->parent == new->parent->parent->left) { // 새로운 노드의 부모가 조부모의 왼쪽 자식이라면
      node_t *y = new->parent->parent->right; // y는 삼촌(조부모의 오른쪽 자식)
      if (y->color == RBTREE_RED) { // 삼촌의 색깔이 RED라면
        new->parent->color = RBTREE_BLACK; // 부모의 색깔을 BLACK으로
        y->color = RBTREE_BLACK;
        new->parent->parent->color = RBTREE_RED;
        new = new->parent->parent;
      } else if (new == new->parent->left) { // 새로운 노드가 왼쪽 자식이라면
        new = new->parent; // 새로운 노드를 부모로
        left_rotate(t, new); // 새로운 노드 좌회전
      } else { // 새로운 노드가 오른쪽이라면
        new->parent->color = RBTREE_BLACK; // 부모의 색깔을 BLACK으로
        new->parent->parent->color = RBTREE_RED; // 조부모의 색깔을 RED로
        right_rotate(t, new->parent->parent); // 조부모 우회전  
      }
    } else { // 새로운 노드의 부모가 조부모의 오른쪽 자식이라면
      node_t *y = new->parent->parent->left; 
      if (y->color == RBTREE_RED) {
        new->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        new->parent->parent->color = RBTREE_RED;
        new = new->parent->parent;
      } else if (new == new->parent->right) {
        new = new->parent;
        right_rotate(t, new);
        new->parent->color = RBTREE_BLACK;
        new->parent->parent->color = RBTREE_RED;
        left_rotate(t, new->parent->parent);
      }
    }
  }
}

void rbtree_transplant(rbtree *t, node_t *u, node_t *v) {
  if (u->parent == t->nil) {
    t->root = v;
  } else if (u == u->left) {
    u->parent->left = v;
  } else {
    u->parent->right = v;
  }
  v->parent = u->parent;
}

rbtree *new_rbtree(void) {
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
#ifdef SENTINEL
  node_t *nil = (node_t *)calloc(1, sizeof(node_t));
  nil->color = RBTREE_BLACK;
  nil->left = nil->right = nil->parent = NULL; // calloc으로 nil을 초기화해서 left, right, parent도 0(==NULL)로 초기화되어있지만 코드 명시성과 방어로직을 위해 해당 코드 작성
  p->nil = nil;
  p->root = nil;
#endif
  return p;
}

void delete_rbtree(rbtree *t) {
  // TODO: reclaim the tree nodes's memory
  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key) {
#ifdef SENTINEL
  node_t *y = t->nil;
  node_t *x = t->root;

  // 삽입할 노드 초기화
  node_t *new = (node_t *)calloc(1, sizeof(node_t));
  new->color = RBTREE_RED;
  new->key = key;
  new->left = new->right = new->parent = NULL;

  while (x != t->nil) {
    y = x;
    if (new->key < x->key) {
      x = x->left;
    } else {
      x = x->right;
    }
  }

  new->parent = y;
  if (y == t->nil) {
    t->root = new;
  } else if (new->key < y->key) {
    new->left = new;
  } else {
    y->left = new;
  }

  new->left = t->nil;
  new->right = t->nil;
  new->color = RBTREE_RED;

  insert_fixup(t, new);

  t->root->color = RBTREE_BLACK;
#endif
  return t->root;
}

node_t *rbtree_find(const rbtree *t, const key_t key) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_min(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

node_t *rbtree_max(const rbtree *t) {
  // TODO: implement find
  return t->root;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}