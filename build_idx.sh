cd cmake-build-release-mem-monitor
make build_indices

./build_indices /d1/apacheco/collections/Escherichia_Coli_N0  /d1/apacheco/indices/Escherichia_Coli_N0  /d1/apacheco/results/Escherichia_Coli_N0-mem --benchmark_out=/d1/apacheco/results/build-Escherichia_Coli_N0-mem.csv       --benchmark_out_format=csv
./build_indices /d1/apacheco/collections/cere_N0              /d1/apacheco/indices/cere_N0              /d1/apacheco/results/cere_N0-mem --benchmark_out=/d1/apacheco/results/build-cere_N0-mem.csv       --benchmark_out_format=csv
./build_indices /d1/apacheco/collections/coreutils_N0              /d1/apacheco/indices/coreutils_N0              /d1/apacheco/results/coreutils_N0-mem --benchmark_out=/d1/apacheco/results/build-coreutils_N0-mem.csv       --benchmark_out_format=csv
./build_indices /d1/apacheco/collections/influenza_N0              /d1/apacheco/indices/influenza_N0              /d1/apacheco/results/influenza_N0-mem --benchmark_out=/d1/apacheco/results/build-influenza_N0-mem.csv       --benchmark_out_format=csv
./build_indices /d1/apacheco/collections/para_N0              /d1/apacheco/indices/para_N0              /d1/apacheco/results/para_N0-mem --benchmark_out=/d1/apacheco/results/build-para_N0-mem.csv       --benchmark_out_format=csv
./build_indices /d1/apacheco/collections/kernel_N0              /d1/apacheco/indices/kernel_N0              /d1/apacheco/results/kernel_N0-mem  --benchmark_out=/d1/apacheco/results/build-kernel_N0-mem.csv       --benchmark_out_format=csv
./build_indices /d1/apacheco/collections/einstein.en.txt_N0              /d1/apacheco/indices/einstein.en.txt_N0              /d1/apacheco/results/einstein.en.txt_N0-mem  --benchmark_out=/d1/apacheco/results/build-einstein.en.txt_N0-mem.csv       --benchmark_out_format=csv
