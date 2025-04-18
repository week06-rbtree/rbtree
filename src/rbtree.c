#include "rbtree.h"

#include <stdlib.h>
#define FIND_BROTHER(node, parent) ((node) == (parent)->left ? (parent)->right : (parent)->left)

void left_rotate(rbtree *t, node_t *target);
void right_rotate(rbtree *t, node_t *target);
node_t *find_insert_location(rbtree *tree, node_t *node, key_t key);
void double_red_check(rbtree *tree, node_t *node);
void remove_all_node(rbtree *tree, node_t *node);
void translate_array(node_t *nil, node_t *cur_node, key_t *arr, const size_t n, size_t *cur_idx);

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->nil = malloc(sizeof(node_t));
  p->nil->color = RBTREE_BLACK; // NIL 노드는 항상 블랙
  p->nil->key = 0;
  p->nil->left = NULL;
  p->nil->right = NULL;
  p->nil->parent = NULL;
  p->root = p->nil;
  return p;
}

void delete_rbtree(rbtree *t)
{
  if (t->root == t->nil)
    return;

  remove_all_node(t, t->root);

  free(t);
}

node_t *rbtree_insert(rbtree *t, const key_t key)
{
  // TODO: implement insert
  node_t *new_node = malloc(sizeof(node_t));
  new_node->color = RBTREE_RED; // 새로운 노드는 항상 레드
  new_node->key = key;          // 키값 할당
  new_node->left = t->nil;      // 새 노드의 레프트와 라이트는 항상 경계 노드 가리킴
  new_node->right = t->nil;
  new_node->parent = NULL;
  node_t *parent_node = NULL;

  if (t->root == t->nil) // 만약 트리가 비어있으면
  {
    t->root = new_node;
    t->root->color = RBTREE_BLACK; // 루트 노드는 항상 블랙
    t->root->parent = t->nil;
  }
  else // 트리가 비어있지 않다면
  {
    parent_node = find_insert_location(t, t->root, key); // 넣어줘야할 위치의 부모 찾기
    if (new_node->key <= parent_node->key)
    {
      parent_node->left = new_node;
      new_node->parent = parent_node;
    }
    else
    {
      parent_node->right = new_node;
      new_node->parent = parent_node;
    }
  }

  double_red_check(t, new_node);

  return new_node;
}

node_t *rbtree_find(const rbtree *t, const key_t key)
{
  if (t->root == t->nil)
    return NULL;
  node_t *cur = t->root;
  while (cur != t->nil && cur->key != key) // 끝까지 내려가서 찾기
  {
    if (cur->key < key)
    {
      cur = cur->left;
    }
    else
    {
      cur = cur->right;
    }
  }

  if (cur == t->nil)
    return NULL;

  return cur;
}

node_t *rbtree_min(const rbtree *t)
{
  if (t->root == t->nil)
    return NULL;

  node_t *cur = t->root;
  while (cur->left != t->nil)
  {
    cur = cur->left;
  }

  return cur;
}

node_t *rbtree_max(const rbtree *t)
{
  if (t->root == t->nil)
    return NULL;

  node_t *cur = t->root;
  while (cur->right != t->nil)
  {
    cur = cur->right;
  }

  return cur;
}

int rbtree_erase(rbtree *t, node_t *p)
{

  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  if (t->root == t->nil)
    return 0;

  translate_array(t->nil, t->root, arr, n, 0);
  return 1;
}

void left_rotate(rbtree *t, node_t *target) // 타겟이 내려가고 right가 올라가는 함수 !!
{
  // TODO: 왼쪽 회전 구현
  node_t *Y = target->right; // 타겟 오른쪽의 노드, Y라 함
  target->right = Y->left;   // 회전하면서 Y의 왼쪽 노드는 타겟의 오른쪽에 붙게 됨
  Y->left = target;          // 기존 타겟은 Y의 왼쪽으로 내려감
  if (t->root == target)     // 만약 타겟이 트리의 루트였었다면
  {
    t->root = Y;                   // 루트 교체
    t->root->color = RBTREE_BLACK; // 루트는 항상 블랙
    Y->parent = t->nil;            // 루트의 부모는 없음 -> NULL 로 교체
  }
  else // 루트가 아니라면
  {
    Y->parent = target->parent; // 자리가 바뀌었으니 Y의 부모도 target으로 교체
    target->parent->right = Y;  // 회전해주었으니까 부모의 right를 Y로 교체
  }
  target->parent = Y; // 기존 타겟의 부모 교체
}

void right_rotate(rbtree *t, node_t *target) // 타겟이 아래로 내려가고 left가 위로 올라가는 함수 !!
{
  // TODO: 오른쪽 회전 구현
  node_t *X = target->left; // 오른쪽으로 회전해야 하니까 타겟의 왼쪽을 X로 두자
  target->left = X->right;  // 타겟의 왼쪽에는 X의 오른쪽 노드가 달린다
  X->right = target;        // X의 오른쪽에는 타겟이 됨. 내려간다
  if (t->root == target)    // 만약 타겟이 루트였으면
  {
    t->root = X;                   // 루트를 새로 설정해주고
    t->root->color = RBTREE_BLACK; // 루트는 항상 블랙
    X->parent = t->nil;            // 루트의 부모는 없으니까 NIL로 설정
  }
  else // 루트가 아니라면
  {
    X->parent = target->parent; // X의 부모를 타겟의 부모로 바꿔주기
    target->parent->left = X;   // 회전해주었으니까 부모의 left를 X로 바꾸자
  }
  target->parent = X; // 타겟의 부모는 X로
}

node_t *find_insert_location(rbtree *tree, node_t *node, key_t key)
{
  int cur_key = node->key; // 현재 노드의 키값
  node_t *ret_node = node; // 현재 노드 저장
  if (cur_key <= key)      // 키값보다 작으면 같으면 -> multiset이기 때문에 같은 값이 들어갈 수 있음 !!
  {
    if (node->left != tree->nil)                              // 노드의 왼쪽이 존재하면
                                                              // NIL은 경계노드 -> 가상의 리프노드, NULL 이라고 생각하면 됨
      ret_node = find_insert_location(tree, node->left, key); // 왼쪽으로 탐색
    else                                                      // 왼쪽이 없으면
      return ret_node;                                        // 현재 노드를 리턴
  }
  else // 현재 노드의 키값보다 크면
  {
    if (node->right != tree->nil)                              // 노드의 오른쪽이 존재하면
      ret_node = find_insert_location(tree, node->right, key); // 오른쪽으로 탐색
    else                                                       // 오른쪽이 없으면
      return ret_node;                                         // 현재 노드를 리턴
  }

  return ret_node;
}

void double_red_check(rbtree *tree, node_t *node)
{
  if (node->parent == tree->nil) // 이미 루트 노드일 경우 함수 종료
    return;
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *uncle = FIND_BROTHER(parent, grand_parent); // 삼촌 찾기
  if (grand_parent != NULL && parent->color == RBTREE_RED && node->color == RBTREE_RED)
  {
    if (uncle->color == RBTREE_RED) // 삼촌이 레드 노드면
    {
      uncle->color = RBTREE_BLACK;    // 삼촌을 블랙으로
      parent->color = RBTREE_BLACK;   // 부모를 블랙으로
      if (grand_parent != tree->root) // 조부모가 루트가 아니라면
      {
        grand_parent->color = RBTREE_RED;     // 조부모를 레드로
        double_red_check(tree, grand_parent); // 조부모도 더블 레드가 발생할 수 있으니 더블 레드 체크
      }
    }
    else // 삼촌이 블랙 노드면
    {
      // LL, RR, LR, RL 케이스 모두 해결해야함
      if (parent == grand_parent->left) // L
      {
        if (node == parent->left) // L
        {
          grand_parent->color = RBTREE_RED; // 조부모를 레드로
          parent->color = RBTREE_BLACK;     // 부모를 블랙으로
          right_rotate(tree, grand_parent); // 우회전 수행
        }
        else // R
        {
          left_rotate(tree, parent);      // LL 케이스로 만들고
          double_red_check(tree, parent); // 다시 더블 레드 체크
        }
      }
      else // R
      {
        if (node == parent->left) // L
        {
          right_rotate(tree, parent);     // RR 케이스로 만들고
          double_red_check(tree, parent); // 다시 더블 레드 체크
        }
        else // R
        {
          grand_parent->color = RBTREE_RED; // 조부모를 레드로
          parent->color = RBTREE_BLACK;     // 부모를 블랙으로
          left_rotate(tree, grand_parent);  // 좌회전 수행
        }
      }
    }
  }
}

void remove_all_node(rbtree *tree, node_t *node)
{
  if (node->left != tree->nil)
    remove_all_node(tree, node->left);
  if (node->right != tree->nil)
    remove_all_node(tree, node->right);

  free(node);
}

void translate_array(node_t *nil, node_t *cur_node, key_t *arr, const size_t n, size_t *cur_idx)
{
  if (*cur_idx >= n) // 사이즈를 넘어가면 끝
    return;

  if (cur_node->left != nil)                               // 왼쪽이 비지 않았으면
    translate_array(nil, cur_node->left, arr, n, cur_idx); // 왼쪽으로 계속 탐색

  arr[*cur_idx] = cur_node->key; // 배열에 키를 저장하면
  (*cur_idx)++;                  // 키 인덱스 증가

  if (cur_node->right != nil)                              // 오른쪽이 비지 않았으면
    translate_array(nil, cur_node->left, arr, n, cur_idx); // 오른쪽으로 계속 탐색
}