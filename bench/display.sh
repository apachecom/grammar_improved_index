#!/bin/bash

cd ../cmake-build-release/

make process_coll

./process_coll /d1/apacheco/collections/Escherichia_Coli /d1/apacheco/collections/Escherichia_Coli

./process_coll /d1/apacheco/collections/cere /d1/apacheco/collections/cere

./process_coll /d1/apacheco/collections/coreutils /d1/apacheco/collections/coreutils

./process_coll /d1/apacheco/collections/einstein.en.txt /d1/apacheco/collections/einstein.en.txt

./process_coll /d1/apacheco/collections/influenza /d1/apacheco/collections/influenza

./process_coll /d1/apacheco/collections/kernel /d1/apacheco/collections/kernel

./process_coll /d1/apacheco/collections/para /d1/apacheco/collections/para



make display_indices



####################prefijo de indice###############prefijo de fichero de queries ####mL#ML#gap
#./display_indices ../build-idx/escherichia_coli_p_10 ../collections/escherichia_coli_p_10 5 100 30 --benchmark_out=../display-Escherichia_Coli.csv       --benchmark_out_format=csv


./display_indices /d1/apacheco/indices/Escherichia_Coli /d1/apacheco/collections/Escherichia_Coli 5 100 30 --benchmark_out=/d1/apacheco/results/self-indices/display-Escherichia_Coli.csv       --benchmark_out_format=csv

./display_indices /d1/apacheco/indices/cere /d1/apacheco/collections/cere 5 100 30 --benchmark_out=/d1/apacheco/results/self-indices/display-cere.csv       --benchmark_out_format=csv

./display_indices /d1/apacheco/indices/coreutils /d1/apacheco/collections/coreutils 5 100 30 --benchmark_out=/d1/apacheco/results/self-indices/display-coreutils.csv       --benchmark_out_format=csv

./display_indices /d1/apacheco/indices/einstein.en.txt  /d1/apacheco/collections/einstein.en.txt  5 100 30 --benchmark_out=/d1/apacheco/results/self-indices/display-einstein.en.txt .csv       --benchmark_out_format=csv

./display_indices /d1/apacheco/indices/influenza /d1/apacheco/collections/influenza 5 100 30 --benchmark_out=/d1/apacheco/results/self-indices/display-influenza.csv       --benchmark_out_format=csv

./display_indices /d1/apacheco/indices/kernel /d1/apacheco/collections/kernel 5 100 30 --benchmark_out=/d1/apacheco/results/self-indices/display-kernel.csv       --benchmark_out_format=csv

./display_indices /d1/apacheco/indices/para /d1/apacheco/collections/para 5 100 30 --benchmark_out=/d1/apacheco/results/self-indices/display-para.csv       --benchmark_out_format=csv./display_indices /d1/apacheco/indices/Escherichia_Coli /d1/apacheco/collections/Escherichia_Coli 5 100 30 --benchmark_out=/d1/apacheco/results/self-indices/display-Escherichia_Coli.csv       --benchmark_out_format=csv

