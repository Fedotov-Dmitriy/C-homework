#include "graph.h"
#include "heap.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/* 1024x1024 */
#define CITY_COUNT_LIMIT (1048510)

typedef struct VertexRoad {
    unsigned vert;
    unsigned weight;
} VertexRoad;

/* Компаратор для кучи */
static int lessRoad(const void* a, const void* b)
{
    const VertexRoad* aRoad = a;
    const VertexRoad* bRoad = b;
    if (aRoad->weight < bRoad->weight)
        return 1;
    if (aRoad->weight > bRoad->weight)
        return -1;
    return 0;
}

static bool readStates(const char* filename, Graph** graph, int** states, int* stateCount)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Не удалось открыть");
        return false;
    }

    int roadCount = 0;
    int cityCount = 0;
    if (fscanf(file, "%d %d", &cityCount, &roadCount) != 2) {
        fprintf(stderr, "Не удалось считать количество дорог и городов\n");
        fclose(file);
        return false;
    }
    if (cityCount > CITY_COUNT_LIMIT) {
        fprintf(stderr, "Некорректное значение количества городов\n");
        fclose(file);
        return false;
    }

    Graph* newGraph = graphCreate();
    if (newGraph == NULL) {
        fprintf(stderr, "Не удалось выделить память для графа\n");
        fclose(file);
        return false;
    }
    if (!graphAdd(newGraph, cityCount)) {
        fprintf(stderr, "Не удалось добавить вершины в граф\n");
        graphFree(&newGraph);
        fclose(file);
        return false;
    }

    for (int i = 0; i < roadCount; i++) {
        int a = 0;
        int b = 0;
        int weight = 0;
        if (fscanf(file, "%d %d %d", &a, &b, &weight) != 3) {
            fprintf(stderr, "Не удалось считать данные для ребра\n");
            fclose(file);
            return false;
        }
        if (!graphConnect(newGraph, a - 1, b - 1, weight)) {
            fprintf(stderr, "Не удалось соединить вершины в графе\n");
            graphFree(&newGraph);
            fclose(file);
            return false;
        }
    }

    *stateCount = 0;
    if (fscanf(file, "%d", stateCount) != 1) {
        fprintf(stderr, "Не удалось получить количество государств из файла\n");
        graphFree(&newGraph);
        fclose(file);
        return false;
    }

    if (*stateCount > cityCount || *stateCount <= 0) {
        fprintf(stderr, "Некорректное количество государств в файле\n");
        graphFree(&newGraph);
        fclose(file);
        return false;
    }

    *states = calloc(1, sizeof((*states)[0]) * (*stateCount));
    if (*states == NULL) {
        fprintf(stderr, "Не удалось выделить память для государств\n");
        graphFree(&newGraph);
        fclose(file);
        return false;
    }
    for (int i = 0; i < *stateCount; i++) {
        if (fscanf(file, "%d", &(*states)[i]) != 1) {
            fprintf(stderr, "Не удалось получить номер государства из файла\n");
            free(*states);
            graphFree(&newGraph);
            fclose(file);
            return false;
        }
        /* Нумерация с 0 в графе */
        (*states)[i]--;
    }

    fclose(file);
    *graph = newGraph;
    return true;
}

static void heapsFree(Heap** heaps, int count)
{
    for (int i = 0; i < count; i++)
        heapFree(&heaps[i], free);
}

bool divideConquer(Graph* graph, int* cityStates, unsigned citiesCount, int* states, int stateCount)
{
    assert(citiesCount >= stateCount);
    /* Инициализация куч */
    Heap* stateHeaps[stateCount];

    /* Инициализация значением -1 означает что ни одно государство не заняло город */
    for (int i = 0; i < citiesCount; i++) {
        cityStates[i] = -1;
    }

    /* Инициализация стартовых городов и куч для BFS */
    for (int i = 0; i < stateCount; i++) {
        VertexRoad* newRoad = malloc(sizeof(*newRoad));
        if (newRoad == NULL) {
            fprintf(stderr, "Не удалось выделить память\n");
            heapsFree(stateHeaps, stateCount);
            return false;
        }
        newRoad->vert = states[i];
        newRoad->weight = 0;
        stateHeaps[i] = heapCreate(lessRoad, 1, (void**)&newRoad);
        if (stateHeaps[i] == NULL) {
            fprintf(stderr, "Не удалось выделить память для кучи\n");
            heapsFree(stateHeaps, i);
            return false;
        }
    }

    /* BFS для каждого государства */
    while (true) {
        bool hasNonEmpty = false;

        for (int i = 0; i < stateCount; i++) {
            if (!heapEmpty(stateHeaps[i])) {
                Heap* heap = stateHeaps[i];
                while (!heapEmpty(heap)) {
                    VertexRoad* vertRoad = heapPop(heap);
                    unsigned vert = vertRoad->vert;
                    free(vertRoad);
                    vertRoad = NULL;
                    /* Город занят */
                    if (cityStates[vert] != -1)
                        continue;
                    cityStates[vert] = states[i];
                    /* Получить соседние города */
                    bool err = false;
                    AdjacentList* adjList = graphGetAdjacent(graph, vert, &err);
                    if (err) {
                        fprintf(stderr, "Не удалось получить соседние города\n");
                        heapsFree(stateHeaps, stateCount);
                        return false;
                    }
                    /* Добавить соседние города */
                    for (unsigned adjVert = 0; adjVert < citiesCount; adjVert++) {
                        if (cityStates[adjVert] == -1 && adjacentHasConnection(adjList, adjVert)) {
                            VertexRoad* newRoad = malloc(sizeof(*newRoad));
                            if (newRoad == NULL) {
                                fprintf(stderr, "Не удалось выделить память\n");
                                heapsFree(stateHeaps, stateCount);
                                return false;
                            }
                            newRoad->vert = adjVert;
                            newRoad->weight = adjacentGetConnection(adjList, adjVert);
                            if (!heapPush(heap, newRoad)) {
                                fprintf(stderr, "Не удалось добавить значение в кучу\n");
                                heapsFree(stateHeaps, stateCount);
                                return false;
                            }
                        }
                    }
                    hasNonEmpty = true;
                    break;
                }
            }
        }

        if (!hasNonEmpty)
            break;
    }

    heapsFree(stateHeaps, stateCount);
    return true;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        fprintf(stderr, "Ожидался входной файл\n");
        return 1;
    }

    Graph* graph = NULL;
    int* states = NULL;
    int stateCount = 0;

    if (!readStates(argv[1], &graph, &states, &stateCount)) {
        fprintf(stderr, "Не удалось прочитать \"%s\"\n", argv[1]);
        return 1;
    }

    /* Какому государству соответствует город */
    unsigned citiesCount = graphSize(graph);
    int cityStates[citiesCount];

    if (!divideConquer(graph, cityStates, citiesCount, states, stateCount)) {
        graphFree(&graph);
        free(states);
        fprintf(stderr, "Не удалось разделить города между государствами\n");
        return 1;
    }

    for (unsigned j = 0; j < stateCount; j++) {
        int state = states[j];
        printf("Государство номер %d имеет города:", state + 1);
        for (unsigned i = 0; i < citiesCount; i++) {
            if (cityStates[i] == state)
                printf(" %d", i + 1);
        }
        putchar('\n');
    }

    graphFree(&graph);
    free(states);
    return 0;
}
