#!/bin/sh
echo "15 NODES:" ;
cd AllPairsShortestPath-15;
make;
./run.sh;
cd ..;

echo $" \n" ;

echo "265 NODES:" ;
cd AllPairsShortestPath-265;
make;
./run.sh;
cd ..;

echo $" \n" ;

echo "515 NODES:" ;
cd AllPairsShortestPath-515;
make;
./run.sh;
cd ..;

echo $" \n" ;

echo "765 NODES:" ;
cd AllPairsShortestPath-765;
make;
./run.sh;
cd ..;

echo $" \n" ;

echo "1015 NODES:" ;
cd AllPairsShortestPath-1015;
make;
./run.sh;
cd ..;

echo $" \n" ;

echo "1265 NODES:" ;
cd AllPairsShortestPath-1265;
make;
./run.sh;
cd ..;

echo $" \n" ;

echo "1515 NODES:" ;
cd AllPairsShortestPath-1515;
make;
./run.sh;
cd ..;

echo "END" ;



