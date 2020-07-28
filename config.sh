#!/bin/bash

for pi in {2,4,8,16}
  do
    mkdir cmake-build-release-${pi}-${pi}-${pi}
    cd cmake-build-release-${pi}-${pi}-${pi}
    cmake -DUSE_PRINT_MODE=ON  -DCMAKE_BUILD_TYPE=Release -DINV_PI_T=${pi} -DINV_PI_T_TRIE=${pi} -DINV_PI_T_QGRAM=${pi}  ../
    make
done


