//
// Created by alejandro on 23-05-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include <slp/RePairSLPIndex.h>
#include "../SelfGrammarIndexBS.h"

#include "../bench/utils.h"
#include "../bench/repetitive_collections.h"

#include <ri/r_index.hpp>
#include "../utils/build_hyb_lz77.h"
#include <hyb/HybridSelfIndex.h>
#include <slp/RePairSLPIndex.h>



static void measures(benchmark::State &state) {

    int code = state.range(0);
    int ind = state.range(1);
    int s = state.range(2);

    SelfGrammarIndex *idx = load_idx(ind, s, 1, 1, 1, code);

    grammar G;

    std::string data = "";
    load_data(std::numeric_limits<size_t>::max(),data,collections_code_inv_dir[code]);

    for (auto _ : state) {

        fstream fr("../files/"+std::to_string(code)+"repair_g",std::ios::in|std::ios::binary);
        G.load(fr);


        fstream fi("../files/"+std::to_string(code)+"indice_"+std::to_string(code)+ "_" + std::to_string(ind)+"_"+std::to_string(s),std::ios::out|std::ios::binary);
        idx->save(fi);
        // sleep(3);
    }

    size_t ndata = idx->get_grammar().get_size_text();
    state.counters["repair_grammar_rules"] = G.n_rules();
    state.counters["repair_grammar"] = G.get_size();
    state.counters["nrules"] = idx->get_grammar().n_rules();
    state.counters["grammar_size"] = idx->get_grammar().size_of_grammar();
//    state.counters["space"] = idx->size_in_bytes();
    state.counters["data_len"] = data.size();
    state.counters["data_len_idx"] = ndata;
    state.counters["bps"] = (idx->size_in_bytes()/data.size())*8.0;
//    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/ndata;
//    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/ndata;
//    state.counters["tries"] = lt+rt;
//    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();

}



BENCHMARK(measures)  ->Args({1,1,0})->Unit(benchmark::kMicrosecond);
///BENCHMARK(measures)  ->Args({1,3,4})->Unit(benchmark::kMicrosecond);
///BENCHMARK(measures)  ->Args({1,3,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({1,3,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({1,3,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({1,3,64})->Unit(benchmark::kMicrosecond);


BENCHMARK(measures)  ->Args({2,1,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({2,3,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({2,3,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({2,3,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({2,3,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({2,3,64})->Unit(benchmark::kMicrosecond);

BENCHMARK(measures)  ->Args({4,1,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({4,3,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({4,3,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({4,3,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({4,3,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({4,3,64})->Unit(benchmark::kMicrosecond);


BENCHMARK(measures)  ->Args({5,1,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({5,3,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({5,3,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({5,3,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({5,3,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({5,3,64})->Unit(benchmark::kMicrosecond);


BENCHMARK(measures)  ->Args({6,1,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({6,3,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({6,3,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({6,3,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({6,3,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({6,3,64})->Unit(benchmark::kMicrosecond);

BENCHMARK(measures)  ->Args({7,1,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({7,3,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({7,3,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({7,3,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({7,3,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({7,3,64})->Unit(benchmark::kMicrosecond);

BENCHMARK(measures)  ->Args({8,1,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({8,3,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({8,3,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({8,3,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({8,3,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({8,3,64})->Unit(benchmark::kMicrosecond);

//BENCHMARK(measures)  ->Args({4,3,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({4,3,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({4,3,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({4,3,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(measures)  ->Args({4,3,64})->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();






///name	            data_len	grammar_size	nrules	repair_grammar	repair_grammar_rules
///measures/1/1/0	461287000	4069450	        850491	   5780080	           2561120
///measures/2/1/0	205282000	2409460	        433854	   3798100	           1822380
///measures/4/1/0	467627000	212903	        49843	   263962	           100763
///measures/5/1/0	112690000	4342880	        726595	   5627970	           2011670
///measures/6/1/0	154809000	1957370	        425697	   2174650	           642965
///measures/7/1/0	257962000	1374650	        247212	   2185860	           1058260
///measures/8/1/0	429266000	5344480	        1099500	   7338520	           3093540








