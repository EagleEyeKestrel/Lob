cd build
make
cd ..
rm -r snapdata/*
rm -r input_split_data/*

#g++ -std=c++17 -fsanitize=thread main.cpp global.cpp fileWorker.cpp stockSim.cpp dateUtils.cpp -o ./build/Lob
#./build/Lob -c -f ./data/sample.in -o lyji.out 2>&1
#./cmake-build-debug/Lob -b -f ./data/sample.dat -o lyji.out 2>&1
./build/Lob -b -f ./data/sample.dat -o lyji.out 2>&1