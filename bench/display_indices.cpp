//
// Created by via56 on 03-04-20.
//

#include <iostream>
#include <benchmark/benchmark.h>
#include "../SelfGrammarIndexBS.h"


#define MAX_SAMPLES 1000
#define CHECK_DISPLAY 1

std::vector<size_t> pos;


void load_ptos(const std::string& pos_file){
    std::cout<<pos_file<<std::endl;
    pos.resize(MAX_SAMPLES);
    std::fstream f(pos_file, std::ios::in| std::ios::binary);
    if(!f.is_open()){
        std::cout<<"Error the pos file could not opened!!\n";
    }
    std::string buff; uint i = 0;
    while (i < MAX_SAMPLES && !f.eof() && std::getline(f, buff)) {

        pos[i] = atoi(buff.c_str());
//        std::cout<<i<<" "<<pos[i]<<std::endl;
        ++i;
    }
    f.close();
}

auto gibsdisplay = [](benchmark::State &st, const string &file_index, const uint& len, bool trie = false
){


    std::fstream  f  (file_index+"-bs.gi",std::ios::in|std::ios::binary);
    SelfGrammarIndexBS idx_gibs;
    idx_gibs.load(f);

    uint queries;

    std::string ss = "";

    for (auto _ : st)
    {
        queries = 0;
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < pos.size();++ii) {

            idx_gibs.display_L(pos[ii],pos[ii]+len,ss);
            queries++;
        }

    }

    st.counters["pLen"] = len;
    st.counters["queries"] = queries;
    st.counters["size"] = idx_gibs.size_in_bytes() - idx_gibs.get_grammar().get_right_trie().size_in_bytes() - idx_gibs.get_grammar().get_left_trie().size_in_bytes();

};



auto gibsdisplaybs = [](benchmark::State &st, const string &file_index, const uint& len, bool trie = false
){

    std::fstream  f  (file_index+"-bs.gi",std::ios::in|std::ios::binary);
    SelfGrammarIndexBS idx_gibs;
    idx_gibs.load(f);
    uint queries;

    std::string ss = "";

    for (auto _ : st)
    {
        queries = 0;
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < pos.size();++ii) {
            idx_gibs.display(pos[ii],pos[ii]+len,ss);
            queries++;
        }

    }

    st.counters["pLen"] = len;
    st.counters["queries"] = queries;
    st.counters["size"] = idx_gibs.size_in_bytes() - idx_gibs.get_grammar().get_right_trie().size_in_bytes() - idx_gibs.get_grammar().get_left_trie().size_in_bytes();


};



int main (int argc, char *argv[] ){

    if(argc < 2){
        std::cout<<"bad parameters...."<<std::endl;
        return 0;
    }
    /**
     * collection-name
     */
    std::string index_prefix  = argv[1];
    std::string ptos_file = argv[2];

    uint min_len_patten = std::atoi(argv[3]);
    uint max_len_patten = std::atoi(argv[4]);
    uint gap_len_patten = std::atoi(argv[5]);




    load_ptos(ptos_file+"-"+std::to_string(max_len_patten)+".pos");
    for (uint i = min_len_patten; i <= max_len_patten; i+=gap_len_patten)
    {

        benchmark::RegisterBenchmark("G-INDEX-RANK-PHRASES",gibsdisplay,index_prefix,i)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G_INDEX_BINARY_SEARCH_LEAF",gibsdisplaybs,index_prefix,i)->Unit({benchmark::kMicrosecond});;

    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;

}
