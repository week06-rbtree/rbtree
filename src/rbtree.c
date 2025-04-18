#include "rbtree.h"

#include <stdlib.h>
#define FIND_BROTHER(node, parent) ((node) == (parent)->left ? (parent)->right : (parent)->left)

void left_rotate(rbtree *t, node_t *target);
void right_rotate(rbtree *t, node_t *target);
node_t *find_insert_location(rbtree *tree, node_t *node, key_t key);
void double_red_check(rbtree *tree, node_t *node);
void remove_all_node(rbtree *tree, node_t *node);
void translate_array(node_t *nil, node_t *cur_node, key_t *arr, const size_t n, size_t *cur_idx);
node_t *find_predecessor(node_t *cur_node, rbtree *tree);
void delete_my_position(node_t *me, node_t *parent, rbtree *tree);
int is_my_position_right(node_t *cur);
void double_black_check(node_t *cur, rbtree *tree);

rbtree *new_rbtree(void)
{
  rbtree *p = (rbtree *)calloc(1, sizeof(rbtree));
  p->nil = malloc(sizeof(node_t));
  p->nil->color = RBTREE_BLACK; // NIL 노드는 항상 블랙
  p->nil->key = 0;
  p->nil->left = p->nil;
  p->nil->right = p->nil;
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
    if (new_node->key < parent_node->key)
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
    if (key < cur->key)
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
  if (t->root != p && p->left == t->nil && p->right == t->nil) // 루트가 아닌 리프 노드 삭제 시
  {
    delete_my_position(p, p->parent, t); // 그냥 삭제하면 됨 !!
    free(p);
    return 1;
  }
  // 인터널 노드 삭제시에는?
  node_t *predecessor = find_predecessor(p, t);
  if (predecessor == t->root)     // 찾은 후계자가 그냥 루트라면?
  {                               // 그럼 트리에는 루트 하나밖에 없다는 뜻? right에도 있을 수 있는거 아닌가?
    if (t->root->right == t->nil) // 오른쪽 서브 트리도 없으면 트리에 루트 하나밖에 없다는 뜻
      t->root = t->nil;           // 루트는 NIL을 가리키도록
    else
      t->root = t->root->right; // 만약 오른쪽 서브트리가 존재하면 루트는 오른쪽을 가리키도록
    free(predecessor);
    return 1;
  }

  // 이제 삭제할 노드가 루트일 경우, 리프 노드일 경우는 모두 처리했다. 진짜 인터널 노드일 경우 처리하자
  if (predecessor == p)     // 만약 후계자랑 나랑 같다? 그럼 left가 없다는 뜻이다
  {                         // 지금 후계자 노드에서 삭제를 진행하고 있음을 유의해야 한다
    predecessor = p->right; // 그럼 후계자를 predecessor 말고 오른쪽 서브트리의 루트로 하자
  }
  p->key = predecessor->key; // 인터널 노드의 키를 후계자의 키로 교체하자

  if (predecessor->color == RBTREE_RED) // 후계자가 RED이면 문제없음!! 그냥 삭제
  {
    delete_my_position(predecessor, predecessor->parent, t);
    free(predecessor);
    return 1;
  }
  else // 후계자가 BLACK인 경우... 더블 블랙 체크를 해 줘야함
  {
    double_black_check(predecessor, t);                      // 더블 블랙 체크
    delete_my_position(predecessor, predecessor->parent, t); // 내 포지션 없애기
    free(predecessor);
    return 1;
  }

  return 0;
}

int rbtree_to_array(const rbtree *t, key_t *arr, const size_t n)
{
  if (t->root == t->nil)
    return 0;
  size_t idx = 0;
  translate_array(t->nil, t->root, arr, n, &idx);
  return 1;
}

void left_rotate(rbtree *t, node_t *target) // 타겟이 내려가고 right가 올라가는 함수 !!
{
  // TODO: 왼쪽 회전 구현
  node_t *Y = target->right; // 타겟 오른쪽의 노드, Y라 함
  target->right = Y->left;   // 회전하면서 Y의 왼쪽 노드는 타겟의 오른쪽에 붙게 됨
  Y->left->parent = target;
  Y->left = target;      // 기존 타겟은 Y의 왼쪽으로 내려감
  if (t->root == target) // 만약 타겟이 트리의 루트였었다면
  {
    t->root = Y;                   // 루트 교체
    t->root->color = RBTREE_BLACK; // 루트는 항상 블랙
    Y->parent = t->nil;            // 루트의 부모는 없음 -> NULL 로 교체
  }
  else // 루트가 아니라면
  {
    Y->parent = target->parent;       // 자리가 바뀌었으니 Y의 부모도 target으로 교체
    if (is_my_position_right(target)) // 회전해주었으니까 부모의 left나 right를 Y로 교체
      target->parent->right = Y;
    else
      target->parent->left = Y;
  }
  target->parent = Y; // 기존 타겟의 부모 교체
}

void right_rotate(rbtree *t, node_t *target) // 타겟이 아래로 내려가고 left가 위로 올라가는 함수 !!
{
  // TODO: 오른쪽 회전 구현
  node_t *X = target->left; // 오른쪽으로 회전해야 하니까 타겟의 왼쪽을 X로 두자
  target->left = X->right;  // 타겟의 왼쪽에는 X의 오른쪽 노드가 달린다
  X->right->parent = target;
  X->right = target;     // X의 오른쪽에는 타겟이 됨. 내려간다
  if (t->root == target) // 만약 타겟이 루트였으면
  {
    t->root = X;                   // 루트를 새로 설정해주고
    t->root->color = RBTREE_BLACK; // 루트는 항상 블랙
    X->parent = t->nil;            // 루트의 부모는 없으니까 NIL로 설정
  }
  else // 루트가 아니라면
  {
    X->parent = target->parent;       // X의 부모를 타겟의 부모로 바꿔주기
    if (is_my_position_right(target)) // 회전해주었으니까 부모의 left나 right를 X로 바꾸자
      target->parent->right = X;
    else
      target->parent->left = X;
  }
  target->parent = X; // 타겟의 부모는 X로
}

node_t *find_insert_location(rbtree *tree, node_t *node, key_t key)
{
  if (key < node->key)
  {
    // 왼쪽 서브트리
    return (node->left != tree->nil)
               ? find_insert_location(tree, node->left, key)
               : node;
  }
  else
  {
    // 오른쪽 서브트리 (key ≥ node->key: 중복은 오른쪽에 둔다고 통일)
    return (node->right != tree->nil)
               ? find_insert_location(tree, node->right, key)
               : node;
  }
}

void double_red_check(rbtree *tree, node_t *node)
{
  if (node->parent == tree->nil) // 이미 루트 노드일 경우 함수 종료
    return;
  node_t *parent = node->parent;
  node_t *grand_parent = parent->parent;
  node_t *uncle = FIND_BROTHER(parent, grand_parent);                                   // 삼촌 찾기
  if (grand_parent != NULL && parent->color == RBTREE_RED && node->color == RBTREE_RED) // 더블 레드가 발생한 상황이면
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

void remove_all_node(rbtree *tree, node_t *node) // 재귀적으로 모든 노드 삭제하기
{
  if (node->left != tree->nil)
    remove_all_node(tree, node->left);
  if (node->right != tree->nil)
    remove_all_node(tree, node->right);

  free(node);
}

void translate_array(node_t *nil, node_t *cur_node, key_t *arr, const size_t n, size_t *cur_idx)
{                    // 재귀적으로 배열에 키값 넣어주기
  if (*cur_idx >= n) // 사이즈를 넘어가면 끝
    return;

  if (cur_node->left != nil)                               // 왼쪽이 비지 않았으면
    translate_array(nil, cur_node->left, arr, n, cur_idx); // 왼쪽으로 계속 탐색

  arr[*cur_idx] = cur_node->key; // 배열에 키를 저장하면
  (*cur_idx)++;                  // 키 인덱스 증가

  if (cur_node->right != nil)                               // 오른쪽이 비지 않았으면
    translate_array(nil, cur_node->right, arr, n, cur_idx); // 오른쪽으로 계속 탐색
}

node_t *find_predecessor(node_t *cur_node, rbtree *tree) // 후계자 찾기
{
  node_t *temp = cur_node;
  if (temp->left != tree->nil)
  {
    temp = temp->left; // 자신보다 작은 서브트리에서
    while (temp->right != tree->nil)
    {
      temp = temp->right; // 가장 큰 노드 찾기
    }
  }

  return temp;
}

void delete_my_position(node_t *me, node_t *parent, rbtree *tree) // 항상 후계자 노드가 삭제됨. predecessor
{
  if (me == tree->root)
  {
    tree->root = tree->nil;
    return;
  }

  if (parent->left == me)
    parent->left = me->left;
  else
    parent->right = me->left;
}

int is_my_position_right(node_t *cur)
{
  if (cur == cur->parent->right)
    return 1;
  else
    return 0;
}

void double_black_check(node_t *cur, rbtree *tree) // 재귀적으로 더블 블랙 체크
{
  if (cur->color == RBTREE_RED || cur == tree->root) // 현재 노드가 레드이거나 루트면
  {
    cur->color = RBTREE_BLACK; // 컬러를 블랙으로 바꾸고
    return;                    // 종료
  }
  node_t *parent = cur->parent;
  node_t *brother = FIND_BROTHER(cur, parent); // 더블 블랙이 발생한 노드(cur)의 형제 찾기

  if (brother->color == RBTREE_RED) // 형제 노드가 레드일 때
  {
    brother->color = RBTREE_BLACK;     // 형제를 블랙으로
    parent->color = RBTREE_RED;        // 형제를 레드로
    if (is_my_position_right(brother)) // 형제의 위치를 기준으로 회전
      left_rotate(tree, brother);
    else
      right_rotate(tree, brother);
    double_black_check(cur, tree); // 회전은 했지만 아직 현재 노드의 이중 블랙은 해결되지 않았다 !!
  }
  else // 형제 노드가 블랙일 때
  {
    parent = cur->parent;                                                            // 새 부모가 생기나??
    brother = FIND_BROTHER(cur, parent);                                             // 일단 형제는 바뀌었기에 다시 대입
    node_t *left_sibling = brother->left;                                            // 왼쪽 조카
    node_t *right_sibling = brother->right;                                          // 오른쪽 조카
    if (left_sibling->color == RBTREE_BLACK && right_sibling->color == RBTREE_BLACK) // 모든 조카가 블랙일 경우
    {
      brother->color = RBTREE_RED;
      double_black_check(parent, tree); // 재귀적으로 부모에게 이중 블랙 전파
    }
    else
    {
      if (is_my_position_right(cur))
      { // 내 포지션이 right일 경우
        if (left_sibling->color == RBTREE_RED)
        {                                     // 형제는 left인데, left의 조카도 레드일 때 (형제와 같은 방향의 조카)
          brother->color = parent->color;     // 형제의 색을 부모의 색으로 교체
          parent->color = RBTREE_BLACK;       // 부모를 블랙으로
          left_sibling->color = RBTREE_BLACK; // 왼쪽 조카를 블랙으로
          right_rotate(tree, parent);         // 우회전 수행
        }
        else if (right_sibling == RBTREE_RED) // 형제와 다른 방향의 조카만 레드일 때
        {
          right_sibling->color = RBTREE_BLACK;
          brother->color = RBTREE_RED;
          left_rotate(tree, brother);          // 좌회전 수행 -> 이러면 형제와 같은 방향의 조카가 레드가 됨!!
          brother = FIND_BROTHER(cur, parent); // 새로운 형제 찾기
          left_sibling = brother->left;        // 새로운 LL 조카 찾기
          brother->color = parent->color;      // 형제를 부모의 색으로 교체
          parent->color = RBTREE_BLACK;        // 부모를 블랙으로
          left_sibling->color = RBTREE_BLACK;  // 왼쪽 조카를 블랙으로
          right_rotate(tree, parent);          // 우회전 수행
        }
      }
      else // 내 포지션이 left일 경우, 형제 노드는 right 포지션
      {
        if (right_sibling->color == RBTREE_RED) // 형제 노드와 같은 방향의 조카가 RED일 때
        {
          brother->color = parent->color;      // 형제의 색깔의 부모의 색깔로 교체
          parent->color = RBTREE_BLACK;        // 부모의 색깔은 블랙으로
          right_sibling->color = RBTREE_BLACK; // 조카의 색깔은 블랙으로
          left_rotate(tree, parent);           // 우회전 수행
        }
        else if (left_sibling == RBTREE_RED) // 형제와 다른 방향의 조카만 레드일 때
        {
          left_sibling->color = RBTREE_BLACK;
          brother->color = RBTREE_RED;
          right_rotate(tree, brother);
          brother = FIND_BROTHER(cur, parent);
          right_sibling = brother->right;
          brother->color = parent->color;
          parent->color = RBTREE_BLACK;
          right_sibling->color = RBTREE_BLACK;
          left_rotate(tree, parent);
        }
      }
    }
  }
}