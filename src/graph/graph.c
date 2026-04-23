#include "graph.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>


typedef struct Graph {
    AdjacentList* vertices;
    unsigned size;
    unsigned capacity;
} Graph;

/*
 *  это список, содержащий
 * вершины, смежные с некоторой другой вершиной.
 */

typedef struct AdjacentList {
    unsigned vertex;
    unsigned* vertices;
    unsigned* count;
} AdjacentList;

/*
 * Перевыделяет список смежных вершин в графе.
 * Возвращает true и заменяет список на перевыделенный, если операция успешна.
 * Возвращает false и не изменяет список графа, если произошла ошибка.
 */
static bool reallocAdjacentLists(Graph* graph, unsigned newCap);

/*
 * Перевыделяет список смежных вершин.
 * Возвращает true при успехе и изменяет метаданные AdjacentList.
 * Возвращает false при ошибке и не изменяет метаданные.
 */
static bool reallocNeighbours(AdjacentList* list, unsigned newCap, unsigned oldCap);

/*
 * Освобождает список смежных вершин в графе.
 */
static void listFree(Graph* graph);

static bool initAdjacentList(Graph* graph, unsigned vertex);

Graph* graphCreate()
{
    return calloc(1, sizeof(Graph));
}

static bool reallocAdjacentLists(Graph* graph, unsigned newCap)
{
    assert(graph != NULL);
    assert(newCap > 0);
    assert(newCap > graph->capacity);

    AdjacentList* newList = realloc(graph->vertices, newCap * sizeof(graph->vertices[0]));
    if (newList == NULL)
        return false;

    graph->vertices = newList;
    unsigned oldCap = graph->capacity;
    graph->capacity = newCap;

    int i = 0;
    for (; i < oldCap; i++) {
        if (!reallocNeighbours(&graph->vertices[i], newCap, oldCap)) {
            i = oldCap;
            for (; i >= 0; i--)
                free(graph->vertices[i].vertices);
            free(graph->vertices);
            graph->capacity = 0;
            graph->size = 0;
            graph->vertices = NULL;
            return false;
        }
    }

    for (; i < newCap; i++) {
        if (!initAdjacentList(graph, i)) {
            for (; i >= 0; i--)
                free(graph->vertices[i].vertices);
            free(graph->vertices);
            graph->capacity = 0;
            graph->size = 0;
            graph->vertices = NULL;
            return false;
        }
    }

    return true;
}

static bool reallocNeighbours(AdjacentList* list, unsigned newCap, unsigned oldCap)
{
    assert(list != NULL);
    assert(newCap > 0);
    assert(newCap > oldCap);

    unsigned* vertices = realloc(list->vertices, sizeof(vertices[0]) * newCap);
    if (vertices == NULL) {
        return false;
    }

    list->vertices = vertices;
    memset(vertices + oldCap, 0, sizeof(vertices[0]) * (newCap - oldCap));
    return true;
}

static bool initAdjacentList(Graph* graph, unsigned vertex)
{
    assert(graph != NULL);
    assert(graph->vertices);
    assert(graph->capacity > vertex);

    AdjacentList* list = &graph->vertices[vertex];
    list->vertices = calloc(1, sizeof(list->vertices[0]) * graph->capacity);
    if (list->vertices == NULL)
        return false;

    list->count = &graph->size;
    list->vertex = vertex;
    return true;
}

static void listFree(Graph* graph)
{
    if (graph == NULL || graph->vertices == NULL)
        return;

    for (unsigned i = 0; i < graph->capacity; i++)
        free(graph->vertices[i].vertices);

    free(graph->vertices);
    graph->vertices = NULL;
    graph->size = 0;
    graph->capacity = 0;
}

void graphFree(Graph** graph)
{
    if (graph == NULL || *graph == NULL)
        return;

    listFree(*graph);
    free(*graph);
    *graph = NULL;
}

unsigned graphSize(Graph* graph)
{
    return graph->size;
}

AdjacentList* graphGetAdjacent(Graph* graph, unsigned vertex, bool* err)
{
    if (graph == NULL || vertex >= graph->size) {
        *err = true;
        return NULL;
    }

    *err = false;
    return &graph->vertices[vertex];
}

void graphFreeAdjacent(AdjacentList** list)
{
    if (list == NULL || *list == NULL)
        return;

    free(*list);
    *list = NULL;
}

bool graphAdd(Graph* graph, unsigned count)
{
    if (graph == NULL)
        return false;

    if (graph->capacity <= graph->size + count) {
        unsigned newCap = graph->capacity > 0 ? graph->capacity << 1 : 4;
        while (newCap < graph->size + count)
            newCap <<= 1;

        if (!reallocAdjacentLists(graph, newCap))
            return false;
    }

    graph->size += count;
    assert(graph->size <= graph->capacity);
    return true;
}

bool graphHasConnection(Graph* graph, unsigned a, unsigned b)
{
    return graph != NULL && graph->size > a && graph->size > b && graph->vertices[a].vertices[b] > 0;
}

unsigned graphConnection(Graph* graph, unsigned a, unsigned b, bool* err)
{
    if (graph == NULL || graph->size <= a || graph->size <= b) {
        if (err)
            *err = true;
        return 0;
    }

    return graph->vertices[a].vertices[b];
}

bool graphConnect(Graph* graph, unsigned a, unsigned b, unsigned weight)
{
    if (graph == NULL || graph->size <= a || graph->size <= b || weight == 0)
        return false;

    graph->vertices[a].vertices[b] = weight;
    graph->vertices[b].vertices[a] = weight;
    return true;
}

unsigned adjacentGetSize(AdjacentList* list)
{
    if (list == NULL)
        return 0;

    assert(list->count != NULL);
    return *list->count;
}

unsigned adjacentGetVertex(AdjacentList* list)
{
    if (list == NULL)
        return (unsigned)-1;

    return list->vertex;
}

unsigned adjacentGetConnection(AdjacentList* list, unsigned vert)
{
    if (list == NULL)
        return (unsigned)-1;

    assert(list->count != NULL);
    if (vert >= *list->count)
        return (unsigned)-1;

    return list->vertices[vert];
}

bool adjacentHasConnection(AdjacentList* list, unsigned vertex)
{
    if (list == NULL)
        return false;

    assert(list->count != NULL);
    if (*list->count <= vertex)
        return false;

    return list->vertices[vertex] > 0;
}