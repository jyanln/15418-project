#ifndef RBTREE_HPP
#define RBTREE_HPP

#include <string>

// Function signatures for red black tree implementations
template <typename T>
class RBTree {
    public:
        RBTree() {}

        virtual ~RBTree() {}

        virtual const std::string name() = 0;

        /**
         *  Inserts a value at a specific key in the tree. If a value is already
         *  present, the function replaces and returns the old value.
         */
        virtual T insert(int key, T val) = 0;

        /**
         *  Removes and returns a value at a specific key in the tree.
         */
        virtual T remove(int key) = 0;

        /**
         *  Returns the value at a specific key in the tree without removing it.
         */
        virtual T lookup(int key) = 0;
};

//TODO
#define NUM_RB_IMPL 1

#endif // RBTREE_HPP
