#pragma once
#include "avl_tree.h"

/*
 * This header is for internal purposes of AVL-tree library.
 * It cannot be included from the user side.
 * This header is used for testing purposes.
 */

typedef struct AVLNode {
    void* key;
    void* value;
    struct AVLNode* left;
    struct AVLNode* right;
    int balance; /* balance = rightChildren - leftChildren */
} AVLNode;

typedef struct AVLTree {
    Comparator comp;
    KeyCleaner keyFree;
    ValueCleaner valueFree;
    struct AVLNode* root;
    int nodes;
} AVLTree;

/*
 * Check 'balance' value validity.
 * Very slow function used for debug.
 */
bool avlIsBalanced(AVLTree* tree);
/*
 * Very slow function used for debug.
 */
bool avlIsMetadataCorrect(AVLTree* tree);