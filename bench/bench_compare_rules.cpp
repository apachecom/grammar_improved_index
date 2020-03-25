//
// Created by alpachec on 21-05-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "repetitive_collections.h"
#include "../bench/utils.h"

#define MAX_DATA 1e14
#define CODE_DATA 4
#define MAX_QUERIES 1000

std::vector<string> coll = {"../tests/collections/repetitive/reals/einstein.en.txt",
                            "../tests/collections/repetitive/reals/Escherichia_Coli"};
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

void get_expansions_rule(int c) {

    /*LOAD DATA*/
    load_data(MAX_DATA,data,collections_code_inv_dir[c]);
    //data = "panamabanana";
    /*build temporal grammar*/

    grammar G;
    //G.buildRepair(data);
    fstream f("../files/new_divided_indices/bal/"+std::to_string(c)+"bal_repair_g",std::ios::in|std::ios::binary);

    G.load(f);
    std::cout<<"rules: "<<G.n_rules()<<std::endl;
    std::cout<<"size:"<<G.get_size()<<std::endl;
    //G.print(data);
    expansions.clear();
    for (size_t i = 1; i <= G.n_rules(); ++i) {
        std::string tt;
        tt.resize(G[i].r - G[i].l + 1);
        std::copy(data.begin() + G[i].l, data.begin() + G[i].r + 1, tt.begin());
        expansions[i] = tt;
//        std::cout<<tt<<std::endl;
    }

    generate_random_queries(G.n_rules(), MAX_QUERIES);

    //std::cout << "TEST DATA GENERATED\n";
}

static void build_idx(benchmark::State &state){
    int code_data = state.range(0);
    get_expansions_rule(code_data);

    for (auto _ : state) {
        sleep(4);
        //idx->build(data);
    }
    state.counters["MAX_QUERIES"] = MAX_QUERIES;
}

static void compare_rule_trie(benchmark::State &state) {
    int code_data = state.range(0);

    idx = load_idx(1, 0, 1, 1, 1, code_data);

    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s2 = expansions[L[ii]];
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->bp_cmp_prefix(R[ii],iter,iend);
        }
    }
    delete idx;
}

static void compare_rule_NoTrie(benchmark::State &state) {
    int code_data = state.range(0);

    idx = load_idx(1, 0, 1, 1, 1, code_data);

    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s2 = expansions[L[ii]];
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->cmp_prefix(R[ii],iter,iend);
        }
    }
    delete idx;
}

static void compare_rule_NoTrie_L(benchmark::State &state) {
    //idx = new SelfGrammarIndexBS();
    //idx->build(data);
    int code_data = state.range(0);

    idx = load_idx(1, 0, 1, 1, 1, code_data);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s2 = expansions[L[ii]];
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->cmp_prefix_L(R[ii],iter,iend);
        }
    }
    delete idx;
}

static void compare_srule_trie(benchmark::State &state) {
    int code_data = state.range(0);

    idx = load_idx(1, 0, 1, 1, 1, code_data);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s2 = expansions[L[ii]];
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->bp_cmp_suffix(R[ii],iend,iter);
        }
    }
    delete idx;
}

static void compare_srule_NoTrie(benchmark::State &state) {
    int code_data = state.range(0);

    //idx = load_idx(1, 0, 1, 1, 1, code_data);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s2 = expansions[L[ii]];
            auto ibegin  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->cmp_suffix(R[ii],iend,ibegin);
        }
    }
    delete idx;
}

static void compare_srule_NoTrie_L(benchmark::State &state) {

    int code_data = state.range(0);

    idx = load_idx(1, 0, 1, 1, 1, code_data);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s2 = expansions[L[ii]];
            auto ibegin  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->cmp_suffix_L(R[ii],iend,ibegin);
        }
    }
    delete idx;
}


static void x_compare_rule_trie(benchmark::State &state) {

    int code_data = state.range(0);

//    idx = load_idx(1, 0, 1, 1, 1, code_data);
    idx = load_idx(1, 0, code_data);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s2 = expansions[R[ii]];
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->bp_cmp_prefix(R[ii],iter,iend);
//            ASSERT_EQ(r1,0);
        }
    }

    state.counters["MAX_QUERIES"] = MAX_QUERIES;

    delete idx;
}

static void x_compare_rule_NoTrie(benchmark::State &state) {
    int code_data = state.range(0);

//    idx = load_idx(1, 0, 1, 1, 1, code_data);
    idx = load_idx(1, 0, code_data);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s2 = expansions[R[ii]];
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->cmp_prefix(R[ii],iter,iend);
//            ASSERT_EQ(r1,0);
        }
    }

    state.counters["MAX_QUERIES"] = MAX_QUERIES;

    delete idx;
}

static void x_compare_rule_NoTrie_L(benchmark::State &state) {
    //idx = new SelfGrammarIndexBS();
    //idx->build(data);
    int code_data = state.range(0);

//    idx = load_idx(1, 0, 1, 1, 1, code_data);
    idx = load_idx(1, 0, code_data);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s2 = expansions[R[ii]];
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->cmp_prefix_L(R[ii],iter,iend);
//            ASSERT_EQ(r1,0);
        }
    }
    state.counters["MAX_QUERIES"] = MAX_QUERIES;

    delete idx;
}

static void x_compare_srule_trie(benchmark::State &state) {
    int code_data = state.range(0);

//    idx = load_idx(1, 0, 1, 1, 1, code_data);
    idx = load_idx(1, 0, code_data);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s2 = expansions[R[ii]];
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->bp_cmp_suffix(R[ii],iend,iter);
//            ASSERT_EQ(r1,0);
        }
    }
    state.counters["MAX_QUERIES"] = MAX_QUERIES;

    delete idx;
}

static void x_compare_srule_NoTrie(benchmark::State &state) {
    int code_data = state.range(0);

//    idx = load_idx(1, 0, 1, 1, 1, code_data);
    idx = load_idx(1, 0, code_data);
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s2 = expansions[R[ii]];
            auto ibegin  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->cmp_suffix(R[ii],iend,ibegin);
//            ASSERT_EQ(r1,0);
        }
    }
    state.counters["MAX_QUERIES"] = MAX_QUERIES;

    delete idx;
}

static void x_compare_srule_NoTrie_L(benchmark::State &state) {
    //idx = new SelfGrammarIndexBS();
    //idx->build(data);
    int code_data = state.range(0);

//    idx = load_idx(1, 0, 1, 1, 1, code_data);
    idx = load_idx(1, 0, code_data);

    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s2 = expansions[R[ii]];
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->cmp_suffix_L(R[ii],iend,iter);
//            ASSERT_EQ(r1,0);
        }
    }

    state.counters["MAX_QUERIES"] = MAX_QUERIES;
    delete idx;
}

BENCHMARK(build_idx)->Args({4})->Unit(benchmark::kMicrosecond);
//
//BENCHMARK(compare_rule_trie)      ->Args({4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_rule_NoTrie)    ->Args({4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_rule_NoTrie_L)  ->Args({4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_srule_trie)     ->Args({4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_srule_NoTrie)   ->Args({4})->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_srule_NoTrie_L) ->Args({4})->Unit(benchmark::kMicrosecond);
//


BENCHMARK(x_compare_rule_trie)      ->Args({4})->Unit(benchmark::kMicrosecond);
BENCHMARK(x_compare_rule_NoTrie)    ->Args({4})->Unit(benchmark::kMicrosecond);
BENCHMARK(x_compare_rule_NoTrie_L)  ->Args({4})->Unit(benchmark::kMicrosecond);
BENCHMARK(x_compare_srule_trie)     ->Args({4})->Unit(benchmark::kMicrosecond);
BENCHMARK(x_compare_srule_NoTrie)   ->Args({4})->Unit(benchmark::kMicrosecond);
BENCHMARK(x_compare_srule_NoTrie_L) ->Args({4})->Unit(benchmark::kMicrosecond);



BENCHMARK(build_idx)->Args({5})->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_rule_trie)      ->Args({5})->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_rule_NoTrie)    ->Args({5})->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_rule_NoTrie_L)  ->Args({5})->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_srule_trie)     ->Args({5})->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_srule_NoTrie)   ->Args({5})->Unit(benchmark::kMicrosecond);
//BENCHMARK(compare_srule_NoTrie_L) ->Args({5})->Unit(benchmark::kMicrosecond);

BENCHMARK(x_compare_rule_trie)      ->Args({5})->Unit(benchmark::kMicrosecond);
BENCHMARK(x_compare_rule_NoTrie)    ->Args({5})->Unit(benchmark::kMicrosecond);
BENCHMARK(x_compare_rule_NoTrie_L)  ->Args({5})->Unit(benchmark::kMicrosecond);
BENCHMARK(x_compare_srule_trie)     ->Args({5})->Unit(benchmark::kMicrosecond);
BENCHMARK(x_compare_srule_NoTrie)   ->Args({5})->Unit(benchmark::kMicrosecond);
BENCHMARK(x_compare_srule_NoTrie_L) ->Args({5})->Unit(benchmark::kMicrosecond);


BENCHMARK_MAIN();



