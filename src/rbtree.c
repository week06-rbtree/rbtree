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
  
node_t *find_insert_location(rbtree *tree, node_t *node, key_t key) {
  /*
  노드 삽입 직전 위치 반환
  이 함수를 호출해서 노드 삽입 직전 위치를 반환받았다면 키값을 검사해서 left나 right 중 하나에 삽입해야 한다.
  */
  if (key < node->key) {
  return (node->left != tree->nil)
    ? find_insert_location(tree, node->left, key)
    : node;
  } else {
  return (node->right != tree->nil)
    ? find_insert_location(tree, node->right, key)
    : node;
  }
}

node_t *find_successor(node_t *cur_node, rbtree *tree) {
  /*
  cur_node 기준 오른쪽 서브트리에서 가장 작은 값을 가진 노드 포인터 반환
  */
  node_t *temp = cur_node; // 현재 노드로 설정
  if (temp->right != tree->nil) { // 오른쪽 자식이 있다면
    temp = temp->right; // 현재 노드를 오른쪽 자식으로 이동 
    while (temp->left != tree->nil) { // 왼쪽 자식이 존재할 때까지만
      temp = temp->left; // 현재 노드를 왼쪽 자식으로 이동
    }
  }
  return temp; // cur_node의 successor 반환
}

void red_red_violation_check(rbtree *tree, node_t *node) {
  /*
  부모가 RED인 경우에 대한 처리
  1) 삼촌이 RED라면
   부모, 삼촌을 BLACK, 조부모를 RED로 색깔 변경 후 재귀적으로 조부모에 대해 red_red_violation_check
  2) 삼촌이 BLACK이라면
   2-1) LR 또는 RL 이라면
     회전시켜서 LL 또는 RR로 변환
   2-2) LL 또는 RR 이라면
     부모를 BLACK, 조부모를 RED로 변경한 뒤 조부모 회전
  */
  if (node->parent == tree->nil) return; // 노드의 부모가 없다면 바로 리턴
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *uncle = FIND_BROTHER(grand_parent, parent);
  // red-red violation이 발생했다면
  if (grand_parent != tree->nil && parent->color == RBTREE_RED && node->color == RBTREE_RED) {
    if (uncle->color == RBTREE_RED) { // 삼촌이 RED이면
      uncle->color = RBTREE_BLACK;
      parent->color = RBTREE_BLACK;
      if (grand_parent != tree->root) {
        grand_parent->color = RBTREE_RED; // 조부모를 RED로
        double_red_check(tree, grand_parent); // 조부모로 인해 red-red-violation이 발생할 수 있으므로 체크
      } 
    } else { // 삼촌이 BLACK이면
      if (parent == grand_parent->left) {        // L
        if (node == parent->right) {              // LR
          left_rotate(tree, parent);                // 부모 좌회전 -> LL로
          red_red_violation_check(tree, parent);    // 재귀적으로 red-red-violation 체크
        } else {                                  // LL
          grand_parent->color = RBTREE_RED;         // 조부모를 RED로
          parent->color = RBTREE_BLACK;             // 부모를 BLACK으로
          right_rotate(tree, grand_parent);         // 조부모 우회전
        }
      } else {                                   // R
        if (node == parent->left) {               // RL
          right_rotate(tree, parent);               // 부모 우회전 -> RR로
          red_red_violation_check(tree, parent);    // 재귀적으로 red-red-violation 체크 
        } else {                                  // RR 
          grand_parent->color = RBTREE_RED;         // 조부모를 RED로
          parent->color = RBTREE_BLACK;             // 부모를 BLACK으로
          left_rotate(tree, grand_parent);          // 조부모 좌회전
        }
      }
    }
  }
}

void double_black_check(node_t *cur, rbtree *tree) {
  /*
  case 1. 자식이 RED인 경우
    : 노드를 자식으로 대체 후 검정으로 변경 
  case 2. 형제가 RED인 경우
    : 형제를 BLACK으로, 부모를 RED로 + 부모 회전 + 더블블랙 처리
     - 회전: 형제가 오른쪽 자식이면 좌회전, 형제가 왼쪽 자식이면 우회전
     - 더블 블랙 처리:
      1) 새로운 형제가 블랙 + 조카 모두 블랙
      2) 새로운 형제가 블랙 조카 1개는 RED
  case 3. 형제, 두 조카 모두 BLACK인 경우
    : 형제 RED로 바꾸고 부모에게 이중블랙 전파
       - 부모가 레드라면, 다음 double_black_check에서 BLACK으로 바뀌고 끝
       - 부모가 블랙이라면, doubly black 해결될 때까지 재귀적으로 이중블랙 처리
  case 4. 형제 BLACK, 조카는 한 개 이상 RED인 경우
     1) 형제 반대 방향 조카만 RED : RL(부모-형제) 또는 LR(부모-형제)
        // 조카-형제 회전해서 형제와 같은 방향에 RED가 있는 경우로 만들기
        - 조카 색깔을 BLACK으로
        - 형제 색깔을 RED로
        - 형제 회전(RL이었다면 우회전, LR이었다면 좌회전)
        - 새로운 형제 찾기
        - 새로운 조카 찾기
        => 색 재조정, 회전으로 균형 회복
     2) 형제와 같은 방향에 RED가 있는 경우 : LL(부모-형제) 또는 RR(부모-형제)
        // 색 재조정, 회전으로 균형 회복
        - 형제 색깔을 부모의 색깔로
        - 부모를 BLACK으로
        - 왼쪽 조카를 BLACK으로
        - 부모 회전(LL이었다면 우회전, RR이었다면 좌회전)
  */
  
  if (cur->color == RBTREE_RED || cur == tree->root) { // cur이 RED거나 루트라면
    cur->color = RBTREE_BLACK; // BLACK으로 바꾸고 종료 
    return;
  }

  node_t *parent = cur->parent;
  node_t *brother = FIND_BROTHER(cur, parent);

  if (brother->color == RBTREE_RED) { // case 2: 형제가 RED인 경우
    brother->color = RBTREE_BLACK;       // 형제 BLACK으로
    parent->color = RBTREE_RED;          // 부모 RED로
    if (is_left(brother)) {
      right_rotate(tree, parent);
    } else {
      left_rotate(tree, parent);
    } 
    double_black_check(cur, tree);
  } else {                           // case 3, 4: 형제가 BLACK인 경우
    parent = cur->parent;
    brother = FIND_BROTHER(cur, parent);
    node_t *left_nephew = brother->left;
    node_t *right_nephew = brother->right;
    if (left_nephew->color == RBTREE_BLACK && right_nephew->color == RBTREE_BLACK) {  // case 3: 두 조카 모두 BLACK인 경우
      brother->color = RBTREE_RED;
      double_black_check(parent, tree); // 재귀적으로 부모에게 DOUBLY BLACK 전파
    } else {                        // case 4: 적어도 하나의 조카는 RED인 경우
      if (is_left(brother)) {                      // 형제가 왼쪽 자식인 경우
        if (right_nephew->color == RBTREE_RED) {       // 오른쪽 조카(형제와 반대 방향)가 RED인 경우
          right_nephew->color = RBTREE_BLACK;               // 오른쪽 조카를 BLACK으로
          brother->color = RBTREE_RED;                      // 형제의 색을 RED
          left_rotate(tree, parent);                        // 부모 좌회전 => 형제와 같은 방향의 조카가 레드로!
          brother = FIND_BROTHER(cur, parent);              // 새로운 형제 찾기
          left_nephew = brother->left;                      // 새로운 LL 조카 찾기
          }
        if (left_nephew->color == RBTREE_RED) {        // 왼쪽 조카(형제와 동일 방향)가 RED인 경우
          brother->color = parent->color;                   // 형제 색깔을 부모의 색깔로
          parent->color = RBTREE_BLACK;                     // 부모를 BLACK으로
          left_nephew->color = RBTREE_BLACK;                // 왼쪽 조카를 BLACK으로
          right_rotate(tree, parent);                       // 부모 우회전
        }
      } else {                                     // 형제가 오른쪽 자식인 경우
        if (left_nephew->color == RBTREE_RED) {        // 왼쪽 조카(형제와 반대 방향)가 RED인 경우
          left_nephew->color = RBTREE_BLACK;                // 왼쪽 조카를 BLACK으로
          brother->color = RBTREE_RED;                      // 형제의 색을 RED
          right_rotate(tree, parent);                       // 부모 우회전 => 형제와 같은 방향의 조카가 레드로!
          brother = FIND_BROTHER(cur, parent);              // 새로운 형제 찾기
          right_nephew = brother->right;                    // 새로운 RR 조카 찾기
          }
        if (right_nephew->color == RBTREE_RED) {       // 오른쪽 조카(형제와 동일 방향)가 RED인 경우
          brother->color = parent->color;                   // 형제 색깔을 부모의 색깔로
          parent->color = RBTREE_BLACK;                     // 부모를 BLACK으로
          left_nephew->color = RBTREE_BLACK;                // 오른쪽 조카를 BLACK으로
          left_rotate(tree, parent);                        // 부모 좌회전 
        }
      }
    } 
  }
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