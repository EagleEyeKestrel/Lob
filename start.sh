cd build
make
cd ..
rm -r snapdata/*
rm -r input_split_data/*
ulimit -n 6000

#g++ -std=c++17 -fsanitize=address main.cpp global.cpp fileWorker.cpp stockSim.cpp dateUtils.cpp -o ./build/Lob
#./build/Lob -c -f ./data/sample.in -o lyji.out 2>&1
./build/Lob -b -f ./data/sample.dat -o lyji.out 2>&1
#/Users/jiluyang/Desktop/Lob/cmake-build-debug/Lob -b -f ./data/big.dat -o lyji.out 2>&1