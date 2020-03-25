//
// Created by alejandro on 15-05-19.
//


#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "../SelfGrammarIndexBS.h"
#include "../bench/repetitive_collections.h"
#include "../bench/utils.h"

#define MAX_DATA 1e6
#define  MAX_Q 1e4
#define len_q 100
#define CODE_DATA 5

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

SelfGrammarIndexBS * idx;
std::string coll = "../tests/collections/repetitive/reals/einstein.en.txt";
std::string data;
std::vector<std::pair<size_t ,size_t >>R;

void generate_random_queries(const size_t &n, const size_t &max_q) {

    std::srand(std::time(nullptr));
    for (int i = 0; i < max_q; ++i) {
        size_t r,l = rand()%n;
        if(l > n/2)
            r = l - len_q;
        else
            r = l + len_q;
        //size_t r = rand()%n;

        if(l > r) swap(l,r);

        R.push_back(std::make_pair(l,r));
    }

}

static void build_idx(benchmark::State &state){

    load_data(MAX_DATA,data,collections_code_inv_dir[CODE_DATA]);
//    data = "abradabracadabra";

    for (auto _ : state) {
        //idx = new SelfGrammarIndexBS();
        ///SelfGrammarIndex *idx = load_idx(1, 0, 3, 1, 1, CODE_DATA);
        sleep(4);
        //idx->build(data);
    }

    generate_random_queries(data.size(),MAX_Q);

}


static void display_trie (benchmark::State &state)
{
    std::string s;
//    SelfGrammarIndex *idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
    SelfGrammarIndex *idx = load_idx(1, 0, CODE_DATA);
    s.resize(len_q);

    for (auto _ : state) {
        uint i = 0;
        for (  auto &ii:R ) {

//            std::cout<<++i<<"[ "<<ii.first<<" , "<<ii.second<<" ]:";
            idx->display_trie(ii.first,ii.second,s);
//            std::cout<<s<<std::endl;
//            std::string real;
//            real.resize(r-l+1);
//
//            std::copy(data.begin()+l,data.begin()+r+1,real.begin());
//            ASSERT_EQ(s, real);
        }
    }
//    std::cout<<"end display trie"<<std::endl;

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
    SelfGrammarIndex *idx = load_idx(1, 0, CODE_DATA);
//    SelfGrammarIndex *idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
    s.resize(len_q);
    for (auto _ : state) {
        uint i = 0;
        for (  auto &ii:R ) {
//            std::cout<<++i<<"[ "<<ii.first<<" , "<<ii.second<<" ]:";
            idx->display(ii.first,ii.second,s);
//            std::cout<<s<<std::endl;
            //std::string real;
            //real.resize(r-l+1);

            //std::copy(data.begin()+l,data.begin()+r+1,real.begin());
            //ASSERT_EQ(s, real);
        }

    }
//    std::cout<<"end display no trie"<<std::endl;
    delete idx;
}
static void display_L_trie (benchmark::State &state)
{

    std::string s;
    SelfGrammarIndex *idx = load_idx(1, 0, CODE_DATA);
    //SelfGrammarIndex *idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
    s.resize(len_q);
    for (auto _ : state) {
        uint i = 0;
        for (  auto &ii:R ) {
      //      std::cout<<++i<<"[ "<<ii.first<<" , "<<ii.second<<" ]:";
            idx->display_L_trie(ii.first,ii.second,s);
      //      std::cout<<s<<std::endl;
            //std::string real;
            //real.resize(r-l+1);

            //std::copy(data.begin()+l,data.begin()+r+1,real.begin());

            ///ASSERT_EQ(s, real);
        }

    }
    //std::cout<<"end display L trie"<<std::endl;
    delete idx;
}

static void display_L_no_trie (benchmark::State &state)
{
    std::string s;
    SelfGrammarIndex *idx = load_idx(1, 0, CODE_DATA);
//    SelfGrammarIndex *idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
    s.resize(len_q);

    for (auto _ : state) {
        uint i = 0;
        for (  auto &ii:R ) {
      //      std::cout<<++i<<"[ "<<ii.first<<" , "<<ii.second<<" ]:";
            idx->display_L(ii.first,ii.second,s);
      //      std::cout<<s<<std::endl;
//            std::string real;
//            real.resize(r-l+1);
//
//            std::copy(data.begin()+l,data.begin()+r+1,real.begin());
//
//            ASSERT_EQ(s, real);
        }

    }
    //std::cout<<"end display L no trie"<<std::endl;
    delete idx;
}
static void display_L_rec (benchmark::State &state)
{

    std::string s;
    SelfGrammarIndex *idx = load_idx(1, 0, CODE_DATA);
//    SelfGrammarIndex *idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
    s.resize(len_q);

    for (auto _ : state) {
        uint i =0;
        for (  auto &ii:R ) {
            //std::cout<<++i<<"[ "<<ii.first<<" , "<<ii.second<<" ]:";
            idx->display_L_rec(ii.first,ii.second,s);
            //std::cout<<s<<std::endl;
//            std::string real;
//            real.resize(r-l+1);
//
//            std::copy(data.begin()+l,data.begin()+r+1,real.begin());
//
//            ASSERT_EQ(s, real);
        }

    }
    //std::cout<<"end display L rec"<<std::endl;
    delete idx;
}

static void display_dfs (benchmark::State &state)
{

    std::string s;
    SelfGrammarIndex *idx = load_idx(1, 0, CODE_DATA);
//    SelfGrammarIndex *idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
    s.resize(len_q);

    for (auto _ : state) {
        uint i =0;
        for (  auto &ii:R ) {
            //std::cout<<++i<<"[ "<<ii.first<<" , "<<ii.second<<" ]:";
            idx->display_dfs(ii.first,ii.second,s);
            //std::cout<<s<<std::endl;
//            std::string real;
//            real.resize(r-l+1);
//
//            std::copy(data.begin()+l,data.begin()+r+1,real.begin());
//
//            ASSERT_EQ(s, real);
        }

    }
    //std::cout<<"end display L rec"<<std::endl;
    delete idx;
}
BENCHMARK(build_idx)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_L_rec)->Unit(benchmark::kMicrosecond);
BENCHMARK(display_dfs)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();