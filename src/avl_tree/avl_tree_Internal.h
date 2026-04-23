#pragma once
#include "avl_tree.h"

/*
 * Этот заголовочный файл нужен для внутренних целей библиотеки AVL дерева
 * Его нельзя подключать со стороны пользователя
 * Этот заголовочный файл используется для тестирования
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
 * Проверяет корректность значения balance
 * Очень медленная функция используется для отладки
 */
bool avlIsBalanced(AVLTree* tree);
/*
 * Очень медленная функция используется для отладки
 */
bool avlIsMetadataCorrect(AVLTree* tree);