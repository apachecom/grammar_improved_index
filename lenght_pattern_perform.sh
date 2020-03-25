#!/bin/bash


cd cmake-build-release


sudo make bench_qgram_locate
sudo make bench_qgram_locate_pattern_len

sudo ./bench_qgram_locate                --benchmark_out=../last_results/bench_qgram_locate.csv                --benchmark_out_format=csv
sudo ./bench_qgram_locate_pattern_len    --benchmark_out=../last_results/bench_qgram_locate_pattern_len.csv    --benchmark_out_format=csv



#
##./create_files_patterns ../tests/collections/repetitive/reals/coreutils      200 1000 200
##./create_files_patterns ../tests/collections/repetitive/reals/Escherichia_Coli      200 1000 200
##./create_files_patterns ../tests/collections/repetitive/reals/cere      200 1000 200
#
#
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 3     --benchmark_out=l3_loc_core_utils.csv       --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 5     --benchmark_out=l5_loc_core_utils.csv       --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 10   --benchmark_out=l10_loc_core_utils.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 15   --benchmark_out=l15_loc_core_utils.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 20   --benchmark_out=l20_loc_core_utils.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 25   --benchmark_out=l25_loc_core_utils.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 30   --benchmark_out=l30_loc_core_utils.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 50   --benchmark_out=l50_loc_core_utils.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 75   --benchmark_out=l75_loc_core_utils.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 100  --benchmark_out=l100_loc_core_utils.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 150  --benchmark_out=l150_loc_core_utils.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 175  --benchmark_out=l175_loc_core_utils.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/coreutils 100  ../files/ 200  --benchmark_out=l200_loc_core_utils.csv     --benchmark_out_format=csv
#
#
#
##./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 1000  ../files/ 50   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/l50_loc_Escherichia_Coli.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 1000  ../files/ 75   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/l75_loc_Escherichia_Coli.csv     --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 1000  ../files/ 100  --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/l100_loc_Escherichia_Coli.csv     --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 1000  ../files/ 150  --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/l150_loc_Escherichia_Coli.csv     --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 1000  ../files/ 175  --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/l175_loc_Escherichia_Coli.csv     --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 1000  ../files/ 200  --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/l200_loc_Escherichia_Coli.csv     --benchmark_out_format=csv
#
#./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 10     ../files/ 3    --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl3_loc_Escherichia_Coli.csv       --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 10    ../files/ 5    --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl5_loc_Escherichia_Coli.csv       --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 10  ../files/ 10   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl10_loc_Escherichia_Coli.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 1000  ../files/ 15   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl15_loc_Escherichia_Coli.csv      --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 10  ../files/ 20   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl20_loc_Escherichia_Coli.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 1000  ../files/ 25   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl25_loc_Escherichia_Coli.csv      --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 10  ../files/ 30   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl30_loc_Escherichia_Coli.csv      --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 10  ../files/ 50   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl50_loc_Escherichia_Coli.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/Escherichia_Coli 1000  ../files/ 100   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl100_loc_Escherichia_Coli.csv      --benchmark_out_format=csv
#
#./idx_locate_performance ../tests/collections/repetitive/reals/einstein.en.txt 10  ../files/ 3     --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl3_loc_einstein.en.csv       --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/einstein.en.txt 10  ../files/ 5     --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl5_loc_einstein.en.csv       --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/einstein.en.txt 10  ../files/ 10   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl10_loc_einstein.en.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/einstein.en.txt 1000  ../files/ 15   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl15_loc_einstein.en.csv      --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/einstein.en.txt 10  ../files/ 20   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl20_loc_einstein.en.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/einstein.en.txt 1000  ../files/ 25   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl25_loc_einstein.en.csv      --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/einstein.en.txt 10  ../files/ 30   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl30_loc_einstein.en.csv      --benchmark_out_format=csv
#./idx_locate_performance ../tests/collections/repetitive/reals/einstein.en.txt 10  ../files/ 50   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl50_loc_einstein.en.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/einstein.en.txt 1000  ../files/ 100   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/pnnl100_loc_einstein.en.csv      --benchmark_out_format=csv
#
##./idx_locate_performance ../tests/collections/repetitive/reals/kernel 100  ../files/ 3     --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/nnl3_loc_kernel.csv       --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/kernel 100  ../files/ 5     --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/nnl5_loc_kernel.csv       --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/kernel 1000  ../files/ 10   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/nnl10_loc_kernel.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/kernel 1000  ../files/ 15   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/nnl15_loc_kernel.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/kernel 1000  ../files/ 20   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/nnl20_loc_kernel.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/kernel 1000  ../files/ 25   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/nnl25_loc_kernel.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/kernel 1000  ../files/ 30   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/nnl30_loc_kernel.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/kernel 1000  ../files/ 50   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/nnl50_loc_kernel.csv      --benchmark_out_format=csv
##./idx_locate_performance ../tests/collections/repetitive/reals/kernel 1000  ../files/ 100   --benchmark_out=../results/pattern_length/patrones_normalizados/vs_All/nnl100_loc_kernel.csv      --benchmark_out_format=csv
#
#
#
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 3     --benchmark_out=l3_loc_cere.csv       --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 5     --benchmark_out=l5_loc_cere.csv       --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 10   --benchmark_out=l10_loc_cere.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 15   --benchmark_out=l15_loc_cere.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 20   --benchmark_out=l20_loc_cere.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 25   --benchmark_out=l25_loc_cere.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 30   --benchmark_out=l30_loc_cere.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 50   --benchmark_out=l50_loc_cere.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 75   --benchmark_out=l75_loc_cere.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 100  --benchmark_out=l100_loc_cere.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 150  --benchmark_out=l150_loc_cere.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 175  --benchmark_out=l175_loc_cere.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/cere 100  ../files/ 200  --benchmark_out=l200_loc_cere.csv     --benchmark_out_format=csv
#
#
#
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt  1000 ../files/ 5     --benchmark_out= ../results/l5_loc_einstein_en_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt  1000 ../files/ 10    --benchmark_out= ../results/l10_loc_einstein_en_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt  1000 ../files/ 15    --benchmark_out= ../results/l15_loc_einstein_en_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt  1000 ../files/ 20    --benchmark_out= ../results/l20_loc_einstein_en_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt  1000 ../files/ 25    --benchmark_out= ../results/l25_loc_einstein_en_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt  1000 ../files/ 30    --benchmark_out= ../results/l30_loc_einstein_en_out.csv      --benchmark_out_format=csv
#
#
#
#
##./create_files_patterns ../tests/collections/repetitive/reals/influenza                      3    1000 3
##./create_files_patterns ../tests/collections/repetitive/reals/influenza                      5    1000 5
##./create_files_patterns ../tests/collections/repetitive/reals/influenza                      10   1000 10
##./create_files_patterns ../tests/collections/repetitive/reals/influenza                      15   1000 15
##./create_files_patterns ../tests/collections/repetitive/reals/influenza                      20   1000 20
##./create_files_patterns ../tests/collections/repetitive/reals/influenza                      25   1000 25
##./create_files_patterns ../tests/collections/repetitive/reals/influenza                      30   1000 30
##./create_files_patterns ../tests/collections/repetitive/reals/influenza                      50   1000 50
##./create_files_patterns ../tests/collections/repetitive/reals/influenza                      100  1000 100
##./create_files_patterns ../tests/collections/repetitive/reals/influenza                      150  1000 150
##./create_files_patterns ../tests/collections/repetitive/reals/influenza                      175  1000 175
##./create_files_patterns ../tests/collections/repetitive/reals/influenza                      200  1000 200
#
#
#
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 3   --benchmark_out=l3_loc_einstein.csv       --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 5   --benchmark_out=l5_loc_einstein.csv       --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 10  --benchmark_out=l10_loc_einstein.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 15  --benchmark_out=l15_loc_einstein.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 20  --benchmark_out=l20_loc_einstein.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 25  --benchmark_out=l25_loc_einstein.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 30  --benchmark_out=l30_loc_einstein.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 50  --benchmark_out=l50_loc_einstein.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 75  --benchmark_out=l75_loc_einstein.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 100 --benchmark_out=l100_loc_einstein.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 150 --benchmark_out=l150_loc_einstein.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 175 --benchmark_out=l175_loc_einstein.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/einstein.en.txt 100  ../files/ 200 --benchmark_out=l200_loc_einstein.csv     --benchmark_out_format=csv
#
#
#
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 3   --benchmark_out=l3_loc_influenza.csv       --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 5   --benchmark_out=l5_loc_influenza.csv       --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 10  --benchmark_out=l10_loc_influenza.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 15  --benchmark_out=l15_loc_influenza.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 20  --benchmark_out=l20_loc_influenza.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 25  --benchmark_out=l25_loc_influenza.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 30  --benchmark_out=l30_loc_influenza.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 50  --benchmark_out=l50_loc_influenza.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 75  --benchmark_out=l75_loc_influenza.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 100 --benchmark_out=l100_loc_influenza.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 150 --benchmark_out=l150_loc_influenza.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 175 --benchmark_out=l175_loc_influenza.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza 100  ../files/ 200 --benchmark_out=l200_loc_influenza.csv     --benchmark_out_format=csv
#
#
#
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 3     --benchmark_out=l3_loc_kernel.csv       --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 5     --benchmark_out=l5_loc_kernel.csv       --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 10   --benchmark_out=l10_loc_kernel.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 15   --benchmark_out=l15_loc_kernel.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 20   --benchmark_out=l20_loc_kernel.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 25   --benchmark_out=l25_loc_kernel.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 30   --benchmark_out=l30_loc_kernel.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 50   --benchmark_out=l50_loc_kernel.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 75   --benchmark_out=l75_loc_kernel.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 100  --benchmark_out=l100_loc_kernel.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 150  --benchmark_out=l150_loc_kernel.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 175  --benchmark_out=l175_loc_kernel.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel 100  ../files/ 200  --benchmark_out=l200_loc_kernel.csv     --benchmark_out_format=csv
#
#
#
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 3     --benchmark_out=l3_loc_para.csv       --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 5     --benchmark_out=l5_loc_para.csv       --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 10   --benchmark_out=l10_loc_para.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 15   --benchmark_out=l15_loc_para.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 20   --benchmark_out=l20_loc_para.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 25   --benchmark_out=l25_loc_para.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 30   --benchmark_out=l30_loc_para.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 50   --benchmark_out=l50_loc_para.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 75   --benchmark_out=l75_loc_para.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 100  --benchmark_out=l100_loc_para.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 150  --benchmark_out=l150_loc_para.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 175  --benchmark_out=l175_loc_para.csv     --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para 100  ../files/ 200  --benchmark_out=l200_loc_para.csv     --benchmark_out_format=csv
#
#
#
#
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza  1000 ../files/ 5     --benchmark_out= ../results/l5_loc_influenza_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza  1000 ../files/ 10    --benchmark_out= ../results/l10_loc_influenza_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza  1000 ../files/ 15    --benchmark_out= ../results/l15_loc_influenza_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza  1000 ../files/ 20    --benchmark_out= ../results/l20_loc_influenza_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza  1000 ../files/ 25    --benchmark_out= ../results/l25_loc_influenza_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/influenza  1000 ../files/ 30    --benchmark_out= ../results/l30_loc_influenza_out.csv      --benchmark_out_format=csv
#
#
#
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel  1000 ../files/ 5      --benchmark_out= ../results/l5_loc_kernel_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel  1000 ../files/ 10    --benchmark_out= ../results/l10_loc_kernel_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel  1000 ../files/ 15    --benchmark_out= ../results/l15_loc_kernel_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel  1000 ../files/ 20    --benchmark_out= ../results/l20_loc_kernel_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel  1000 ../files/ 25    --benchmark_out= ../results/l25_loc_kernel_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/kernel  1000 ../files/ 30    --benchmark_out= ../results/l30_loc_kernel_out.csv      --benchmark_out_format=csv
#
##./pattern_length_bench ../tests/collections/repetitive/reals/para  1000 ../files/ 5      --benchmark_out= ../results/l5_loc_para_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para  1000 ../files/ 10    --benchmark_out= ../results/l10_loc_para_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para  1000 ../files/ 15    --benchmark_out= ../results/l15_loc_para_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para  1000 ../files/ 20    --benchmark_out= ../results/l20_loc_para_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para  1000 ../files/ 25    --benchmark_out= ../results/l25_loc_para_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/para  1000 ../files/ 30    --benchmark_out= ../results/l30_loc_para_out.csv      --benchmark_out_format=csv
#
##./pattern_length_bench ../tests/collections/repetitive/reals/world_leaders  1000 ../files/ 5      --benchmark_out= ../results/l5_loc_world_leaders_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/world_leaders  1000 ../files/ 10    --benchmark_out= ../results/l10_loc_world_leaders_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/world_leaders  1000 ../files/ 15    --benchmark_out= ../results/l15_loc_world_leaders_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/world_leaders  1000 ../files/ 20    --benchmark_out= ../results/l20_loc_world_leaders_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/world_leaders  1000 ../files/ 25    --benchmark_out= ../results/l25_loc_world_leaders_out.csv      --benchmark_out_format=csv
##./pattern_length_bench ../tests/collections/repetitive/reals/world_leaders  1000 ../files/ 30    --benchmark_out= ../results/l30_loc_world_leaders_out.csv      --benchmark_out_format=csv
#
#
#
#