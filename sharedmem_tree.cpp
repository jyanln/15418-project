
using namespace std;

//This code is adapted from GeeksForGeeks
struct opInfo {
  short status;
  void* nextLocation;
};

#define FREE true
#define OWNED false

struct pNode {
  bool flag;
  dNode* dataNode;
};

struct dNode
{
  //Red = False
  //Black = True
  bool color;

  int key;
  vRecord* valData;
  opRecord* opData;
  Node* left, * right, * parent;
  opInfo* next;

  dNode(int key, int value) {
    this->key = value;
    this->valData, nullptr;
    this->opData = nullptr;
    left = right = parent = nullptr;
    this->color = false;
  }
};

struct vRecord
{
  int value;
  //variables used by Chuong ed tal's wait free alg
};

#define WAITING 1
#define IN_PROGRESS 2
#define COMPLETED 3

struct state {
  short status;
  void* position;
};

struct opRecord
{
  int type;
  int key;
  int value;
  int pid;
  state state;

  opRecord(int type, int key, int value) {
    this->type = type;
    this->key = key;
    this->value = value;
    this->pid = -1;
  }
};

/*
Tables Needed:
  Modify Table
  Search Table

*/


class RedBlackTree
{
private:
  Node* root;
protected:
  void rotateLeft(Node*&, Node*&);
  void rotateRight(Node*&, Node*&);
  void fixColoring(Node*&, Node*&);
  void fixDoubleBlack(Node* v);
public:
  //Constructor
  RedBlackTree() {
    root = nullptr;
  }
  void insertNode(const int& n);
  void deleteNode(Node* x);
};

//insert new node with given key in BST
Node* insertBSTNode(dNode* root, dNode* newNode) {
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
/*
void RedBlackTree::rotateLeft(dNode*& root, dNode*& curr) {
  Node* rightNode = curr->right;
  curr->right = rightNode->left;
  if (curr->right != nullptr) {
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

void RedBlackTree::rotateRight(dNode*& root, dNode*& curr) {
  Node* leftNode = curr->left;
  curr->left = leftNode->right;
  if (curr->left != nullptr) {
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

void RedBlackTree::fixColoring(dNode*& root, dNode*& newNode) {
  Node* parentNode = nullptr;
  Node* grandParentNode = nullptr;

  while ((newNode != root) && (newNode->color != true) &&
    (newNode->parent->color == false)) {
    parentNode = newNode->parent;
    grandParentNode = parentNode->parent;

    if (parentNode == grandParentNode->left) {
      Node* uncleNode = parentNode->right;

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

        rotateLeft(root, parentNode);
        bool tempColor = parentNode->color;
        parentNode->color = grandParentNode->color;
        grandParentNode->color = tempColor;
        newNode = parentNode;
      }
    }
  }
}

void RedBlackTree::insertNode(const int& value) {
  mtx.lock();
  Node* newNode = new Node(value);
  root = insertBSTNode(root, newNode);
  fixColoring(root, newNode);
  mtx.unlock();
}
*/
int search(int key) {
  //TODO: figure out what myid is
  int myid = 0;
  opRecord* operations = new opRecord(SEARCH, key, -1);
  operations->state->status = IN_PROGRESS;
  operations->state->position = nullptr;
  searchT[myid] = operations;
  traverse(operations);
  if (opData->state->position != nullptr) {
    //read the value stored in teh record using Chuong et al's algorithm and return it

  }
}

void insertOrUpdate(int key, int value) {
  int* valData = nullptr;
  search(key);
  valData = searchT[myid]->state->position;
  if (valData == nullptr) {
    int pid = 0; // the process selected to help in RR
    opRecord* opData = new opRecord(INSERT, key, value);
    executeOperation(opData);
    valData = opData->state->position;
    if (opData != nullptr) {
      traverse(opData);
    }
  }
  if (valData != nullptr) {
    //phase 3 update value using Chung's alg
  }
}

void deleteNode(int key) {
  //phase 1: determine if the key already exists
  if (search(key)) {
    //phase 2: delete using MTL-framework

    //select a search operation to help at the end of phase2
    int pid = 0; //select process using RR
    opRecord* opData = new opRecord(DELETE, key, -1);
    executeOperation(opData);
    if (opData != nullptr) {
      traverse(opData);
    }
  }
}

void traverse(opRecord* opData) {
  dNode* dCurrent = root->dNode;
  while (dCurrent != null) {
    if (opData->state->status == COMPLETED) {
      return;
    }
    if (opData->key < dCurrent->key) {
      dCurrent = dCurrent->left->dNode;
    }
    else {
      dCurrent = dCurrent->right->dNode;
    }
  }
  if (dCurrent->key = opData->key) {
    valData = dCurrent->valData;
  else {
    valData = nullptr;
  }
  opData->state->status = COMPLETED;
  opData->state->position = valData;
  }
}

void executeOperation(opRecord opData) {
  opData->state->status = WAITING;
  opData->state->position = root;
  movementTable[myid] = opData;

  //select a modify operation to help later
  int pid = 0; //select in round robin manner

  injectOperation(opData);
  int status = opData->state->status;
  pNode* pCurrent = opData->state->position;

  while (status != COMPLETED) {
    dNode* dCurrent = pCurrent->dNode;
    if (dCurrent->opData == opData) {
      executeWindowTransaction(pCurrent, dCurrent);
    }
    status = opData->state->status;
    pCurrent = opData->state->position;
  }

  if (opData != nullptr) {
    injectOperation(opData);
  }
}

void injectOperation(opRecord opData) {
  while (opData->state->status == WAITING) {
    dNode* dRoot = root->dNode;
    if (dRoot->opData!= nullptr) {
      executeWindowTransaction(root, dRoot);
    }
    int status = opData->state->status;
    pNode* pCurrent = opData->state->position;
  }
  if (opData != nullptr) {
    injectOperation(opData);
  }
}

executeWindowTransaction(pNode* pNode, dNode* dNode) {
  opRecord* opData = dNode->opData;
  int flag = pNode->flag;
  dNode* dCurrent = pNode->dataNode;
  if (dCurrent->opData) {
    if (flag == OWNED) {
      if (pNode == pRoot) {
        //the operation may have just been injected but op state may not have been updated yet
        //CAS switch
      }
      if (!exchangeCheapWindowTransaction(pNode, dCurrent)) {
        dNode* windowSoFar = copy(dCurrent);
        while (more nodes to add) {
          pNode* pNextToAdd = address of the ponter node of the next tree node to be copied;
          pNode* pNextToAdd->dNode;
          if (dNextToAdd->opData != nullptr) {
            executeWindowTransaction(pNextToAdd, dNextToAdd);
          }
          dNextToAdd = pNextToAdd->dNode;
          // copy pNextToAdd and dNextToAdd and add to windowSoFar
        }
        //window has been copied so apply transformation by Tarjan's to windowSoFar
        dNode* dWindowRoot = address of data node now acting as window;
        if (laster / terminal window transaction) {
          status = COMPLETED;
          ...add more
        }
      }
    }
  }
}

bool executeCheapWindowTransaction(pNode* pNode, dNode* dNode) {
  blah
}

void slideWindowDown(pNode) {
  blah
}

/*
Node* BSTReplace(dNode* x) {
  if (x->left != nullptr && x->right != nullptr) {
    Node* temp = x;
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

void RedBlackTree::deleteNode(Node* v) {
  mtx.lock();
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
        if (siblingNode != nullptr)
          // sibling is not null, make it red"
          siblingNode->color = false;
      }

      // delete v from the tree
      if (parentNode != nullptr && parentNode->left == v) {
        parentNode->left = nullptr;
      }
      else {
        parentNode->right = nullptr;
      }
    }
    delete v;
    return;
  }

  if (v->left == nullptr || v->right == nullptr) {
    // v has 1 child
    if (v == root) {
      // v is root, assign the value of u to v, and delete u
      v->value = u->value;
      v->left = v->right = nullptr;
      delete u;
    }
    else {
      // Detach v from tree and move u up
      if (v->parent != nullptr && v->parent->left == v) {
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

  // v has 2 children, swap values with successor and recurse
  int tempVal = u->value;
  u->value = v->value;
  v->value = tempVal;
  deleteNode(u);
  mtx.unlock();
}

void RedBlackTree::fixDoubleBlack(Node* x) {
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
      if (siblingNode->left != nullptr && siblingNode->left->color == false ||
        siblingNode->right != nullptr && siblingNode->right->color == false) {
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
*/
int main()
{
  RedBlackTree rb;

  rb.insertNode(4);
  rb.insertNode(5);
  rb.insertNode(8);
  rb.insertNode(11);
  rb.deleteNode(rb->left);
  rb.deleteNode(rb.);


}
