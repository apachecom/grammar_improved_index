#!/bin/bash

mkdir build-release
cd build-release
cmake -DCMAKE_BUILD_TYPE=Release ../
cp ../run.sh ./
chmod 777 ./run.sh


