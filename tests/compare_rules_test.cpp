//
// Created by alpachec on 21-05-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>

#include "../bench/utils.h"

#define MAX_DATA 1e3
#define CODE_DATA 4

std::string coll = "../tests/collections/repetitive/reals/einstein.en.txt";
std::string data;
std::map<size_t, std::string> expansions;
std::vector<size_t> R;
std::vector<size_t> L;
SelfGrammarIndex *idx;

void generate_random_queries(const size_t &n, const size_t &max_q) {
    std::srand(std::time(nullptr));
    R.clear();
    L.clear();
    for (int i = 0; i < max_q; ++i) {
        R.push_back(rand() % n + 1);
        L.push_back(rand() % n + 1);
    }

}

void get_expansions_rule() {

    /*LOAD DATA*/
    std::string d1 = "";
    load_data(MAX_DATA,d1,coll);
    data.resize(MAX_DATA);
    std::copy(d1.begin(),d1.begin()+MAX_DATA,data.begin());
//    data = "abradabracabradabracadabraabradabracadabraadabra";
    idx = new SelfGrammarIndexBS();
    idx->build(data);
    //
    /*build temporal grammar*/

    grammar G;
    G.buildRepair(data);
//    fstream f("../files/new_divided_indices/bal/"+std::to_string(CODE_DATA)+"bal_repair_g",std::ios::in|std::ios::binary);

//    G.load(f);
    //G.print(data);
    expansions.clear();
    for (size_t i = 1; i <= G.n_rules(); ++i) {
        std::string tt;
        tt.resize(G[i].r - G[i].l + 1);
        std::copy(data.begin() + G[i].l, data.begin() + G[i].r + 1, tt.begin());
        expansions[i] = tt;
    }

    generate_random_queries(G.n_rules(), 1000);

    std::cout << "TEST DATA GENERATED\n";
}

static void build_idx(benchmark::State &state){


    for (auto _ : state) {
        get_expansions_rule();
        sleep(1);
        //idx->build(data);
    }
}

static void compare_rule_trie(benchmark::State &state) {
//    idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
//    idx = load_idx(1, 0, CODE_DATA);

    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s1 = expansions[R[ii]];
            std::string s2 = expansions[L[ii]];
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->bp_cmp_prefix(R[ii],iter,iend);
            int r2 = std::strcmp(s2.c_str(),s1.c_str()) ;
            if(r2 > 0) r2 = 1;
            if(r2 < 0 ) r2 = -1;

            ASSERT_EQ(r1, r2);
        }
    }
    delete idx;
}

static void compare_rule_NoTrie(benchmark::State &state) {
//    idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
//    idx = load_idx(1, 0, CODE_DATA);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s1 = expansions[R[ii]];
            std::string s2 = expansions[L[ii]];
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->cmp_prefix(R[ii],iter,iend);
            int r2 = std::strcmp(s2.c_str(),s1.c_str()) ;
            if(r2 > 0) r2 = 1;
            if(r2 < 0 ) r2 = -1;

            ASSERT_EQ(r1, r2);
        }
    }
    delete idx;
}

static void compare_rule_NoTrie_L(benchmark::State &state) {
    //idx = new SelfGrammarIndexBS();
    //idx->build(data);
//    idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
//    idx = load_idx(1, 0, CODE_DATA);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s1 = expansions[R[ii]];
            if(s1.size()==1) continue;
            std::string s2 = expansions[L[ii]];
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->cmp_prefix_L(R[ii],iter,iend);
            int r2 = 0;

            if(r1 == 0 && iter!=iend) r1 = 1;

            if(s1.size() > s2.size()){
                std::string s3;
                s3.resize(s2.size());
                std::copy(s1.begin(),s1.begin()+s2.size(),s3.begin());
                r2 = std::strcmp(s2.c_str(),s3.c_str());
            } else
                r2 = std::strcmp(s2.c_str(),s1.c_str()) ;

            if(r2 > 0) r2 = 1;
            if(r2 < 0 ) r2 = -1;

            if(r1!=r2){
                 iter  = s2.begin();
                 iend  = s2.end();
                 r1 = idx->cmp_prefix_L(R[ii],iter,iend);
            }

            EXPECT_EQ(r1, r2);
        }
    }
    delete idx;
}

static void compare_srule_NoTrie_L(benchmark::State &state) {
//    idx = new SelfGrammarIndexBS();
//    idx->build(data);
//    idx = load_idx(1, 0, 1, 1, 1, CODE_DATA);
//    idx = load_idx(1, 0, CODE_DATA);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s1 = expansions[R[ii]];
            if(s1.size()==1) continue;
            std::string s2 = expansions[L[ii]];
            auto iter  = s2.begin()-1;
            auto iend  = s2.end()-1;
            int r1 = idx->cmp_suffix_L(R[ii],iend,iter);
            int r2 = 0;

            if(r1 == 0 && iter!=iend) r1 = 1;

            if(s1.size() > s2.size()){
                std::string s3,s4;
                s3.resize(s2.size());
                s4.resize(s2.size());
                std::copy(s1.end()-s2.size(),s1.end(),s3.begin());
                std::copy(s2.begin(),s2.end(),s4.begin());
                std::reverse(s4.begin(),s4.end());
                std::reverse(s3.begin(),s3.end());
                r2 = std::strcmp(s4.c_str(),s3.c_str());
            } else{

                std::string s3,s4;
                s3.resize(s1.size());
                s4.resize(s1.size());
                std::copy(s2.end()-s1.size(),s2.end(),s4.begin());
                std::copy(s1.begin(),s1.end(),s3.begin());
                std::reverse(s4.begin(),s4.end());
                std::reverse(s3.begin(),s3.end());
                r2 = std::strcmp(s4.c_str(),s3.c_str());
            }

            if(r2 == 0 && s1.size() > s2.size()) r2 = 0;
            if(r2 == 0 && s1.size() < s2.size()) r2 = 1;
            if(r2 > 0) r2 = 1;
            if(r2 < 0 ) r2 = -1;

            if(r1!=r2){
                iter  = s2.begin()-1;
                iend  = s2.end()-1;
                r1 = idx->cmp_suffix_L(R[ii],iend,iter);
            }

            EXPECT_EQ(r1, r2);
        }
    }
    delete idx;
}

static void compare_rule_dfs(benchmark::State &state){
//    idx = load_idx(1, 0, CODE_DATA);

    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s1 = expansions[R[ii]];
            std::string s2 = expansions[L[ii]];

            auto iter  = s2.begin();
            auto iend  = s2.end();

            int r1 = idx->dfs_cmp_prefix(R[ii],iter,iend);

            if(r1 == 0 && iter != iend)
                r1 = 1;

            std::string s3;
            int r2 = 0;
            if(s1.size() > s2.size()){
                s3.resize(s2.size());
                std::copy(s1.begin(),s1.begin()+s2.size(),s3.begin());
                r2 = std::strcmp(s2.c_str(),s3.c_str()) ;

            }else{
                r2 = std::strcmp(s2.c_str(),s1.c_str()) ;
            }


            if(r2 < 0) r2 = -1;
            if(r2 > 0) r2 = 1;
            if(r2 != r1)
            {
                 iter  = s2.begin();
                 iend  = s2.end();

                 r1 = idx->dfs_cmp_prefix(R[ii],iter,iend);
            }
            ASSERT_EQ(r1, r2);
        }
    }
    //delete idx;
}
static void compare_srule_dfs(benchmark::State &state){
//    idx = load_idx(1, 0, CODE_DATA);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s1 = expansions[R[ii]];
            if(s1.size()==1) continue;
            std::string s2 = expansions[L[ii]];
            auto iter  = s2.begin()-1;
            auto iend  = s2.end()-1;
            int r1 = idx->dfs_cmp_suffix(R[ii],iend,iter);
            int r2 = 0;

            if(r1 == 0 && iter!=iend) r1 = 1;

            if(s1.size() > s2.size()){
                std::string s3,s4;
                s3.resize(s2.size());
                s4.resize(s2.size());
                std::copy(s1.end()-s2.size(),s1.end(),s3.begin());
                std::copy(s2.begin(),s2.end(),s4.begin());
                std::reverse(s4.begin(),s4.end());
                std::reverse(s3.begin(),s3.end());
                r2 = std::strcmp(s4.c_str(),s3.c_str());
            } else{

                std::string s3,s4;
                s3.resize(s1.size());
                s4.resize(s1.size());
                std::copy(s2.end()-s1.size(),s2.end(),s4.begin());
                std::copy(s1.begin(),s1.end(),s3.begin());
                std::reverse(s4.begin(),s4.end());
                std::reverse(s3.begin(),s3.end());
                r2 = std::strcmp(s4.c_str(),s3.c_str());
            }

            if(r2 == 0 && s1.size() > s2.size()) r2 = 0;
            if(r2 == 0 && s1.size() < s2.size()) r2 = 1;
            if(r2 > 0) r2 = 1;
            if(r2 < 0 ) r2 = -1;

            if(r1!=r2){
                iter  = s2.begin()-1;
                iend  = s2.end()-1;
                r1 = idx->dfs_cmp_suffix(R[ii],iend,iter);
            }

            EXPECT_EQ(r1, r2);
        }
    }
    //delete idx;
}
static void free(benchmark::State &state){
    for (auto _ : state) {
        sleep(3);
    }
        delete idx;
}

BENCHMARK(build_idx)->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_rule_trie)->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_rule_NoTrie)->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_rule_dfs)->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_rule_NoTrie_L)->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_srule_NoTrie_L)->Unit(benchmark::kMicrosecond);
BENCHMARK(compare_rule_dfs)->Unit(benchmark::kMicrosecond);
BENCHMARK(compare_srule_dfs)->Unit(benchmark::kMicrosecond);
BENCHMARK(free)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();