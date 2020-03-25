//
// Created by alejandro on 14-05-19.
//

#include <iostream>
#include <fstream>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>


#include "repetitive_collections.h"
#include "utils.h"

#define MAX 1000

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

uint code;

//std::vector<size_t> R(MAX);

std::map<uint, std::vector<size_t >> rules_len;
std::map<uint, std::vector<size_t>> R;

std::vector<std::string> xpTrie(MAX);
std::vector<std::string> xp(MAX);

std::fstream frules("rules_to_extract", std::ios::in | std::ios::binary);

auto generate_random_queries = [](benchmark::State &st, const size_t &n) {
    for (auto _ : st) {
        SelfGrammarIndex *idx = load_idx(1, 0, 1, 2, 1, code);

        size_t n_rules = idx->get_grammar().n_rules();

        uint C[7] = {3, 5, 10, 20, 30, 50, 100};

        for (int l = 0; l < 7; ++l) {

            for (int j = 0; j < n_rules-1; ++j) {
                size_t node = idx->get_grammar().select_occ(j + 1, 1);
                size_t len = idx->get_grammar().len_rule(node);

                if (len > C[l])
                    rules_len[C[l]].push_back(j + 1);
            }

        }

        for (int l = 0; l < 7; ++l) {

            std::sort(rules_len[C[l]].begin(), rules_len[C[l]].end());
            auto it = std::unique(rules_len[C[l]].begin(), rules_len[C[l]].end());
            rules_len[C[l]].erase(it, rules_len[C[l]].end());

        }


        std::srand(std::time(nullptr));
        for (int k = 0; k < 7; ++k) {
            R[C[k]] = std::vector<size_t>(MAX);
            for (int i = 0; i < MAX; ++i) {
                size_t ii = rand() % rules_len[C[k]].size();
                R[C[k]][i] = rules_len[C[k]][ii];
            }
        }
//
//
//        for (int k = 0; k < 7; ++k) {
//            std::cout<<"len" <<C[k]<<std::endl;
//            for (int i = 0; i < R[C[k]].size(); ++i) {
//                std::cout<<"\trule "<<R[C[k]][i]<<std::endl;
//            }
//        }


//
//    for (auto _ : st) {
//        std::srand(std::time(nullptr));
//        size_t i = 0;
//        while (!frules.eof() || R.size() < MAX) {
//            uint r;
//            frules >> r;
//
//            R.push_back(r);
//
//            ++i;
//        }

//        std::cout << "numero de queries " << R.size() << std::endl;
//        for (int i = 0; i < MAX; ++i) {
//            //R[i] = rand()%n+1;
//            ///std::cout << R[i] << " ";
//        }
//        std::cout << "\n  ";
//        sleep(4);
// }
        sleep(4);
    }
};

auto expand_rule_trie = [](benchmark::State &st, const uint &nchar) {


    SelfGrammarIndex *idx = load_idx(1, 0, 1, 2, 1, code);

    std::string s;
    s.resize(nchar);

    for (auto _ : st) {
        for (size_t i = 0; i < MAX; ++i) {
            size_t pos = 0;
            auto X = R[nchar][i];
            idx->bp_expand_prefix(R[nchar][i], s, nchar, pos);
        }
    }
};
auto expand_rule_no_trie = [](benchmark::State &st, const uint &nchar) {

    SelfGrammarIndex *idx = load_idx(1, 0, 1, 2, 2, code);
    size_t n_rules = idx->get_grammar().n_rules();
    std::string s;
    s.resize(nchar);
    size_t j = 0;
    for (auto _ : st) {
        for (size_t i = 0; i < MAX; ++i) {
            size_t pos = 0;
            idx->expand_prefix(R[nchar][i], s, nchar, pos);
        }
    }

};

auto expand_rule_dfs = [](benchmark::State &st, const uint &nchar) {

    SelfGrammarIndex *idx = load_idx(1, 0, 1, 2, 2, code);
    size_t n_rules = idx->get_grammar().n_rules();

    std::string s;
    s.resize(nchar);
    size_t j = 0;
    for (auto _ : st) {
        for (size_t i = 0; i < MAX; ++i) {
            size_t pos = 0;
            idx->expand_prefix_slp(R[nchar][i], s, nchar, pos);
        }
    }

};

auto expand_sfx_rule_trie = [](benchmark::State &st, const uint &nchar) {

    SelfGrammarIndex *idx = load_idx(1, 0, 1, 2, 1, code);
    size_t n_rules = idx->get_grammar().n_rules();
    std::string s;
    s.resize(nchar);
    size_t j = 0;
    for (auto _ : st) {
        for (size_t i = 0; i < MAX; ++i) {
            size_t pos = 0;
            idx->bp_expand_suffix(R[nchar][i], s, nchar, pos);
        }
    }

};

auto expand_sfx_rule_no_trie = [](benchmark::State &st, const uint &nchar) {

    SelfGrammarIndex *idx = load_idx(1, 0, 1, 2, 2, code);
    size_t n_rules = idx->get_grammar().n_rules();
    std::string s;
    s.resize(nchar);
    size_t j = 0;
    for (auto _ : st) {
        for (size_t i = 0; i < MAX; ++i) {
            size_t pos = 0;
            idx->expand_suffix(R[nchar][i], s, nchar, pos);
        }
    }

};

auto expand_sfx_rule_dfs = [](benchmark::State &st, const uint &nchar) {

    SelfGrammarIndex *idx = load_idx(1, 0, 1, 2, 2, code);
    size_t n_rules = idx->get_grammar().n_rules();
    std::string s;
    s.resize(nchar);
    size_t j = 0;
    for (auto _ : st) {
        for (size_t i = 0; i < MAX; ++i) {
            size_t pos = 0;
            idx->expand_suffix_slp(R[nchar][i], s, nchar, pos);
        }
    }

};

int main(int argc, char *argv[]) {

/*

    if(argc < 2){
        std::cout<<"bad parameters....";
         return 0;
    }
*/


    std::string collection = "../tests/collections/repetitive/reals/Escherichia_Coli";//(argv[1]);

    if (collections_code.find(collection) == collections_code.end()) {
        std::cout << "bad parameters (not collection code found)....\n";
        return 0;
    }

    code = collections_code[collection];

    benchmark::RegisterBenchmark("load random queries", generate_random_queries, 5)->Unit(benchmark::kMicrosecond);
    //benchmark::RegisterBenchmark("Expand prefix Trie 1 ", expand_rule_trie, 1)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix Trie 3 ", expand_rule_trie, 3)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix Trie 5 ", expand_rule_trie, 5)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix Trie 10", expand_rule_trie, 10)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix Trie 20", expand_rule_trie, 20)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix Trie 30", expand_rule_trie, 30)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix Trie 50", expand_rule_trie, 50)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix Trie 100", expand_rule_trie, 100)->Unit(benchmark::kMicrosecond);
//
    //benchmark::RegisterBenchmark("Expand prefix no Trie 1 ", expand_rule_no_trie, 1)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix no Trie 3 ", expand_rule_no_trie, 3)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix no Trie 5 ", expand_rule_no_trie, 5)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix no Trie 10", expand_rule_no_trie, 10)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix no Trie 20", expand_rule_no_trie, 20)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix no Trie 30", expand_rule_no_trie, 30)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix no Trie 50", expand_rule_no_trie, 50)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand prefix no Trie 100", expand_rule_no_trie, 100)->Unit(benchmark::kMicrosecond);



/*
    benchmark::RegisterBenchmark("Expand prefix SLP 1 ", expand_rule_slp,1);
    benchmark::RegisterBenchmark("Expand prefix SLP 3 ", expand_rule_slp,3);
    benchmark::RegisterBenchmark("Expand prefix SLP 5 ", expand_rule_slp,5);
    benchmark::RegisterBenchmark("Expand prefix SLP 10", expand_rule_slp,10);
    benchmark::RegisterBenchmark("Expand prefix SLP 20", expand_rule_slp,20);
    benchmark::RegisterBenchmark("Expand prefix SLP 30", expand_rule_slp,30);
    benchmark::RegisterBenchmark("Expand prefix SLP 50", expand_rule_slp,50);
*/

//
//    benchmark::RegisterBenchmark("Expand suffix Trie 1 ", expand_sfx_rule_trie, 1)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand suffix Trie 3 ", expand_sfx_rule_trie, 3)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand suffix Trie 5 ", expand_sfx_rule_trie, 5)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand suffix Trie 10", expand_sfx_rule_trie, 10)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand suffix Trie 20", expand_sfx_rule_trie, 20)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand suffix Trie 30", expand_sfx_rule_trie, 30)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand suffix Trie 50", expand_sfx_rule_trie, 50)->Unit(benchmark::kMicrosecond);
//
//    benchmark::RegisterBenchmark("Expand suffix no Trie 1 ", expand_sfx_rule_no_trie, 1)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand suffix no Trie 3 ", expand_sfx_rule_no_trie, 3)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand suffix no Trie 5 ", expand_sfx_rule_no_trie, 5)->Unit(benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand suffix no Trie 10", expand_sfx_rule_no_trie, 10)->Unit(
//            benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand suffix no Trie 20", expand_sfx_rule_no_trie, 20)->Unit(
//            benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand suffix no Trie 30", expand_sfx_rule_no_trie, 30)->Unit(
//            benchmark::kMicrosecond);
//    benchmark::RegisterBenchmark("Expand suffix no Trie 50", expand_sfx_rule_no_trie, 50)->Unit(
//            benchmark::kMicrosecond);


    /*benchmark::RegisterBenchmark("Expand suffix SLP 1 ", expand_sfx_rule_slp,1);
    benchmark::RegisterBenchmark("Expand suffix SLP 3 ", expand_sfx_rule_slp,3);
    benchmark::RegisterBenchmark("Expand suffix SLP 5 ", expand_sfx_rule_slp,5);
    benchmark::RegisterBenchmark("Expand suffix SLP 10", expand_sfx_rule_slp,10);
    benchmark::RegisterBenchmark("Expand suffix SLP 20", expand_sfx_rule_slp,20);
    benchmark::RegisterBenchmark("Expand suffix SLP 30", expand_sfx_rule_slp,30);
    benchmark::RegisterBenchmark("Expand suffix SLP 50", expand_sfx_rule_slp,50);
*/


    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
    //return 0;

}