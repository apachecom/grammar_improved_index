#!/bin/bash


./build_idx.sh
cd bench
./locate.sh

#
#cd cmake-build-release
#
#sudo make bench_qgram_locate_pattern_len
#sudo make bench_pattern_len_other_locate
#sudo make bench_qgram_display_pattern_len
#
#sudo ./bench_pattern_len_other_locate --benchmark_out=../last_results/state_of_art_locate_results.csv --benchmark_out_format=csv

#sudo ./bench_qgram_locate_pattern_len 1 --benchmark_out=../last_results/indexes_locate_results_1.csv --benchmark_out_format=csv
#sudo ./bench_qgram_locate_pattern_len 2 --benchmark_out=../last_results/indexes_locate_results_2.csv --benchmark_out_format=csv
#sudo ./bench_qgram_locate_pattern_len 4 --benchmark_out=../last_results/indexes_locate_results_4.csv --benchmark_out_format=csv
#sudo ./bench_qgram_locate_pattern_len 5 --benchmark_out=../last_results/indexes_locate_results_5.csv --benchmark_out_format=csv
#sudo ./bench_qgram_locate_pattern_len 6 --benchmark_out=../last_results/indexes_locate_results_6.csv --benchmark_out_format=csv
#sudo ./bench_qgram_locate_pattern_len 7 --benchmark_out=../last_results/indexes_locate_results_7.csv --benchmark_out_format=csv
#sudo ./bench_qgram_locate_pattern_len 8 --benchmark_out=../last_results/indexes_locate_results_8.csv --benchmark_out_format=csv

#sudo ./bench_qgram_display_pattern_len 1 --benchmark_out=../last_results/indexes_display_results_1.csv --benchmark_out_format=csv
#sudo ./bench_qgram_display_pattern_len 2 --benchmark_out=../last_results/indexes_display_results_2.csv --benchmark_out_format=csv
#sudo ./bench_qgram_display_pattern_len 4 --benchmark_out=../last_results/indexes_display_results_4.csv --benchmark_out_format=csv
#sudo ./bench_qgram_display_pattern_len 5 --benchmark_out=../last_results/indexes_display_results_5.csv --benchmark_out_format=csv
#sudo ./bench_qgram_display_pattern_len 6 --benchmark_out=../last_results/indexes_display_results_6.csv --benchmark_out_format=csv
#sudo ./bench_qgram_display_pattern_len 7 --benchmark_out=../last_results/indexes_display_results_7.csv --benchmark_out_format=csv
#sudo ./bench_qgram_display_pattern_len 8 --benchmark_out=../last_results/indexes_display_results_8.csv --benchmark_out_format=csv

