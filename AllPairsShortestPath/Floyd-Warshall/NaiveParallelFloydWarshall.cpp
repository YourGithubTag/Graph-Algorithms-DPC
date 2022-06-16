#include <CL/sycl.hpp>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <array>
#include <oneapi/dpl/random>

using namespace std;
using namespace sycl;
using namespace chrono;

int infinite = 0xFFF;
int max_distance = 40;

constexpr int nodes = 15;

constexpr size_t array_size = nodes * nodes;
typedef array<int, array_size> IntArray;



int main () {

IntArray parallel_output;
IntArray inputGraph;


for (int i = 0; i < nodes; i++) {
          //std::cout << "\n";
      for (int j = 0; j < nodes; j++) {
          if (i == j) {
              inputGraph[i * nodes +j] = 0;
          }
          else if (!(rand() % 2)) { 
              inputGraph[i * nodes + j] = infinite;;
              inputGraph[j * nodes + i] = infinite;;
          }
          else {
              int randnum = rand() % max_distance + 1;
              inputGraph[i * nodes + j] = randnum;
              inputGraph[j * nodes + i] = randnum;
          }
          //std::cout << inputGraph[i * nodes + j] << " ";
      }
  }
  auto Starttime = chrono::high_resolution_clock::now();

  //Parallel Block
  try {

  queue Q{default_selector{} };
  //cout << "Device: " << q.get_device().get_info<info::device::name>() << "\n";


  buffer<int, 2> graph_buf(sycl::range(nodes, nodes));
  buffer inputGraph_buf(inputGraph);

  buffer output_buf(parallel_output);

  std::uint32_t seed = 107;

  Starttime = chrono::high_resolution_clock::now();

   Q.submit([&](auto &h) {
      // Get write only access to the buffer on a device.

      accessor graph(graph_buf, h, write_only);

      // Execute kernel.

      h.parallel_for(sycl::range(nodes, nodes), [=](auto index) 
      {
        std::uint64_t offset = index.get_linear_id();
        void* malloc_shared(size_t num_bytes, const queue& q);

        oneapi::dpl::minstd_rand engine(seed, offset);
        oneapi::dpl::uniform_int_distribution<int> distr;

        auto rand = distr(engine);

        if (index[0] == index[1]) {
            graph[index] = 0;
        } else if (rand % 2) {
            graph[index] = infinite;
        } else {
            graph[index] = rand % max_distance + 1;
        }

      });
    });
    
  Q.wait();

  for (int k = 0; k < nodes; k++) {

    Q.submit([&](handler &h) {
      accessor inputGraphAcc(inputGraph_buf, h, read_write);
      accessor graph(graph_buf, h, read_write);
      
        h.parallel_for(sycl::range(nodes, nodes), [=] (auto index) {
          int row = index[0];
          int col = index[1];     
             
          /*
          for (int j = 0 ; j < 10; j++) {
            if (graph[row][j] > graph[row][k] + graph[k][j]) {
              graph[row][j]] = graph[row][k] + graph[k][j];
            }void* malloc_shared(size_t num_bytes,
                    const queue& q);
          }
          */

          // if (graph[index] > graph[row][k] + graph[k][col]) {
          //     graph[index] = graph[row][k] + graph[k][col];
          // }

          if (inputGraphAcc[row * nodes + col] > inputGraphAcc[row * nodes + k] + inputGraphAcc[k * nodes + col]) {
              inputGraphAcc[row * nodes + col] = inputGraphAcc[row * nodes + k] + inputGraphAcc[k * nodes + col];
          }

        });
    });
    Q.wait();
  }

   Q.submit([&](handler &h) {
    accessor graph(graph_buf, h, read_write);
    accessor output(output_buf,h,write_only);
     accessor inputGraphAcc(inputGraph_buf, h, read_write);
     h.parallel_for(sycl::range(nodes, nodes), [=] (auto index) {

       int row = index[0];
       int col = index[1];


      //output[row * nodes + col] = graph[index];
      output[row * nodes + col] = inputGraphAcc[row * nodes + col];


       });
    });
    Q.wait();

  } catch (std::exception const &e) {
    cout << "An exception is caught while computing on device.\n";
    terminate();
  }

  auto end = chrono::high_resolution_clock::now();
  auto dur = end - Starttime;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
 




  // for (int i = 0; i < nodes; i++) {
  //     std::cout << "\n";
  //     for (int j = 0; j < nodes; j++) {
  //         std::cout << parallel_output[i * nodes + j] << " ";
  //     }
  // }

 std::cout << "\n Naive Parllel Floyd Warshall took " << ms << " milliseconds to run." << std::endl;

return 0;

}

