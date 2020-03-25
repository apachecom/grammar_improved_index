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
#define  MAX_Q 1000
#define len_q 5

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

SelfGrammarIndex *idx;
std::string coll = "../tests/collections/repetitive/reals/Escherichia_Coli";
std::string data;

std::vector<std::string> R(MAX_Q);
std::vector<std::vector<uint>> P(MAX_Q);

void generate_random_queries(const size_t &n, const size_t &max_q) {

    std::srand(std::time(nullptr));
    P.clear();
    for (int i = 0; i < max_q; ++i) {
        size_t r, l = rand() % n;
        if (l > n / 2)
            r = l - len_q;
        else
            r = l + len_q;
        //size_t r = rand()%n;

        if (l > r) swap(l, r);

        std::string patt;

        patt.resize(r - l + 1);

        std::copy(data.begin() + l, data.begin() + r + 1, patt.begin());

        R[i]= patt;

        uint noc = 0;
        size_t pos = data.find(patt, 0);
        while (pos != string::npos) {
            noc++;
            //_X[pos] = true;
            P[i].push_back((uint) pos);
            pos = data.find(patt, pos + 1);
        }

    }

}


void build_idx(benchmark::State &state) {
////////////0123456789012345
    load_data(MAX_DATA, data, coll);
//    data = "abradabracaacavscsfdafdfafaeaefcacadcd4dacdadaa4za4daadacaadadaadadsdaddacdaabcdaddabaaaraardarbadabra";

    for (auto _ : state) {
        sleep(4);
    }

    generate_random_queries(data.size(), MAX_Q);
}


static void locate_trie(benchmark::State &state) {

    size_t occ;
    idx=load_idx(3, 8, 1, 1, 1, collections_code[coll]);
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
            if(X != P[ii])
                idx->locate(R[ii],X);
            ASSERT_EQ(X,P[ii]);
            if(occ > 1e5)break;
        }
    }

    state.counters["occ"] = occ;

}

static void locate_no_trie(benchmark::State &state) {

    size_t occ;
    idx=load_idx(3, 8, 1, 1, 1, collections_code[coll]);
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

            ASSERT_EQ(X,P[ii]);
            if(occ > 1e5)break;
        }
    }

    state.counters["occ"] = occ;
}
BENCHMARK(build_idx)->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_trie)->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_no_trie)->Unit(benchmark::kMicrosecond);


BENCHMARK_MAIN();