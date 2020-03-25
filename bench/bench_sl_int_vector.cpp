//
// Created by alejandro on 24-05-19.
//


#include <iostream>
#include <fstream>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include "repetitive_collections.h"
#include <sdsl/vlc_vector.hpp>

#define MAX_QUERIES 1000

std::string path = "../files/new_divided_indices/";

sdsl::vlc_vector<sdsl::coder::elias_delta> sl_delta_vlc;
sdsl::vlc_vector<> sl_gama_vlc;
std::vector<size_t > Q(MAX_QUERIES);

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

static void access_int_vec_sl(benchmark::State &state) {
    __int64_t code = state.range(0);
    sdsl::int_vector<> sl;
    std::fstream int_vec_sl_f(path+std::to_string(code)+"_int_vec_sl",std::ios::in|std::ios::binary);
    sdsl::load(sl,int_vec_sl_f);
    int_vec_sl_f.close();

    std::srand(std::time(nullptr));
    for (int i = 0; i < MAX_QUERIES ; ++i) {
        Q[i] = rand() % sl.size();
    }

    for (auto _ : state) {
        for (int i = 0; i < Q.size(); ++i) {
            auto a = sl[i];
        }
    }

    state.counters["space"] = sdsl::size_in_bytes(sl);

}


static void access_vlc_int_gama_sl(benchmark::State &state) {
    __int64_t code = state.range(0);

    std::fstream f(path+std::to_string(code)+"_int_vec_vlc_gama_sl",std::ios::in|std::ios::binary);
    sdsl::load(sl_gama_vlc,f);
    f.close();
    for (auto _ : state) {
        for (int i = 0; i < Q.size(); ++i) {
            auto a = sl_gama_vlc[i];
        }
    }
    state.counters["space"] = sdsl::size_in_bytes(sl_gama_vlc);

}


static void access_vlc_int_delta_sl(benchmark::State &state) {
    __int64_t code = state.range(0);

    std::fstream f(path+std::to_string(code)+"_int_vec_vlc_delta_sl",std::ios::in|std::ios::binary);
    sdsl::load(sl_delta_vlc,f);
    f.close();
    for (auto _ : state) {
        for (int i = 0; i < Q.size(); ++i) {
            auto a = sl_delta_vlc[i];
        }
    }
    state.counters["space"] = sdsl::size_in_bytes(sl_delta_vlc);

}
BENCHMARK(access_int_vec_sl)      ->Args({1})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_gama_sl) ->Args({1})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_delta_sl)->Args({1})->Unit(benchmark::kMicrosecond);

BENCHMARK(access_int_vec_sl)      ->Args({1})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_gama_sl) ->Args({1})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_delta_sl)->Args({1})->Unit(benchmark::kMicrosecond);

BENCHMARK(access_int_vec_sl)      ->Args({2})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_gama_sl) ->Args({2})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_delta_sl)->Args({2})->Unit(benchmark::kMicrosecond);

BENCHMARK(access_int_vec_sl)      ->Args({4})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_gama_sl) ->Args({4})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_delta_sl)->Args({4})->Unit(benchmark::kMicrosecond);

BENCHMARK(access_int_vec_sl)      ->Args({5})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_gama_sl) ->Args({5})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_delta_sl)->Args({5})->Unit(benchmark::kMicrosecond);

BENCHMARK(access_int_vec_sl)      ->Args({6})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_gama_sl) ->Args({6})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_delta_sl)->Args({6})->Unit(benchmark::kMicrosecond);

BENCHMARK(access_int_vec_sl)      ->Args({7})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_gama_sl) ->Args({7})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_delta_sl)->Args({7})->Unit(benchmark::kMicrosecond);

BENCHMARK(access_int_vec_sl)      ->Args({8})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_gama_sl) ->Args({8})->Unit(benchmark::kMicrosecond);
BENCHMARK(access_vlc_int_delta_sl)->Args({8})->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();