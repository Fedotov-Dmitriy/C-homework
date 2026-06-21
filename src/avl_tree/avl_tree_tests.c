#include "avl_tree.h"
#include "avl_tree_Internal.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int intCompare(void* a, void* b)
{
    int* aVal = a;
    int* bVal = b;
    if (*aVal == *bVal)
        return 0;
    if (*aVal > *bVal)
        return 1;
    return -1;
}

void printStr(void* p)
{
    printf("%s", (char*)p);
}

void printInt(void* p)
{
    printf("%d", *((int*)p));
}

void avlTestSmallRotations()
{
    int treeKeys[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    char* treeValues[] = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten" };

    /* In order */

    AVLTree* tree = avlAlloc(intCompare, NULL, NULL);
    assert(tree != NULL);
    bool hasNew = false;
    for (int i = 0; i < sizeof(treeKeys) / sizeof(treeKeys[0]); i++) {
        assert(avlInsert(tree, &treeKeys[i], treeValues[i], &hasNew));
        assert(avlContains(tree, &treeKeys[i]));
        assert(avlIsMetadataCorrect(tree));
        assert(avlIsBalanced(tree));
    }

    for (int i = 0; i < sizeof(treeKeys) / sizeof(treeKeys[0]); i++) {
        assert(avlContains(tree, &treeKeys[i]));
        bool hasFound = false;
        char* data = avlFind(tree, &treeKeys[i], &hasFound);
        assert(data != NULL);
        assert(hasFound);
        assert(strcmp(data, treeValues[i]) == 0);
    }
    assert(avlIsMetadataCorrect(tree));
    assert(avlIsBalanced(tree));
    avlFree(&tree);
    assert(tree == NULL);

    /* In reverse order */

    tree = avlAlloc(intCompare, NULL, NULL);
    assert(tree != NULL);
    for (int i = sizeof(treeKeys) / sizeof(treeKeys[0]) - 1; i >= 0; i--) {
        assert(avlInsert(tree, &treeKeys[i], treeValues[i], &hasNew));
        assert(avlContains(tree, &treeKeys[i]));
        assert(avlIsMetadataCorrect(tree));
        assert(avlIsBalanced(tree));
    }

    for (int i = sizeof(treeKeys) / sizeof(treeKeys[0]) - 1; i >= 0; i--) {
        assert(avlContains(tree, &treeKeys[i]));
        bool hasFound = false;
        char* data = avlFind(tree, &treeKeys[i], &hasFound);
        assert(data != NULL);
        assert(hasFound);
        assert(strcmp(data, treeValues[i]) == 0);
    }
    assert(avlIsMetadataCorrect(tree));
    assert(avlIsBalanced(tree));

    avlFree(&tree);
    assert(tree == NULL);
    printf("Passed: %s()\n", __func__);
}

void avlTestBigRotations()
{
    int treeKeys[] = { 10, 20, 15, 25, 17, 23, 12, 28, 13, 27, 11, 29, 14, 26, 16, 24, 18, 22 };

    AVLTree* tree = avlAlloc(intCompare, NULL, NULL);
    assert(tree != NULL);
    bool hasNew = false;
    for (int i = 0; i < sizeof(treeKeys) / sizeof(treeKeys[0]); i++) {
        assert(avlInsert(tree, &treeKeys[i], &treeKeys[i], &hasNew));
        assert(avlContains(tree, &treeKeys[i]));
        assert(avlIsMetadataCorrect(tree));
        assert(avlIsBalanced(tree));
    }

    for (int i = 0; i < sizeof(treeKeys) / sizeof(treeKeys[0]); i++) {
        assert(avlContains(tree, &treeKeys[i]));
        bool hasFound = false;
        int* data = avlFind(tree, &treeKeys[i], &hasFound);
        assert(data != NULL);
        assert(hasFound);
        assert(*data == treeKeys[i]);
    }
    assert(avlIsMetadataCorrect(tree));
    assert(avlIsBalanced(tree));

    avlFree(&tree);
    assert(tree == NULL);
    printf("Passed: %s()\n", __func__);
}

void avlTestStressInsert()
{
    int treeKeys[] = { 50, 40, 60, 30, 45, 55, 65, 20, 35, 42, 47, 52, 57, 62, 67,
        15, 25, 32, 38, 41, 43, 46, 48, 51, 53, 56, 58, 61, 63, 66, 68 };

    AVLTree* tree = avlAlloc(intCompare, NULL, NULL);
    assert(tree != NULL);
    bool hasNew = false;
    for (int i = 0; i < sizeof(treeKeys) / sizeof(treeKeys[0]); i++) {
        assert(avlInsert(tree, &treeKeys[i], &treeKeys[i], &hasNew));
        assert(avlContains(tree, &treeKeys[i]));
        assert(avlIsMetadataCorrect(tree));
        assert(avlIsBalanced(tree));
    }

    for (int i = 0; i < sizeof(treeKeys) / sizeof(treeKeys[0]); i++) {
        assert(avlContains(tree, &treeKeys[i]));
        bool hasFound = false;
        int* data = avlFind(tree, &treeKeys[i], &hasFound);
        assert(data != NULL);
        assert(hasFound);
        assert(*data == treeKeys[i]);
    }
    assert(avlIsMetadataCorrect(tree));
    assert(avlIsBalanced(tree));

    avlFree(&tree);
    assert(tree == NULL);
    printf("Passed: %s()\n", __func__);
}

void avlTestStressDelete()
{
    int treeKeys[] = { 50, 40, 60, 30, 45, 55, 65, 20, 35, 42, 47, 52, 57, 62, 67, 15, 25, 32, 38, 41, 43, 46, 48, 51, 53, 56, 58, 61, 63, 66, 68 };
    AVLTree* tree = avlAlloc(intCompare, NULL, NULL);
    assert(tree != NULL);
    bool hasNew = false;
    for (int i = 0; i < sizeof(treeKeys) / sizeof(treeKeys[0]); i++) {
        assert(avlInsert(tree, &treeKeys[i], &treeKeys[i], &hasNew));
        assert(avlContains(tree, &treeKeys[i]));
        assert(avlIsMetadataCorrect(tree));
        assert(avlIsBalanced(tree));
    }
    for (int i = 0; i < sizeof(treeKeys) / sizeof(treeKeys[0]); i++) {
        avlDelete(tree, &treeKeys[i]);
        assert(!avlContains(tree, &treeKeys[i]));
        assert(avlIsMetadataCorrect(tree));
        assert(avlIsBalanced(tree));
    }

    avlFree(&tree);
    assert(tree == NULL);
    printf("Passed: %s()\n", __func__);
}

void avlTestDelete(int* values, int size, int deleteValue)
{
    AVLTree* tree = avlAlloc(intCompare, NULL, NULL);
    assert(tree != NULL);
    bool hasNew = false;
    for (int i = 0; i < size; i++) {
        assert(avlInsert(tree, &values[i], &values[i], &hasNew));
        assert(avlContains(tree, &values[i]));
        assert(avlIsMetadataCorrect(tree));
        assert(avlIsBalanced(tree));
    }
    avlDelete(tree, &deleteValue);
    assert(!avlContains(tree, &deleteValue));
    assert(avlIsMetadataCorrect(tree));
    assert(avlIsBalanced(tree));

    avlFree(&tree);
    assert(tree == NULL);
}

void avlTestDeletes()
{
    struct {
        int testValues[8];
        int sizeTest;
        int testDelete;
    } tests[] = {
        { .testValues = { 2, 1, 3 }, // 1
            .sizeTest = 3,
            .testDelete = 2 },
        { .testValues = { 3, 2, 4, 5 }, // 2
            .sizeTest = 4,
            .testDelete = 4 },
        { .testValues = { 4, 2, 6, 1, 3, 5, 7 }, // 3
            .sizeTest = 7,
            .testDelete = 2 },
        {
            .testValues = { 3, 2, 4, 5 },
            .sizeTest = 4,
            .testDelete = 2,
        },
        { .testValues = { 5, 2, 7, 1, 4, 6, 8, 3 },
            .sizeTest = 8,
            .testDelete = 1 }

    };

    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++)
        avlTestDelete(tests[i].testValues, tests[i].sizeTest, tests[i].testDelete);

    printf("Passed: %s()\n", __func__);
}

void avlTestCleanup()
{
    int treeKeys[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    char* treeValues[] = { "one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten" };

    AVLTree* tree = avlAlloc(intCompare, free, free);
    assert(tree != NULL);
    bool hasNew = false;
    for (int i = 0; i < sizeof(treeKeys) / sizeof(treeKeys[0]); i++) {
        int* newKey = (int*)malloc(sizeof(*newKey));
        assert(newKey != NULL);
        *newKey = treeKeys[i];

        char* newValue = strdup(treeValues[i]);
        assert(newValue != NULL);

        assert(avlInsert(tree, newKey, newValue, &hasNew));
        assert(avlContains(tree, newKey));
        assert(avlIsMetadataCorrect(tree));
        assert(avlIsBalanced(tree));
    }

    for (int i = 0; i < sizeof(treeKeys) / sizeof(treeKeys[0]); i++) {
        avlDelete(tree, &treeKeys[i]);
        assert(!avlContains(tree, &treeKeys[i]));
        assert(avlIsMetadataCorrect(tree));
        assert(avlIsBalanced(tree));
    }

    avlFree(&tree);
    assert(tree == NULL);
    printf("Passed: %s()\n", __func__);
}

int main()
{
    avlTestSmallRotations();
    avlTestBigRotations();
    avlTestStressInsert();
    avlTestDeletes();
    avlTestStressDelete();
    avlTestCleanup();
    return 0;
}