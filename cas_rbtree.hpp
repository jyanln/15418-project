#ifndef CAS_RBTREE_HPP
#define CAS_RBTREE_HPP

#include "rbtree.hpp"
#include <atomic>
#include <vector>

using namespace std;

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

  MoveUpStruct(Node* goal) {
    this->goalNode = goal;
  }
};

class CASRBTree : public RBTree
{
private:
  Node* root;
protected:
  void rotateLeft(Node*&, Node*&);
  void rotateRight(Node*&, Node*&);
  void fixupInsert(Node*&, Node*&);
  void fixupDelete(Node* v);
  void deleteNode(Node* x);
  Node* search(int n);
public:
  //Constructor
  CASRBTree() {
    root = nullptr;
  }

  ~CASRBTree() {}

  virtual const std::string name() { return "CAS RBTree"; };

  Node* initializeTree();
  int insert(int key, int val);
  int remove(int key);
  int lookup(int key);
};

#endif