
#include <CL/sycl.hpp>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <oneapi/dpl/random>

using namespace std;
using namespace sycl;


int maxVal = 0xFFF;
constexpr int block_count = 16;


constexpr int nodes = 15;

constexpr int M = nodes;
constexpr int N = nodes;

int * APD(queue Q, int *Graph, int range) {


    //std::cout << "APD Started" << "\n";
    int fullrange = range * range;

    //MALLOC HOST MATRICES
   int *ZMatrixHost = malloc_host<int>(fullrange, Q);
   int *AMatrixHost = malloc_host<int>(fullrange, Q);
   int *BMatrixHost = malloc_host<int>(fullrange, Q);
   int *XMatrixHost = malloc_host<int>(fullrange, Q);
   int *DegreeVectorHost = malloc_host<int>(range, Q);
   int *DMatrixHost = malloc_host<int>(fullrange, Q);

   memcpy(AMatrixHost, Graph, fullrange * sizeof(int));


   //TODO HAVE ALL OPERATIONS COPY TO AND FROM DEVICE

    //Z matrix mutiplication
    try {
        Q.submit([&](auto &h) {
            //nd_item<1> item
            h.parallel_for(sycl::range(M,N), [=](auto index) 
            {

            //int row = item.get_global_id(0);
            //int col = item.get_local_id(0);

            // Get global position in Y direction.
            int row = index[0];
            // Get global position in X direction.
            int col = index[1];

            int sum = 0;

            for (int i = 0; i < range; i++) {
                sum += AMatrixHost[row * range + i] * AMatrixHost[i * range + col];
            }

            ZMatrixHost[row * range + col] = sum;

            });
        });

        Q.wait();

    } catch (std::exception const &e) {
        cout << "An exception is caught while computing on device.\n";
        terminate();
    }


    //Create B Matrix
    try {
        Q.submit([&](auto &h) {

            h.parallel_for(sycl::range(M,N), [=](auto index) 
            {
                int row = index[0];
                int col = index[1];

                if (row != col) {

                    if (AMatrixHost[row * range + col] == 1 || ZMatrixHost[row * range + col] > 0) {
                        BMatrixHost[row * range + col] = 1;
                    } else {
                        BMatrixHost[row * range + col] = 0;
                    }

                } else {
                    BMatrixHost[row * range + col] = 0;
                }
            });
        });

    Q.wait();
    } catch (std::exception const &e) {
        cout << "An exception is caught while computing on device.\n";
        terminate();
    }
   // std::cout << "B matrix" << "\n";

   // std::cout << "B MATRIX:" << "\n";


    bool *noZero = malloc_shared<bool>(1, Q);
    noZero[0] = true;

  //   std::cout << "GOING TO DO B MATRIX CHECK"  <<"\n";
    //int b = CheckBMatrix(Q, BMatrix,range);

    try {
        Q.submit([&](auto &h) {
            h.parallel_for(sycl::range(M,N), [=](auto index)   
            {
                int row = index[0];
                int col = index[1];
                if (row != col && BMatrixHost[row * range + col] == 0) {
                    noZero[0] = false;
                }
            });
        });

    Q.wait();

    } catch (std::exception const &e) {
        cout << "An exception is caught while computing on device.\n";
        terminate();
    }

  //  std::cout << "Check B matrix DONE" << "\n";
   // std::cout << noZero[0] << "\n";

    //FREE Z
    free(ZMatrixHost,Q);

    //Create T
    int *TMatrixHost = malloc_shared<int>(fullrange, Q);

    if (noZero[0] == false) {
        //Recursion

       //  std::cout << "Recurring NOW" << "\n";

        TMatrixHost = APD(Q,BMatrixHost, range);
    } else {
        //Create T
            try {
                    Q.submit([&](auto &h) {
                        h.parallel_for(sycl::range(M,N), [=](auto index) 
                        {
                        int row = index[0];
                        int col = index[1];
                        TMatrixHost[row * range + col] = 2 * BMatrixHost[row * range + col] - AMatrixHost[row * range + col];
                        });
                    });

                    Q.wait();


            } catch (std::exception const &e) {
                cout << "An exception is caught while computing on device.\n";
                terminate();
        }

        free(XMatrixHost,Q);
        free(DegreeVectorHost,Q);
        free(AMatrixHost,Q);
        free(BMatrixHost,Q);

       // std::cout << "Finished Recurring" << "\n";

        return TMatrixHost;
    }


    //MatrixMult(Q,TMatrix, AMatrix, XMatrix, range);
    try {
        Q.submit([&](auto &h) {
            h.parallel_for(sycl::range(M,N), [=](auto index) 
            {
                int row = index[0];
                int col = index[1];
                int sum = 0;

                for (int i = 0; i < range; i++) {
                    sum += TMatrixHost[row * range + i] * AMatrixHost[i * range + col];
                }

                XMatrixHost[row * range + col] = sum;
            });
        });
        Q.wait();

    } catch (std::exception const &e) {
        cout << "An exception is caught while computing on device.\n";
        terminate();
    }
    //std::cout << "XMATRIX" << "\n";

    //Degree(Q, AMatrix, DegreeVector,range);
    try {
        Q.submit([&](auto &h) {
            h.parallel_for(sycl::range(M,N), [=](auto index) 
            {
                int row = index[0];
                int col = index[1];
                DegreeVectorHost[row] += AMatrixHost[row * range + col];
            });
        });
        Q.wait();
    } catch (std::exception const &e) {
        cout << "An exception is caught while computing on device.\n";
        terminate();
    }
    //std::cout << "Finished Degree" << "\n";

    //CreateDMatrix(Q, XMatrix, BMatrix, degree, DMatrix,range);
    try {

        Q.submit([&](auto &h) {

            h.parallel_for(sycl::range(M,N), [=](auto index) 
            {
                int row = index[0];
                int col = index[1];

            if (XMatrixHost[row * range + col] >= TMatrixHost[row * range + col] * DegreeVectorHost[col]) {
                DMatrixHost[row * range + col] = 2 * TMatrixHost[row * range + col];

            } else {
                DMatrixHost[row * range + col] = 2 * TMatrixHost[row * range + col] - 1;
            }

            });
        });

        Q.wait();

    } catch (std::exception const &e) {
        cout << "An exception is caught while computing on device.\n";
        terminate();
    }
    //Free ALL
    free(TMatrixHost,Q);
    free(XMatrixHost,Q);
    free(DegreeVectorHost,Q);
    free(AMatrixHost,Q);
    free(BMatrixHost,Q);
    std::cout << "Final return" << "\n";
    return DMatrixHost; 
}

int main() {

    int range = nodes;
    int fullrange = range * range;
    int graph[range * range];
    queue q(default_selector{});

    //int *graph = malloc_shared<int>(fullrange, q);

    for (int i = 0; i < range; i++) {
        //std::cout << "\n";
        for (int j = 0; j < range; j++) {
           int cell = i * range + j;

            if (i == j) {
                graph[cell] = 0;
                
            }
            else if (!(rand() % 45)) {
                graph[cell] = 1;
                graph[j*range + i] = 1;
            }
            else {
                graph[cell] = 0;
                graph[j*range + i] = 0;
            }
            //std::cout << graph[cell] << " ";
        }
    }

    std::cout << "\n";

    int *FinalGraph = malloc_shared<int>(fullrange, q);

    auto begin = chrono::high_resolution_clock::now();

    FinalGraph = APD(q,graph, range);

    auto end = chrono::high_resolution_clock::now();
    auto dur = end - begin;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
   


     for (int i = 0; i < range; i++) {
        //std::cout << "\n";
        for (int j = 0; j < range; j++) {
            int cell = i * range + j;
           //std::cout << FinalGraph[cell] << " ";
        }
     }
    std::cout << "\n";

    std::cout << "Finished main" << "\n";

     std::cout << "Naive Parallel Seidel took " << ms << " milliseconds to run." << std::endl;
    
    

    return 0;

}