//
// Created by alejandro on 22-05-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "utils.h"
#include "repetitive_collections.h"

#define MAX_DATA 1e6
#define  MAX_Q 1e3
#define CODE_DATA
#define MAX_OCC 1e6
//
//struct range{
//    uint x1,x2,y1,y2,len;
//    range(){};
//};

std::vector<range> Q;
SelfGrammarIndex* idx;

std::vector<string> coll = {
        "../tests/collections/repetitive/reals/einstein.en.txt",
        "../tests/collections/repetitive/reals/Escherichia_Coli"
};

void load_ranges(int code, int len){

    Q.clear();

    std::fstream fr("../files/"+std::to_string(code)+"_"+std::to_string(len)+"_ranges",std::ios::in|std::ios::binary);

    if(!fr.is_open()){
        std::cout<<"Error the ranges file could not opened!!\n";
        std::cout<<"../files/"+std::to_string(code)+"_"+std::to_string(len)+"_ranges"<<std::endl;
        return;
    }
    std::string buff; uint i = 0;

    while (!fr.eof() && std::getline(fr, buff) ) {
        if(buff.size() > 1){
            range r;
            sscanf(buff.c_str(),"%u %u %u %u %u\n",&r.x1,&r.x2,&r.y1,&r.y2, &r.len);
            Q.emplace_back(r);
        }
    }
    fr.close();
}


static void load_data(benchmark::State &state)
{
    int op_coll = state.range(0);
    int len = state.range(1);

    load_ranges(op_coll, len);

    for (auto _ : state) {
        sleep(4);
    }

    //idx = load_idx(1, 0, 1, 1, 1, collections_code[coll[op_coll]]);
    idx = load_idx(1, 0, op_coll);
    state.counters["occ"] = 0;
    state.counters["Q size"] = Q.size();
}


static void find_second_occ(benchmark::State &state)
{
    uint nocc;

    for (auto _ : state) {
        nocc = 0;

        for (  auto &ii:Q ) {

            std::vector<uint> X;

            idx->find_second_occ(ii.x1,ii.x2,ii.y1,ii.y2,ii.len,X);

            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());

            nocc += X.size();
            if(nocc > MAX_OCC)break;
        }
    }

    state.counters["occ"] = nocc;

    state.counters["Q size"] = Q.size();
}


BENCHMARK(load_data)->Args({4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);
BENCHMARK(load_data)->Args({4,10})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);
BENCHMARK(load_data)->Args({4,20})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);
BENCHMARK(load_data)->Args({4,30})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);
BENCHMARK(load_data)->Args({4,40})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);
BENCHMARK(load_data)->Args({4,50})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);
BENCHMARK(load_data)->Args({4,100})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);

BENCHMARK(load_data)->Args({4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);
BENCHMARK(load_data)->Args({4,10})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);
BENCHMARK(load_data)->Args({4,20})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);
BENCHMARK(load_data)->Args({4,30})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);
BENCHMARK(load_data)->Args({4,40})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);
BENCHMARK(load_data)->Args({4,50})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);
BENCHMARK(load_data)->Args({4,100})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_second_occ)->Unit(benchmark::kMicrosecond);



BENCHMARK_MAIN();



