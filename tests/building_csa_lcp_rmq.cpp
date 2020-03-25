//
// Created by alpachec on 25-08-18.
//
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include <sdsl/int_vector.hpp>
#include <sdsl/inv_perm_support.hpp>
#include <sdsl/lcp_bitcompressed.hpp>
#include <sdsl/bp_support_sada.hpp>
#include <sdsl/rmq_succinct_sada.hpp>
#include "collections.cpp"

static void grammar_big_size(benchmark::State& state) {

    size_t points = state.range(0);
    for (auto _ : state)
    {

        uint folder = state.range(0);
        uint coll = state.range(1);
        std::string collection = "../" + dirFolder[folder] + dircollection[coll];
        std::fstream f(collection, std::ios::in | std::ios::binary);
        std::string data;
        std::cout << "collection: " << collection << std::endl;

        if (!f.is_open())
        {
            std::cout << "Error the file could not opened!!\n";
            return;
        }

        std::string buff;

        while (!f.eof() && std::getline(f, buff))
        {
            data += buff;
        }
        std::cout << "size of the string: " << data.length() << std::endl;
        f.close();
        uint size = state.range(2);


        {
            std::fstream f_csa("csa_1_" + dircollection[coll], std::ios::out | std::ios::binary);
            std::fstream f_lcp("lcp_" + dircollection[coll], std::ios::out | std::ios::binary);
            std::fstream f_rmq("rmq_" + dircollection[coll], std::ios::out | std::ios::binary);

            sdsl::int_vector<> SA(data.size(),0);
            sdsl::algorithm::calculate_sa( (unsigned char*)data.c_str(),data.size(),SA);
            sdsl::inv_perm_support<> SA_1(&SA);
            sdsl::lcp_wt<> LCP;
            sdsl::construct_im(LCP,data.c_str(),sizeof(char));
            sdsl::rmq_succinct_sada<true,sdsl::bp_support_sada<>> rmq(&LCP);

            sdsl::serialize(SA_1,f_csa);
            sdsl::serialize(LCP,f_lcp);
            sdsl::serialize(rmq,f_rmq);

            f_csa.close();
            f_lcp.close();
            f_rmq.close();
        }

        {
            std::string ss = data;
            std::reverse(ss.begin(),ss.end());

            std::fstream f_csa_rev("csa_rev_1_" + dircollection[coll], std::ios::out | std::ios::binary);
            std::fstream f_lcp_rev("lcp_rev_" + dircollection[coll], std::ios::out | std::ios::binary);
            std::fstream f_rmq_rev("rmq_rev_" + dircollection[coll], std::ios::out | std::ios::binary);


            sdsl::int_vector<> SA(data.size(),0);
            sdsl::algorithm::calculate_sa( (unsigned char*)data.c_str(),data.size(),SA);
            sdsl::inv_perm_support<> SA_1(&SA);
            sdsl::lcp_wt<> LCP;
            sdsl::construct_im(LCP,data.c_str(),sizeof(char));
            sdsl::rmq_succinct_sada<true,sdsl::bp_support_sada<>> rmq(&LCP);

            sdsl::serialize(SA_1,f_csa_rev);
            sdsl::serialize(LCP ,f_lcp_rev);
            sdsl::serialize(rmq ,f_rmq_rev);

            f_csa_rev.close();
            f_lcp_rev.close();
            f_rmq_rev.close();

        }
    }
}


/*
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA100, 10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA200, 10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA,    10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA50,  10000})->Unit(benchmark::kMillisecond);

*/

BENCHMARK(grammar_big_size)->Args({dataDir::dir_sources, dataCollection::sources100, 10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_sources, dataCollection::sources200, 10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_sources, dataCollection::sources,    10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_sources, dataCollection::sources50,  10000})->Unit(benchmark::kMillisecond);

BENCHMARK(grammar_big_size)->Args({dataDir::dir_pitches, dataCollection::pitches50, 10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_pitches, dataCollection::pitches, 10000})->Unit(benchmark::kMillisecond);

BENCHMARK(grammar_big_size)->Args({dataDir::dir_english, dataCollection::english50, 10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_english, dataCollection::english100, 10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_english, dataCollection::english200,    10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_english, dataCollection::english1024,  10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_english, dataCollection::english,  10000})->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();