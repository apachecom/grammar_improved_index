#!/bin/bash

cd ../cmake-build-release/

#make process_coll
#
#./process_coll /d1/apacheco/collections/Escherichia_Coli /d1/apacheco/collections/Escherichia_Coli
#
#./process_coll /d1/apacheco/collections/cere /d1/apacheco/collections/cere
#
#./process_coll /d1/apacheco/collections/coreutils /d1/apacheco/collections/coreutils
#
#./process_coll /d1/apacheco/collections/einstein.en.txt /d1/apacheco/collections/einstein.en.txt
#
#./process_coll /d1/apacheco/collections/influenza /d1/apacheco/collections/influenza
#
#./process_coll /d1/apacheco/collections/kernel /d1/apacheco/collections/kernel
#
#./process_coll /d1/apacheco/collections/para /d1/apacheco/collections/para
#


make locate_indices



####################prefijo de indice###############prefijo de fichero de queries ####mL#ML#gap
#./locate_indices ../build-idx/escherichia_coli_p_10 ../collections/escherichia_coli_p_10 5 100 50 --benchmark_out=../locate-Escherichia_Coli.csv       --benchmark_out_format=csv
#
#
#./locate_indices /d1/apacheco/indices/Escherichia_Coli /d1/apacheco/collections/Escherichia_Coli 10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-Escherichia_Coli.csv       --benchmark_out_format=csv

#./locate_indices /d1/apacheco/indices/cere /d1/apacheco/collections/cere 10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-cere.csv       --benchmark_out_format=csv

./locate_indices /d1/apacheco/indices/coreutils_N0 /d1/apacheco/collections/coreutils_N0 10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-coreutils_N0.csv       --benchmark_out_format=csv

./locate_indices /d1/apacheco/indices/einstein.en.txt_N0  /d1/apacheco/collections/einstein.en.txt_N0  10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-einstein.en.txt_N0.csv       --benchmark_out_format=csv

#./locate_indices /d1/apacheco/indices/influenza /d1/apacheco/collections/influenza 10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-influenza.csv       --benchmark_out_format=csv

./locate_indices /d1/apacheco/indices/kernel_N0 /d1/apacheco/collections/kernel_N0 10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-kernel_N0.csv       --benchmark_out_format=csv

#./locate_indices /d1/apacheco/indices/para /d1/apacheco/collections/para 10 100 10 --benchmark_out=/d1/apacheco/results/self-indices/locate/locate-para.csv       --benchmark_out_format=csv

