//
// Created by alejandro on 21-05-19.
//


#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "../SelfGrammarIndexBS.h"
#include "utils.h"
#include "repetitive_collections.h"

#define MAX_DATA 1e6
#define  MAX_Q 1e4
#define CODE_DATA 4


int len_q = 0;
int code = 0;
using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

SelfGrammarIndexBS * idx;
std::vector<string> coll = {"../tests/collections/repetitive/reals/einstein.en.txt",
                      "../tests/collections/repetitive/reals/Escherichia_Coli"};

std::string data;
std::vector<std::pair<size_t ,size_t >>R;

void generate_random_queries(const size_t &n, const size_t &max_q) {

    R.clear();

    std::string rg_file = "../files/";

    std::fstream f(rg_file + "rangos/"+std::to_string(code)+"_"+std::to_string(len_q)+".rg", std::ios::in| std::ios::binary);
    std::cout<<rg_file + "rangos/"+std::to_string(code)+"_"+std::to_string(len_q)+".rg"<<std::endl;

    if(!f.is_open()){
        std::cout<<"Error the ranges file could not opened!!\n";
        return;
    }
    std::string buff; uint i = 0;

    while (!f.eof() && std::getline(f, buff) && i < MAX_Q) {
        if(buff.size() > 1){
            uint l,r;
            sscanf(buff.c_str(),"%u %u",&l,&r);
            R.emplace_back(l,r);
            i++;
//            std::cout<<l<<" "<<r<<std::endl;
        }
    }
    f.close();

    //    for (int i = 0; i < max_q; ++i) {
    //        size_t r,l = rand()%n;
    //        if(l > n/2)
    //            r = l - len_q;
    //        else
    //            r = l + len_q;
    //        //size_t r = rand()%n;
    //
    //        if(l > r) swap(l,r);
    //
    //        R.push_back(std::make_pair(l,r));
    //    }

    if(R.size() == 0) std::cout<<"Error leyendo el fichero\n";
}

static void build_idx(benchmark::State &state){


    len_q = state.range(0);
    code  = state.range(1);
    data = "";
    load_data(MAX_DATA,data,collections_code_inv_dir[code]);
    for (auto _ : state) {
        sleep(4);
    }
    generate_random_queries(data.size(),MAX_Q);

//    std::cout<<coll[code]<<std::endl;
//    std::cout<<len_q<<std::endl;
    state.counters["space"] = 0;
    state.counters["data_len"] = data.size();
    state.counters["bps"] = 0;
    state.counters["tries"] = 0;
    state.counters["tries bytes"] = 0;

}


static void display_trie (benchmark::State &state)
{
    std::string s;
//    SelfGrammarIndex *idx = load_idx(1, 0, 1, 1, 1, code);
    SelfGrammarIndex *idx = load_idx(1, 0,code);
    s.resize(len_q);

    for (auto _ : state) {
        uint i = 0;
        for (  auto &ii:R ) {
            idx->display_trie(ii.first,ii.second,s);
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


    delete idx;
}
static void display_no_trie (benchmark::State &state)
{

    std::string s;
//    SelfGrammarIndex *idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
    SelfGrammarIndex *idx = load_idx(1, 0,code);
    s.resize(len_q);
    for (auto _ : state) {
        uint i = 0;
        for (  auto &ii:R ) {
            idx->display(ii.first,ii.second,s);
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

    delete idx;
}
static void display_L_trie (benchmark::State &state)
{

    std::string s;
//    SelfGrammarIndex *idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
    SelfGrammarIndex *idx = load_idx(1, 0,code);
s.resize(len_q);
    for (auto _ : state) {
        uint i = 0;
        for (  auto &ii:R ) {
            idx->display_L_trie(ii.first,ii.second,s);
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


    delete idx;
}

static void display_L_no_trie (benchmark::State &state)
{
    std::string s;
//    SelfGrammarIndex *idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
    SelfGrammarIndex *idx = load_idx(1, 0,code);
s.resize(len_q);

    for (auto _ : state) {
        uint i = 0;
        for (  auto &ii:R ) {
            idx->display_L(ii.first,ii.second,s);

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

    delete idx;
}
static void display_L_rec (benchmark::State &state)
{

    std::string s;
//    SelfGrammarIndex *idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);

    SelfGrammarIndex *idx = load_idx(1, 0,code);
s.resize(len_q);

    for (auto _ : state) {
        uint i =0;
        for (  auto &ii:R ) {
            idx->display_L_rec(ii.first,ii.second,s);

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

    delete idx;
}
BENCHMARK(build_idx)->Args({5,1})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({10,1})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({20,1})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({30,1})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({50,1})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({100,1})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);

BENCHMARK(build_idx)->Args({5,2})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({10,2})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({20,2})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({30,2})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({50,2})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({100,2})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);


BENCHMARK(build_idx)->Args({5,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({10,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({20,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({30,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({50,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({100,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);


BENCHMARK(build_idx)->Args({5,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({10,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({20,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({30,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({50,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({100,5})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);

BENCHMARK(build_idx)->Args({5,6})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({10,6})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({20,6})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({30,6})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({50,6})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({100,6})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);

BENCHMARK(build_idx)->Args({5,7})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({10,7})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({20,7})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({30,7})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({50,7})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({100,7})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);

BENCHMARK(build_idx)->Args({5,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({10,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({20,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({30,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({50,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(build_idx)->Args({100,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);


BENCHMARK_MAIN();
