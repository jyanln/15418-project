#ifndef SHARED_RBTREE_HPP
#define SHARED_RBTREE_HPP

#include "rbtree.hpp"
#include <atomic>
#include <vector>
#include <map>
#include <thread>

using namespace std;

#define FREE true
#define OWNED false

class SharedRBTree : public RBTree
{
    public:
        struct opInfo {
            short status;
            void* nextLocation;
        };

        struct vRecord
        {
            int value;
            //variables used by Chuong ed tal's wait free alg
        };

        enum Status {
            Waiting,
            InProgress,
            Completed
        };

        enum opType {
            Search,
            Delete,
            Insert
        };

        struct State {
            Status status;
            void* position;
        };
        
        struct opRecord
        {
            opType type;
            int key;
            int value;
            std::thread::id pid;
            State state;

            opRecord(opType type, int key, int value) {
                this->type = type;
                this->key = key;
                this->value = value;
            }
        };

        // Predeclaration to resolve circular structs
        struct dNode;

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
            pNode* left, *right;
            opInfo* next;

            dNode(int key, int value) {
                this->key = value;
                valData = nullptr;
                opData = nullptr;
                left = right = nullptr;
                color = false;
            }
        };

    private:
        pNode* root;
        std::map<std::thread::id, opRecord*> modifyTable;
        std::map<std::thread::id, opRecord*> searchTable;

    protected:
        void traverse(opRecord* opData);
        void executeOperation(opRecord* opData);
        void injectOperation(opRecord* opData);
        void executeWindowTransaction(pNode* pointNode, dNode* dataNode);
        bool executeCheapWindowTransaction(pNode* pointNode, dNode* dataNode);
        void slideWindowDown(pNode* pMoveFrom, dNode* dMoveFrom, pNode* pMoveTo, dNode* dMoveTo);

    public:
        //TODO default nthreads value
        SharedRBTree(int nthreads = 8) {
            root = nullptr;
        }

        ~SharedRBTree() {}

        const std::string name() { return "Shared RBTree"; };

        int insert(int key, int val);
        int remove(int key);
        int lookup(int key);
};

#endif
