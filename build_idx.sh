cd cmake-build-release-mem-monitor
make build_indices

./build_indices /d1/apacheco/collections/Escherichia_Coli  /d1/apacheco/indices/Escherichia_Coli  /d1/apacheco/results/Escherichia_Coli-mem --benchmark_out=/d1/apacheco/results/build-Escherichia_Coli-mem.csv       --benchmark_out_format=csv
./build_indices /d1/apacheco/collections/cere              /d1/apacheco/indices/cere              /d1/apacheco/results/cere-mem --benchmark_out=/d1/apacheco/results/build-cere-mem.csv       --benchmark_out_format=csv
./build_indices /d1/apacheco/collections/coreutils              /d1/apacheco/indices/coreutils              /d1/apacheco/results/coreutils-mem --benchmark_out=/d1/apacheco/results/build-coreutils-mem.csv       --benchmark_out_format=csv
./build_indices /d1/apacheco/collections/influenza              /d1/apacheco/indices/influenza              /d1/apacheco/results/influenza-mem --benchmark_out=/d1/apacheco/results/build-influenza-mem.csv       --benchmark_out_format=csv
./build_indices /d1/apacheco/collections/para              /d1/apacheco/indices/para              /d1/apacheco/results/para-mem --benchmark_out=/d1/apacheco/results/build-para-mem.csv       --benchmark_out_format=csv
./build_indices /d1/apacheco/collections/kernel              /d1/apacheco/indices/kernel              /d1/apacheco/results/kernel-mem  --benchmark_out=/d1/apacheco/results/build-kernel-mem.csv       --benchmark_out_format=csv
./build_indices /d1/apacheco/collections/einstein.en.txt              /d1/apacheco/indices/einstein.en.txt              /d1/apacheco/results/einstein.en.txt-mem  --benchmark_out=/d1/apacheco/results/build-einstein.en.txt-mem.csv       --benchmark_out_format=csv
