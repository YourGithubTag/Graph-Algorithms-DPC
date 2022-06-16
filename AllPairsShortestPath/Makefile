CXX = dpcpp
CXXFLAGS = -g -std=c++17

SEIDEL_PARALLEL_NAIVE_EXE = Seidel-Parallel
SEIDEL_SEQUENTIAL_EXE = Seidel-Sequential

SEIDEL_SEQ_SRC = Seidel/SequentialSeidel.cpp
SEIDEL_PARA_SRC = Seidel/NaiveParallelSeidel.cpp

FLOYDWARSHALL_PARALLEL_BLOCK_EXE = FloydWarshall-Parallel-Blocked
FLOYDWARSHALL_PARALLEL_NAIVE_EXE = FloydWarshall-Parallel-Naive
FLOYDWARSHALL_SEQUENTIAL_EXE = FloydWarshall-Sequential

FLOYDWARSHALL_PARABL_SRC = Floyd-Warshall/BlockParallelFloydWarshall.cpp
FLOYDWARSHALL_PARANV_SRC = Floyd-Warshall/NaiveParallelFloydWarshall.cpp
FLOYDWARSHALL_SEQ_SRC = Floyd-Warshall/SequentialFloydWarshall.cpp

DJIKSTRA_PARA_EXE = Djik_Parallel
DJIKSTRA_SEQ_EXE  = Djik_Sequential

DJIKSTRA_PARA_SRC = RepeatedDjikstra/DjikstraBasicAndParallel.cpp
DJIKSTRA_SEQ_SRC = RepeatedDjikstra/DjikstraBasic.cpp


all: seidel_para seidel_seq FW_para_b FW_para_n FW_seq dj_seq dj_para

seidel_para:
	$(CXX) $(CXXFLAGS) -o $(SEIDEL_PARALLEL_NAIVE_EXE) $(SEIDEL_PARA_SRC)

seidel_seq:
	$(CXX) $(CXXFLAGS) -O0 -o $(SEIDEL_SEQUENTIAL_EXE) $(SEIDEL_SEQ_SRC)

FW_para_b: 
	$(CXX) $(CXXFLAGS)  -o $(FLOYDWARSHALL_PARALLEL_BLOCK_EXE) $(FLOYDWARSHALL_PARABL_SRC)

FW_para_n: 
	$(CXX) $(CXXFLAGS)  -o $(FLOYDWARSHALL_PARALLEL_NAIVE_EXE) $(FLOYDWARSHALL_PARANV_SRC)

FW_seq: 
	$(CXX) $(CXXFLAGS) -O0 -o $(FLOYDWARSHALL_SEQUENTIAL_EXE) $(FLOYDWARSHALL_SEQ_SRC)

dj_para: 
	$(CXX) $(CXXFLAGS) -o $(DJIKSTRA_PARA_EXE) $(DJIKSTRA_PARA_SRC)

dj_seq: 
	$(CXX) $(CXXFLAGS) -O0 -o $(DJIKSTRA_SEQ_EXE) $(DJIKSTRA_SEQ_SRC)

clean: 
	rm -f $(SEIDEL_PARALLEL_NAIVE_EXE) $(SEIDEL_SEQUENTIAL_EXE) $(FLOYDWARSHALL_PARALLEL_BLOCK_EXE) $(FLOYDWARSHALL_PARALLEL_NAIVE_EXE) $(FLOYDWARSHALL_SEQUENTIAL_EXE) $(DJIKSTRA_PARA_EXE) $(DJIKSTRA_SEQ_EXE)