#ifndef RBTREE_H
#define RBTREE_H

// Function signatures for red black tree implementations

// List of implementations
#define NUM_RB_IMPL 1
char* rb_names[] = {"TODO"};
/**
 *  Initializes a red-black tree. Returns 0 on success, a negative integer on
 *  failure.
 */
int (*rb_inits[])() = {NULL};

/**
 *  Destroys a red-black tree.
 */
void (*rb_destroys[])() = {NULL};

/**
 *  Inserts a value at a specific key in the tree. If a value is already
 *  present, the function replaces and returns the old value.
 */
int (*rb_inserts[])() = {NULL};

/**
 *  Removes and returns a value at a specific key in the tree.
 */
int (*rb_removes[])() = {NULL};

/**
 *  Returns the value at a specific key in the tree without removing it.
 */
int (*rb_lookups[])() = {NULL};

#endif // RBTREE_H
