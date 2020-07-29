#!/bin/bash


#
#/home/apacheco/projects/g-repair/bal/repair /d1/apacheco/collections/para_N0
#/home/apacheco/projects/g-repair/bal/repair /d1/apacheco/collections/cere_N0
#/home/apacheco/projects/g-repair/bal/repair /d1/apacheco/collections/influenza_N0
#/home/apacheco/projects/g-repair/bal/repair /d1/apacheco/collections/einstein.en.txt_N0
#/home/apacheco/projects/g-repair/bal/repair /d1/apacheco/collections/kernel_N0
#/home/apacheco/projects/g-repair/bal/repair /d1/apacheco/collections/coreutils_N0
#
#
#mv /d1/apacheco/collections/para_N0.C /d1/apacheco/collections/para_N0-bal.C
#mv /d1/apacheco/collections/cere_N0.C /d1/apacheco/collections/cere_N0-bal.C
#mv /d1/apacheco/collections/influenza_N0.C /d1/apacheco/collections/influenza_N0-bal.C
#mv /d1/apacheco/collections/einstein.en.txt_N0.C /d1/apacheco/collections/einstein.en.txt_N0-bal.C
#mv /d1/apacheco/collections/kernel_N0.C /d1/apacheco/collections/kernel_N0-bal.C
#mv /d1/apacheco/collections/coreutils_N0.C /d1/apacheco/collections/coreutils_N0-bal.C
#
#mv /d1/apacheco/collections/para_N0.R /d1/apacheco/collections/para_N0-bal.R
#mv /d1/apacheco/collections/cere_N0.R /d1/apacheco/collections/cere_N0-bal.R
#mv /d1/apacheco/collections/influenza_N0.R /d1/apacheco/collections/influenza_N0-bal.R
#mv /d1/apacheco/collections/einstein.en.txt_N0.R /d1/apacheco/collections/einstein.en.txt_N0-bal.R
#mv /d1/apacheco/collections/kernel_N0.R /d1/apacheco/collections/kernel_N0-bal.R
#mv /d1/apacheco/collections/coreutils_N0.R /d1/apacheco/collections/coreutils_N0-bal.R

# shellcheck disable=SC2054


for pi in {2,4,8,16,32}
  do
    cd cmake-build-release-${pi}-${pi}-${pi}-mem
    make build_indices
#     mkdir /d1/apacheco/built-indices/pi${pi}

#    ./build_indices /d1/apacheco/collections/para_N0            /d1/apacheco/built-indices/pi${pi}/para_N0 /d1/apacheco/results/mem/mem-para_N0 --benchmark_out=/d1/apacheco/results/build/build-para_N0.csv --benchmark_out_format=csv
#    ./build_indices /d1/apacheco/collections/cere_N0            /d1/apacheco/built-indices/pi${pi}/cere_N0 /d1/apacheco/results/mem/mem-cere_N0 --benchmark_out=/d1/apacheco/results/build/build-cere_N0.csv --benchmark_out_format=csv
#    ./build_indices /d1/apacheco/collections/influenza_N0       /d1/apacheco/built-indices/pi${pi}/influenza_N0 /d1/apacheco/results/mem/mem-influenza_N0 --benchmark_out=/d1/apacheco/results/build/build-influenza_N0.csv --benchmark_out_format=csv
#    ./build_indices /d1/apacheco/collections/einstein.en.txt_N0 /d1/apacheco/built-indices/pi${pi}/einstein.en.txt_N0 /d1/apacheco/results/mem/mem-einstein.en.txt_N0 --benchmark_out=/d1/apacheco/results/build/build-einstein.en.txt_N0.csv --benchmark_out_format=csv
#    ./build_indices /d1/apacheco/collections/kernel_N0          /d1/apacheco/built-indices/pi${pi}/kernel_N0 /d1/apacheco/results/mem/mem-kernel_N0 --benchmark_out=/d1/apacheco/results/build/build-kernel_N0.csv --benchmark_out_format=csv
#    ./build_indices /d1/apacheco/collections/coreutils_N0       /d1/apacheco/built-indices/pi${pi}/coreutils_N0 /d1/apacheco/results/mem/mem-coreutils_N0 --benchmark_out=/d1/apacheco/results/build/build-coreutils_N0.csv --benchmark_out_format=csv
    cd ..
done

#
#
#for pi in {2,4,8,16,32}
#  do
#    cd cmake-build-release-${pi}-${pi}-${pi}-mem
#    make display_indices
#    ./display_indices /d1/apacheco/collections/para_N0 /d1/apacheco/built-indices/para_N0 /d1/apacheco/results/mem/mem-para_N0 --benchmark_out=/d1/apacheco/results/build/build-para_N0.csv --benchmark_out_format=csv
#    ./display_indices /d1/apacheco/collections/cere_N0 /d1/apacheco/built-indices/cere_N0 /d1/apacheco/results/mem/mem-cere_N0 --benchmark_out=/d1/apacheco/results/build/build-cere_N0.csv --benchmark_out_format=csv
#    ./display_indices /d1/apacheco/collections/influenza_N0 /d1/apacheco/built-indices/influenza_N0 /d1/apacheco/results/mem/mem-influenza_N0 --benchmark_out=/d1/apacheco/results/build/build-influenza_N0.csv --benchmark_out_format=csv
#    ./display_indices /d1/apacheco/collections/einstein.en.txt_N0 /d1/apacheco/built-indices/einstein.en.txt_N0 /d1/apacheco/results/mem/mem-einstein.en.txt_N0 --benchmark_out=/d1/apacheco/results/build/build-einstein.en.txt_N0.csv --benchmark_out_format=csv
#    ./display_indices /d1/apacheco/collections/kernel_N0 /d1/apacheco/built-indices/kernel_N0 /d1/apacheco/results/mem/mem-kernel_N0 --benchmark_out=/d1/apacheco/results/build/build-kernel_N0.csv --benchmark_out_format=csv
#    ./display_indices /d1/apacheco/collections/coreutils_N0 /d1/apacheco/built-indices/coreutils_N0 /d1/apacheco/results/mem/mem-coreutils_N0 --benchmark_out=/d1/apacheco/results/build/build-coreutils_N0.csv --benchmark_out_format=csv
#    cd ..
#done
