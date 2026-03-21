#include "avl_tree.h"
#include "avl_tree_Internal.h"
#include <assert.h>
#include <stdlib.h>

struct InsertData {
    void* key;
    void* value;
    AVLTree* tree;
    bool hasNew;
    bool hasError;
    bool hasIncHeight;
};

struct DeleteData {
    void* key;
    AVLTree* tree;
    bool hasDeleted;
    bool hasDecHeight;
};

static AVLNode* avlNodeAlloc(void* key, void* value);
static void avlTreeFree(const AVLTree* tree, AVLNode* root);
static void avlNodeFree(AVLNode* node, KeyCleaner keyFree, ValueCleaner valueFree);

static AVLNode* avlInsertInternal(AVLNode* node, struct InsertData* data);
static AVLNode* avlFindInternal(AVLNode* node, void* key, Comparator comp);
static AVLNode* avlDeleteInternal(AVLNode* node, struct DeleteData* data);

static AVLNode* avlNodeBalance(AVLNode* node);
static AVLNode* avlNodeRotateLeft(AVLNode* node);
static AVLNode* avlNodeRotateRight(AVLNode* node);
static AVLNode* avlNodeRotateRightLeft(AVLNode* node);
static AVLNode* avlNodeRotateLeftRight(AVLNode* node);

static int avlInorderInternal(AVLNode* node, FILE* file, PrintKeyValue printFunc);

AVLTree* avlAlloc(Comparator comp, KeyCleaner keyFree, ValueCleaner valueFree)
{
    AVLTree* tree = malloc(sizeof(*tree));
    if (tree == NULL)
        return NULL;

    tree->comp = comp;
    tree->keyFree = keyFree;
    tree->valueFree = valueFree;
    tree->root = NULL;
    tree->nodes = 0;
    return tree;
}

void avlFree(AVLTree** tree)
{
    if (tree == NULL || *tree == NULL)
        return;
    avlTreeFree(*tree, (*tree)->root);
    free(*tree);
    *tree = NULL;
}

bool avlInsert(AVLTree* tree, void* key, void* value, bool* hasNew)
{
    struct InsertData data = {
        .key = key,
        .value = value,
        .tree = tree,
        .hasNew = false,
        .hasError = false,
        .hasIncHeight = false,
    };
    tree->root = avlInsertInternal(tree->root, &data);

    *hasNew = data.hasNew;
    if (data.hasNew)
        tree->nodes++;

    return !data.hasError;
}

void* avlFind(AVLTree* tree, void* key, bool* isFound)
{
    if (tree == NULL || tree->root == NULL) {
        *isFound = false;
        return NULL;
    }
    AVLNode* node = avlFindInternal(tree->root, key, tree->comp);
    if (node == NULL) {
        *isFound = false;
        return NULL;
    }
    *isFound = true;
    return node->value;
}

bool avlDelete(AVLTree* tree, void* key)
{
    struct DeleteData data = {
        .key = key,
        .tree = tree,
        .hasDecHeight = false,
        .hasDeleted = false,
    };

    tree->root = avlDeleteInternal(tree->root, &data);
    if (data.hasDeleted)
        tree->nodes--;
    return data.hasDeleted;
}

bool avlContains(AVLTree* tree, void* key)
{
    bool isFound = false;
    avlFind(tree, key, &isFound);
    return isFound;
}

static AVLNode* avlNodeAlloc(void* key, void* value)
{
    AVLNode* node = malloc(sizeof(*node));
    if (node == NULL)
        return node;

    node->balance = 0;
    node->left = node->right = NULL;
    node->key = key;
    node->value = value;
    return node;
}

static void avlTreeFree(const AVLTree* tree, AVLNode* node)
{
    if (node == NULL)
        return;

    avlTreeFree(tree, node->left);
    avlTreeFree(tree, node->right);
    avlNodeFree(node, tree->keyFree, tree->valueFree);
}

static AVLNode* avlInsertInternal(AVLNode* node, struct InsertData* data)
{
    if (node == NULL) {
        AVLNode* newNode = avlNodeAlloc(data->key, data->value);
        data->hasNew = newNode != NULL;
        data->hasError = !data->hasNew;
        data->hasIncHeight = data->hasNew;
        return newNode;
    }

    int compRes = data->tree->comp(node->key, data->key);
    if (compRes == 0) {
        data->hasError = false;
        data->hasNew = false;
        data->hasIncHeight = false;
        /* Free memory if cleaner is defined*/
        if (data->tree->valueFree != NULL)
            data->tree->valueFree(node->value);

        node->value = data->value;
        return node;
    }
    if (compRes > 0) {
        node->left = avlInsertInternal(node->left, data);
        node->balance = data->hasIncHeight ? node->balance - 1 : node->balance;
    } else {
        node->right = avlInsertInternal(node->right, data);
        node->balance = data->hasIncHeight ? node->balance + 1 : node->balance;
    }

    /* When inserted node has a sibling or when need to balance. */
    if (node->balance == 0 || abs(node->balance) == 2)
        data->hasIncHeight = false;
    return avlNodeBalance(node);
}

static AVLNode* avlFindInternal(AVLNode* node, void* key, Comparator comp)
{
    int compRes = comp(node->key, key);
    if (compRes == 0) {
        return node;
    } else if (compRes > 0) {
        if (node->left == NULL)
            return NULL;
        return avlFindInternal(node->left, key, comp);
    } else {
        if (node->right == NULL)
            return NULL;
        return avlFindInternal(node->right, key, comp);
    }
}

static AVLNode* avlNodeBalance(AVLNode* node)
{
    assert(node != NULL);
    if (node->balance == 2) {
        assert(node->right != NULL);
        if (node->right->balance >= 0)
            return avlNodeRotateLeft(node);
        return avlNodeRotateRightLeft(node);
    }
    if (node->balance == -2) {
        assert(node->left != NULL);
        if (node->left->balance <= 0)
            return avlNodeRotateRight(node);
        return avlNodeRotateLeftRight(node);
    }
    return node;
}

static AVLNode* avlNodeRotateLeft(AVLNode* node)
{
    AVLNode* center = node->right->left;
    AVLNode* newRoot = node->right;
    node->right = center;
    newRoot->left = node;

    if (newRoot->balance == 0) {
        newRoot->balance = -1;
        node->balance = 1;
    } else { /* newRoot->balance == 1 */
        newRoot->balance = 0;
        node->balance = 0;
    }

    return newRoot;
}
static AVLNode* avlNodeRotateRight(AVLNode* node)
{
    AVLNode* center = node->left->right;
    AVLNode* newRoot = node->left;
    node->left = center;
    newRoot->right = node;

    if (newRoot->balance == 0) {
        newRoot->balance = 1;
        node->balance = -1;
    } else { /* newRoot->balance == -1 */
        newRoot->balance = 0;
        node->balance = 0;
    }

    return newRoot;
}
static AVLNode* avlNodeRotateRightLeft(AVLNode* node)
{
    AVLNode* X = node;
    AVLNode* Z = node->right;
    AVLNode* Y = node->right->left;

    Z->left = Y->right;
    Y->right = Z;
    X->right = Y->left;
    Y->left = X;

    if (Y->balance == 0) {
        X->balance = 0;
        Z->balance = 0;
    } else if (Y->balance == 1) {
        X->balance = -1;
        Z->balance = 0;
    } else { /* Y->balance == -1 */
        X->balance = 0;
        Z->balance = 1;
    }

    return Y;
}
static AVLNode* avlNodeRotateLeftRight(AVLNode* node)
{
    AVLNode* X = node;
    AVLNode* Z = node->left;
    AVLNode* Y = node->left->right;

    Z->right = Y->left;
    Y->left = Z;
    X->left = Y->right;
    Y->right = X;

    if (Y->balance == 0) {
        X->balance = 0;
        Z->balance = 0;
    } else if (Y->balance == 1) {
        X->balance = 0;
        Z->balance = -1;
    } else { /* Y->balance == -1 */
        X->balance = 1;
        Z->balance = 0;
    }

    return Y;
}

static AVLNode* avlDeleteInternal(AVLNode* node, struct DeleteData* data)
{
    if (node == NULL) {
        data->hasDeleted = false;
        data->hasDecHeight = false;
        return NULL;
    }
    int compRes = data->tree->comp(node->key, data->key);
    if (compRes > 0) {
        node->left = avlDeleteInternal(node->left, data);
        if (data->hasDecHeight == true) {
            data->hasDecHeight = (++node->balance) == 0;
        }
    } else if (compRes < 0) {
        node->right = avlDeleteInternal(node->right, data);
        if (data->hasDecHeight == true) {
            data->hasDecHeight = (--node->balance) == 0;
        }
    } else {
        if (node->left == NULL) {
            AVLNode* temp = node->right;
            avlNodeFree(node, data->tree->keyFree, data->tree->valueFree);
            data->hasDeleted = true;
            data->hasDecHeight = true;
            return temp;
        }

        if (node->right == NULL) {
            AVLNode* temp = node->left;
            avlNodeFree(node, data->tree->keyFree, data->tree->valueFree);
            data->hasDeleted = true;
            data->hasDecHeight = true;
            return temp;
        }

        AVLNode* maxLeft = node->left;
        while (maxLeft->right != NULL) {
            maxLeft = maxLeft->right;
        }
        void* oldKey   = node->key;
        void* oldValue = node->value;
        node->key      = maxLeft->key;
        node->value    = maxLeft->value;
        maxLeft->key   = oldKey;
        maxLeft->value = oldValue;
        data->key      = node->key;
        node->left = avlDeleteInternal(node->left, data);

        if (data->hasDecHeight == true) {
            node->balance++;
            data->hasDecHeight = (node->balance == 0);
        }
    }

    return avlNodeBalance(node);
}

static void avlNodeFree(AVLNode* node, KeyCleaner keyFree, ValueCleaner valueFree)
{
    if (keyFree != NULL)
        keyFree(node->key);
    if (valueFree != NULL)
        valueFree(node->value);
    free(node);
}

int avlSize(AVLTree* tree)
{
    if (tree == NULL)
        return 0;
    return tree->nodes;
}

int avlInorder(AVLTree* tree, FILE* file, PrintKeyValue printFunc)
{
    if (tree != NULL && tree->root != NULL)
        return avlInorderInternal(tree->root, file, printFunc);
    return 0;
}

static int avlInorderInternal(AVLNode* node, FILE* file, PrintKeyValue printFunc)
{
    int result = 0;
    if (node->left != NULL)
        result = avlInorderInternal(node->left, file, printFunc);
    if (result != 0)
        return result;

    result = printFunc(file, node->key, node->value);
    if (result != 0)
        return result;

    if (node->right != NULL)
        result = avlInorderInternal(node->right, file, printFunc);

    return result;
}