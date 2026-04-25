#include "heap.h"
#include <stdlib.h>
#include <string.h>

typedef struct Heap {
    void** data;
    unsigned size;
    unsigned capacity;
    HeapComparator comp;
} Heap;

#define HEAP_TOP(heap) (heap->data[0])
#define HEAP_BACK(heap) ((heap->data[heap->size - 1]))

/* Поиск наименьшей степени двойки которая больше числа */
static unsigned nearestPow2(unsigned num)
{
    unsigned res = 1;
    while (res < num)
        res <<= 1;
    return res;
}

static void swapPtr(void** a, void** b)
{
    void* temp = *a;
    *a = *b;
    *b = temp;
}

bool heapIncreaseCapacity(Heap* heap)
{
    void** newData = realloc(heap->data, (heap->capacity << 1) * sizeof(heap->data[0]));
    if (newData == NULL)
        return false;

    heap->capacity <<= 1;
    heap->data = newData;
    return true;
}

Heap* heapCreate(HeapComparator comp, unsigned count, void** data)
{
    if (comp == NULL || (count != 0 && data == NULL) || (data != NULL && count == 0))
        return NULL;

    Heap* heap = malloc(sizeof(*heap));
    if (heap == NULL)
        return NULL;

    heap->capacity = count > 8 ? nearestPow2(count) : 8;
    heap->data = calloc(sizeof(heap->data[0]), heap->capacity);
    if (heap->data == NULL) {
        free(heap);
        return NULL;
    }

    heap->size = 0;
    heap->comp = comp;

    for (unsigned i = 0; i < count; i++) {
        heapPush(heap, data[i]);
    }

    return heap;
}

bool heapPush(Heap* heap, void* val)
{
    if (heap == NULL)
        return false;

    /* Проверка нужно ли увеличить вектор данных */
    if (heap->size + 1 > heap->capacity && !heapIncreaseCapacity(heap))
        return false;

    /* Вставка числа в конец кучи */
    unsigned numPos = heap->size++;
    HEAP_BACK(heap) = val;

    /* Подъем числа вверх */
    while (numPos > 0 && heap->comp(heap->data[numPos], heap->data[(numPos - 1) / 2]) > 0) {
        swapPtr(&heap->data[numPos], &heap->data[(numPos - 1) / 2]);
        numPos = (numPos - 1) / 2;
    }

    return true;
}

void* heapPop(Heap* heap)
{
    if (heapEmpty(heap))
        return NULL;

    void* ret = HEAP_TOP(heap);
    swapPtr(&HEAP_TOP(heap), &HEAP_BACK(heap));
    heap->size--;

    unsigned numPos = 0;
    while (numPos < heap->size) {
        unsigned topValue = numPos;
        unsigned left = numPos * 2 + 1;
        unsigned right = numPos * 2 + 2;

        if (left < heap->size && heap->comp(heap->data[left], heap->data[topValue]) > 0) {
            topValue = left;
        }

        if (right < heap->size && heap->comp(heap->data[right], heap->data[topValue]) > 0) {
            topValue = right;
        }

        if (topValue != numPos) {
            swapPtr(&heap->data[numPos], &heap->data[topValue]);
            numPos = topValue;
        } else {
            break;
        }
    }

    return ret;
}

void* heapTop(Heap* heap)
{
    return heapEmpty(heap) ? NULL : HEAP_TOP(heap);
}

void heapFree(Heap** heap, HeapCleaner cleaner)
{
    if (heap == NULL || *heap == NULL)
        return;

    if (cleaner) {
        for (unsigned i = 0; i < (*heap)->size; i++) {
            cleaner((*heap)->data[i]);
        }
    }

    free((*heap)->data);
    free(*heap);
    *heap = NULL;
}

bool heapEmpty(const Heap* heap)
{
    return heap == NULL || heap->size == 0;
}

unsigned heapSize(const Heap* heap)
{
    return heap == NULL ? 0 : heap->size;
}
