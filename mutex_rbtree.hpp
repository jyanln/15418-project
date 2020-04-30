#ifndef MUTEX_RBTREE_HPP
#define MUTEX_RBTREE_HPP

#include "rbtree.hpp"

//This code is adapted from GeeksForGeeks
struct Node
{
  int key;
  int val;
  //Red = False
  //Black = True
  bool color;
  Node* left, * right, * parent;

  Node(int _key, int _val) : key(_key), val(_val) {
    left = right = parent = nullptr;
    this->color = false;
  }
};

class MutexRBTree : public RBTree
{
private:
  Node* root;
protected:
  void rotateLeft(Node*&, Node*&);
  void rotateRight(Node*&, Node*&);
  void fixColoring(Node*&, Node*&);
  void fixDoubleBlack(Node* v);
  void deleteNode(Node* x);
  Node* search(int n);
public:
  //Constructor
  MutexRBTree() {
    root = nullptr;
  }
  
  ~MutexRBTree() {}

  virtual const std::string name() { return "Mutex RBTree"; };

  int insert(int key, int val);
  int remove(int key);
  int lookup(int key);

  void printLevelOrder();
};

#endif // MUTEX_RBTREE_HPP
