//#include <CL/sycl.hpp>
#include <chrono>
#include <cstdlib>
#include <iostream>

// dpc_common.hpp can be found in the dev-utilities include folder.
// e.g., $ONEAPI_ROOT/dev-utilities/<version>/include/dpc_common.hpp
//#include "dpc_common.hpp"

using namespace std;
//using namespace sycl;

// Number of nodes in the graph.
constexpr int nodes = 15;


// Maximum distance between two adjacent nodes.
constexpr int max_distance = 100;
constexpr int infinite = (nodes * max_distance);

void InitializeDirectedGraph(int* graph) {
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            int cell = i * nodes + j;
            int temp = 0;

            if (i == j) {
                graph[cell] = 0;
            }
            else if (rand() % 2) {
                graph[cell] = infinite;
                graph[j * nodes + i] = infinite;
            }
            else {
                temp = rand() % max_distance + 1;
                graph[j * nodes + i] = temp;
                graph[cell] = temp;
            }
        }
    }
}

void CopyGraph(int* to, int* from) {
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            int cell = i * nodes + j;
            to[cell] = from[cell];
        }
    }
}



void Djikstra(int* graph, int source) {
    int* temp_graph = (int*)malloc(sizeof(int) * nodes * nodes);
    bool done[nodes] = { 0 };
    int next = source;
    int compare = infinite;
    CopyGraph(temp_graph, graph);
        for (int i = 0; i < nodes - 1; i++) {
            for (int k = 0; k < nodes; k++) {
                compare = infinite;
                if ((temp_graph[source * nodes + k] < compare) && (done[k] == 0)) {
                    next = k;
                    compare = temp_graph[source * nodes + k];
                }
            }

            for (int j = 0; j < nodes; j++) {
                if ((temp_graph[source * nodes + next] + temp_graph[nodes * next + j]) < temp_graph[source * nodes + j]) {
                    temp_graph[source * nodes + j] = temp_graph[source * nodes + next] + temp_graph[nodes * next + j];
                }
            }

            done[next] = 1;
        }

    for (int i = 0; i < nodes; i++) {
        graph[source * nodes + i] = temp_graph[source * nodes + i];
    }
}

int main() {
    int* graph = (int*)malloc(sizeof(int) * nodes * nodes);
    InitializeDirectedGraph(graph);
    for (int z = 0; z < nodes; z++) {
        Djikstra(graph, z);
    }

    // for (int i = 0; i < nodes; i++) {
    //     /*cout << endl;
    //     for (int j = 0; j < nodes; j++) {
    //         cout << graph[i * nodes + j] << " ";
    //     }*/
    // }

    cout << "done" << std::endl; 
    return 0;
}