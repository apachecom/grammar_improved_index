#!/bin/bash

cd cmake-build-release
mkdir /d1/apacheco/results/self-indices/space/


sudo make bench_space

./bench_space /d1/apacheco/indices/cere_N0 --benchmark_out=/d1/apacheco/results/self-indices/space/space-cere_N0.csv       --benchmark_out_format=csv
./bench_space /d1/apacheco/indices/coreutils_N0 --benchmark_out=/d1/apacheco/results/self-indices/space/space-coreutils_N0.csv       --benchmark_out_format=csv
./bench_space /d1/apacheco/indices/einstein.en.txt_N0 --benchmark_out=/d1/apacheco/results/self-indices/space/space-einstein.en.txt_N0.csv       --benchmark_out_format=csv
./bench_space /d1/apacheco/indices/influenza_N0 --benchmark_out=/d1/apacheco/results/self-indices/space/space-influenza_N0.csv       --benchmark_out_format=csv
./bench_space /d1/apacheco/indices/kernel_N0 --benchmark_out=/d1/apacheco/results/self-indices/space/space-kernel_N0.csv       --benchmark_out_format=csv
./bench_space /d1/apacheco/indices/para_N0 --benchmark_out=/d1/apacheco/results/self-indices/space/space-para_N0.csv       --benchmark_out_format=csv
