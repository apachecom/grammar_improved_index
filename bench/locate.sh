#!/bin/bash

cd ../cmake-build-release/


make locate_indices



####################prefijo de indice###############prefijo de fichero de queries ####mL#ML#gap
#./locate_indices ../build-idx/escherichia_coli_p_10 ../collections/escherichia_coli_p_10 5 100 50 --benchmark_out=../locate-Escherichia_Coli.csv       --benchmark_out_format=csv
#
#


#
##
#./locate_indices ../build-idx/coreutils_N0_10             ../collections/coreutils_N0 10 100 10
#./locate_indices ../build-idx/einstein.en.txt_N0_10       ../collections/einstein.en.txt_N0  10 100 10
#./locate_indices ../build-idx/kernel_N0_10                ../collections/kernel_N0 10 100 10
#./locate_indices ../build-idx/influenza_N0_10             ../collections/influenza_N0 10 100 10
#./locate_indices ../build-idx/para_N0_10                  ../collections/para_N0 10 100 10
#./locate_indices ../build-idx/Escherichia_Coli_N0_10      ../collections/Escherichia_Coli_N0 10 100 10
#./locate_indices ../build-idx/cere_N0_10                  ../collections/cere_N0 10 100 10
#
#
#







./locate_indices /d1/apacheco/indices/coreutils_N0 /d1/apacheco/collections/coreutils_N0 10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-coreutils_N0.csv       --benchmark_out_format=csv

./locate_indices /d1/apacheco/indices/einstein.en.txt_N0  /d1/apacheco/collections/einstein.en.txt_N0  10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-einstein.en.txt_N0.csv       --benchmark_out_format=csv

./locate_indices /d1/apacheco/indices/kernel_N0 /d1/apacheco/collections/kernel_N0 10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-kernel_N0.csv       --benchmark_out_format=csv

./locate_indices /d1/apacheco/indices/influenza_N0 /d1/apacheco/collections/influenza_N0 10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-influenza_N0.csv       --benchmark_out_format=csv

./locate_indices /d1/apacheco/indices/para_N0 /d1/apacheco/collections/para_N0 10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-para_N0.csv       --benchmark_out_format=csv

./locate_indices /d1/apacheco/indices/Escherichia_Coli_N0 /d1/apacheco/collections/Escherichia_Coli_N0 10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-Escherichia_Coli_N0.csv       --benchmark_out_format=csv

./locate_indices /d1/apacheco/indices/cere_N0 /d1/apacheco/collections/cere_N0 10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-cere_N0.csv       --benchmark_out_format=csv


