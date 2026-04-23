#include "graph.h"
#include <assert.h>
#include <stdio.h>

int main()
{
#define COUNT (7)
    unsigned adjacency[COUNT][COUNT] = {
        { 0, 2, 0, 0, 0, 0, 3 },
        { 2, 0, 1, 0, 0, 0, 1 },
        { 0, 1, 0, 1, 0, 1, 0 },
        { 0, 0, 1, 0, 7, 0, 0 },
        { 0, 0, 0, 7, 0, 1, 0 },
        { 0, 0, 1, 0, 1, 0, 4 },
        { 3, 1, 0, 0, 0, 4, 0 },
    };
    Graph* graph = graphCreate();
    assert(graph != NULL);
    assert(graphAdd(graph, COUNT));
    assert(graphConnect(graph, 0, 1, adjacency[0][1]));
    assert(graphConnect(graph, 0, 6, adjacency[0][6]));
    assert(graphConnect(graph, 1, 6, adjacency[1][6]));
    assert(graphConnect(graph, 1, 2, adjacency[1][2]));
    assert(graphConnect(graph, 2, 5, adjacency[2][5]));
    assert(graphConnect(graph, 2, 3, adjacency[2][3]));
    assert(graphConnect(graph, 3, 4, adjacency[3][4]));
    assert(graphConnect(graph, 4, 5, adjacency[4][5]));
    assert(graphConnect(graph, 5, 6, adjacency[5][6]));

    bool err = false;
    for (int i = 0; i < COUNT; i++) {
        AdjacentList* adjList = graphGetAdjacent(graph, i, &err);
        assert(adjList != NULL);
        assert(adjacentGetVertex(adjList) == i);

        assert(!err && adjList != NULL);
        for (int j = 0; j < COUNT; j++) {
            assert(adjacency[i][j] == adjacency[j][i]);
            if (adjacency[i][j] == 1) {
                assert(graphHasConnection(graph, i, j));
                assert(graphConnection(graph, i, j, &err) == adjacency[i][j]);
                assert(!err);
                assert(adjacentHasConnection(adjList, j));
            }
        }
    }

    assert(graphAdd(graph, COUNT));
    assert(graphSize(graph) == COUNT * 2);
    assert(graphConnect(graph, COUNT - 1, 2 * COUNT - 1, 11));
    assert(graphHasConnection(graph, COUNT - 1, 2 * COUNT - 1));
    assert(graphConnection(graph, COUNT - 1, 2 * COUNT - 1, &err) == 11);
    assert(!err);

    graphFree(&graph);
    assert(graph == NULL);

    printf("Tests passed.\n");
    return 0;
#undef COUNT
}