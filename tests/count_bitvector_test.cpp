//
// Created by alejandro on 07-08-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include <slp/Samplings/sampling.h>
#include "../SelfGrammarIndexBSQ.h"
#include "../bench/utils.h"
#include "../bench/repetitive_collections.h"

SelfGrammarIndexBS* idx;
sdsl::bit_vector B;
uint code = 5;
std::vector<size_t>random_pos(1000);
std::vector<size_t>random_ones(1000);
std::string data = "";

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;


static void create_random_pos(benchmark::State &state) {

    std::random_device rd;
    std::mt19937_64 mt(rd());
    std::uniform_int_distribution<uint64_t > dist(1.0,10000);


    for (auto _ : state) {

        for (int i = 0; i < 1000; ++i)
            random_pos[i] = dist(mt)%B.size();

        uint ones = 0;

        for (int j = 0; j < B.size(); ++j)
            if(B[j])ones++;

        for (int i = 0; i < 1000; ++i)
            random_ones[i] = dist(mt)%ones+1;

        std::cout<<"cantidad de unos: "<<ones<<std::endl;
        std::cout<<"tamano total: "<<B.size()<<std::endl;
        std::cout<<"m/n: "<<ones*1.0/B.size()<<std::endl;

        sleep(3);
    }

}

static void build_bitvector(benchmark::State &state)
{
    std::cout<<"building bitvector"<<std::endl;
//    idx = load_idx(1, 0, 1, 1, 1, code);
//    data = "abradabracadabra";
    std::string total_coll = "";
    load_data(1e9,total_coll,collections_code_inv_dir[code]);
    data.resize(1000000);
    std::copy(total_coll.begin(),total_coll.begin()+1000000,data.begin());
    std::cout<<"data read:"<<std::endl;
    std::cout<<"data:"<<data<<std::endl;

    idx = new SelfGrammarIndexBS();

    idx->build(data);
    std::cout<<"index built:"<<std::endl;

    for (auto _ : state)
    {
        idx->build_bitvector_occ(B);
        std::cout<<"plain bitvector: "<<sdsl::size_in_bytes(B)<<std::endl;
        std::cout<<"plain bitvector(compressed using min bits): "<<idx->get_grid().get_SL_size()<<std::endl;

        sleep(3);
    }
}
static void sd_vector_select_1(benchmark::State &state){

    sdsl::sd_vector<> sd(B);
    sdsl::sd_vector<>::select_1_type sl(&sd);
    std::cout<<"sd_vector_size: "<<sdsl::size_in_bytes(sd)<<std::endl;
    std::cout<<"sd_vector_select_size: "<<sdsl::size_in_bytes(sl)<<std::endl;
    for (auto _ : state)
    {
        for (int i = 0; i < 1000; ++i) {
            auto s = sl(random_ones[i]);
        }
        sleep(3);
    }
}
static void sd_vector_rank_1(benchmark::State &state){

    sdsl::sd_vector<> sd(B);
    sdsl::sd_vector<>::rank_1_type rk(&sd);

    std::cout<<"sd_vector_size: "<<sdsl::size_in_bytes(sd)<<std::endl;
    std::cout<<"sd_vector_rank_size: "<<sdsl::size_in_bytes(rk)<<std::endl;
    for (auto _ : state)
    {
        for (int i = 0; i < 1000; ++i) {
            auto r = rk(random_pos[i]);
        }
        sleep(3);
    }
}
static void rrr_vector_select_1(benchmark::State &state){

    sdsl::rrr_vector<> sd(B);
    sdsl::rrr_vector<>::select_1_type sl(&sd);
    std::cout<<"rrr_vector_size: "<<sdsl::size_in_bytes(sd)<<std::endl;
    std::cout<<"rrr_vector_select_size: "<<sdsl::size_in_bytes(sl)<<std::endl;
    for (auto _ : state)
    {
        for (int i = 0; i < 1000; ++i) {
            auto s = sl(random_ones[i]);
        }
        sleep(3);
    }
}
static void rrr_vector_rank_1(benchmark::State &state){

    sdsl::rrr_vector<> sd(B);
    sdsl::rrr_vector<>::rank_1_type rk(&sd);

    std::cout<<"rrr_vector_size: "<<sdsl::size_in_bytes(sd)<<std::endl;
    std::cout<<"rrr_vector_rank_size: "<<sdsl::size_in_bytes(rk)<<std::endl;
    for (auto _ : state)
    {
        for (int i = 0; i < 1000; ++i) {
            auto r = rk(random_pos[i]);
        }
        sleep(3);
    }
}


BENCHMARK(build_bitvector)->Unit(benchmark::kMicrosecond);
BENCHMARK(create_random_pos)->Unit(benchmark::kMicrosecond);
BENCHMARK(sd_vector_rank_1)->Unit(benchmark::kMicrosecond);
BENCHMARK(sd_vector_select_1)->Unit(benchmark::kMicrosecond);
BENCHMARK(rrr_vector_rank_1)->Unit(benchmark::kMicrosecond);
BENCHMARK(rrr_vector_select_1)->Unit(benchmark::kMicrosecond);
BENCHMARK_MAIN();
