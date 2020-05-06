#ifndef CAS_RBTREE_HPP
#define CAS_RBTREE_HPP

#include "rbtree.hpp"
#include <atomic>
#include <vector>

using namespace std;


class CasRBTree : public RBTree
{
    public:

struct Node
{
  int key;
  int val;
  //Red = False
  //Black = True
  bool color;
  std::atomic<bool> flag;
  int marker;
  Node* left, * right, * parent;

  //Normal Node Constructor
  Node(int _key, int _val) : key(_key), val(_val) {
    left = right = parent = nullptr;
    this->color = false;
    this->flag = false;
    this->marker = -1;
  }

  //NIL Node Constructor
  Node() {
    this->key = -1;
    this->val = -1;
    this->flag = false;
    this->marker = -1;
    this->color = true;
    left = right = parent = nullptr;
  }
};
struct MoveUpStruct
{
  std::vector<Node*> nodeList;
  Node* goalNode;

  MoveUpStruct() {
    this->goalNode = nullptr;
  }

  MoveUpStruct(Node* goal) {
    this->goalNode = goal;
  }
};
private:

  Node* root;
protected:
  void rotateLeft(Node*&, Node*&);
  void rotateRight(Node*&, Node*&);
  void fixupInsert(Node*&, Node*&);
  void fixupDelete(Node* v);
  bool deleteNode(Node* x);
  Node* search(int n);
Node* Successor(Node* x);
bool IsIn(Node* curr, MoveUpStruct* mvstruct);
bool IsGoalNode(Node* curr, MoveUpStruct* mvstruct);
void ReleaseFlags(MoveUpStruct* mvstruct, bool success, Node** nodesToRelease, int numNodes);
bool SpacingRuleIsSatisfied(Node* curr, Node* start, MoveUpStruct* mvstruct);
bool GetFlagsForMarkers(Node* start, MoveUpStruct* mvstruct, Node* pos1, Node* pos2, Node* pos3, Node* pos4);
bool GetFlagsAndMarkersAbove(Node* startNode, int numAdditional);
bool ApplyMoveUpRule(Node* x, Node* w);
bool SetupLocalAreaForInsert(Node* curr);
bool SetupLocalAreaForDelete(Node* successor, Node* deletedNode);
Node* MoveInserterUp(Node* oldNode);
Node* MoveDeleterUp(Node* oldNode);
void fixupDeleteCase1(Node* x, Node* siblingNode);
void fixupDeleteCase3(Node* x, Node* siblingNode);
void fixupDeleteCase1_sym(Node* x, Node* siblingNode);
void fixupDeleteCase3_sym(Node* x, Node* siblingNode);
public:
  //Constructor
  CasRBTree() {
      Node* root = new Node();
      Node* dummy1 = new Node();
      Node* dummy2 = new Node();
      Node* dummy3 = new Node();
      Node* dummy4 = new Node();
      Node* dummy5 = new Node();
      Node* dummySibling = new Node();
      root->color = true;
      dummy1->color = true;
      dummy2->color = true;
      dummy3->color = true;
      dummy4->color = true;
      dummy5->color = true;
      dummySibling->color = true;

      dummySibling->parent = root;
      root->parent = dummy5;
      dummy5->parent = dummy4;
      dummy4->parent = dummy3;
      dummy3->parent = dummy2;
      dummy2->parent = dummy1;

      dummy1->left = dummy2;
      dummy2->left = dummy3;
      dummy3->left = dummy4;
      dummy4->left = dummy5;
      dummy5->left = root;
      root->right = dummySibling;
  }

  ~CasRBTree() {}

  const std::string name() { return "CAS RBTree"; };

  int insert(int key, int val);
  int remove(int key);
  int lookup(int key);
};

#endif
