
using namespace std;

/*
TODOS for data structures:
  figure out if our RB tree wants keys and values or just values (either changed locked and CAS or change this)
  figure out how to put pNode and opInfo in a single word.
  figure out what opInfo's next location's pointer type should be
*/

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
  pNode* left, * right;
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

opRecord** modifyTable = calloc(nthreads, sizeof(opRecord*));
opRecord** searchTable = calloc(nthreads, sizeof(opRecord*));

class RedBlackTree
{
private:
  pNode* root;
protected:
  void executeOperation(opRecord* opData);
  void injectOperation(opRecord* opData);
  void executeWindowTransaction(pNode* pointNode, dNode* dataNode);
  void executeCheapWindowTransaction(pNode* pointNode, dNode* dataNode);
  void slideWindowDown(pNode* pMoveFrom, dNode* dMoveFrom, pNode* pMoveTo, dNode* dMoveTo);

public:
  //Constructor
  RedBlackTree() {
    root = nullptr;
  }
  int search(int key);
  int insertOrUpdate(int key, int value);
  int deleteNode(int key);
  void traverse(opRecord* opData);
};

/*
TODO:
  Change myid to current pid
  Read value using Chuong et al's algorithm and return it
*/
int RedBlackTree::search(int key) {
  //TODO: figure out what myid is
  int myid = 0;
  opRecord* operations = new opRecord(SEARCH, key, -1);
  operations->state->status = IN_PROGRESS;
  operations->state->position = nullptr;
  searchTable[myid] = operations;
  traverse(operations);
  if (opData->state->position != nullptr) {
    //read the value stored in teh record using Chuong et al's algorithm and return it

  }
}

/*
 TODO:
  Change pid to the process selected using round robin (need to implement round robin)
    store previous picked and add 1?
  Update value using Chuong et al's algorithm
*/
void RedBlackTree::insertOrUpdate(int key, int value) {
  int* valData = nullptr;
  search(key);
  valData = searchTable[myid]->state->position;
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

/*
TODO:
  Change pid to the process selected using round robin
  Figure out how to delete using MTL-framework
*/
void RedBlackTree::deleteNode(int key) {
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

/*
  should be fine
*/
void RedBlackTree::traverse(opRecord* opData) {
  dNode* dCurrent = root->dataNode;
  while (dCurrent != null) {
    if (opData->state->status == COMPLETED) {
      return;
    }
    if (opData->key < dCurrent->key) {
      dCurrent = dCurrent->left->dataNode;
    }
    else {
      dCurrent = dCurrent->right->dataNode;
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

/*
TODO:
  select pid in RR
  also figure out root (use RB TREE STRUCT?)
*/
void RedBlackTree::executeOperation(opRecord* opData) {
  opData->state->status = WAITING;
  opData->state->position = root;
  modifyTable[myid] = opData;

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

/*
  figure out root
*/
void RedBlackTree::injectOperation(opRecord* opData) {
  while (opData->state->status == WAITING) {
    dNode* dRoot = root->dataNode;
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

/*
TODO:
  a lot of pseudo code in this point
  figure out when there are more nodes to add (373)
  figure out the address of the enxt node to be copied
  figure out address of data now acting as window
  figure out when it is the last window transaction (when we have reached leaf node)
  figure out when it is an update operation 
  figure out what addres it will move to
*/
void RedBlackTree::executeWindowTransaction(pNode* pointNode, dNode* dataNode) {
  opRecord* opData = dataNode->opData;
  int flag = pointNode->flag;
  dNode* dCurrent = pointNode->dataNode;
  if (dCurrent->opData) {
    if (flag == OWNED) {
      if (pointNode == root) {
        //the operation may have just been injected but op state may not have been updated yet
        //CAS switch
      }
      if (!exchangeCheapWindowTransaction(pointNode, dCurrent)) {
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
          pNode* pMoveTo;
          if (update operation) {
            pMoveTo->dataNode = address of record containing update value;
          }
          else {
            pMoveTo->dataNode = nullptr;
          }
        }
        else {
          status = IN_PROGRESS;
          pNode* pMoveTo = address of pointer in windowSoFar that op will move;
          pMoveTo->flag = OWNED;
          dNode* dMoveTo = pMoveTo->dNode;
          dMoveTo->opData = opData;
        }
        dWindowRoot->opData = opData;
        dWindowRoot->next->status = status;
        dWindowRoot->next->nextLocation = pMoveTo;
        //double compare and swap here
        CAS(pNode, (OWNED < dCurrent), (FREE, dWindowRoot));
      }
    }
    dNode* dNow = pNode->dNode;
    if (dNow->opData == opData) {
      CAS(opData->state, (IN_PROGRESS, pNode), dNow->next);
    }
  }
}

/*
TODO:
  a lot of pseudo code here too
  figure out when traversal not complete
  figure out address of next pointer node
  figure out how to visit dNextToVisit
  figure out when no transformation is needed
    or last terminal window transaction

*/
bool RedBlackTree::executeCheapWindowTransaction(pNode* pointNode, dNode* dataNode) {
  opRecord* opData = dataNode->opData;
  int pid = opData->pid;
  while (traversal not complete) {
    pNode* pNextToVisit = address of next pointer node to be visited;
    dNode* dNextToVisit = pNextToVisit->dNode;
    if (opData->status->postion != pointNode) {
      return True;
    }
    if (dNextToVisit->opData != nullptr) {
      if (dNextToVisit->opData->pid != pid) {
        executeWindowTransaction(pNextToVisit, dNextToVisit);
        dNextToVisit = pNextToVisit->dNode;
        if (opData->state->position != pointNode) {
          return true;
        }
      }
      else if (dNextToVisit->opData == dataNode->opData) {
        if (opData->state->position == pointNode) {
          slideWindowDown(pointNode, dataNode, pNextToVisit, dNextToVisit);
        }
        return true;
      }
      else if (modifyTable[pid] != opData) {
        return true;
      }
      //no idea what this mans
      visit dNextToVisit;
    }
  }
  if (no transofrmation needed) {
    pNode* pMoveTo = nullptr;
    dNode* dMoveTo = nullptr;
    if (last terminal window transaction) {
      if (update operation) {
        pMoveTo->dataNode = address of record containing update value;
      }
      else {
        pMoveTo->dataNode = nullptr;
      }
      dMoveTo = nullptr;
    }
    else {
      pMoveTo = address of the pointer we are moving to;
      dMoveTo = pMoveTo->dNode;
    }
    if (opData->state->position == pNode) {
      slideWindowDown(pNode, dNode, pMoveTo, dMoveTo);
    }
    return true;
  }
  else {
    return false;
  }
}

/*
  TODO:
    find out clone
    fix CAS 
    figure out how to put pNodes in one word
    figure out how to put state in one word
*/
void RedBlackTree::slideWindowDown(pNode* pMoveFrom, dNode* dMoveFrom, pNode* pMoveTo, dNode* dMoveTo) {
  opRecord* opData = dMoveFrom->opData;
  //copy the data node of the current window location
  dNode* dCopyMoveFrom = clone(dMoveFrom);
  dCopyMoveFrom->opData = opData;
  if (dMoveTo !+ nullptr) {
    dCopyMoveFrom->next->status = IN_PROGRESS;
    dCopyMoveFrom->next->nextLocation = pMoveTo;
  }
  else {
    dCopyMoveFrom->next->status = COMPLETED;
    dCopyMoveFrom->next->nextLocation = pMoveTo;
  }
  if (dMoveTo != nullptr) {
    if (dMoveTo->opData != opData) {
      dNode* dCopyMoveTo = clone(dMoveTo);
      dCopyMoveTo->opData = opData;
      CAS(pMoveTo, (FREE, dMoveTo), (OWNED, dCopyMoveTo));
    }
  }
  CAS(pMoveFrom, (OWNED, dMoveFrom), (FREE, dCopyMoveFrom));
  CAS(opData->state, (IN_PROGRESS, pMoveFrom), (dCopyMoveFrom->next));
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
