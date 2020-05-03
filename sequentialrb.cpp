
//This code is adapted from GeeksForGeeks
struct Node
{
  int value;
  //Red = False
  //Black = True
  bool color;
  Node* left, * right, * parent;

  Node(int value) {
    this->value = value;
    left = right = parent = nullptr;
    this->color = false;
  }
};

class RedBlackTree
{
private:
  Node* root;
protected:
  void rotateLeft(Node*&, Node*&);
  void rotateRight(Node*&, Node*&);
  void fixupInsert(Node*&, Node*&);
  void fixupDelete(Node* v);
  void deleteNode(Node* x);
public:
  //Constructor
  RedBlackTree() {
    root = nullptr;
  }
  Node* search(int n);
  void insertNode(const int& n);
  void deleteByVal(int n);
  void printLevelOrder();
};

Node* Successor(Node* x) {
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
  if (newNode->value < root->value) {
    root->left = insertBSTNode(root->left, newNode);
    root->left->parent = root;
  }
  if (newNode->value > root->value) {
    root->right = insertBSTNode(root->right, newNode);
    root->right->parent = root;
  }
  return root;
}

void RedBlackTree::rotateLeft(Node*& root, Node*& curr) {
  Node* rightNode = curr->right;
  curr->right = rightNode->left;
  rightNode->left->parent = curr;
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

void RedBlackTree::rotateRight(Node*& root, Node*& curr) {
  Node* leftNode = curr->left;
  curr->left = leftNode->right;
  leftNode->right->parent = curr;
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

void RedBlackTree::fixupInsert(Node*& root, Node*& newNode) {
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

void RedBlackTree::insertNode(const int& value) {
  Node* newNode = new Node(value);
  Node* searchPtr = root;
  Node* prevSearchNode = nullptr;

  while (searchPtr != nullptr) {
    prevSearchNode = searchPtr;
    if (newNode->value < searchPtr->value) {
      searchPtr = searchPtr->left;
    }
    else {
      searchPtr = searchPtr->right;
    }
  }

  newNode->parent = rootPtr;
  if (prevSearchNode == null) {
    root = newNode;
  }
  else if (newNode->value < rootPtr->value) {
    prevSearchNode->left = newNode;
  }
  else {
    prevSearchNode->right = newNode;
  }
  newNode->left = nullptr;
  newNode->right = nullptr;
  newNode->color = false;
  fixupInsert(root, newNode);
}

void RedBlackTree::fixupDelete(Node* x) {
  while (x != root && x->color == true) {
    if (x == x->parent->left) {
      Node* siblingNode = x->parent->right;
      if (siblingNode->color == true) {
        siblingNode->color = true;
        x->parent->color = false;
        rotateLeft(root,x->parent);
        siblingNode = x->parent->right;
      }
      if (siblingNode->left->color == true && siblingNode->right->color == true) {
        siblingNode->color = red;
        x = x->parent;
      }
      else {
        if (siblingNode->right->color == true) {
          siblingNode->left->color = true;
          siblingNode->color = false;
          rotateRight(root, siblingNode);
          siblingNode = x->parent->right;
        }
        siblingNode->color = x->parent->color;
        x->parent->color = true;
        siblingNode->right->color = true;
        rotateLeft(root, x - parent);
        x = root;
      }
    }
    else {
      Node* siblingNode = x->parent->left;
      if (siblingNode->color == true) {
        siblingNode->color = true;
        x->parent->color = false;
        rotateRight(root, x->parent);
        siblingNode = x->parent->left;
      }
      if (siblingNode->right->color == true && siblingNode->left>color == true) {
        siblingNode->color = red;
        x = x->parent;
      }
      else {
        if (siblingNode->left->color == true) {
          siblingNode->right->color = true;
          siblingNode->color = false;
          rotateLeft(root, siblingNode);
          siblingNode = x->parent->left;
        }
        siblingNode->color = x->parent->color;
        x->parent->color = true;
        siblingNode->left->color = true;
        rotateRight(root, x - parent);
        x = root;
      }
    }
  }
  x->color = true;
}

void RedBlackTree::deleteNode(Node* v) {
  //mtx.lock();
  Node* u;
  if (v->left==nullptr || v->right == nullptr) {
    u = z;
  }
  else {
    u = Successor(v);
  }

  Node* siblingNode;
  if (u->left != nullptr) {
    siblingNode = u->left;
  }
  else {
    siblingNode = u->right;
  }
  siblingNode->parent = u->parent;
  if (u->parent == nullptr) {
    root = siblingNode;
  }
  else {
    if (u == u->parent->left) {
      u->parent->left = siblingNode;
    }
    else {
      u->parent->right = siblingNode;
    }
  }
  if (u != v) {
    v->value = u->value;
  }
  if (u->color == true) {
    fixupDelete(siblingNode);
  }
  return u;
}

Node* RedBlackTree::search(int n) {
  Node* temp = root;
  while (temp != NULL) {
    if (n < temp->value) {
      if (temp->left == NULL)
        break;
      else
        temp = temp->left;
    }
    else if (n == temp->value) {
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

// utility function that deletes the node with given value 
void RedBlackTree::deleteByVal(int n) {
  if (root == NULL)
    // Tree is empty 
    return;

  Node* v = search(n);

  if (v->value != n) {
    cout << "No node found to delete with value:" << n << endl;
    return;
  }
  mtx.lock();


  deleteNode(v);
  mtx.unlock();
}

void RedBlackTree::printLevelOrder() {
  if (root == NULL)
    return;

  std::queue<Node*> q;
  q.push(root);

  while (!q.empty())
  {
    Node* temp = q.front();
    if (temp->color) {
      cout << temp->value << "(BLACK)  ";
    }
    else {
      cout << temp->value << "(RED)  ";
    }
    q.pop();

    if (temp->left != NULL)
      q.push(temp->left);

    if (temp->right != NULL)
      q.push(temp->right);
  }
}

int main()
{
  RedBlackTree test1;

  cout << "Test 1 : Insertion and Deletion \n";

  test1.insertNode(1);
  test1.insertNode(2);
  test1.insertNode(3);
  test1.printLevelOrder();
  cout << "\n";
  test1.deleteByVal(1);
  test1.deleteByVal(2);
  test1.deleteByVal(3);
  test1.printLevelOrder();
  cout << "\n";

  RedBlackTree test2;

  cout << "\nTest 2 : Deleting Non Existing Values \n";
  test2.insertNode(7);
  test2.insertNode(6);
  test2.insertNode(9);
  test2.insertNode(4);
  test2.printLevelOrder();
  cout << "\n";
  test2.deleteByVal(4);
  test2.deleteByVal(7);
  test2.deleteByVal(9);
  test2.deleteByVal(4);
  test2.deleteByVal(6);
  test2.printLevelOrder();
}
