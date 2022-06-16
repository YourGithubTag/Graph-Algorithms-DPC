
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <dpct/dpct.hpp>
#include <oneapi/dpl/random>

#include <CL/sycl.hpp>
#if defined(FPGA) || defined(FPGA_EMULATOR)
#include <CL/sycl/INTEL/fpga_extensions.hpp>
#endif

using namespace std;
using namespace sycl;

// Number of nodes in the graph.
constexpr int nodes = 15;

// Maximum distance between two adjacent nodes.
constexpr int max_distance = 10000;
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

    memcpy(graph + source * nodes, temp_graph + source * nodes, sizeof(int) * nodes);

    free(temp_graph);
}



///////////////////////////////////////////////////////////////////////////////////////////////////////

void DjikstraP(int* temp_graph, int* graph, int source) {
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

    memcpy(graph + source * nodes, temp_graph + source * nodes, sizeof(int) * nodes);

//    free(temp_graph);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////






int main() {

    // INIT BASIC SHIT
    queue q{cpu_selector{}};

    int* graph = (int*)malloc(sizeof(int) * nodes * nodes);
    int* graphparallel = (int*)malloc_host<int>(nodes*nodes,q);
    InitializeDirectedGraph(graph);
    CopyGraph(graphparallel, graph);

    // PROCESS BASIC SHIT

    auto begin = chrono::high_resolution_clock::now();
    for (int waa = 0; waa < 5; waa++){
	for (int z = 0; z < nodes; z++) {
        Djikstra(graph, z);
    }
}
    auto end = chrono::high_resolution_clock::now();

    auto dur = end - begin;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();

    // PROCESS PARALLEL SHIT

	int* temp_graph = (int*) malloc_device<int> (nodes*nodes, q);

    auto beginparallel = chrono::high_resolution_clock::now();
	for (int waa = 0; waa < 5; waa++){
    q.submit([&](handler &h){
        h.parallel_for(nodes, [=](id<1> i){
		DjikstraP(temp_graph, graphparallel,i);
        });
    });
    q.wait();
	}
    auto endparallel = chrono::high_resolution_clock::now();

    auto durparallel = endparallel - beginparallel;
    auto msparallel = std::chrono::duration_cast<std::chrono::milliseconds>(durparallel).count();

	for (int i = 0; i < nodes; i++) {
//        cout << std::endl;
        for (int j = 0; j < nodes; j++) {
	 if (graph[i * nodes + j] == graphparallel[i * nodes + j]){
//            cout << graph[i * nodes + j] << " ";
		}
//	else {cout << "NOO";}
        }
    }

    free(graph);
    free(graphparallel,q);
    free(temp_graph,q);

    cout << "done" << std::endl;
    cout << "The Sequential operation required: " << ms << " milliseconds" << std::endl;
    cout << "Parallel operation required: " << msparallel << " milliseconds" << std::endl;

    return 0;

}


