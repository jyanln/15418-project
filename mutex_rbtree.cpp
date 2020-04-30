#include "mutex_rbtree.hpp"

#include "rbtree.hpp"

#include <mutex>
#include <iostream>
#include <queue>

std::mutex mtx;

Node* BSTReplace(Node* x) {
  if (x->left != nullptr && x->right != nullptr) {
    Node* temp = x->right;
    while (temp->left != nullptr) {
      temp = temp->left;
    }
    return temp;
  }
  if (x->left == nullptr && x->right == nullptr) {
    return nullptr;
  }

  if (x->left != nullptr) {
    return x->left;
  }
  else {
    return x->right;
  }
}

Node* insertBSTNode(Node* root, Node* newNode) {
  if (root == nullptr) {
    return newNode;
  }
  if (newNode->key < root->key) {
    root->left = insertBSTNode(root->left, newNode);
    root->left->parent = root;
  }
  if (newNode->key > root->key) {
    root->right = insertBSTNode(root->right, newNode);
    root->right->parent = root;
  }
  return root;
}

void MutexRBTree::rotateLeft(Node*& root, Node*& curr) {
  Node* rightNode = curr->right;
  curr->right = rightNode->left;
  if (curr->right != nullptr) {
    curr->right->parent = curr;
  }

  rightNode->parent = curr->parent;
  if (curr->parent == nullptr) {
    root = rightNode;
  }
  else if (curr == curr->parent->left) {
    curr->parent->left = rightNode;
  }
  else {
    curr->parent->right = rightNode;
  }
  rightNode->left = curr;
  curr->parent = rightNode;
}

void MutexRBTree::rotateRight(Node*& root, Node*& curr) {
  Node* leftNode = curr->left;
  curr->left = leftNode->right;
  if (curr->left != nullptr) {
    curr->left->parent = curr;
  }

  leftNode->parent = curr->parent;
  
  if (curr->parent == NULL) {
    root = leftNode;
  }
  else if (curr == curr->parent->right) {
    curr->parent->right = leftNode;
  }
  else {
    curr->parent->left = leftNode;
  }
  leftNode->right = curr;
  curr->parent = leftNode;
}

void MutexRBTree::fixColoring(Node*& root, Node*& newNode) {
  Node* parentNode = nullptr;
  Node* grandParentNode = nullptr;

  while ((newNode != root) && (newNode->color != true) &&
    (newNode->parent->color == false)) {
    parentNode = newNode->parent;
    grandParentNode = parentNode->parent;

    if (parentNode == grandParentNode->left) {
      Node* uncleNode = grandParentNode->right;

      //If uncle is red, then recolor
      if (uncleNode != nullptr && uncleNode->color == false) {
        grandParentNode->color = false;
        parentNode->color = true;
        uncleNode->color = true;
        newNode = grandParentNode;
      }
      //otherwise, rotation neceassary
      else {
        if (newNode == parentNode->right) {
          rotateLeft(root, parentNode);
          newNode = parentNode;
          parentNode = newNode->parent;
        }

        rotateRight(root, parentNode);
        bool tempColor = parentNode->color;
        parentNode->color = grandParentNode->color;
        grandParentNode->color = tempColor;
        newNode = parentNode;
      }
    }
    else {
      Node* uncleNode = parentNode->left;

      //If uncle is red, then recolor
      if (uncleNode != nullptr && uncleNode->color == false) {
        grandParentNode->color = false;
        parentNode->color = true;
        uncleNode->color = true;
        newNode = grandParentNode;
      }
      //otherwise, rotation is necesary
      else {
        if (newNode == parentNode->left) {
          rotateRight(root, parentNode);
          newNode = parentNode;
          parentNode = newNode->parent;
        }

        rotateLeft(root, grandParentNode);
        bool tempColor = parentNode->color;
        parentNode->color = grandParentNode->color;
        grandParentNode->color = tempColor;
        newNode = parentNode;
      }
    }
  }
  root->color = true;
}

int MutexRBTree::insert(int key, int val) {
  mtx.lock();
  
  Node* v = search(key);
  // Update value and return old value
  if(v && v->key == key) {
    std::swap(v->val, val);
    mtx.unlock();
    return val;
  } else {
    Node* newNode = new Node(key, val);
    root = insertBSTNode(root, newNode);
    fixColoring(root, newNode);

    mtx.unlock();
    //TODO return prev value?
    return -1;
  }
}

void MutexRBTree::deleteNode(Node* v) {
  Node* u = BSTReplace(v);

  // True when u and v are both black
  bool uvBlack = ((u == nullptr || u->color == true) && (v->color == true));
  Node* parentNode = v->parent;

  if (u == nullptr) {
    // u is nullptr therefore v is leaf
    if (v == root) {
      // v is root, making root null
      root = nullptr;
    }
    else {
      if (uvBlack) {
        // u and v both black
        // v is leaf, fix double black at v
        fixDoubleBlack(v);
      }
      else {
        // u or v is red
        Node* siblingNode = nullptr;
        if (parentNode != nullptr && parentNode->left == v) {
          siblingNode = v->parent->right;
        }
        else if (parentNode != nullptr) {
          siblingNode = v->parent->left;
        }
        if (siblingNode != nullptr){
          // sibling is not null, make it red"
          siblingNode->color = false;
        }
      }

      // delete v from the tree
      if (parentNode != nullptr && parentNode->left == v) {
        parentNode->left = nullptr;
      }
      else if (parentNode != nullptr) {
        parentNode->right = nullptr;
      }
    }
    delete v;
    return;
  }

  if (v->left == nullptr || v->right == nullptr) {
    // v has 1 child
    if (v == root) {
      // v is root, assign the key of u to v, and delete u
      v->key = u->key;
      v->left = v->right = nullptr;
      delete u;
    }
    else {
      // Detach v from tree and move u up
      if (parentNode != nullptr && parentNode->left == v) {
        parentNode->left = u;
      }
      else {
        parentNode->right = u;
      }
      delete v;
      u->parent = parentNode;
      if (uvBlack) {
        // u and v both black, fix double black at u
        fixDoubleBlack(u);
      }
      else {
        // u or v red, color u black
        u->color = true;
      }
    }
    return;
  }

  // v has 2 children, swap keys with successor and recurse
  int tempVal = u->key;
  u->key = v->key;
  v->key = tempVal;
  deleteNode(u);
}

void MutexRBTree::fixDoubleBlack(Node* x) {
  if (x->parent == nullptr)
    // Reached root
    return;

  Node* siblingNode = nullptr, * parentNode = x->parent;
  if (parentNode->left == x) {
    siblingNode = parentNode->right;
  }
  else {
    siblingNode = parentNode->left;
  }
  if (siblingNode == nullptr) {
    // No sibiling, double black pushed up
    fixDoubleBlack(parentNode);
  }
  else {
    if (siblingNode->color == false) {
      // Sibling red
      parentNode->color = false;
      siblingNode->color = true;
      if (siblingNode->parent->left == siblingNode) {
        // left case
        rotateRight(root, parentNode);
      }
      else {
        // right case
        rotateLeft(root, parentNode);
      }
      fixDoubleBlack(x);
    }
    else {
      // Sibling black
      if ((siblingNode->left != nullptr && siblingNode->left->color == false) ||
        (siblingNode->right != nullptr && siblingNode->right->color == false)) {
        // at least 1 red children
        if (siblingNode->left != nullptr && siblingNode->left->color == false) {
          if (siblingNode->parent->left == siblingNode) {
            // left left
            siblingNode->left->color = siblingNode->color;
            siblingNode->color = parentNode->color;
            rotateRight(root, parentNode);
          }
          else {
            // right left
            siblingNode->left->color = parentNode->color;
            rotateRight(root, siblingNode);
            rotateLeft(root, parentNode);
          }
        }
        else {
          if (siblingNode->parent->left == siblingNode) {
            // left right
            siblingNode->right->color = parentNode->color;
            rotateLeft(root, siblingNode);
            rotateRight(root, parentNode);
          }
          else {
            // right right
            siblingNode->right->color = siblingNode->color;
            siblingNode->color = parentNode->color;
            rotateLeft(root, parentNode);
          }
        }
        parentNode->color = true;
      }
      else {
        // 2 black children
        siblingNode->color = false;
        if (parentNode->color == true)
          fixDoubleBlack(parentNode);
        else
          parentNode->color = true;
      }
    }
  }
}

Node* MutexRBTree::search(int n) {
  Node* temp = root;
  while (temp != NULL) {
    if (n < temp->key) {
      if (temp->left == NULL)
        break;
      else
        temp = temp->left;
    }  
    else if (n == temp->key) {
      break;
    }
    else {
      if (temp->right == NULL)
        break;
      else
        temp = temp->right;
    }
  }

  return temp;
}

// utility function that deletes the node with given key 
int MutexRBTree::remove(int key) {
  mtx.lock();

  if (root == NULL) {
    // Tree is empty 
    mtx.unlock();
    return -1;
  }

  Node* v = search(key);

  if (v->key != key) {
    mtx.unlock();
    return -1;
  }

  int ret = v->val;
  deleteNode(v);

  mtx.unlock();
  return ret;
}

int MutexRBTree::lookup(int key) {
  mtx.lock();
  if (root == NULL) {
    // Tree is empty 
    mtx.unlock();
    return -1;
  }

  Node* v = search(key);

  if (v && v->key == key) {
    mtx.unlock();
    return v->val;
  } else {
    mtx.unlock();
    return -1;
  }
}

void MutexRBTree::printLevelOrder() {
  if (root == NULL)
    return;

  std::queue<Node*> q;
  q.push(root);

  while (!q.empty())
  {
    Node* temp = q.front();
    if (temp->color) {
        std::cout << temp->key << "(BLACK)  ";
    }
    else {
        std::cout << temp->key << "(RED)  ";
    }
    q.pop();

    if (temp->left != NULL)
      q.push(temp->left);

    if (temp->right != NULL)
      q.push(temp->right);
  }
}
