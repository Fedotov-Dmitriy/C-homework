#pragma once

#include <stdbool.h>

typedef struct Heap Heap;

/*
 * Компаратор для упорядочивания значений в куче
 * Функция должна возвращать значение > 0 если левый операнд больше
 * возвращать 0 если равны возвращать < 0 если меньше
 */
typedef int (*HeapComparator)(const void*, const void*);

/*
 * Очиститель для очистки данных после уничтожения кучи
 */
typedef void (*HeapCleaner)(void*);

/*
 * Создает кучу с заданными значениями и компаратором
 * Создает пустую кучу если nums == NULL и count == 0
 * иначе возвращает NULL или если произошли ошибки
 */
Heap* heapCreate(HeapComparator comp, unsigned count, void** data);

/*
 * Добавляет значение в кучу
 * Возвращает false если произошла ошибка
 */
bool heapPush(Heap* heap, void* val);

/*
 * Удаляет и возвращает значение из вершины кучи
 * Если heap == NULL или куча пуста всегда возвращает 0
 * Пользователь должен проверять размер кучи вручную через heapSize() или heapEmpty()
 */
void* heapPop(Heap* heap);

/*
 * Возвращает значение из вершины кучи
 * Если heap == NULL или куча пуста всегда возвращает 0
 * Пользователь должен проверять размер кучи вручную через heapSize() или heapEmpty()
 */
void* heapTop(Heap* heap);

/*
 * Возвращает true если куча пуста или указатель NULL
 */
bool heapEmpty(const Heap* heap);

/*
 * Возвращает размер кучи или 0 если указатель NULL
 */
unsigned heapSize(const Heap* heap);

/*
 * Освобождает данные выделенные для кучи и устанавливает указатель heap в NULL
 * Передайте функцию free если данные нужно очистить иначе NULL
 */
void heapFree(Heap** heap, HeapCleaner cleaner);