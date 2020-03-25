//
// Created by root on 10-06-19.
//


#include <iostream>
#include <fstream>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>

#include "repetitive_collections.h"
#include "utils.h"

#include "../SelfGrammarIndexBSQ.h"

//
//static void buildFromBS(benchmark::State &state) {
//
//    int op_coll = state.range(0);
//    int qs = state.range(1);
//
//    SelfGrammarIndexBSQ idx(load_idx(1, 0, 1, 1, 1,op_coll ));
//    grammar repair_g;
//    load_grammar(repair_g,op_coll,false);
//    std::string text;
//    load_data(1e19,text,collections_code_inv_dir[op_coll]);
//
//    for (auto _ : state) {
//        std::cout<<"building samplings\n";
//        idx.buildSamplingsFromGrammar(qs,repair_g,text);
//
//        std::ofstream foutG(read_path_div+std::to_string(op_coll)+".qgramSampG_"+std::to_string(qs),std::ios::out|std::ios::binary);
//        std::ofstream foutGR(read_path_div+std::to_string(op_coll)+".qgramSampGR_"+std::to_string(qs),std::ios::out|std::ios::binary);
//        std::ofstream fout(read_path_div+std::to_string(op_coll)+".qgramSamp_"+std::to_string(qs),std::ios::out|std::ios::binary);
//        idx.saveSampling(foutG,foutGR);
//    }
//
//    state.counters["size"] = idx.size_in_bytes();
//    state.counters["GS size"] = idx.grammarSamp->getSize();
//    state.counters["GRS size"] = idx.reverseSamp->getSize();
//
//
//}

static void build_basics(benchmark::State &state) {

    int code = state.range(0);
    int qs = state.range(1);

    std::string text;

    load_data(1e19,text,collections_code_inv_dir[code]);

//    SelfGrammarIndexBSQ idx(load_idx(1, 0, 1, 1, 1,op_coll ));
    SelfGrammarIndexBSQ idx;
//    grammar repair_g;
//    load_grammar(repair_g,code,false);
    for (auto _ : state) {
        std::cout<<"building basics for :"<<collections_name[collections_code_inv_dir[code]]<<std::endl;
        idx.build(text,qs);
        std::fstream fout(read_path_div+std::to_string(code)+".basic_index",std::ios::out|std::ios::binary);
        std::cout<<"\tsaving basics in $:"<<read_path_div+std::to_string(code)+".basic_index"<<std::endl;
        idx.save(fout);
//        std::ofstream foutG(read_path_div+std::to_string(op_coll)+".qgramSampG_"+std::to_string(qs),std::ios::out|std::ios::binary);
//        std::ofstream foutGR(read_path_div+std::to_string(op_coll)+".qgramSampGR_"+std::to_string(qs),std::ios::out|std::ios::binary);
//
//        idx.saveSampling(foutG,foutGR);
    }
}


static void build_sampling(benchmark::State &state) {

    int code = state.range(0);
    int qs = state.range(1);

    SelfGrammarIndexBSQ idx;
    std::fstream fin(read_path_div+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    idx.load(fin);

    for (auto _ : state) {

        std::cout<<"\tbuilding sampling: ("<<std::to_string(qs)<<")"<<std::endl;

        idx.buildSamplings(qs);
        std::cout<<"\tsaving sampling rules rev in $:"<<read_path_div+std::to_string(code)+".qgramSampG_"+std::to_string(qs)<<std::endl;
        std::ofstream foutG(read_path_div+std::to_string(code)+".qgramSampG_"+std::to_string(qs),std::ios::out|std::ios::binary);
        std::cout<<"\tsaving sampling rules in $:"<<read_path_div+std::to_string(code)+".qgramSampGR_"+std::to_string(qs)<<std::endl;
        std::ofstream foutGR(read_path_div+std::to_string(code)+".qgramSampGR_"+std::to_string(qs),std::ios::out|std::ios::binary);

        std::ofstream foutGS(read_path_div+std::to_string(code)+".qgramSampGS_"+std::to_string(qs),std::ios::out|std::ios::binary);

        idx.saveSampling(foutG,foutGR,foutGS);
    }

    state.counters["size"] = idx.size_in_bytes();
    state.counters["GS size"] = idx.grammarSamp->getSize();
    state.counters["GRS size"] = idx.reverseSamp->getSize();


}


//BENCHMARK(build_basics  )->Args({1    })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({1, 2 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({1, 4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({1, 6 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({1, 8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({1, 10})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({1, 12})->Unit(benchmark::kMicrosecond);
//
//
//BENCHMARK(build_basics  )->Args({2    })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({2, 2 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({2, 4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({2, 6 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({2, 8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({2, 10})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({2, 12})->Unit(benchmark::kMicrosecond);


BENCHMARK(build_basics  )->Args({4    })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({4, 2 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({4, 4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({4, 6 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({4, 8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({4, 10})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({4, 12})->Unit(benchmark::kMicrosecond);
//
//BENCHMARK(build_basics  )->Args({5    })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({5, 2 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({5, 4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({5, 6 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({5, 8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({5, 10})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({5, 12})->Unit(benchmark::kMicrosecond);

//
//BENCHMARK(build_basics  )->Args({6    })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({6, 2 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({6, 4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({6, 6 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({6, 8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({6, 10})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({6, 12})->Unit(benchmark::kMicrosecond);

//
//BENCHMARK(build_basics  )->Args({7    })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({7, 2 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({7, 4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({7, 6 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({7, 8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({7, 10})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({7, 12})->Unit(benchmark::kMicrosecond);

//
//BENCHMARK(build_basics  )->Args({8    })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({8, 2 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({8, 4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({8, 6 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({8, 8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({8, 10})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_sampling)->Args({8, 12})->Unit(benchmark::kMicrosecond);


BENCHMARK_MAIN();