//
// Created by alpachec on 27-09-18.
//




#include <iostream>
#include <fstream>


#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include "repetitive_collections.h"
#include "../SelfGrammarIndexBS.h"


std::vector<std::string> patt;

SelfGrammarIndexBS* idx;

SelfGrammarIndex::range_search2d grid;
SelfGrammarIndex::grammar_representation G;
uint nrules;
uint N;
std::string data_bs;


auto index_locate_it = [](benchmark::State &st){
    std::srand(std::time(nullptr));

    uint nocc = 0;
    for (auto _ : st)
    {

        for (auto &&  i: patt) {

            sdsl::bit_vector b(data_bs.size(),0);
            idx->locate2(i,b);
            sdsl::bit_vector::rank_1_type rrr(&b);
            nocc = rrr.rank(b.size());
        }


    }
    st.counters["n_occ"] = nocc;
};

auto index_locate_r = [](benchmark::State &st){
    std::srand(std::time(nullptr));
    uint nocc = 0;
    for (auto _ : st)
    {
        for (auto &&  i: patt) {

            sdsl::bit_vector b(data_bs.size(),0);
            idx->locate(i,b);
            sdsl::bit_vector::rank_1_type rrr(&b);
            nocc = rrr.rank(b.size());
        }

    }
    st.counters["n_occ"] = nocc;
};

auto index_expand_rule_bp = [](benchmark::State &st){

    std::srand(std::time(nullptr));

    std::string s;
    s.resize(10);
    size_t l,p;

    for (auto _ : st)
    {
        l = 10,p = 0;
        compressed_grammar::g_long X = std::rand() % nrules + 1;
        idx->bp_expand_prefix(X,s,l,p);


    }
};

auto index_expand_rule_dfuds = [](benchmark::State &st){

    std::srand(std::time(nullptr));

    std::string s,s2;
    s.resize(10);
    size_t l,p;

    for (auto _ : st)
    {
        l = 10,p = 0;
        compressed_grammar::g_long X = std::rand() % nrules + 1;
        idx->expand_prefix(X,s,l,p);
  //      std::cout<<s<<std::endl;

    }
};

int main (int argc, char *argv[] ){

    /*if(argc < 1){
        std::cout<<"bad parameters....";
        return 0;
    }*/


    std::string collection = "../tests/collections/repetitive/reals/einstein.de.txt";//(argv[1]);]

    if(collections_code.find(collection) == collections_code.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
        return 0;
    }


    std::fstream f_c(collection, std::ios::in| std::ios::binary);

    unsigned char buffer[1000];
    while(!f_c.eof()){
        f_c.read((char*)buffer,1000);
        data_bs.append((char*) buffer,f_c.gcount());
    }

    for (int i = 0; i < data_bs.size(); ++i) {
        if(data_bs[i] == 0 || data_bs[i] == 1)
            data_bs[i] = 2;
    }

    f_c.close();

    uint8_t  code = collections_code[collection];

    unsigned int num_patterns = 100;
    //patt.reserve(num_patterns);

    std::string pattern_file = "../files/" + std::to_string(code)+".ptt";
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);

    if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
        return 0;
    }
    std::string buff; uint i = 0;
    while (!f.eof() && std::getline(f, buff) && i < 10000) {
        if(buff.size() > 1){
            patt.push_back(buff);
            i++;
        }
    }

    f.close();

    std::fstream g_f(std::to_string(collections_code[collection])+".gbsidx", std::ios::in | std::ios::binary);
    idx = new SelfGrammarIndexBS();
    idx->load(g_f);
    g_f.close();

    grid = idx->get_grid();
    G = idx->get_grammar();
    nrules = G.n_rules()-1;

    benchmark::RegisterBenchmark("index_expand_rule_bp",index_expand_rule_bp);
    benchmark::RegisterBenchmark("index_expand_rule_dfuds",index_expand_rule_dfuds);
    benchmark::RegisterBenchmark("index_locate_r",index_locate_r);
    benchmark::RegisterBenchmark("index_locate_it",index_locate_it);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


    return 0;

}