#ifndef RBTREE_H
#define RBTREE_H

// Function signatures for red black tree implementations

/**
 *  Initializes a red-black tree. Returns 0 on success, a negative integer on
 *  failure.
 */
typedef int (*rb_init_t)(void* rbtree);

/**
 *  Destroys a red-black tree.
 */
typedef void (*rb_destroy_t)(void* rbtree);

/**
 *  Inserts a value at a specific key in the tree. If a value is already
 *  present, the function replaces and returns the old value.
 */
typedef void* (*rb_insert_t)(void* rbtree, int key, void* val);

/**
 *  Removes and returns a value at a specific key in the tree.
 */
typedef void* (*rb_remove_t)(void* rbtree, int key);

/**
 *  Returns the value at a specific key in the tree without removing it.
 */
typedef void* (*rb_lookup_t)(void* rbtree, int key);

#endif // RBTREE_H
