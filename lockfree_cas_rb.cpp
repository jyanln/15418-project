using namespace std;

//This code is adapted from GeeksForGeeks
struct Node
{
  int value;
  //Red = False
  //Black = True
  bool color, flag;
  int marker;
  Node* left, * right, * parent;

  Node(int value) {
    this->value = value;
    left = right = parent = nullptr;
    this->color = false;
    this->flag = false;
    this->marker = -1;
  }
};

struct MoveUpStruct
{
  Node** nodeList;
  Node* goalNode;

  MoveUpStruct(Node* goal) {
    this->nodeList = nullptr;
    goalNode = goal;
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
  Node* initializeTree();
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

//LockFree CAS Helper functions
//TODO: Figure out what moveupStruct is
//      Fix ReleaseFlags structure
//      Check correctness


/*
To Fix: 
1) On initialization empty tree should have:
      6 dummy ancester nodes
      1 dummy sibling to the NIL root (all black)
2) Fix checks with nullptr to dummy node checks (incorrect assumption)

3) Figure out moveupStruct which does:
      pg 19:
      Both routines check (using the IsIn
      routine on the moveUpStruct) whether or not a node whose flag is to be acquired has been inherited from
      another process via the Move-Up rule

      set in the ApplyMoveUpRule (Figure 31)

      contains nodes and if it is in the moveupstruct, the flag doesnt need to be acquired

      moveupstruct contains a goal node which identifies which, when reached, allows another process ot move up
      Also done by release flags


*/

//this entire function is a mess
int ReleaseFlags(moveUpStruct* mvstruct, bool success, Node** nodesToRelease, int numNodes) {
  for (int i = 0; i < numNodes; i++) {
    if (success) {
      if (!IsIn(nodesToRelease[i], mvstruct)) {
        nodesToRelease[i]->flag = false;
      }
      else {
        if (IsGoalNode(nodeToRelease[i], mvstruct)) {
          //release uneeded flags in mvstruct and discard mvstruct
        }
      }
    }
    else {
      if (!IsIn(nodesToRelease[i], mvstruct)) {
        nodesToRelease[i]->flag = false;
      }
    }
  }
}

bool SpacingRuleIsSatisfied(Node* curr, Node* start, int PIDtoIgnore) {
  if (curr != start) {
    if (curr->marker != 0) {
      return false;
    }
  }
  Node* parentNode = curr->parent;
  if (parentNode != start) {
    if ((!IsIn(parentNode, mvstruct)) && (!parentNode->flag.compare_exchange_weak(false, true))) {
      return false;
    }
    if (parentNode != curr->parent) {
      parentNode->flag = false;
      return false;
    }
    if (parentNode->marker != 0) {
      parentNode->flag = false;
      return false;
    }
  }
  Node* siblingNode;
  if (curr == parentNode->left) {
    siblingNode = parentNode->right;
  }
  else {
    siblingNode = parentNode->left;
  }
  if ((!IsIn(ts, mvstruct)) && (!siblingNode->flag.compare_exchange_weak(false, true))) {
    if (parentNode != curr) {
      ReleaseFlags(mvstruct, false, [parentNode], 1);
    }
    return false;
  }
  if ((siblingNode->marker != 0) && (siblingNode->marker != PIDtoIgnore)){
    ReleaseFlags(mvstruct, false, [siblingNode], 1);
    if (parentNode != curr) {
      ReleaseFlags(mvstruct, false, [parentNode], 1);
    }
    return false;
  }
  if (parentNode != curr) {
    ReleaseFlags(mvstruct, false, [siblingNode], 1);
  }
  return true;
}

bool GetFlagsForMarkers(Node* start, moveUpStruct* mvstruct, Node* pos1, Node* pos2, Node* pos3, Node* pos4) {
  pos1 = start->parent;
  if ((!IsIn(pos1, mvstruct) && (!pos1->flag.compare_exchange_weak(false, true)))) {
    return false;
  }
  if (pos1 != start->parent) {
    ReleaseFlags(mvstruct, false, [pos1], 1);
    return false;
  }
  pos2 = pos1->parent;
  if ((!IsIn(pos2, mvstruct) && (!pos2->flag.compare_exchange_weak(false, true)))) {
    return false;
  }
  if (pos2 != pos1->parent) {
    ReleaseFlags(mvstruct, false, [pos2], 1);
    return false;
  }
  pos3 = pos2->parent;
  if ((!IsIn(pos3, mvstruct) && (!pos3->flag.compare_exchange_weak(false, true)))) {
    return false;
  }
  if (pos3 != pos2->parent) {
    ReleaseFlags(mvstruct, false, [pos3], 1);
    return false;
  }
  pos4 = pos3->parent;
  if ((!IsIn(pos4, mvstruct) && (!pos4->flag.compare_exchange_weak(false, true)))) {
    return false;
  }
  if (pos4 != pos3->parent) {
    ReleaseFlags(mvstruct, false, [pos4], 1);
    return false;
  }
}

bool GetFlagsAndMarkersAbove(Node* startNode, int numAdditional) {
  if (!GetFlagsForMarkers(start, mvstruct, pos1, pos2, pos3, pos4)) {
    return false;
  }
  Node* firstNew = pos4->parent;
  if (!IsIn(firstNew, mvstruct) && (!firstNew->flag.compare_exchange_weak(false, true))) {
    ReleaseFlags(mvstruct, false, [pos4, pos3, pos2, pos1], 4);
    return false;
  }
  if ((firstNew != pos4->parent) && 
    (!SpacingRuleIsSatisfied(firstNew, start, PIDtoIgnore, mvstruct))){
    ReleaseFlags(mvstruct, false, [firstNew, pos4, pos3, pos2, pos1], 5);
    return false;
  }

  Node* secondNew = nullptr;
  if (numAdditional == 2) {
    secondNew = firstNew->parent;
    if ((!IsIn(secondNew, mvstruct) && !secondNew->flag.compare_exchange_weak(false, true))) {
      ReleaseFlags(mvstruct, false, [firstNew, pos4, pos3, pos2, pos1], 5);
      return false;
    }
    if ((secondNew != firstNew->parent) &&
      (!SpacingRuleIsSatisfied(secondNew, start, PIDtoIgnore, mvstruct))) {
      ReleaseFlags(mvstruct, false, [secondNew, firstNew, pos4, pos3, pos2, pos1], 5);
      return false;
    }
  }
  firstNew->marker = myPID;
  if (numAdditional == 2) {
    secondNew->marker = myPID;
  }


  if (numAdditional == 2) 
    ReleaseFlags(mvstruct, true, [secondNew], 1);
  ReleaseFlags(mvstruct, true, [firstnew, pos4, pos3], 3);
  if (numAdditional == 1) {
      ReleaseFlags(mvstruct, true, [pos2], 1);
  }
  return true;
}

bool ApplyMoveUpRule(Node* x, Node* w) {
  if (((w->marker == w->parent->marker) && (w->marker == w->right->marker) &&
    (w->marker != 0) && (w->left->marker != 0)) ||
    ((w->marker == w->right->marker) && (w->marker != 0) && (w->left->marker != 0)) ||
    ((w->marker == 0) && (w->left->marker != 0) && (w->right->marker != 0))) {
    return true;
  }
  else {
    false;
  }
}

bool SetupLocalAreaForInsert(Node* curr) {
  Node* parentNode = curr->parent;
  if (!parentNode->flag.compare_exchange_weak(false, true)) {
    return false;
  }
  if (parentNode != curr->parent) {
    parentNode->flag = false;
    return false;
  }
  Node* uncleNode = nullptr;
  if (curr == curr->parent->left) {
    uncleNode = curr->parent->right;
  }
  else {
    uncleNode = curr->parent->left;
  }
  if (!uncleNode->flag.compare_exchange_weak(false, true)) {
    curr->parent->flag = false;
    return false;
  }

  if (!GetFlagsAndMarkersAbove(curr->parent, curr)) {
    curr->parent->flag = uncleNode->flag = false;
    return false;
  }
  return true;
}

bool SetupLocalAreaForDelete(Node* successor, Node* deletedNode) {
  Node* curr;
  if (successor->left != nullptr) {
    curr = successor->left;
  }
  else {
    curr = successor->right;
  }

  if (!curr->flag.compare_exchange_weak(false, true)) {
    return false;
  }

  Node* succParent = successor->parent;
  if ((succParent != deletedNode) && (!succParent->flag.compare_exchange_weak(false, true))) {
    curr->flag = false;
    return false;
  }
  if (currParent != successor->parent) {
    curr->flag = false;
    return false;
  }

  Node* siblingNode;
  if (successor == successor->parent->left) {
    siblingNode = successor->parent->right;
  }
  else {
    siblingNode = successor->parent->left;
  }

  Node* leftNieceNode = siblingNode->left;
  Node* rightNieceNode = siblingNode->right;
  if (!siblingNode->flag.compare_exchange_weak(false, true)) {
    curr->flag = false;
    if (succParent != deletedNode) {
      succParent->flag = false;
    }
    return false;
  }

  if (siblingNode != nullptr) {
    if (!leftNieceNode->flag.compare_exchange_weak(false, true)) {
      curr->flag = siblingNode->flag = false;
      if (succParent != deletedNode) {
        succParent->flag = false;
      }
      return false;
    }
    if (!rightNieceNode->flag.compare_exchange_weak(false, true)) {
      curr->flag = siblingNode->flag = false;
      if (succParent != deletedNode) {
        succParent->flag = false;
      }
      return false;
    }
  }
  if (!GetFlagsAndMarkersAbove(succParent, deletedNode)) {
    curr->flag = siblingNode->flag = leftNieceNode->flag = leftNieceNode->flag = false;
    if (succParent != deletedNode) {
      succParent->flag = false;
    }
    return false;
  }
  return true;
}

Node* MoveInserterUp(Node* oldNode) {
  Node* oldParent = oldNode->parent;
  Node* oldGrandParent = oldParent->parent;
  Node* oldUncle = nullptr;
  if (oldParent == oldGrandParent->parent) {
    oldUncle = oldGrandParent->right;
  }
  else {
    oldUncle = oldGrandParent->left;
  }
  while (!GetFlagsAndMarkersAbove(oldgp, 2));
  Node* newNode = oldGrandParent;
  Node* newParent = newNode->parent;
  Node* newGrandParent = newParent->parent;
  Node* newUncle = nullptr;
  if (newParent == newGrandParent->left) {
    newUncle = newGrandParent->right;
  }
  else {
    newUncle = newGrandParent->left;
  }
  if (!IsIn(newUncle, mvstruct)) {
    while (!newUncle->flag.compare_exchange_weak(false, true));
  }
  ReleaseFlags(mvstruct, true, [oldNode, oldParent, oldUncle], 3);
  return newNode;
}

Node* MoveDeleterUp(Node* oldNode) {
  Node* oldParent = oldNode->parent;
  Node* oldSibling = nullptr;
  if (oldNode == oldParent->left) {
    oldSibling = oldParent->right;
  }
  else {
    oldSibling = oldParent->left;
  }
  Node* oldLeftNiece = oldSibling->left;
  Node* oldRightNiece = oldSibling->right;

  while (!GetFlagsAndMarkersAbove(oldGrandParent, 1));

  Node* newNode = oldNode;
  Node* newParent = newNode->parent;
  Node* newSibling = nullptr;
  if (newNode == newParent->left) {
    newSibling = newParent->right;
  }
  else {
    newSibling = newParent->left;
  }
  if (!IsIn(newSibling, mvstruct)) {
    while (!newSibling->flag.compare_exchange_weak(false, true));
  }

  Node* newLeftNiece = newSibling->left;
  Node* newRightNiece = newSibling->right;
  if (!IsIn(newLeftNiece, mvstruct)) {
    while (!newLeftNiece->flag.compare_exchange_weak(false, true));
  }
  if (!IsIn(newRightNiece, mvstruct)) {
    while (!newRightNiece->flag.compare_exchange_weak(false, true));
  }
  ReleaseFlags(mvstruct, true, [oldNode, oldSibling, oldLeftNiece, oldRightNiece], 4);
  return newNode;
}

Node* RedBlackTree::initializeTree() {
  Node* root = new Node(0);
  Node* dummy1 = new Node(0);
  Node* dummy2 = new Node(0);
  Node* dummy3 = new Node(0);
  Node* dummy4 = new Node(0);
  Node* dummy5 = new Node(0);
  Node* dummySibling = new Node(0);
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
  
  dummy1->left_child = dummy2;
  dummy2->left_child = dummy3;
  dummy3->left_child = dummy4;
  dummy4->left_child = dummy5;
  dummy5->left_child = root;
  root->right_child = dummySibling;

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
        newNode = MoveInserterUp(x);
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

//TODO: figure out restart and goto
void RedBlackTree::insertNode(const int& value) {
  //restart: prevSearcHNode = nullptr
  Node* prevSearchNode = nullptr;

  while (!root->flag.compare_exchange_weak(false, true));
  Node* newNode = new Node(value);
  Node* searchPtr = root;
  
  while (searchPtr != nullptr) {
    prevSearchNode = searchPtr;
    if (newNode->value < searchPtr->value) {
      searchPtr = searchPtr->left;
    }
    else {
      searchPtr = searchPtr->right;
    }
    if (!searchPtr->flag.compare_exchange_weak(false, true)) {
      prevSearchNode->flag = false;
      goto restart;
    }
    if (searchPtr != nullptr) {
      prevSearchNode->flag = false;
    }
  }
  newNode->flag = true;
  if (!SetupLocalAreaForInsert(z)) {
    prevSearchNode->flag = false;
    goto restart;
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
  bool done = false;
  bool didMoveUp = false;
  while (x != root && x->color == true) {
    if (x == x->parent->left) {
      Node* siblingNode = x->parent->right;
      if (siblingNode->color == true) {
        siblingNode->color = true;
        x->parent->color = false;
        rotateLeft(root, x->parent);
        siblingNode = x->parent->right;
        fixupDeleteCase1(x);
      }
      if (siblingNode->left->color == true && siblingNode->right->color == true) {
        siblingNode->color = red;
        x = moveDeleterUP(x);
      }
      else {
        if (siblingNode->right->color == true) {
          siblingNode->left->color = true;
          siblingNode->color = false;
          rotateRight(root, siblingNode);
          fixupDeleteCase3(x);
          siblingNode = x->parent->right;
        }
        siblingNode->color = x->parent->color;
        x->parent->color = true;
        siblingNode->right->color = true;
        rotateLeft(root, x - parent);
        didMoveUp = applyMoveUpRule(x, siblingNode);
        done = true;
      }
    }
    else {
      Node* siblingNode = x->parent->left;
      if (siblingNode->color == true) {
        siblingNode->color = true;
        x->parent->color = false;
        rotateRight(root, x->parent);
        siblingNode = x->parent->left;
        fixupDeleteCase1(x);
      }
      if (siblingNode->right->color == true && siblingNode->left > color == true) {
        siblingNode->color = red;
        x = moveDeleterUp(x);
      }
      else {
        if (siblingNode->left->color == true) {
          siblingNode->right->color = true;
          siblingNode->color = false;
          rotateLeft(root, siblingNode);
          fixupDeleteCase3(x);
          siblingNode = x->parent->left;
        }
        siblingNode->color = x->parent->color;
        x->parent->color = true;
        siblingNode->left->color = true;
        rotateRight(root, x - parent);
        didMoveUp = applyMoveUpRule(x, siblingNode);
        done = true;
      }
    }
  }
  if (!didMoveUP) {
   x->color = true;
   //release local area flags and fix relocated markers
  }
}

bool RedBlackTree::deleteNode(Node* v) {
  Node* u;
  if (v->left == nullptr || v->right == nullptr) {
    u = v;
  }
  else {
    u = Successor(v);
  }

  //Setting up local area
  if (!SetupLocalAreaForDelete(u, v)) {
    u->flag = false;
    if (u != v) {
      v->flag = false;
    }
    return false;
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
    v->flag = false;
  }
  if (u->color == true) {
    fixupDelete(siblingNode);
  }
  else {
    //release flags and marker held in local area
  }
  return true;
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
