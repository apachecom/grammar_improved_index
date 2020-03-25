//
// Created by alejandro on 21-05-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "utils.h"
#include "repetitive_collections.h"

#define MAX_DATA 1e6
#define  MAX_Q 1000



std::vector<range> Q;

int len_q = 0;
int _code = 0;
using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

std::vector<string> coll = {"../tests/collections/repetitive/reals/einstein.en.txt",
                            "../tests/collections/repetitive/reals/Escherichia_Coli"};

std::string data;
std::map< int,std::vector<std::string> > R;

void generate_random_queries(const size_t &max_q, const int& l, const int& c) {

    std::srand(std::time(nullptr));
    R.clear();
    ////////////////reading patterns/////////////

    std::string pattern_file = "../files/patrones/" + std::to_string(c)+"_len_200.ptt";
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);
    if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
    }
    std::string buff; uint i = 0;

    while (!f.eof() && std::getline(f, buff) && i < max_q) {
        std::string sss;
        sss.resize(l);
        std::copy(buff.begin(),buff.begin()+l,sss.begin());
        ///std::cout<<i<<"\t"<<sss<<std::endl;

        if(sss == "NNNNNNNNNN"){
            ////std::cout<<"patron malo malo \t"<<sss<<std::endl;
            continue;
        }

        for (int k = 0; k < sss.size(); ++k){
            if(int(sss[k]) == 0 || int(sss[k]) == 1 || int(sss[k]) == 2 ){
                sss[k] = 65;
                //return 0;

            }

        }
        R[len_q].push_back(sss);
        i++;
    }

    std::cout<<R[len_q][i-1]<<std::endl;
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
    state.counters["ptt"] = R[len_q].size();

//
//    std::cout<<coll[_code]<<std::endl;
//    std::cout<<len_q<<std::endl;

}

static void write_ranges(benchmark::State &state)
{

    for (auto _ : state) {

        fstream fr(std::to_string(_code) + "_" + std::to_string(len_q)+"_"+"ranges", std::ios::out|std::ios::binary);

        for (  auto && i: Q ) {
            fr << i.x1 << " " << i.x2<< " " << i.y1 << " " << i.y2 << " " << i.len <<"\n";
        }

    }
    Q.clear();

}
static void find_ranges_trie (benchmark::State &state)
{
    int op_idx  = state.range(0);
    int op_s  = state.range(1);

    fstream fr(std::to_string(_code) + "_" + std::to_string(len_q)+"_"+"ranges", std::ios::out|std::ios::binary);

    std::string s;
    SelfGrammarIndex *idx = load_idx(op_idx, op_s, 1, 1, 1, _code);
//    SelfGrammarIndex *idx = load_idx(op_idx, op_s,_code);

    s.resize(len_q);
    size_t  nocc = 0;

    for (auto _ : state) {
        Q.clear();
        for (  auto &ii:R[len_q] ) {
            std::vector<uint> X;
            idx->find_ranges_trie(ii, X, Q);
        }
    }

    state.counters["space"] = idx->size_in_bytes();
    state.counters["data_len"] = data.size();
    state.counters["bps"] = idx->size_in_bytes()*8.0/data.size();
    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/data.size();
    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/data.size();
    state.counters["tries"] = lt+rt;
    //std::cout<<lt+rt<<std::endl;
    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();
    state.counters["nocc"] = nocc;
    state.counters["ptt"] = R[len_q].size();

    delete idx;
}
static void find_ranges_dfs (benchmark::State &state)
{
    int op_idx  = state.range(0);
    int op_s  = state.range(1);
    std::string s;

    SelfGrammarIndex *idx = load_idx(op_idx, op_s, 1, 1, 1, _code);
//    SelfGrammarIndex *idx = load_idx(op_idx, op_s,_code);
    s.resize(len_q);
    size_t nocc = 0;

    for (auto _ : state) {
        for (  auto &ii:R[len_q] ) {
            std::vector<uint> X;
            idx->find_ranges_dfs(ii, X);
        }
    }


    state.counters["space"] = idx->size_in_bytes();
    state.counters["data_len"] = data.size();
    state.counters["bps"] = idx->size_in_bytes()*8.0/data.size();
    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/data.size();
    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/data.size();
    state.counters["tries"] = lt+rt;
    //std::cout<<lt+rt<<std::endl;
    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();
    state.counters["nocc"] = nocc;
    state.counters["ptt"] = R[len_q].size();


    delete idx;

}
static void find_ranges_no_trie (benchmark::State &state)
{
    int op_idx  = state.range(0);
    int op_s  = state.range(1);
    std::string s;

    SelfGrammarIndex *idx = load_idx(op_idx, op_s, 1, 1, 1, _code);
//    SelfGrammarIndex *idx = load_idx(op_idx, op_s,_code);
    s.resize(len_q);
    size_t nocc = 0;

    for (auto _ : state) {
        for (  auto &ii:R[len_q] ) {
            std::vector<uint> X;
            idx->find_ranges(ii, X);
        }
    }


    state.counters["space"] = idx->size_in_bytes();
    state.counters["data_len"] = data.size();
    state.counters["bps"] = idx->size_in_bytes()*8.0/data.size();
    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/data.size();
    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/data.size();
    state.counters["tries"] = lt+rt;
    //std::cout<<lt+rt<<std::endl;
    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();
    state.counters["nocc"] = nocc;
    state.counters["ptt"] = R[len_q].size();


    delete idx;
}
//
//BENCHMARK(build_idx)     ->Args({5  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//
//BENCHMARK(build_idx)     ->Args({10  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//
//BENCHMARK(build_idx)     ->Args({20  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
////BENCHMARK(write_ranges)->Unit(benchmark::kMicrosecond);
//
//
//BENCHMARK(build_idx)     ->Args({30  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
////BENCHMARK(write_ranges)->Unit(benchmark::kMicrosecond);
//
//
//BENCHMARK(build_idx)     ->Args({40  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
////BENCHMARK(write_ranges)->Unit(benchmark::kMicrosecond);
//
//
//BENCHMARK(build_idx)     ->Args({50  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
////BENCHMARK(write_ranges)->Unit(benchmark::kMicrosecond);
//
//
//BENCHMARK(build_idx)     ->Args({100  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(write_ranges)->Unit(benchmark::kMicrosecond);
//
//
//

/////////////////////////////////////////////
//
//BENCHMARK(build_idx)     ->Args({5  ,5})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//
//BENCHMARK(build_idx)     ->Args({10  ,5})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//
//BENCHMARK(build_idx)     ->Args({20  ,5})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
////BENCHMARK(write_ranges)->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({30  ,5})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
////BENCHMARK(write_ranges)->Unit(benchmark::kMicrosecond);
//BENCHMARK(build_idx)     ->Args({40  ,5})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(write_ranges)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({50  ,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_dfs)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(write_ranges)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)     ->Args({100  ,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_dfs)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_no_trie)->Args({1  ,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_trie)   ->Args({1  ,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_no_trie)->Args({3  ,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_trie)   ->Args({3  ,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_no_trie)->Args({3  ,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_trie)   ->Args({3  ,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_no_trie)->Args({3  ,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(find_ranges_trie)   ->Args({3  ,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(write_ranges)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();






