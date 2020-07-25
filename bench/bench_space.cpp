//
// Created by via56 on 24-07-20.
//

#include <iostream>
#include <fstream>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include <cstring>
#include <ctime>
#include "unistd.h"
#include "../SelfGrammarIndexBS.h"
#include "../SelfGrammarIndexPTS.h"

auto patricia_bench = [](benchmark::State &st,const std::string &file,uint32_t smp) {


    std::fstream  file_indice(file+"-pts-<"+std::to_string(smp)+">.gi",std::ios::in|std::ios::binary);
    SelfGrammarIndexPTS idx_gipts(smp);
    idx_gipts.load(file_indice);

    for (auto _ : st)
    {

    }


    st.counters["Text-len"] = idx_gipts.get_grammar().get_size_text();
    st.counters["Total"] = idx_gipts.size_in_bytes();

    st.counters["G"] = idx_gipts.get_grammar().size_in_bytes();
    st.counters["G-X"] = idx_gipts.get_grammar().get_X_size();
    st.counters["G-Pi"] = idx_gipts.get_grammar().get_F_size();
    st.counters["G-Z"] = idx_gipts.get_grammar().get_Z_size();
    st.counters["G-Y"] = idx_gipts.get_grammar().get_Y_size();
    st.counters["G-L"] = idx_gipts.get_grammar().get_L_size();
    st.counters["G-Tree"] = idx_gipts.get_grammar().get_tree_size();
    st.counters["G-Tree+Z"] = idx_gipts.get_grammar().get_tree_size() + idx_gipts.get_grammar().get_Z_size();;


    st.counters["G-Tries"] = idx_gipts.get_grammar().get_compact_trie_left_size() + idx_gipts.get_grammar().get_compact_trie_right_size();
    st.counters["G-Tries-Tree"] = idx_gipts.get_grammar().get_left_trie().get_tree().size_in_bytes() + idx_gipts.get_grammar().get_right_trie().get_tree().size_in_bytes();
    st.counters["G-Tries-Pi"] = sdsl::size_in_bytes(idx_gipts.get_grammar().get_left_trie().get_seq()) +
                                sdsl::size_in_bytes(idx_gipts.get_grammar().get_left_trie().get_seq_inv())+
                                sdsl::size_in_bytes(idx_gipts.get_grammar().get_right_trie().get_seq()) +
                                sdsl::size_in_bytes(idx_gipts.get_grammar().get_right_trie().get_seq_inv());


    st.counters["Grid"] = idx_gipts.get_grid().size_in_bytes();
    st.counters["Grid - L"] = idx_gipts.get_grid().size_in_bytes();
    st.counters["Grid:SL"] = idx_gipts.get_grid().get_SL_size();
    st.counters["Grid:SB"] = idx_gipts.get_grid().get_SB_size();
    st.counters["Grid:XA"] = idx_gipts.get_grid().get_XA_size();
    st.counters["Grid:XB"] = idx_gipts.get_grid().get_XB_size();

    st.counters["Text-len"] = idx_gipts.get_grammar().get_size_text();
    st.counters["Patricia-Trees"] =idx_gipts.get_pt_rules().size_in_bytes() + idx_gipts.get_pt_suffixes().size_in_bytes();;
    st.counters["Patricia-Tree-Rules"] = idx_gipts.get_pt_rules().size_in_bytes();
    st.counters["Patricia-Tree-Rules-Tree"] = idx_gipts.get_pt_rules().get_tree().size_in_bytes();
    st.counters["Patricia-Tree-Rules-Seq"] = sdsl::size_in_bytes(idx_gipts.get_pt_rules().get_seq());
    st.counters["Patricia-Tree-Rules-Jmp"] = sdsl::size_in_bytes(idx_gipts.get_pt_rules().get_jumps());

    st.counters["Patricia-Tree-Sfx"] = idx_gipts.get_pt_suffixes().size_in_bytes();
    st.counters["Patricia-Tree-Sfx-Tree"] = idx_gipts.get_pt_suffixes().get_tree().size_in_bytes();
    st.counters["Patricia-Tree-Sfx-Seq"] = sdsl::size_in_bytes(idx_gipts.get_pt_suffixes().get_seq());
    st.counters["Patricia-Tree-Sfx-Jmp"] = sdsl::size_in_bytes(idx_gipts.get_pt_suffixes().get_jumps());


};
auto basic_bench = [](benchmark::State &st,const std::string &file) {

    std::fstream file_indice(file + "-bs.gi",std::ios::in);
    SelfGrammarIndexBS bs;
    bs.load(file_indice);

    for (auto _ : st)
    {

    }

    st.counters["Text-len"] = bs.get_grammar().get_size_text();
    st.counters["Total"] = bs.size_in_bytes();
    st.counters["G"] = bs.get_grammar().size_in_bytes();
    st.counters["G-X"] = bs.get_grammar().get_X_size();
    st.counters["G-Pi"] = bs.get_grammar().get_F_size();
    st.counters["G-Z"] = bs.get_grammar().get_Z_size();
    st.counters["G-Y"] = bs.get_grammar().get_Y_size();
    st.counters["G-L"] = bs.get_grammar().get_L_size();
    st.counters["G-Tree-Z"] = bs.get_grammar().get_tree_size()+bs.get_grammar().get_Z_size();;
    st.counters["G-Tree"] = bs.get_grammar().get_tree_size();
    st.counters["G-Tries"] = bs.get_grammar().get_compact_trie_left_size() + bs.get_grammar().get_compact_trie_right_size();
    st.counters["G-Tries-Tree"] = bs.get_grammar().get_left_trie().get_tree().size_in_bytes() + bs.get_grammar().get_right_trie().get_tree().size_in_bytes();
    st.counters["G-Tries-Pi"] = sdsl::size_in_bytes(bs.get_grammar().get_left_trie().get_seq()) +
                                  sdsl::size_in_bytes(bs.get_grammar().get_left_trie().get_seq_inv())+
                                    sdsl::size_in_bytes(bs.get_grammar().get_right_trie().get_seq()) +
                                    sdsl::size_in_bytes(bs.get_grammar().get_right_trie().get_seq_inv());

    st.counters["Grid size"] = bs.get_grid().size_in_bytes();
    st.counters["Grid size"] = bs.get_grid().get_SL_size();
    st.counters["Grid size"] = bs.get_grid().get_SB_size();
    st.counters["Grid size"] = bs.get_grid().get_XA_size();
    st.counters["Grid size"] = bs.get_grid().get_XB_size();



};


auto first_test = [](benchmark::State &st){

    for (auto _ : st)
    {
        sleep(3);
    }

    st.counters["Size"] = 0;

};


int main (int argc, char *argv[] ){

//    std::cout<<argc<<std::endl;

    if(argc < 2){
        std::cout<<"bad parameters...."<<std::endl;
        return 0;
    }

    std::string file  = argv[1];
//    benchmark::RegisterBenchmark("SPACE-G-INDEX",  basic_bench, file);
    benchmark::RegisterBenchmark("SPACE-G-INDEX-PT-2",  patricia_bench, file, 2);
    benchmark::RegisterBenchmark("SPACE-G-INDEX-PT-4",  patricia_bench, file, 4);
    benchmark::RegisterBenchmark("SPACE-G-INDEX-PT-8",  patricia_bench, file, 8);
    benchmark::RegisterBenchmark("SPACE-G-INDEX-PT-16",  patricia_bench, file, 16);
    benchmark::RegisterBenchmark("SPACE-G-INDEX-PT-32",  patricia_bench, file, 32);
    benchmark::RegisterBenchmark("SPACE-G-INDEX-PT-64",  patricia_bench, file, 64);

//    benchmark::RegisterBenchmark("First Test",  first_test);



    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;

}