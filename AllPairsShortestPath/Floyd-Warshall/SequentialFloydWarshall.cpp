#include <chrono>
#include <cstdlib>
#include <iostream>
#include <array>
#include <string.h>


using namespace std;


int infinite = 0xFFF;
int max_distance = 100;

constexpr int nodes = 15;
constexpr int SquareNode = nodes * nodes;

constexpr size_t array_size = SquareNode;

typedef array<int, array_size> IntArray;



void SeqFloydWarshall(IntArray graph) {


 for (int k = 0; k < nodes; k++) {
    for (int i = 0; i < nodes; i++) {
      for (int j = 0; j < nodes; j++) {
        if (graph[i * nodes + j] > graph[i * nodes + k] + graph[k * nodes + j]) {
            graph[i * nodes + j] = graph[i * nodes + k] + graph[k * nodes + j];
        }
      }
    }
  }

  //  for (int i = 0; i < nodes; i++) {
  //       std::cout << "\n";
  //       for (int j = 0; j < nodes; j++) {
  //          std::cout << graph[i * nodes + j] << " ";
  //       }
  //   }

}

int main() {

    IntArray graph;

    std::cout << "Initial GRAPH:" << "\n";

   for (int i = 0; i < nodes; i++) {
         // std::cout << "\n";
      for (int j = 0; j < nodes; j++) {
          if (i == j) {
              graph[i * nodes +j] = 0;
          }
          else if ((rand() % 2)) { 
              graph[i * nodes + j] = infinite;
              graph[j * nodes + i] = infinite;
          }
          else {
              int randnum = rand() % max_distance + 1;
              graph[i * nodes + j] = randnum;
              graph[j * nodes + i] = randnum;
          }
          //std::cout << graph[i * nodes + j] << " ";
      }
  }

    std::cout << "FINAL GRAPH:" << "\n";
    auto begin = chrono::high_resolution_clock::now();

    SeqFloydWarshall(graph);

    auto end = chrono::high_resolution_clock::now();
  auto dur = end - begin;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
  std::cout << "\n Sequential Floyd Warshall took " << ms << " milliseconds to run." << std::endl;


}
