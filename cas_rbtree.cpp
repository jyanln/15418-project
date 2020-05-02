#include "cas_rbtree.hpp"
#include "rbtree.hpp"
#include <atomic>

using namespace std;

//This code is adapted from Kim's Algorithm

//LockFree CAS Helper functions
//TODO: Figure out what moveupStruct is
//      Fix ReleaseFlags structure
//      Check correctness
//      Make leaf nodes black NIL nodes, not null ptr

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

4) Write IsIn and IsGoalNode and FixupCase1 and FixupCase3


*/

//helper functions

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

bool IsIn(Node* curr, MoveUpStruct* mvstruct) {
  for (int i = 0; i < mvstruct->nodeList.size(); i++) {
    if (curr == mvstruct->nodeList[i]) {
      return true;
    }
  }
  return false;
}

bool IsGoalNode(Node* curr, MoveUpStruct* mvstruct) {
  return (curr == mvstruct->goalNode);
}

//figure out how to release unneeded flags
void ReleaseFlags(MoveUpStruct* mvstruct, bool success, Node** nodesToRelease, int numNodes) {
  for (int i = 0; i < numNodes, i++) {
    if (success) {
      if (!IsIn(nodesToRelease[i], mvstruct)) {
        nodesToRelease[i]->flag = false;
      }
      else {
        if (IsGoalNode(nd, mvstruct)) {
          //release unneeded flags in mvstruct and discard mvstruct
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

//figure out where mvstruct comes from
bool SpacingRuleIsSatisfied(Node* curr, Node* start, int PIDtoIgnore) {
  if (curr != start) {
    if (curr->marker != 0) {
      return false;
    }
  }
  Node* parentNode = curr->parent;
  if (parentNode != start) {
    bool expected = false;
    if ((!IsIn(parentNode, mvstruct)) && (!parentNode->flag.compare_exchange_weak(expected, true))) {
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
  Node* siblingNode = nullptr;
  if (curr == parentNode->left) {
    siblingNode = parentNode->right;
  }
  else {
    siblingNode = parentNode->left;
  }
  bool expected = false;
  if ((!IsIn(siblingNode, mvstruct)) && (!siblingNode->flag.compare_exchange_weak(expected, true))) {
    if (parentNode != curr) {
      Node* releaseList[1] = { parentNode };
      ReleaseFlags(mvstruct, false, releaseList, 1);
    }
    return false;
  }
  if ((siblingNode->marker != 0) && (siblingNode->marker != PIDtoIgnore)){
    Node* releaseList[1] = { siblingNode };
    ReleaseFlags(mvstruct, false, releaseList, 1);
    if (parentNode != curr) {
      Node* releaseList[1] = { parentNode };
      ReleaseFlags(mvstruct, false, releaseList, 1);
    }
    return false;
  }
  if (parentNode != curr) {
    Node* releaseList[1] = { parentNode };
    ReleaseFlags(mvstruct, false, releaseList, 1);
  }
  Node* releaseList[1] = { siblingNode };
  ReleaseFlags(mvstruct, false, releaseList, 1);
  return true;
}

//looks good
bool GetFlagsForMarkers(Node* start, MoveUpStruct* mvstruct, Node* pos1, Node* pos2, Node* pos3, Node* pos4) {
  pos1 = start->parent;
  bool expected = false;
  if ((!IsIn(pos1, mvstruct) && (!pos1->flag.compare_exchange_weak(expected, true)))) {
    return false;
  }
  if (pos1 != start->parent) {
    Node* releaseList[1] = { pos1 };
    ReleaseFlags(mvstruct, false, releaseList, 1);
    return false;
  }
  pos2 = pos1->parent;
  if ((!IsIn(pos2, mvstruct) && (!pos2->flag.compare_exchange_weak(expected, true)))) {
    pos1->flag = false;
    return false;
  }
  if (pos2 != pos1->parent) {
    Node* releaseList[2] = { pos2, pos1 };
    ReleaseFlags(mvstruct, false, releaseList, 2);
    return false;
  }
  pos3 = pos2->parent;
  if ((!IsIn(pos3, mvstruct) && (!pos1->flag.compare_exchange_weak(expected, true)))) {
    pos2->flag = pos1->flag = false;
    return false;
  }
  if (pos3 != pos2->parent) {
    Node* releaseList[3] = { pos3, pos2, pos1 };
    ReleaseFlags(mvstruct, false, releaseList, 3);
    return false;
  }
  pos4 = pos3->parent;
  if ((!IsIn(pos4, mvstruct) && (!pos4->flag.compare_exchange_weak(expected, true)))) {
    pos3->flag = pos2->flag = pos1->flag = false;
    return false;
  }
  if (pos4 != pos3->parent) {
    Node* releaseList[4] = { pos4, pos3, pos2, pos1 };
    ReleaseFlags(mvstruct, false, releaseList, 4);
    return false;
  }
  return true;
}

//mv struct is not passed in but is used. global? or add to RB?
//figure out what pos1-4 is
//figure out what PIDtoIgnore is and myPID
bool GetFlagsAndMarkersAbove(Node* startNode, int numAdditional) {
  if (!GetFlagsForMarkers(startNode, mvstruct, pos1, pos2, pos3, pos4)) {
    return false;
  }
  Node* firstNew = pos4->parent;
  bool expected = false;
  if (!IsIn(firstNew, mvstruct) && (!firstNew->flag.compare_exchange_weak(expected, true))) {
    Node* releaseList[4] = { pos4, pos3, pos2, pos1 };
    ReleaseFlags(mvstruct, false, releaseList, 4);
    return false;
  }
  if ((firstNew != pos4->parent) && 
    (!SpacingRuleIsSatisfied(firstNew, startNode, PIDtoIgnore, mvstruct))){
    Node* releaseList[5] = { firstNew, pos4, pos3, pos2, pos1 };
    ReleaseFlags(mvstruct, false, releaseList, 5);
    return false;
  }

  Node* secondNew = nullptr;
  if (numAdditional == 2) {
    secondNew = firstNew->parent;
    if ((!IsIn(secondNew, mvstruct) && !secondNew->flag.compare_exchange_weak(expected, true))) {
      Node* releaseList[5] = { firstNew, pos4, pos3, pos2, pos1 };
      ReleaseFlags(mvstruct, false, releaseList, 5);
      return false;
    }
    if ((secondNew != firstNew->parent) &&
      (!SpacingRuleIsSatisfied(secondNew, startNode, PIDtoIgnore, mvstruct))) {
      Node* releaseList[6] = { secondNew, firstNew, pos4, pos3, pos2, pos1 };
      ReleaseFlags(mvstruct, false, releaseList, 6);
      return false;
    }
  }
  firstNew->marker = myPID;
  if (numAdditional == 2) {
    secondNew->marker = myPID;
  }

  if (numAdditional == 2) {
    Node* releaseList[1] = { secondNew };
    ReleaseFlags(mvstruct, true, [secondNew], 1);
  }
  Node* releaseList[3] = { firstNew, pos4, pos3 };
  ReleaseFlags(mvstruct, true, releaseList, 3);
  if (numAdditional == 1) {
    Node* releaseList[1] = { pos2 };
    ReleaseFlags(mvstruct, true, releaseList, 1);
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

//looks good
bool SetupLocalAreaForInsert(Node* curr) {
  Node* parentNode = curr->parent;
  bool expected = false;
  if (!parentNode->flag.compare_exchange_weak(expected, true)) {
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
  if (!uncleNode->flag.compare_exchange_weak(expected, true)) {
    curr->parent->flag = false;
    return false;
  }

  if (!GetFlagsAndMarkersAbove(curr->parent, curr)) {
    curr->parent->flag = uncleNode->flag = false;
    return false;
  }
  return true;
}

//looks good
bool SetupLocalAreaForDelete(Node* successor, Node* deletedNode) {
  Node* curr;
  if (successor->left->key != -1) {
    curr = successor->left;
  }
  else {
    curr = successor->right;
  }

  bool expected = false;
  if (!curr->flag.compare_exchange_weak(expected, true)) {
    return false;
  }

  Node* succParent = successor->parent;
  if ((succParent != deletedNode) && (!succParent->flag.compare_exchange_weak(expected, true))) {
    curr->flag = false;
    return false;
  }
  if (succParent != successor->parent) {
    curr->flag = false;
    if (succParent != deletedNode) {
      succParent->flag = false;
    }
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
  if (!siblingNode->flag.compare_exchange_weak(expected, true)) {
    curr->flag = false;
    if (succParent != deletedNode) {
      succParent->flag = false;
    }
    return false;
  }

  if (siblingNode->key != -1) {
    if (!leftNieceNode->flag.compare_exchange_weak(expected, true)) {
      curr->flag = siblingNode->flag = false;
      if (succParent != deletedNode) {
        succParent->flag = false;
      }
      return false;
    }
    if (!rightNieceNode->flag.compare_exchange_weak(expected, true)) {
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

// needs mvstruct
Node* MoveInserterUp(Node* oldNode) {
  Node* oldParent = oldNode->parent;
  Node* oldGrandParent = oldParent->parent;
  Node* oldUncle = nullptr;
  if (oldParent == oldGrandParent->left) {
    oldUncle = oldGrandParent->right;
  }
  else {
    oldUncle = oldGrandParent->left;
  }
  //extend intention markers from oldgp to top and 2 more
  while (!GetFlagsAndMarkersAbove(oldGrandParent, 2));
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
    bool expected = false;
    while (!newUncle->flag.compare_exchange_weak(expected, true));
  }
  Node* releaseList[3] = { oldNode, oldParent, oldUncle };
  ReleaseFlags(mvstruct, true, releaseList, 3);
  return newNode;
}

// needs mvstruct
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
  Node* oldGrandParent = oldParent->parent;
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
    bool expected = false;
    while (!newSibling->flag.compare_exchange_weak(expected, true));
  }

  Node* newLeftNiece = newSibling->left;
  Node* newRightNiece = newSibling->right;
  if (!IsIn(newLeftNiece, mvstruct)) {
    bool expected = false;
    while (!newLeftNiece->flag.compare_exchange_weak(expected, true));
  }
  if (!IsIn(newRightNiece, mvstruct)) {
    bool expected = false;
    while (!newRightNiece->flag.compare_exchange_weak(expected, true));
  }
  Node* releaseList[4] = { oldNode, oldSibling, oldLeftNiece, oldRightNiece };
  ReleaseFlags(mvstruct, true, releaseList, 4);
  return newNode;
}

void fixupDeleteCase1(Node* x) {

}

void fixupDeleteCase3(Node* x) {

}
void fixupDeleteCase1_sym(Node* x) {

}

void fixupDeleteCase3_sym(Node* x) {

}

//private functions

void CASRBTree::rotateLeft(Node*& root, Node*& curr) {
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

void CASRBTree::rotateRight(Node*& root, Node*& curr) {
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

//looks good
void CASRBTree::fixupInsert(Node*& root, Node*& newNode) {
  Node* parentNode = nullptr;
  Node* grandParentNode = nullptr;

  while (newNode->parent->color == false) {
    parentNode = newNode->parent;
    grandParentNode = parentNode->parent;

    if (parentNode == grandParentNode->left) {
      Node* uncleNode = grandParentNode->right;

      //If uncle is red, then recolor
      if (uncleNode != nullptr && uncleNode->color == false) {
        grandParentNode->color = false;
        parentNode->color = true;
        uncleNode->color = true;
        newNode = MoveInserterUp(newNode);
      }
      //otherwise, rotation neceassary
      else {
        if (newNode == parentNode->right) {
          newNode = newNode->parent;
          rotateLeft(root, newNode);
        }
        parentNode->color = true;
        grandParentNode->color = false;
        rotateRight(root, grandParentNode);
      }
    }
    else {
      Node* uncleNode = grandParentNode->left;

      //If uncle is red, then recolor
      if (uncleNode != nullptr && uncleNode->color == false) {
        grandParentNode->color = false;
        parentNode->color = true;
        uncleNode->color = true;
        newNode = MoveInserterUp(newNode);
      }
      //otherwise, rotation is necesary
      else {
        if (newNode == parentNode->left) {
          newNode = newNode->parent;
          rotateRight(root, newNode);
        }
        parentNode->color = true;
        grandParentNode->color = false;
        rotateLeft(root, grandParentNode);
      }
    }
  }
  root->color = true;
}

//figure out how to release local areas and fix relocated markers
void CASRBTree::fixupDelete(Node* x) {
  bool done = false;
  bool didMoveUp = false;
  while (x != root && x->color == true && !done) {
    if (x == x->parent->left) {
      Node* siblingNode = x->parent->right;
      if (siblingNode->color == false) {
        siblingNode->color = true;
        x->parent->color = false;
        rotateLeft(root, x->parent);
        siblingNode = x->parent->right;
        fixupDeleteCase1(x, siblingNode);
      }
      if (siblingNode->left->color == true && siblingNode->right->color == true) {
        siblingNode->color = red;
        x = MoveDeleterUp(x);
      }
      else {
        if (siblingNode->right->color == true) {
          siblingNode->left->color = true;
          siblingNode->color = false;
          rotateRight(root, siblingNode);
          fixupDeleteCase3(x, siblingNode);
          siblingNode = x->parent->right;
        }
        siblingNode->color = x->parent->color;
        x->parent->color = true;
        siblingNode->right->color = true;
        rotateLeft(root, x - parent);
        didMoveUp = ApplyMoveUpRule(x, siblingNode);
        done = true;
      }
    }
    else {
      Node* siblingNode = x->parent->left;
      if (siblingNode->color == false) {
        siblingNode->color = true;
        x->parent->color = false;
        rotateRight(root, x->parent);
        siblingNode = x->parent->left;
        fixupDeleteCase1_sym(x, siblingNode);
      }
      if (siblingNode->right->color == true && siblingNode->left->color == true) {
        siblingNode->color = red;
        x = MoveDeleterUp(x);
      }
      else {
        if (siblingNode->left->color == true) {
          siblingNode->right->color = true;
          siblingNode->color = false;
          rotateLeft(root, siblingNode);
          fixupDeleteCase3_sym(x, siblingNode);
          siblingNode = x->parent->left;
        }
        siblingNode->color = x->parent->color;
        x->parent->color = true;
        siblingNode->left->color = true;
        rotateRight(root, x - parent);
        didMoveUp = ApplyMoveUpRule(x, siblingNode);
        done = true;
      }
    }
  }
  if (!didMoveUp) {
   x->color = true;
   //release local area flags and fix relocated markers
  }
}

//figure out how to release local area
bool CASRBTree::deleteNode(Node* v) {
  Node* u;
  if (v->left->key == -1 || v->right->key == -1) {
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
  if (u->left->key != -1) {
    siblingNode = u->left;
  }
  else {
    siblingNode = u->right;
  }
  siblingNode->parent = u->parent;
  if (u->parent == root->parent) {
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
    v->key = u->key;
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

//need to update
Node* CASRBTree::initializeTree() {
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

Node* CASRBTree::search(int key) {
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

//public functions

//fix NIL nodes
void CASRBTree::insert(int key, int value) {
  restart:
    Node* prevSearchNode = root->parent;
    Node* searchPtr = root;
    bool expected = false;
    while (!root->flag.compare_exchange_weak(expected, true));
    Node* newNode = new Node(key, value);
    
    while (searchPtr->key != -1) {
      prevSearchNode = searchPtr;
      if (newNode->key < searchPtr->key) {
        searchPtr = searchPtr->left;
      }
      else {
        searchPtr = searchPtr->right;
      }
      if (!searchPtr->flag.compare_exchange_weak(expected, true)) {
        prevSearchNode->flag = false;
        goto restart;
      }
      if (searchPtr->key != -1) {
        prevSearchNode->flag = false;
      }
    }

    newNode->flag = true;
    if (!SetupLocalAreaForInsert(prevSearchNode)) {
      prevSearchNode->flag = false;
      goto restart;
    }

    newNode->parent = prevSearchNode;
    if (prevSearchNode == root->parent) {
      root = newNode;
      root->parent->left = newNode;
    }
    else if (newNode->key < searchPtr->key) {
      prevSearchNode->left = newNode;
    }
    else {
      prevSearchNode->right = newNode;
    }
    newNode->left = new Node();
    newNode->right = new Node();
    newNode->color = false;
    fixupInsert(root, newNode);
}

// utility function that deletes the node with given value 
int CASRBTree::remove(int key) {
  if (root == NULL)
    // Tree is empty 
    return -1;

  Node* v = search(n);

  if (v->key != key) {
    return -1;
  }
  int ret = v->val;
  deleteNode(v);

  return ret;
}

int CASRBTree::lookup(int key) {
  Node* v = search(key);

  if (v && v->key == key) {
    return v->val;
  }
  else {
    return -1;
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
