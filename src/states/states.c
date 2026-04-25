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

static bool readStates(const char* filename, Graph** graph, int** states, unsigned* stateCount)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Не удалось открыть");
        return false;
    }

    int roadCountInput = 0;
    int cityCountInput = 0;
    if (fscanf(file, "%d %d", &cityCountInput, &roadCountInput) != 2) {
        fprintf(stderr, "Не удалось считать количество дорог и городов\n");
        fclose(file);
        return false;
    }

    if (cityCountInput <= 0 || cityCountInput > CITY_COUNT_LIMIT || roadCountInput < 0) {
        fprintf(stderr, "Некорректное значение количества городов или дорог\n");
        fclose(file);
        return false;
    }

    const unsigned CITY_COUNT = (unsigned)cityCountInput;
    const unsigned ROAD_COUNT = (unsigned)roadCountInput;

    Graph* newGraph = graphCreate();
    if (newGraph == NULL) {
        fprintf(stderr, "Не удалось выделить память для графа\n");
        fclose(file);
        return false;
    }

    if (!graphAdd(newGraph, CITY_COUNT)) {
        fprintf(stderr, "Не удалось добавить вершины в граф\n");
        graphFree(&newGraph);
        fclose(file);
        return false;
    }

    for (unsigned i = 0; i < ROAD_COUNT; i++) {
        int a = 0;
        int b = 0;
        int weight = 0;

        if (fscanf(file, "%d %d %d", &a, &b, &weight) != 3) {
            fprintf(stderr, "Не удалось считать данные для ребра\n");
            graphFree(&newGraph);
            fclose(file);
            return false;
        }

        if (a <= 0 || b <= 0 || a > cityCountInput || b > cityCountInput || weight < 0) {
            fprintf(stderr, "Некорректные данные ребра\n");
            graphFree(&newGraph);
            fclose(file);
            return false;
        }

        if (!graphConnect(newGraph, (unsigned)(a - 1), (unsigned)(b - 1), (unsigned)weight)) {
            fprintf(stderr, "Не удалось соединить вершины в графе\n");
            graphFree(&newGraph);
            fclose(file);
            return false;
        }
    }

    int stateCountInput = 0;
    if (fscanf(file, "%d", &stateCountInput) != 1) {
        fprintf(stderr, "Не удалось получить количество государств из файла\n");
        graphFree(&newGraph);
        fclose(file);
        return false;
    }

    if (stateCountInput <= 0 || stateCountInput > cityCountInput) {
        fprintf(stderr, "Некорректное количество государств в файле\n");
        graphFree(&newGraph);
        fclose(file);
        return false;
    }

    *stateCount = (unsigned)stateCountInput;
    *states = calloc(*stateCount, sizeof((*states)[0]));
    if (*states == NULL) {
        fprintf(stderr, "Не удалось выделить память для государств\n");
        graphFree(&newGraph);
        fclose(file);
        return false;
    }

    for (unsigned i = 0; i < *stateCount; i++) {
        int state = 0;

        if (fscanf(file, "%d", &state) != 1) {
            fprintf(stderr, "Не удалось получить номер государства из файла\n");
            free(*states);
            graphFree(&newGraph);
            fclose(file);
            return false;
        }

        if (state <= 0 || state > cityCountInput) {
            fprintf(stderr, "Некорректный номер государства в файле\n");
            free(*states);
            graphFree(&newGraph);
            fclose(file);
            return false;
        }

        /* Нумерация с 0 в графе */
        (*states)[i] = state - 1;
    }

    fclose(file);
    *graph = newGraph;
    return true;
}

static void heapsFree(Heap** heaps, unsigned count)
{
    for (unsigned i = 0; i < count; i++)
        heapFree(&heaps[i], free);
}

bool divideConquer(Graph* graph, int* cityStates, unsigned citiesCount, const int* states, unsigned stateCount)
{
    assert(citiesCount >= stateCount);

    /* Инициализация куч */
    Heap* stateHeaps[stateCount];

    /* Инициализация значением -1 означает, что ни одно государство не заняло город */
    for (unsigned i = 0; i < citiesCount; i++) {
        cityStates[i] = -1;
    }

    /* Инициализация стартовых городов и куч для BFS */
    for (unsigned i = 0; i < stateCount; i++) {
        VertexRoad* newRoad = malloc(sizeof(*newRoad));
        if (newRoad == NULL) {
            fprintf(stderr, "Не удалось выделить память\n");
            heapsFree(stateHeaps, i);
            return false;
        }

        newRoad->vert = (unsigned)states[i];
        newRoad->weight = 0;

        stateHeaps[i] = heapCreate(lessRoad, 1, (void**)&newRoad);
        if (stateHeaps[i] == NULL) {
            fprintf(stderr, "Не удалось выделить память для кучи\n");
            free(newRoad);
            heapsFree(stateHeaps, i);
            return false;
        }
    }

    /* BFS для каждого государства */
    while (true) {
        bool hasNonEmpty = false;

        for (unsigned i = 0; i < stateCount; i++) {
            if (!heapEmpty(stateHeaps[i])) {
                Heap* heap = stateHeaps[i];

                while (!heapEmpty(heap)) {
                    VertexRoad* vertRoad = heapPop(heap);
                    unsigned vert = vertRoad->vert;
                    free(vertRoad);

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
                                free(newRoad);
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
    unsigned stateCount = 0;

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
                printf(" %u", i + 1);
        }

        putchar('\n');
    }

    graphFree(&graph);
    free(states);
    return 0;
}
