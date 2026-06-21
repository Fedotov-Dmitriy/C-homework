#pragma once

#include <stdbool.h>
#include <stdio.h>

/*
 * AVL-tree structure. It stores key-value data.
 * Key and values can be arbitrary types, so it must take Comparator as initializer value.
 * If you use some one type, while comparator use the other, the behaviour is undefined.
 */
typedef struct AVLTree AVLTree;

/*
 * Compare function that is used in AVLTree for arbitrary types.
 * It compares keys and must return 0 if they are equal,
 * return value > 0 if the first argument is greater than the second,
 * return value < 0 if the second argument is greater than the first.
 */
typedef int (*Comparator)(void*, void*);

/*
 * Function that will be called in order to free key memory,
 * when deleting in AVLTree.
 */
typedef void (*KeyCleaner)(void*);

/*
 * Function that will be called in order to free value memory,
 * when deleting in AVLTree.
 */
typedef void (*ValueCleaner)(void*);

/*
 * This function is need for printing data of AVL-tree.
 * The function accepts three arguments:
 * the file descriptor, the key and value respectively.
 * The function can return an 'int' value, which is interpreted
 * as error when non equals 0.
 */
typedef int (*PrintKeyValue)(FILE*, void*, void*);

/*
 * Initialize AVL-tree. After initializing it will
 * take types that defined in comparator.
 * User may define KeyCleaner to free memory of key after
 * deleting a node in AVLTree. If nothing is need, KeyCleaner is set to NULL.
 * User may define ValueCleaner to free memory of value after
 * deleting a node in AVLTree. If nothing is need, ValueCleaner is set to NULL.
 * If key == value then user must specify only one 'cleaner' to prevent double-free.
 */
AVLTree* avlAlloc(Comparator comp, KeyCleaner keyFree, ValueCleaner valueFree);

/*
 * Free space used by AVL-tree.
 * Sets tree pointer to NULL
 */
void avlFree(AVLTree** tree);

/*
 * Return true, if has the given key in the tree.
 * Return false otherwise.
 */
bool avlContains(AVLTree* tree, void* key);

/*
 * Return true, if inserted successfully.
 * Return false, if error occurred.
 * If has already the key in the tree, replace the value.
 * hasNew = true if inserted a new node,
 * otherwise hasNew = false and the value is replaced.
 * If a value was replaced, avlInsert() clears memory of value if ValueCleaner was set.
 */
bool avlInsert(AVLTree* tree, void* key, void* value, bool* hasNew);

/*
 * Find the key in the tree and return its value, sets isFound = true.
 * If the key is not in the tree, return NULL and set isFound = false.
 */
void* avlFind(AVLTree* tree, void* key, bool* isFound);

/*
 * Find the key in the tree and delete its data in the tree.
 * Calling KeyCleaner and ValueCleaner for the data of the node directly if specified.
 * Return true if the node was deleted. Return false if the node is not in the tree.
 */
bool avlDelete(AVLTree* tree, void* key);

/*
 * Return count of nodes in the tree.
 */
int avlSize(AVLTree* tree);

/*
 * Print tree values in-order in FILE*.
 * Check PrintKeyValue docs.
 * Return 0 or the first non-zero PrintKeyValue return.
 */
int avlInorder(AVLTree* tree, FILE* file, PrintKeyValue printFunc);