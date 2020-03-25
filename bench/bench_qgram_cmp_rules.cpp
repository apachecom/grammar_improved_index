//
// Created by alejandro on 17-06-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include <slp/Samplings/sampling.h>
#include "../SelfGrammarIndexBSQ.h"

#include "../bench/utils.h"
#include "../bench/repetitive_collections.h"


#define MAX_DATA 1e14
#define CODE_DATA 1
#define MAX_Q 100000
#define len 5

std::string data;
std::map<size_t, std::string> expansions;
std::vector<size_t> R;
std::vector<size_t> L;
SelfGrammarIndexBSQ *idx = new SelfGrammarIndexBSQ();


void generate_random_queries   (const size_t &n, const size_t &max_q) {

    std::srand(std::time(nullptr));
    R.clear();
    L.clear();

    for (int i = 0; i < max_q; ++i) {

        R.push_back(rand() % n + 1);

        L.push_back(rand() % n + 1);

    }
}
void get_expansions_rule       (const uint &code_coll, const uint & max_q) {

    /*LOAD DATA*/
    load_data(MAX_DATA,data,collections_code_inv_dir[code_coll]);
    expansions.clear();

    const auto& _g = idx->get_grammar();
    for (size_t i = 1; i < idx->n_rules(); ++i) {



        auto lim = _g.limits_rule(_g.m_tree[_g.select_occ(i,1)]);

        expansions[i].resize(lim.second-lim.first+1);

        std::copy(data.begin()+lim.first,data.begin()+lim.second,expansions[i].begin());
    }

    generate_random_queries(idx->n_rules(), max_q);

}
static void load_idx           (benchmark::State &state) {

    uint coll_code   = state.range(0);
    uint samp       = state.range(1);

    for (auto _ : state) {
        {
            std::fstream  f(  "../files/new_divided_indices/"+std::to_string(coll_code)+".basic_index",std::ios::in|std::ios::binary);
            std::ifstream fG( "../files/new_divided_indices/"+std::to_string(coll_code)+".qgramSampG_"+std::to_string(samp));
            std::ifstream fGR("../files/new_divided_indices/"+std::to_string(coll_code)+".qgramSampGR_"+std::to_string(samp));

            if(!f.is_open())  std::cout<<"bad file: ../files/new_divided_indices/"+std::to_string(coll_code)+".basic_index\n";
            if(!fG.is_open()) std::cout<<"bad file: ../files/new_divided_indices/"+std::to_string(coll_code)+".qgramSampG_" +std::to_string(samp)+"\n";
            if(!fGR.is_open())std::cout<<"bad file: ../files/new_divided_indices/"+std::to_string(coll_code)+".qgramSampGR_"+std::to_string(samp)+"\n";


            idx->load(f);
//            idx->loadSampling(fG,fGR);
        }
        sleep(3);
    }

    get_expansions_rule(coll_code,1000);

}


static void cmp_qgram_rule_suffix    (benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s1 = expansions[R[ii]];

            auto itera = s1.end()-1;
            auto end = s1.begin()-1;
            int r = idx->match_suffix(R[ii],itera,end);
        }
    }
}
static void cmp_bp_rule_suffix    (benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s1 = expansions[R[ii]];

            auto itera = s1.end()-1;
            auto end = s1.begin()-1;
            int r = idx->bp_cmp_suffix(R[ii],itera,end);
        }
    }
}
static void cmp_dfs_rule_suffix    (benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s1 = expansions[R[ii]];

            auto itera = s1.end()-1;
            auto end = s1.begin()-1;
            int r = idx->dfs_cmp_suffix(R[ii],itera,end);
        }
    }
}
static void cmp_L_rule_suffix    (benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s1 = expansions[R[ii]];

            auto itera = s1.end()-1;
            auto end = s1.begin()-1;
            int r = idx->cmp_suffix_L(R[ii],itera,end);
        }
    }
}
static void cmp_rule_suffix    (benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s1 = expansions[R[ii]];

            auto itera = s1.end()-1;
            auto end = s1.begin()-1;
            int r = idx->cmp_suffix(R[ii],itera,end);
        }
    }
}


static void cmp_qgram_rule_prefix    (benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s1 = expansions[R[ii]];

            auto itera = s1.begin();
            auto end = s1.end();

            int r = idx->match_prefix(R[ii],itera,end);
        }
    }
}
static void cmp_bp_rule_prefix    (benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s1 = expansions[R[ii]];

            auto itera = s1.begin();
            auto end = s1.end();

            int r = idx->bp_cmp_prefix(R[ii],itera,end);
        }
    }
}
static void cmp_dfs_rule_prefix    (benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s1 = expansions[R[ii]];

            auto itera = s1.begin();
            auto end = s1.end();

            int r = idx->dfs_cmp_prefix(R[ii],itera,end);
        }
    }
}
static void cmp_L_rule_prefix    (benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s1 = expansions[R[ii]];

            auto itera = s1.begin();
            auto end = s1.end();

            int r = idx->cmp_prefix_L(R[ii],itera,end);
        }
    }
}
static void cmp_rule_prefix    (benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s1 = expansions[R[ii]];

            auto itera = s1.begin();
            auto end = s1.end();

            int r = idx->cmp_prefix(R[ii],itera,end);
        }
    }
}

BENCHMARK(load_idx)->Args({1,4})->Unit(benchmark::kMicrosecond);

BENCHMARK(cmp_qgram_rule_suffix)->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_bp_rule_suffix)   ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_dfs_rule_suffix)  ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_L_rule_suffix)    ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_rule_suffix)      ->Unit(benchmark::kMicrosecond);

BENCHMARK(cmp_qgram_rule_prefix)->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_bp_rule_prefix)   ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_dfs_rule_prefix)  ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_L_rule_prefix)    ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_rule_prefix)      ->Unit(benchmark::kMicrosecond);



BENCHMARK(load_idx)->Args({2,4})->Unit(benchmark::kMicrosecond);

BENCHMARK(cmp_qgram_rule_suffix)->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_bp_rule_suffix)   ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_dfs_rule_suffix)  ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_L_rule_suffix)    ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_rule_suffix)      ->Unit(benchmark::kMicrosecond);

BENCHMARK(cmp_qgram_rule_prefix)->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_bp_rule_prefix)   ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_dfs_rule_prefix)  ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_L_rule_prefix)    ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_rule_prefix)      ->Unit(benchmark::kMicrosecond);


BENCHMARK(load_idx)->Args({5,4})->Unit(benchmark::kMicrosecond);

BENCHMARK(cmp_qgram_rule_suffix)->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_bp_rule_suffix)   ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_dfs_rule_suffix)  ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_L_rule_suffix)    ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_rule_suffix)      ->Unit(benchmark::kMicrosecond);

BENCHMARK(cmp_qgram_rule_prefix)->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_bp_rule_prefix)   ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_dfs_rule_prefix)  ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_L_rule_prefix)    ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_rule_prefix)      ->Unit(benchmark::kMicrosecond);



BENCHMARK(load_idx)->Args({6,4})->Unit(benchmark::kMicrosecond);

BENCHMARK(cmp_qgram_rule_suffix)->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_bp_rule_suffix)   ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_dfs_rule_suffix)  ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_L_rule_suffix)    ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_rule_suffix)      ->Unit(benchmark::kMicrosecond);

BENCHMARK(cmp_qgram_rule_prefix)->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_bp_rule_prefix)   ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_dfs_rule_prefix)  ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_L_rule_prefix)    ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_rule_prefix)      ->Unit(benchmark::kMicrosecond);



BENCHMARK(load_idx)->Args({7,4})->Unit(benchmark::kMicrosecond);

BENCHMARK(cmp_qgram_rule_suffix)->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_bp_rule_suffix)   ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_dfs_rule_suffix)  ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_L_rule_suffix)    ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_rule_suffix)      ->Unit(benchmark::kMicrosecond);

BENCHMARK(cmp_qgram_rule_prefix)->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_bp_rule_prefix)   ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_dfs_rule_prefix)  ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_L_rule_prefix)    ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_rule_prefix)      ->Unit(benchmark::kMicrosecond);


BENCHMARK(load_idx)->Args({8,4})->Unit(benchmark::kMicrosecond);

BENCHMARK(cmp_qgram_rule_suffix)->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_bp_rule_suffix)   ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_dfs_rule_suffix)  ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_L_rule_suffix)    ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_rule_suffix)      ->Unit(benchmark::kMicrosecond);

BENCHMARK(cmp_qgram_rule_prefix)->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_bp_rule_prefix)   ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_dfs_rule_prefix)  ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_L_rule_prefix)    ->Unit(benchmark::kMicrosecond);
BENCHMARK(cmp_rule_prefix)      ->Unit(benchmark::kMicrosecond);



BENCHMARK_MAIN();

