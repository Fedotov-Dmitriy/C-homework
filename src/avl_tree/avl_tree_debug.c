#include "avl_tree_Internal.h"
#include <stdio.h>
#include <stdlib.h>

static bool avlNodeIsBalanced(AVLNode* node);
static int countNodes(AVLNode* node);
static int calcHeight(AVLNode* node);

bool avlIsBalanced(AVLTree* tree)
{
    if (tree == NULL)
        return true;
    return avlNodeIsBalanced(tree->root);
}

bool avlIsMetadataCorrect(AVLTree* tree)
{
    if (tree == NULL)
        return true;
    int children = countNodes(tree->root);
    return tree->nodes == children;
}

static int countNodes(AVLNode* node)
{
    if (node == NULL)
        return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}

static bool avlNodeIsBalanced(AVLNode* node)
{
    if (node == NULL)
        return true;

    int leftChildren = calcHeight(node->left);
    int rightChildren = calcHeight(node->right);
    int diff = abs(rightChildren - leftChildren);
    return diff < 2 && avlNodeIsBalanced(node->left) && avlNodeIsBalanced(node->right);
}

static int calcHeight(AVLNode* node)
{
    if (node == NULL)
        return 0;
    int left = calcHeight(node->left);
    int right = calcHeight(node->right);
    return 1 + (left > right ? left : right);
}