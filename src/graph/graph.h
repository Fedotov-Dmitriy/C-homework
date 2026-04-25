#pragma once

#include <stdbool.h>

/*
 * Graph хранит вершины
 */
typedef struct Graph Graph;

typedef struct AdjacentList AdjacentList;

/*
 * Создаём граф который затем нужно освободить с помощью graphFree()
 */
Graph* graphCreate(void);

/*
 * Считываем данные графа из файла
 */
Graph* graphRead(const char* filename);

/*
 * Освобождает память выделенную для графа
 */
void graphFree(Graph** graph);

/*
 * Возвращает количество вершин в графе
 */
unsigned graphSize(Graph* graph);

/*
 * Соединяет вершины 'a' и 'b' в графе взвешенным ребром.
 * Возвращает true, если операция успешна, и false если произошла ошибка.
 * weight должен быть больше 0
 */
bool graphConnect(Graph* graph, unsigned a, unsigned b, unsigned weight);

bool graphAdd(Graph* graph, unsigned amount);

/*
 * Возвращает вес ребра между 'a' и 'b'
 * *err = true если ребра между 'a' и 'b' нет, и в этом случае возвращает 0
 */
unsigned graphConnection(Graph* graph, unsigned a, unsigned b, bool* err);

/*
 * Возвращает true если между 'a' и 'b' есть ребро
 */
bool graphHasConnection(Graph* graph, unsigned a, unsigned b);

/*
 * Возвращает список вершин смежных с заданной вершиной
 * *err = true если произошла ошибка
 * *err = false в противном случае
 */
AdjacentList* graphGetAdjacent(Graph* graph, unsigned vertex, bool* err);

/*
 * Возвращает количество вершин в графе для данной вершины
 */
unsigned adjacentGetSize(AdjacentList* list);

/*
 * Возвращает true, если вершина из списка смежности
 * соединена с заданной вершиной
 */
bool adjacentHasConnection(AdjacentList* list, unsigned vertex);

/*
 * Возвращает основную вершину списка смежности
 * Если list освобождён, возвращает ((unsigned)-1)
 */
unsigned adjacentGetVertex(AdjacentList* list);

/*
 * Возвращает вес ребра между основной вершиной и 'vert'
 * Если list освобождён или 'vert' некорректен, возвращает ((unsigned)-1)
 */
unsigned adjacentGetConnection(AdjacentList* list, unsigned vert);
