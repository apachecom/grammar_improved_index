//
// Created by alejandro on 15-05-19.
//


#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>

#include "../bench/utils.h"
#include "../SelfGrammarIndexBSQ.h"
#include "../bench/repetitive_collections.h"

#define MAX_DATA 1e14
#define CODE_DATA 4

std::string coll = "../tests/collections/repetitive/reals/einstein.en.txt";
std::string data;
std::map<size_t, std::string> expansions;
std::vector<size_t> R;
std::vector<size_t> L;
SelfGrammarIndex *idx;

void generate_random_queries(const size_t &n, const size_t &max_q) {
    std::srand(std::time(nullptr));

    for (int i = 0; i < max_q; ++i) {
        R.push_back(rand() % n + 1);
        L.push_back(100);//rand() % expansions[R[i]].size() + 1);
        int pp = 100*i/max_q;

        printf("\rGenerate random queries progress %d",pp);
//        std::cout<<expansions[R[i]]<<std::endl;
    }
    printf("\n");

}


void get_expansions_rule() {


    /*LOAD DATA*/
    std::string temp_data = "";
    load_data(MAX_DATA,temp_data,coll);
    data.resize(10000000);
    std::copy(temp_data.begin(),temp_data.begin()+10000000,data.begin());


//    std::cout<<data<<std::endl;
    //data = "abracadabra";
    /*build temporal grammar*/

    grammar G;
//    G.buildRepair(data);
    fstream fff("../files/einstein-grammar.g",std::ios::in);
    G.load(fff);

//    load_grammar(G,collections_code[coll],false);
//    std::cout<<"Imprimiendo gramatica"<<std::endl;
//    G.print(data);
//    std::cout<<"Compute grammar expansion progress ";

    for (size_t i = 1; i <= G.n_rules(); ++i) {

        std::string tt;
        tt.resize(G[i].r - G[i].l + 1);
        std::copy(data.begin() + G[i].l, data.begin() + G[i].r + 1, tt.begin());
//        std::cout<<G[i].l<<" "<<G[i].r<<std::endl;
        expansions[i] = tt;

//        int pp = 100*i/G.n_rules();

//        printf("\rCompute grammar expansion progress %d",pp);

    }
    printf("\n");
    generate_random_queries(G.n_rules(), 1000);

//    std::cout << "TEST DATA GENERATED\n";
}

static void build_idx(benchmark::State &state){


    for (auto _ : state) {
        get_expansions_rule();
        idx = new SelfGrammarIndexBSQ();

//        SelfGrammarIndex *idx = load_idx(1, 0, 1, 2, 1, CODE_DATA);
//        idx = load_idx(1, 0, CODE_DATA);
        idx->build(data);
        idx->buildSamplings(4);
    }
}

static void expand_prefix_trie(benchmark::State &state) {

    for (auto _ : state) {
        std::string s;

        for (size_t ii = 0; ii < R.size(); ++ii) {
            size_t pos = 0;
            s.resize(L[ii]);
            idx->bp_expand_prefix(R[ii], s, L[ii], pos);

//            std::string real;
//            real.resize(L[ii]);
//            std::copy(expansions[R[ii]].begin(), expansions[R[ii]].begin() + L[ii], real.begin());
//            ASSERT_EQ(s, real);
        }
    }
}

static void expand_prefix_no_trie(benchmark::State &state) {

    for (auto _ : state) {
        size_t n_rules = idx->get_grammar().n_rules();
        std::string s;

        for (size_t ii = 0; ii < R.size(); ++ii) {
            size_t pos = 0;
            s.resize(L[ii]);
            idx->expand_prefix(R[ii], s, L[ii], pos);

//            std::string real;
//            real.resize(L[ii]);
//            std::copy(expansions[R[ii]].begin(), expansions[R[ii]].begin() + L[ii], real.begin());
//            ASSERT_EQ(s, real);
        }
    }

}
//
//static void expand_prefix_slp(benchmark::State &state) {
//    for (auto _ : state) {
//        size_t n_rules = idx->get_grammar().n_rules();
//        std::string s;
//
//        for (size_t ii = 0; ii < R.size(); ++ii) {
//            size_t pos = 0;
//            s.resize(L[ii]);
//            idx->expand_prefix_slp(R[ii], s, L[ii], pos);
//
//            std::string real;
//            real.resize(L[ii]);
//            std::copy(expansions[R[ii]].begin(), expansions[R[ii]].begin() + L[ii], real.begin());
//            ASSERT_EQ(s, real);
//        }
//    }
//}

static void expand_suffix_trie(benchmark::State &state) {

    for (auto _ : state) {
        size_t n_rules = idx->get_grammar().n_rules();
        std::string s;

        for (size_t ii = 0; ii < R.size(); ++ii) {
            size_t pos = 0;
            size_t nn = expansions[R[ii]].size();
            size_t diff = nn - L[ii];
            s.resize(L[ii]);

            idx->bp_expand_suffix(R[ii], s, L[ii], pos);
//            std::reverse(s.begin(),s.end());
//            std::string real;
//            real.resize(L[ii]);

//            std::copy(expansions[R[ii]].begin() + diff, expansions[R[ii]].end(), real.begin());
//            ASSERT_EQ(s, real);
        }
    }


}

static void expand_suffix_no_trie(benchmark::State &state) {


    for (auto _ : state) {
        size_t n_rules = idx->get_grammar().n_rules();
        std::string s;

        for (size_t ii = 0; ii < R.size(); ++ii) {
            size_t pos = 0;
            size_t nn = expansions[R[ii]].size();
            size_t diff = nn - L[ii];
            s.resize(L[ii]);

            idx->expand_suffix(R[ii], s, L[ii], pos);
//            std::reverse(s.begin(),s.end());
//
//            std::string real;
//            real.resize(L[ii]);
//
//            std::copy(expansions[R[ii]].begin() + diff, expansions[R[ii]].end(), real.begin());
//            ASSERT_EQ(s, real);
        }
    }


}

//static void expand_suffix_slp(benchmark::State &state) {
//
//
//    for (auto _ : state) {
//        size_t n_rules = idx->get_grammar().n_rules();
//        std::string s;
//
//        for (size_t ii = 0; ii < R.size(); ++ii) {
//            size_t pos = 0;
//            size_t nn = expansions[R[ii]].size();
//            size_t diff = nn - L[ii];
//            s.resize(L[ii]);
//
//            idx->expand_suffix_slp(R[ii], s, L[ii], pos);
//            std::reverse(s.begin(),s.end());
//
//            std::string real;
//            real.resize(L[ii]);
//
//            std::copy(expansions[R[ii]].begin() + diff, expansions[R[ii]].end(), real.begin());
//
//            if(s!=real)
//            {
//                idx->expand_suffix_slp(R[ii], s, L[ii], pos);
//                std::reverse(s.begin(),s.end());
//            }
//            ASSERT_EQ(s, real);
//        }
//    }
//
//
//}

static void expand_suffix_dfs(benchmark::State &state) {


    for (auto _ : state) {
        size_t n_rules = idx->get_grammar().n_rules();
        std::string s;

        for (size_t ii = 0; ii < R.size(); ++ii) {
            size_t pos = 0;
            size_t nn = expansions[R[ii]].size();
            size_t diff = nn - L[ii];
            s.resize(L[ii]);
            std::string real;
            real.resize(L[ii]);
            std::copy(expansions[R[ii]].begin() + diff, expansions[R[ii]].end(), real.begin());

            idx->dfs_expand_suffix(R[ii], s, L[ii], pos);
//            std::reverse(s.begin(),s.end());
//
//            if(s != real){
//                pos = 0;
//                idx->dfs_expand_suffix(R[ii], s, L[ii], pos);
//            }
//            std::cout<<s<<std::endl;
//
//            ASSERT_EQ(s, real);
        }
    }
}

static void expand_prefix_dfs(benchmark::State &state) {


    for (auto _ : state) {
        size_t n_rules = idx->get_grammar().n_rules();
        for (size_t ii = 0; ii < R.size(); ++ii) {
            std::string s,real;
            s.resize(L[ii]);
            real.resize(L[ii]);

            std::copy(expansions[R[ii]].begin(), expansions[R[ii]].begin()+ L[ii], real.begin());
            size_t pos = 0;
            idx->dfs_expand_prefix(R[ii], s, L[ii], pos);

//            if(s != real){
//                pos = 0;
//                idx->dfs_expand_prefix(R[ii], s, L[ii], pos);
//            }
//            std::cout<<s<<std::endl;

//            ASSERT_EQ(s, real);
        }
    }
}

BENCHMARK(build_idx)->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_prefix_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_prefix_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_prefix_dfs)->Unit(benchmark::kMicrosecond);
//BENCHMARK(expand_prefix_slp)->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_suffix_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_suffix_no_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_suffix_dfs)->Unit(benchmark::kMicrosecond);
//BENCHMARK(expand_suffix_slp)->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();