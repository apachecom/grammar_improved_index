mkdir ../cmake-build-release-mem-monitor

cd ../cmake-build-release-mem-monitor

cmake -DCMAKE_BUILD_TYPE=Release -DUSE_MEM_MONITOR=ON ..

make process_coll

make build_indices

make locate_indices

make display_indices


./process_coll /d1/apacheco/collections/Escherichia_Coli /d1/apacheco/collections/Escherichia_Coli_N0

./process_coll /d1/apacheco/collections/cere /d1/apacheco/collections/cere_N0

./process_coll /d1/apacheco/collections/para /d1/apacheco/collections/para_N0

./process_coll /d1/apacheco/collections/influenza /d1/apacheco/collections/influenza_N0

./process_coll /d1/apacheco/collections/kernel /d1/apacheco/collections/kernel_N0

./process_coll /d1/apacheco/collections/coreutils /d1/apacheco/collections/coreutils_N0

./process_coll /d1/apacheco/collections/einstein.en.txt /d1/apacheco/collections/einstein.en.txt_N0





