
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <array>
#include <string.h>

using namespace std;

constexpr int nodes = 15;
constexpr int SquareNode = nodes * nodes;


array<int,SquareNode> APD(array<int,SquareNode> graph) {

   // std::cout << "APD START" <<"\n";

    //std::cout << "Going to Create  B " <<"\n";

    array<int,SquareNode> BMatrix;
    
    //std::cout << "Created B " <<"\n";

    array<int,SquareNode> AMatrix;

    //memcpy(AMatrix, graph, nodes * nodes * sizeof(int));

    AMatrix = graph;
    
    // std::cout << "MEMCOPY of A STARTED " <<"\n";
    //  for (int i = 0; i < nodes; i++) {
    //     std::cout << "\n";
    //     for (int j = 0; j < nodes; j++) {
    //        std::cout << AMatrix[i * nodes + j] << " ";
    //         }
    //     }
    //     std::cout << "\n";
    // std::cout << "MEMCOPY of A " <<"\n";

    //int* ZMatrix = (int *) calloc (SquareNode,sizeof(int));
    array<int,SquareNode> ZMatrix;

    std::cout << "ZMatrix Calculating " <<"\n";

    //Transpose
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            int sum = 0;
            for (int k=0; k< nodes; k++){
                sum += AMatrix[i * nodes + k] * AMatrix[k * nodes + j];
                ZMatrix[i * nodes + j] = sum;
            }
        }
    }

    // for (int i = 0; i < nodes; i++) {
    //     std::cout << "\n";
    //     for (int j = 0; j < nodes; j++) {
    //        std::cout << ZMatrix[i * nodes + j] << " ";
    //     }
    // }

    std::cout << "ZMatrix calculated " <<"\n";



    std::cout << "Calculating BMatrix " <<"\n";

    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            if (i != j) {
                if (AMatrix[i * nodes + j] == 1 || ZMatrix[i * nodes + j] > 0) {
                    BMatrix[i * nodes + j] = 1;
                } else {
                    BMatrix[i * nodes + j] = 0;
                }
            } else {
                BMatrix[i * nodes + j] = 0;
            }
        }
    }

    std::cout << "Calculated BMatrix " <<"\n";


    //  for (int i = 0; i < nodes; i++) {
    //     std::cout << "\n";
    //     for (int j = 0; j < nodes; j++) {
    //        std::cout << BMatrix[i * nodes + j] << " ";
    //     }
    // }

    bool Recur = false;
    bool continueFlow = false;
    
    for (int i = 0; i < nodes; i++) {
        for (int j = 0; j < nodes; j++) {
            if ( (i != j) && (BMatrix[i * nodes + j] == 0) ) {
                    Recur = true;
            }
        }
    }

    //std::cout << "Creating TMatrix " <<"\n";
    array<int,SquareNode> TMatrix;
    //int* TMatrix = (int *) malloc(nodes*nodes);

    if (Recur) {
        std::cout << "Recurring " <<"\n";

        TMatrix = APD(BMatrix);
        continueFlow = true;

    } else {

        std::cout << "Creating DMatrix BASECASE " <<"\n";
        array<int,SquareNode> DMatrix;

        //int *DMatrix = (int *) malloc(nodes*nodes);


         continueFlow = false;

        for (int i = 0; i < nodes; i++) {
            for (int j = 0; j < nodes; j++) { 
                DMatrix[i * nodes +j] = 2 * BMatrix[i * nodes + j] - AMatrix[i * nodes + j];
            }   
        }

        return DMatrix;

    }

    if (continueFlow) {

        array<int,SquareNode> XMatrix;

        for (int i = 0; i < nodes; i++) {
            for (int j = 0; j < nodes; j++) {
                int sum = 0;
                for (int k=0; k< nodes; k++){
                    sum += TMatrix[i * nodes + k] * AMatrix[k * nodes + j];
                    XMatrix[i * nodes + j] = sum;
                }
            }
        }
    
        int DegreeVector[nodes];

        for (int i = 0; i < nodes; i++) {
            for (int j = 0; j < nodes; j++) {
                int sum =0;
                sum += AMatrix[i * nodes + j];
                DegreeVector[i] = sum;
            }
        }

        //std::cout << "Creating DMatrix " <<"\n";
        array<int,SquareNode> DMatrix;

        for (int i = 0; i < nodes; i++) {
            for (int j = 0; j < nodes; j++) {
                if (XMatrix[i * nodes + j] >= (TMatrix[i * nodes + j]) * DegreeVector[j]) {
                    DMatrix[i * nodes + j] = 2 * TMatrix[i * nodes + j];
                } else {
                    DMatrix[i * nodes + j] = 2 * TMatrix[i * nodes + j] - 1;
                }
            }
        }

       // memcpy(returnGraph,DMatrix, nodes * nodes * sizeof(int));
       return DMatrix;
    }
}



int main() {
    //int FinalGraph[nodes][nodes];
    array<int,nodes*nodes> FinalGraph;
    array<int,nodes*nodes> graph;

    for (int i = 0; i < nodes; i++) {
            //std::cout << "\n";
        for (int j = 0; j < nodes; j++) {
            if (i == j) {
                graph[i * nodes +j] = 0;
            }
            else if (!(rand() % 2)) { 
                graph[i * nodes + j] = 1;
                graph[j * nodes + i] = 1;
            }
            else {
                graph[i * nodes + j] = 0;
                graph[j * nodes + i] = 0;
            }
           // std::cout << graph[i * nodes + j] << " ";
        }
    }

    std::cout << "\n";

    auto begin = chrono::high_resolution_clock::now();

    FinalGraph  = APD(graph);
    
    auto end = chrono::high_resolution_clock::now();
    auto dur = end - begin;
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
    

    // for (int i = 0; i < nodes; i++) {
    //     std::cout << "\n";
    //     for (int j = 0; j < nodes; j++) {
    //        std::cout << FinalGraph[i * nodes + j] << " ";
    //     }
    // }

    std::cout << "\n Sequential Seidel took " << ms << " milliseconds to run." << std::endl;

    return 0;

}