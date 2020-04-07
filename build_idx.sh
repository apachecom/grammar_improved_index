# cd cmake-build-release-mem-monitor
cd cmake-build-release
make build_indices
make process_coll

 ./process_coll /d1/apacheco/collections/Escherichia_Coli    /d1/apacheco/collections/Escherichia_Coli_N0
 ./process_coll /d1/apacheco/collections/cere                /d1/apacheco/collections/cere_N0
 ./process_coll /d1/apacheco/collections/coreutils           /d1/apacheco/collections/coreutils_N0
 ./process_coll /d1/apacheco/collections/einstein.en.txt     /d1/apacheco/collections/einstein.en.txt_N0
 ./process_coll /d1/apacheco/collections/influenza           /d1/apacheco/collections/influenza_N0
 ./process_coll /d1/apacheco/collections/kernel              /d1/apacheco/collections/kernel_N0
 ./process_coll /d1/apacheco/collections/para                /d1/apacheco/collections/para_N0
#
#
 ./build_indices /d1/apacheco/collections/coreutils_N0           /d1/apacheco/indices/coreutils_N0         /d1/apacheco/results/coreutils_N0-mem         --benchmark_out=/d1/apacheco/results/build-coreutils_N0-mem.csv             --benchmark_out_format=csv
 ./build_indices /d1/apacheco/collections/kernel_N0              /d1/apacheco/indices/kernel_N0            /d1/apacheco/results/kernel_N0-mem            --benchmark_out=/d1/apacheco/results/build-kernel_N0-mem.csv                --benchmark_out_format=csv
 ./build_indices /d1/apacheco/collections/einstein.en.txt_N0     /d1/apacheco/indices/einstein.en.txt_N0   /d1/apacheco/results/einstein.en.txt_N0-mem   --benchmark_out=/d1/apacheco/results/build-einstein.en.txt_N0-mem.csv       --benchmark_out_format=csv
 ./build_indices /d1/apacheco/collections/Escherichia_Coli_N0    /d1/apacheco/indices/Escherichia_Coli_N0  /d1/apacheco/results/Escherichia_Coli_N0-mem  --benchmark_out=/d1/apacheco/results/build-Escherichia_Coli_N0-mem.csv      --benchmark_out_format=csv
 ./build_indices /d1/apacheco/collections/cere_N0                /d1/apacheco/indices/cere_N0              /d1/apacheco/results/cere_N0-mem              --benchmark_out=/d1/apacheco/results/build-cere_N0-mem.csv                  --benchmark_out_format=csv
 ./build_indices /d1/apacheco/collections/influenza_N0           /d1/apacheco/indices/influenza_N0         /d1/apacheco/results/influenza_N0-mem         --benchmark_out=/d1/apacheco/results/build-influenza_N0-mem.csv             --benchmark_out_format=csv
 ./build_indices /d1/apacheco/collections/para_N0                /d1/apacheco/indices/para_N0              /d1/apacheco/results/para_N0-mem              --benchmark_out=/d1/apacheco/results/build-para_N0-mem.csv                  --benchmark_out_format=csv
#
#
#./process_coll ../collections/Escherichia_Coli    ../collections/Escherichia_Coli_N0
#./process_coll ../collections/cere                ../collections/cere_N0
#./process_coll ../collections/coreutils           ../collections/coreutils_N0
#./process_coll ../collections/einstein.en.txt     ../collections/einstein.en.txt_N0
#./process_coll ../collections/influenza           ../collections/influenza
#./process_coll ../collections/kernel              ../collections/kernel_N0
#./process_coll ../collections/para                ../collections/para_N0
#
#
#./build_indices ../collections/coreutils_N0_10           ../build-idx/coreutils_N0_10         /tmp/coreutils_N0_10-mem         --benchmark_out=/tmp/build-coreutils_N0_10-mem.csv             --benchmark_out_format=csv
#./build_indices ../collections/kernel_N0_10              ../build-idx/kernel_N0_10            /tmp/kernel_N0_10-mem            --benchmark_out=/tmp/build-kernel_N0_10-mem.csv                --benchmark_out_format=csv
#./build_indices ../collections/einstein.en.txt_N0_10     ../build-idx/einstein.en.txt_N0_10   /tmp/einstein.en.txt_N0_10-mem   --benchmark_out=/tmp/build-einstein.en.txt_N0_10-mem.csv       --benchmark_out_format=csv
#./build_indices ../collections/Escherichia_Coli_N0_10    ../build-idx/Escherichia_Coli_N0_10  /tmp/Escherichia_Coli_N0_10-mem  --benchmark_out=/tmp/build-Escherichia_Coli_N0_10-mem.csv      --benchmark_out_format=csv
#./build_indices ../collections/cere_N0_10                ../build-idx/cere_N0_10              /tmp/cere_N0_10-mem              --benchmark_out=/tmp/build-cere_N0_10-mem.csv                  --benchmark_out_format=csv
#./build_indices ../collections/influenza_N0_10           ../build-idx/influenza_N0_10         /tmp/influenza_N0_10-mem         --benchmark_out=/tmp/build-influenza_N0_10-mem.csv             --benchmark_out_format=csv
#./build_indices ../collections/para_N0_10                ../build-idx/para_N0_10              /tmp/para_N0_10-mem              --benchmark_out=/tmp/build-para_N0_10-mem.csv                  --benchmark_out_format=csv
