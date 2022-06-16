#include <CL/sycl.hpp>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <oneapi/dpl/random>

using namespace std;
using namespace sycl;

int max_distance = 100;
constexpr int numNodes = 15;
unsigned int blockSize =  5;

int main() {

  queue Q{default_selector{} };

  // allocate and init memory used by host
  unsigned int* inputgraph = NULL;
  
  unsigned int matrixSizeBytes;

  matrixSizeBytes = numNodes * numNodes * sizeof(unsigned int);

  inputgraph = (unsigned int *) malloc(matrixSizeBytes);
  assert (inputgraph != NULL);

  std::cout << "Original MAtrix" << " \n";
  srand(2);
  for(unsigned int i = 0; i < numNodes; i++)
    for(unsigned int j = 0; j < numNodes; j++)
    {
      int index = i*numNodes + j;
      inputgraph[index] = rand() % (max_distance + 1);
    }

  for(unsigned int i = 0; i < numNodes; ++i)
  {
    unsigned int iXWidth = i * numNodes;
    inputgraph[iXWidth + i] = 0;
  }

  // for (int i = 0; i < numNodes; i++) {
  //       std::cout << "\n";
  //       for (int j = 0; j < numNodes; j++) {
  //          std::cout << inputgraph[i * numNodes + j] << " ";
  //       }
  // }

  unsigned int numPasses = numNodes;

  unsigned int gthreads[2] = {numNodes, numNodes};
  unsigned int lthreads[2] = {blockSize, blockSize};


  sycl::range<3> blocks(gthreads[0] / lthreads[0],
                       gthreads[1] / lthreads[1], 1);


  sycl::range<3> threads(lthreads[0], lthreads[1], 1);

  unsigned int *DeviceGraph;
  DeviceGraph = (unsigned int *)sycl::malloc_device(matrixSizeBytes, Q);

    Q.memcpy(DeviceGraph, inputgraph, matrixSizeBytes);

    auto begin = chrono::high_resolution_clock::now();

    for(unsigned int i = 0; i < numPasses; i++)
    {

      Q.submit([&](sycl::handler &h) {
        auto Grange = blocks * threads;

        h.parallel_for(
            sycl::nd_range<3>(
                sycl::range<3>(Grange.get(2),
                               Grange.get(1),
                               Grange.get(0)),
                sycl::range<3>(threads.get(2), threads.get(1), threads.get(0))),
            [=](sycl::nd_item<3> item) {
                    int xValue = item.get_local_id(2) +
                                item.get_group(2) * item.get_local_range().get(2);
                    int yValue = item.get_local_id(1) +
                                item.get_group(1) * item.get_local_range().get(1);

                    int oldWeight = DeviceGraph[yValue * numNodes + xValue];

                    int tempWeight = DeviceGraph[yValue * numNodes + i] + DeviceGraph[i * numNodes + xValue];

                    if (tempWeight < oldWeight)
                    {
                        DeviceGraph[yValue * numNodes + xValue] = tempWeight;
                    }
            });
      });
    }
    Q.wait();

  Q.memcpy(inputgraph, DeviceGraph, matrixSizeBytes).wait();

 std::cout << "Final MAtrix" << " \n";
  auto end = chrono::high_resolution_clock::now();
  auto dur = end - begin;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
  

 std::cout << "FInal MAtrix" << " \n";
  
  // for (int i = 0; i < numNodes; i++) {
  //       std::cout << "\n";
  //       for (int j = 0; j < numNodes; j++) {
  //          std::cout << inputgraph[i * numNodes + j] << " ";
  //       }
  //   }

  sycl::free(DeviceGraph, Q);

  free(inputgraph);

  std::cout << "\n Blocked Parllel Floyd Warshall took " << ms << " milliseconds to run." << std::endl;

  return 0;
}