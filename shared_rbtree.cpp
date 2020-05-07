#include "shared_rbtree.hpp"

using namespace std;

/*
TODOS for data structures:
  figure out if our RB tree wants keys and values or just values (either changed locked and CAS or change this)
  figure out how to put pNode and opInfo in a single word.
  figure out what opInfo's next location's pointer type should be
*/




/*
Tables Needed:
  Modify Table
  Search Table

*/


/*
TODO:
  Read value using Chuong et al's algorithm and return it
*/
int SharedRBTree::lookup(int key) {
  opRecord* operations = new opRecord(Search, key, -1);
  operations->state.status = InProgress;
  operations->state.position = nullptr;
  searchTable[std::this_thread::get_id()] = operations;
  traverse(operations);
  if (operations->state.position != nullptr) {
    //TODO read the value stored in teh record using Chuong et al's algorithm and return it
    return 0;
  }

  return -1;
}

/*
 TODO:
  Change pid to the process selected using round robin (need to implement round robin)
    store previous picked and add 1?
  Update value using Chuong et al's algorithm
*/
int SharedRBTree::insert(int key, int value) {
  void* valData = nullptr;
  lookup(key);
  valData = searchTable[std::this_thread::get_id()]->state.position;
  if (valData == nullptr) {
    //TODO int pid = 0; // the process selected to help in RR
    opRecord* opData = new opRecord(Insert, key, value);
    executeOperation(opData);
    valData = opData->state.position;
    if (opData != nullptr) {
      traverse(opData);
    }
  }
  if (valData != nullptr) {
    //phase 3 update value using Chung's alg
  }

  //TODO
  return -1;
}

/*
TODO:
  Change pid to the process selected using round robin
  Figure out how to delete using MTL-framework
*/
int SharedRBTree::remove(int key) {
  int ret;
  //phase 1: determine if the key already exists
  if ((ret = lookup(key)) != -1) {
    //phase 2: delete using MTL-framework

    //select a search operation to help at the end of phase2
    //TODO int pid = 0; //select process using RR
    opRecord* opData = new opRecord(Delete, key, -1);
    executeOperation(opData);
    if (opData != nullptr) {
      traverse(opData);
    }
  }

  //TODO
  return ret;
}

/*
  should be fine
*/
void SharedRBTree::traverse(opRecord* opData) {
  dNode* dCurrent = root->dataNode;
  vRecord* valData;
  while (dCurrent != nullptr) {
    if (opData->state.status == Completed) {
      return;
    }
    if (opData->key < dCurrent->key) {
      dCurrent = dCurrent->left->dataNode;
    }
    else {
      dCurrent = dCurrent->right->dataNode;
    }
  }

  if ((dCurrent->key = opData->key)) {
    valData = dCurrent->valData;
  } else {
    valData = nullptr;
  }
  opData->state.status = Completed;
  opData->state.position = valData;
}

/*
TODO:
  select pid in RR
  also figure out root (use RB TREE STRUCT?)
*/
void SharedRBTree::executeOperation(opRecord* opData) {
  opData->state.status = Waiting;
  opData->state.position = root;
  modifyTable[std::this_thread::get_id()] = opData;

  //select a modify operation to help later
  //TODO int pid = 0; //select in round robin manner

  injectOperation(opData);
  int status = opData->state.status;
  pNode* pCurrent = (pNode*) opData->state.position;

  while (status != Completed) {
    dNode* dCurrent = pCurrent->dataNode;
    if (dCurrent->opData == opData) {
      executeWindowTransaction(pCurrent, dCurrent);
    }
    status = opData->state.status;
    pCurrent = (pNode*) opData->state.position;
  }

  if (opData != nullptr) {
    injectOperation(opData);
  }
}

/*
  figure out root
*/
void SharedRBTree::injectOperation(opRecord* opData) {
  while (opData->state.status == Waiting) {
    dNode* dRoot = root->dataNode;
    if (dRoot->opData!= nullptr) {
      executeWindowTransaction(root, dRoot);
    }
    //int status = opData->state.status;
    //pNode* pCurrent = (pNode*) opData->state.position;
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
void SharedRBTree::executeWindowTransaction(pNode* pointNode, dNode* dataNode) {
  opRecord* opData = dataNode->opData;
  int flag = pointNode->flag;
  dNode* dCurrent = pointNode->dataNode;
  if (dCurrent->opData) {
    if (flag == OWNED) {
      if (pointNode == root) {
        //the operation may have just been injected but op state may not have been updated yet
        //CAS switch
      }

      //TODO
      /*
      if (!exchangeCheapWindowTransaction(pointNode, dCurrent)) {
        dNode* windowSoFar(dCurrent);
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
        //TODO CAS(pNode, (OWNED < dCurrent), (FREE, dWindowRoot));
      }
    */
    }
    dNode* dNow = pointNode->dataNode;
    if (dNow->opData == opData) {
      //TODO CAS(opData->state, (InProgress, pNode), dNow->next);
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
bool SharedRBTree::executeCheapWindowTransaction(pNode* pointNode, dNode* dataNode) {
  opRecord* opData = dataNode->opData;
  std::thread::id pid = opData->pid;
  while(1) {
  //TODO
  //while (traversal not complete) {
  //  pNode* pNextToVisit = address of next pointer node to be visited;
    pNode* pNextToVisit;
    dNode* dNextToVisit = pNextToVisit->dataNode;
    if (opData->state.position != pointNode) {
      return true;
    }
    if (dNextToVisit->opData != nullptr) {
      if (dNextToVisit->opData->pid != pid) {
        executeWindowTransaction(pNextToVisit, dNextToVisit);
        dNextToVisit = pNextToVisit->dataNode;
        if (opData->state.position != pointNode) {
          return true;
        }
      }
      else if (dNextToVisit->opData == dataNode->opData) {
        if (opData->state.position == pointNode) {
          slideWindowDown(pointNode, dataNode, pNextToVisit, dNextToVisit);
        }
        return true;
      }
      else if (modifyTable[pid] != opData) {
        return true;
      }
      //no idea what this means
      //visit dNextToVisit;
    }
  }
  /** TODO
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
  */
    return false;
}

/*
  TODO:
    find out clone
    fix CAS 
    figure out how to put pNodes in one word
    figure out how to put state in one word
*/
void SharedRBTree::slideWindowDown(pNode* pMoveFrom, dNode* dMoveFrom, pNode* pMoveTo, dNode* dMoveTo) {
  opRecord* opData = dMoveFrom->opData;
  //copy the data node of the current window location
  dNode* dCopyMoveFrom(dMoveFrom);
  dCopyMoveFrom->opData = opData;
  if (dMoveTo != nullptr) {
    dCopyMoveFrom->next->status = InProgress;
    dCopyMoveFrom->next->nextLocation = pMoveTo;
  }
  else {
    dCopyMoveFrom->next->status = Completed;
    dCopyMoveFrom->next->nextLocation = pMoveTo;
  }
  if (dMoveTo != nullptr) {
    if (dMoveTo->opData != opData) {
      dNode* dCopyMoveTo(dMoveTo);
      dCopyMoveTo->opData = opData;
      //TODO CAS(pMoveTo, (FREE, dMoveTo), (OWNED, dCopyMoveTo));
    }
  }
  //TODO CAS(pMoveFrom, (Owned, dMoveFrom), (FREE, dCopyMoveFrom));
  //TODO CAS(opData->state, (InProgress, pMoveFrom), (dCopyMoveFrom->next));
}
