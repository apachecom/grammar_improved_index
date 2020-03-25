//
// Created by alejandro on 15-05-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "../SelfGrammarIndexBS.h"

#include "../bench/utils.h"
#include "../bench/repetitive_collections.h"

#define MAX_DATA 1e19
#define  MAX_Q 1e3
#define len_q 10
#define MAX_OCC 1e7

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

SelfGrammarIndex *idx;
std::string data;

std::vector<std::string> R(MAX_Q);
std::vector<std::vector<uint>> P(MAX_Q);

void generate_random_queries(const int & c,const size_t &n, const size_t &max_q) {

//    std::srand(std::time(nullptr));
//    P.clear();
//    for (int i = 0; i < max_q; ++i) {
//        size_t r, l = rand() % n;
//        if (l > n / 2)
//            r = l - len_q;
//        else
//            r = l + len_q;
//        //size_t r = rand()%n;
//
//        if (l > r) swap(l, r);
//
//        std::string patt;
//
//        patt.resize(r - l + 1);
//
//        std::copy(data.begin() + l, data.begin() + r + 1, patt.begin());
//
//        R[i]= patt;
//
//    }

    std::srand(std::time(nullptr));
    R.clear();
    ////////////////reading patterns/////////////

    std::string pattern_file = "../files/patrones/" + std::to_string(c)+"_len_200.ptt";
    std::cout<<pattern_file<<std::endl;
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);
  if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
    }
    std::string buff; uint i = 0;

    while (!f.eof() && std::getline(f, buff) && i < max_q) {
        std::string sss;
        sss.resize(len_q);
        std::copy(buff.begin(),buff.begin()+len_q,sss.begin());
        ///std::cout<<i<<"\t"<<sss<<std::endl;

//        if(sss == N){
//            std::cout<<"patron malo malo \t"<<sss<<std::endl;
//            continue;
//        }

        for (int k = 0; k < sss.size(); ++k){
            if(int(sss[k]) == 0 || int(sss[k]) == 1 || int(sss[k]) == 2 ){
                sss[k] = 65;
                //return 0;
            }
        }

        R.push_back(sss);
        i++;
    }

    std::cout<<R[R.size()-1]<<std::endl;
    f.close();
}
void build_idx(benchmark::State &state) {
////////////0123456789012345
    int code = state.range(0);
    data.clear();
    load_data(MAX_DATA, data, collections_code_inv_dir[code]);


    for (auto _ : state) {
        sleep(4);
    }

    generate_random_queries(code,data.size(), MAX_Q);
    state.counters["space"] = 0;
    state.counters["data_len"] = data.size();
    state.counters["bps"] = 0;
    state.counters["tries"] = 0;
    state.counters["tries bytes"] = 0;
    state.counters["occ"] = 0;
}
static void locate_trie(benchmark::State &state) {
    int code = state.range(0);
    int ind = state.range(1);
    int s = state.range(2);
    size_t occ;
//    idx=load_idx(ind, s,code);
    idx=load_idx(ind, s, 1, 1, 1, code);
//    idx = new SelfGrammarIndexBS();
//    idx->build(data);

    for (auto _ : state) {

        occ = 0;

        for (uint ii=  0; ii < MAX_Q;++ii) {
            std::vector<uint> X;
            //      std::cout<<R[ii]<<std::endl;
            idx->locate(R[ii],X);

            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());

            occ += X.size();


            if(occ > MAX_OCC)break;
        }
    }

    state.counters["occ"] = occ;
    state.counters["space"] = idx->size_in_bytes();
    state.counters["data_len"] = idx->get_grammar().L.size();
    state.counters["bps"] = idx->size_in_bytes()*8.0/idx->get_grammar().L.size();
    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    state.counters["tries"] = lt+rt;
    //std::cout<<lt+rt<<std::endl;
    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();


}
static void locate_no_trie(benchmark::State &state) {

    int code = state.range(0);
    int ind = state.range(1);
    int s = state.range(2);
    size_t occ;
    idx=load_idx(ind, s, 1, 1, 1, code);
    //idx=load_idx(ind, s,code);
//    idx = new SelfGrammarIndexBS();
//    idx->build(data);

    for (auto _ : state) {

        occ = 0;

        for (uint ii=  0; ii < MAX_Q;++ii) {

            std::vector<uint> X;
            idx->locateNoTrie(R[ii],X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();

            if(occ > MAX_OCC)break;
        }
    }
    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/idx->get_grammar().L.size();

    state.counters["occ"] = occ;
    state.counters["space"] = idx->size_in_bytes()-idx->get_grammar().left_path.size_in_bytes()-idx->get_grammar().right_path.size_in_bytes();
    state.counters["data_len"] = data.size();
    state.counters["bps"] = (idx->size_in_bytes()*8.0/idx->get_grammar().L.size())-lt-rt;
    state.counters["tries"] = lt+rt;
    //std::cout<<lt+rt<<std::endl;
    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();

}


static void bt_locate(benchmark::State &state){

    size_t nocc = 0;
    double mean =0;

    for (auto _ : state)
    {
        nocc = 0;
        for (auto &&  i : R )
        {
            uint occs = 0;


            size_t pos = data.find(i, 0);
            while(pos != string::npos)
            {
                ++nocc;
                ++occs;
                pos = data.find(i,pos+1);
            }
            if(nocc > MAX_OCC)break;

        }

    }


    //b_f.close();
    state.counters["occ"] = nocc;
    state.counters["space"] = data.size();
    state.counters["data_len"] = data.size();
    state.counters["bps"] = 8.0;


}

BENCHMARK(build_idx)       ->Args({1})->Unit(benchmark::kMicrosecond);
BENCHMARK(bt_locate)       ->Args({1,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({1,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({1,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({1,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({1,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({1,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({1,3,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({1,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({1,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({1,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({1,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({1,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({1,3,64})->Unit(benchmark::kMicrosecond);


BENCHMARK(build_idx)       ->Args({2})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({2,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({2,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({2,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({2,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({2,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({2,3,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({2,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({2,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({2,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({2,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({2,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({2,3,64})->Unit(benchmark::kMicrosecond);


BENCHMARK(build_idx)    ->Args({4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)  ->Args({4,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)  ->Args({4,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)  ->Args({4,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)  ->Args({4,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)  ->Args({4,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)  ->Args({4,3,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({4,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({4,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({4,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({4,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({4,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({4,3,64})->Unit(benchmark::kMicrosecond);


BENCHMARK(build_idx)    ->Args({5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)  ->Args({5,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)  ->Args({5,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)  ->Args({5,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)  ->Args({5,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)  ->Args({5,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)  ->Args({5,3,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({5,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({5,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({5,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({5,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({5,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({5,3,64})->Unit(benchmark::kMicrosecond);

BENCHMARK(build_idx)       ->Args({6})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({6,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({6,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({6,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({6,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({6,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({6,3,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({6,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({6,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({6,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({6,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({6,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({6,3,64})->Unit(benchmark::kMicrosecond);


BENCHMARK(build_idx)       ->Args({7})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({7,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({7,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({7,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({7,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({7,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({7,3,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({7,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({7,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({7,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({7,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({7,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({7,3,64})->Unit(benchmark::kMicrosecond);

BENCHMARK(build_idx)       ->Args({8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({8,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({8,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({8,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({8,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({8,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)     ->Args({8,3,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({8,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({8,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({8,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({8,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({8,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)  ->Args({8,3,64})->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();