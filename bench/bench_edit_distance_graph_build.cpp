//
// Created by ale on 01-06-21.
//


#include <iostream>
#include <benchmark/benchmark.h>
#include "../SelfGrammarIndexBS.h"
#include "utils.h"

std::string data = "";

auto build_graph = [](benchmark::State &st, const string &file_index, const uint& len,const uint& miss){
    /**
     * load gibsindex
     * */
    std::fstream  f  (file_index+"-bs.gi",std::ios::in|std::ios::binary);
    SelfGrammarIndexBS idx_gibs;
    idx_gibs.load(f);
    uint64_t edges = 0;
    for (auto _ : st)
    {
        edges = idx_gibs.estimate_size_of_ed_graph(len,miss);
    }

    st.counters["size"] = idx_gibs.size_in_bytes();
    st.counters["bps"] = idx_gibs.size_in_bytes()*8.0/data.size();
    st.counters["edges"] = edges;
    st.counters["estimate k2-tree space"] = edges;
};



int main (int argc, char *argv[] ){

    if(argc < 2){
        std::cout<<"bad parameters...."<<std::endl;
        return 0;
    }

    std::string collection  = argv[1];

    std::string path_out    = argv[2];

    std::cout<<"INV_PI_T: "<<INV_PI_T<<std::endl;
    std::cout<<"INV_PI_T_TRIE: "<<INV_PI_T_TRIE<<std::endl;
    std::cout<<"INV_PI_T_QGRAM: "<<INV_PI_T_QGRAM<<std::endl;

    load_data(1e9,data,collection);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;

}