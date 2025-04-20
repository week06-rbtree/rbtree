#include "rbtree.h"
#include <stdio.h>
#include <stdlib.h>

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
  // 
  free(t);
}

void right_rotate(rbtree *t, node_t *x) {
  node_t *y = x->left;
  x->left = y->right;
  if (y->right != t->nil) {
    y->right->parent = x;
  }
  y->parent = x->parent;
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->right = x;
  x->parent = y;
}

void left_rotate(rbtree *t, node_t *x) {
  node_t *y = x->right; // x의 오른쪽 자식을 y로 설정
  x->right = y->left; // x의 오른쪽 자식에 y의 왼쪽 자식을 데려옴
  if (y->left != t->nil) { // y의 왼쪽 자식이 nil이 아니라면 y의 왼쪽 자식의 부모로 x를 설정
    y->left->parent = x;
  }
  y->parent = x->parent; // y의 부모를 x의 기존 부모로 설정
  if (x->parent == t->nil) {
    t->root = y;
  } else if (x == x->parent->left) {
    x->parent->left = y;
  } else {
    x->parent->right = y;
  }
  y->left = x;
  x->parent = y;
}

void insert_fixup(rbtree *t, node_t *new) {
  while (new->parent->color == RBTREE_RED) {
    if (new->parent == new->parent->parent->left) {
      node_t *y = new->parent->parent->right;
      if (y->color == RBTREE_RED) {
        new->parent->color = RBTREE_BLACK;
        y->color = RBTREE_BLACK;
        new->parent->parent->color = RBTREE_RED;
        new = new->parent->parent;
      } else if (new == new->parent->left) {
        new = new->parent;
        left_rotate(t, new);
        new->parent->color = RBTREE_BLACK;
        new->parent->parent->color = RBTREE_RED;
        right_rotate(t, new->parent->parent);
      }
    } else {
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

void rbtree_erase_fixup(rbtree *t, node_t *x) {
#ifdef SENTINEL
  while ((x != t->root) && (x->color == RBTREE_BLACK)) {
    if (x == x->parent->left) {
      node_t *w = x->parent->right;
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        left_rotate(t, x->parent);
        w = x->parent->right;
      }
      if ((w->left->color == RBTREE_BLACK) && (w->right->color == RBTREE_BLACK)) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else if (w->right->color == RBTREE_BLACK) {
        w->left->color == RBTREE_BLACK;
        w->color = RBTREE_RED;
        right_rotate(t, w);
        w = x->parent->right;
      } else {
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->right->color = RBTREE_BLACK;
        left_rotate(t, x->parent);
        x = t->root;  
      }
    } else {
      node_t *w = x->parent->left;
      if (w->color == RBTREE_RED) {
        w->color = RBTREE_BLACK;
        x->parent->color = RBTREE_RED;
        right_rotate(t, x->parent);
        w = x->parent->left;
      }
      if ((w->right->color == RBTREE_BLACK) && (w->left->color == RBTREE_BLACK)) {
        w->color = RBTREE_RED;
        x = x->parent;
      } else if (w->left->color == RBTREE_BLACK) {
        w->right->color == RBTREE_BLACK;
        w->color = RBTREE_RED;
        left_rotate(t, w);
        w = x->parent->left;
      } else {
        w->color = x->parent->color;
        x->parent->color = RBTREE_BLACK;
        w->left->color = RBTREE_BLACK;
        right_rotate(t, x->parent);
        x = t->root;  
      }
    }
  } 
#endif
  x->color = RBTREE_BLACK;
}

int rbtree_erase(rbtree *t, node_t *p) {
  // TODO: implement erase
  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n) {
  // TODO: implement to_array
  return 0;
}
