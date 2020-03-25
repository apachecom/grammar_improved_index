
//
// Created by alejandro on 21-05-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "../SelfGrammarIndexBS.h"
#include "utils.h"
#include "repetitive_collections.h"

#define MAX_DATA 1e19
#define  MAX_Q 1000
#define MAX_OCC 1e6

int len_q = 0;
int _code = 0;
using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

std::vector<string> coll = {
        "../tests/collections/repetitive/reals/einstein.en.txt",
        "../tests/collections/repetitive/reals/Escherichia_Coli"
};

std::string data;
std::map< int,std::vector<std::string> > R;

void generate_random_queries(const size_t &max_q, const int& l, const int& c) {

    std::srand(std::time(nullptr));

    ////////////////reading patterns/////////////
    R[len_q].clear();

    std::string pattern_file = "../files/patrones/" + std::to_string(c)+"_len_200.ptt";
//    std::cout<<"patterns: "<<pattern_file<<std::endl;
//    std::cout<<"collection"<<c<<std::endl;
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);
    if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
    }
    std::string buff; uint i = 0;

    while (!f.eof() && std::getline(f, buff)) {
        std::string sss;
        sss.resize(len_q);
        std::copy(buff.begin(),buff.begin()+len_q,sss.begin());
//        std::cout<<i<<"\t"<<sss<<std::endl;

        for (int k = 0; k < sss.size(); ++k){
            if(int(sss[k]) == 0 || int(sss[k]) == 1 || int(sss[k]) == 2 ){
                sss[k] = 65;
                //return 0;
            }
        }

        R[len_q].push_back( sss );
        i++;
        if(i > max_q) break;
    }
    f.close();



}

static void build_idx(benchmark::State &state){


    len_q = state.range(0);
    _code  = state.range(1);
    data = "";

    load_data(MAX_DATA,data,collections_code_inv_dir[_code]);

    for (auto _ : state) {
        sleep(4);
    }

    generate_random_queries(MAX_Q,len_q,_code);

    state.counters["space"] = 0;
    state.counters["data_len"] = data.size();
    state.counters["bps"] = 0;
    state.counters["tries"] = 0;
    state.counters["tries bytes"] = 0;
    state.counters["nocc"] = 0;

//
//    std::cout<<coll[_code]<<std::endl;
//    std::cout<<len_q<<std::endl;

}


static void locate_trie (benchmark::State &state)
{
    int op_idx  = state.range(0);
    int op_s  = state.range(1);
    int op_coll  = state.range(2);
    std::string s;
//    SelfGrammarIndex *idx = load_idx(op_idx, op_s, 1, 1, 1,op_coll);
    SelfGrammarIndex *idx = load_idx(op_idx, op_s,op_coll);
    s.resize(len_q);
    size_t  nocc ;
    for (auto _ : state) {
        nocc = 0;
        for (  auto &ii:R[len_q] ) {
            std::vector<uint> X;
            idx->locate(ii, X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            nocc += X.size();

            if(nocc > MAX_OCC)break;
        }
    }

    size_t len = idx->get_grammar().L.size();
    state.counters["space"] = idx->size_in_bytes();
    state.counters["data_len"] = len;
    state.counters["bps"] = idx->size_in_bytes()*8.0/len;
    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/len;
    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/len;
    state.counters["tries"] = lt+rt;
    //std::cout<<lt+rt<<std::endl;
    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();
    state.counters["nocc"] = nocc;

    delete idx;
}
static void locate_no_trie (benchmark::State &state)
{
    uint op_idx  = state.range(0);
    uint op_s  = state.range(1);
    uint op_coll  = state.range(2);
    std::string s;
//    SelfGrammarIndex *idx = load_idx(op_idx, op_s, 1, 1, 1, op_coll);
    SelfGrammarIndex *idx = load_idx(op_idx, op_s,op_coll);
    s.resize(len_q);
    size_t nocc ;
    for (auto _ : state) {
        nocc = 0;
        for (  auto &ii:R[len_q] ) {
            std::vector<uint> X;
            idx->locateNoTrie(ii, X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            nocc += X.size();

            if(nocc > MAX_OCC)break;
        }

    }

    size_t len = idx->get_grammar().L.size();
    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/len;
    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/len;
    state.counters["space"] = idx->size_in_bytes() - idx->get_grammar().left_path.size_in_bytes() - idx->get_grammar().right_path.size_in_bytes();
    state.counters["data_len"] = len;
    state.counters["bps"] = (idx->size_in_bytes()*8.0/len )- lt -rt;
    state.counters["tries"] = lt+rt;
    //std::cout<<lt+rt<<std::endl;
    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();
    state.counters["nocc"] = nocc;


    delete idx;
}

//
////
//BENCHMARK(build_idx)     ->Args({5  ,   1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({10  ,  1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({20 ,   1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({30 ,   1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({40 ,   1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({50 ,   1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({100,   1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,1})->Unit(benchmark::kMicrosecond);
//
///////////////////////////////////////////////////////////////////////////////
//BENCHMARK(build_idx)     ->Args({5  ,   2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({10  ,  2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({20 ,   2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({30 ,   2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({40 ,   2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({50 ,   2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({100,   2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,2})->Unit(benchmark::kMicrosecond);
/////////////////////////////////////////////////////////////////////////////
BENCHMARK(build_idx)     ->Args({5  ,   4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({10  ,  4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({20 ,   4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({30 ,   4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({40 ,   4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({50 ,   4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({100,   4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,4})->Unit(benchmark::kMicrosecond);
/////////////////////////////////////////////////////////////////////////////
BENCHMARK(build_idx)     ->Args({5  ,   5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({10  ,  5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({20 ,   5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({30 ,   5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({40 ,   5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({50 ,   5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({100,   5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({1  ,0, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,4, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,8, 5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)   ->Args({3  ,64,5})->Unit(benchmark::kMicrosecond);
////////////////////////////////////////////////////////////////////////////////
//BENCHMARK(build_idx)     ->Args({5  ,   6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({10  ,  6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({20 ,   6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({30 ,   6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({40 ,   6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({50 ,   6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({100,   6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,6})->Unit(benchmark::kMicrosecond);
//////////////////////////////////////////////////////////////////////////////
//BENCHMARK(build_idx)     ->Args({5  ,   7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({10  ,  7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({20 ,   7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({30 ,   7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({40 ,   7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({50 ,   7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({100,   7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,7})->Unit(benchmark::kMicrosecond);
//////////////////////////////////////////////////////////////////////////////
//BENCHMARK(build_idx)     ->Args({5  ,   8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({10  ,  8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({20 ,   8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({30 ,   8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({40 ,   8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({50 ,   8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({100,   8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({1  ,0, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,4, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,8, 8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)   ->Args({3  ,64,8})->Unit(benchmark::kMicrosecond);
////////////////////////////////////////////////////////////////////////////////

BENCHMARK_MAIN();