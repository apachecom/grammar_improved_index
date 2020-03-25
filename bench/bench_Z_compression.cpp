//
// Created by alejandro on 08-01-19.
//


#include <iostream>
#include <fstream>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include "repetitive_collections.h"
#include "utils.h"

sdsl::bit_vector X;

uint code;

sdsl::rrr_vector<> rrr_v;
sdsl::sd_vector<> sd_v;
sdsl::vlc_vector<> vlc_v;

sdsl::rrr_vector<>::rank_1_type rrr_v_rk;
sdsl::rrr_vector<>::select_1_type rrr_v_sl;
sdsl::select_support_mcl<1> mcl_sl1;
sdsl::select_support_mcl<0> mcl_sl0;


sdsl::sd_vector<>::rank_1_type sd_v_rk;
sdsl::sd_vector<>::select_1_type sd_v_sl;
sdsl::rank_support_v<1> v_rk;
sdsl::rank_support_v5<1> v5_rk;

std::vector<unsigned int> r_pos;
std::vector<unsigned int> r_1;


auto sd_rank = [](benchmark::State &st) {
    for (auto _ : st)
    {
        for (int i = 0; i < r_pos.size() ; ++i) {
            auto r = sd_v_rk(r_pos[i]);
        }

    }

    st.counters["it"] = r_pos.size();
    st.counters["size"] = sdsl::size_in_bytes(sd_v) + sdsl::size_in_bytes(sd_v_rk);

};
auto rrr_rank = [](benchmark::State &st) {
    for (auto _ : st)
    {
        for (int i = 0; i < r_pos.size() ; ++i) {
            auto r = rrr_v_rk(r_pos[i]);
        }

    }
    st.counters["it"] = r_pos.size();
    st.counters["size"] = sdsl::size_in_bytes(rrr_v) + sdsl::size_in_bytes(rrr_v_rk);
};
auto v_rank = [](benchmark::State &st) {
    for (auto _ : st)
    {
        for (int i = 0; i < r_pos.size() ; ++i) {
            auto r = v_rk(r_pos[i]);
        }

    }
    st.counters["it"] = r_pos.size();
    st.counters["size"] = sdsl::size_in_bytes(X) + sdsl::size_in_bytes(v_rk);
};

auto v5_rank = [](benchmark::State &st) {
    for (auto _ : st)
    {
        for (int i = 0; i < r_pos.size() ; ++i) {
            auto r = v5_rk(r_pos[i]);
        }

    }
    st.counters["it"] = r_pos.size();
    st.counters["size"] = sdsl::size_in_bytes(X) + sdsl::size_in_bytes(v5_rk);
};

auto sd_select = [](benchmark::State &st) {
    for (auto _ : st)
    {
        for (int i = 0; i < r_1.size() ; ++i) {
            auto r = sd_v_sl(r_1[i]);
        }

    }
    st.counters["it"] = r_1.size();
    st.counters["size"] = sdsl::size_in_bytes(sd_v) + sdsl::size_in_bytes(sd_v_sl);
};
auto rrr_select = [](benchmark::State &st) {
    for (auto _ : st)
    {
        for (int i = 0; i < r_1.size() ; ++i) {
            auto r = rrr_v_sl(r_1[i]);
        }

    }
    st.counters["it"] = r_1.size();
    st.counters["size"] = sdsl::size_in_bytes(rrr_v) + sdsl::size_in_bytes(rrr_v_sl);
};

auto mcl_select = [](benchmark::State &st) {
    for (auto _ : st)
    {
        for (int i = 0; i < r_1.size() ; ++i) {
            auto r = mcl_sl1(r_1[i]);
        }

    }
    st.counters["it"] = r_1.size();
    st.counters["size"] = sdsl::size_in_bytes(X) + sdsl::size_in_bytes(mcl_sl1);
};

auto z_test = [](benchmark::State &st, const int &c) {

    unsigned int ones = 0;
    unsigned int zeros = 0;
    code = c;
    SelfGrammarIndex *idx = load_idx(1, 0, 1, 2, 1, code);

    auto temp = idx->get_grammar().get_Z();

    X = sdsl::bit_vector(temp.size(),0);


//    std::cout<<"size of Z in structure "<<sdsl::size_in_bytes(idx->get_grammar().get_Z())<<std::endl;


    for ( int i = 0 ; i < X.size(); ++i) {
        if(temp[i] == 1){
            X[i] = 1;
            ++ones;
        }
        else{
            ++zeros;
        }
    }

//    std::cout<<"unos: "<<ones<<std::endl;
//    std::cout<<"len of X: "<<X.size()<<std::endl;

    std::srand(std::time(nullptr));
    r_pos.clear();
    for (int j = 0; j < 1000; ++j) {
        r_pos.push_back(rand() % X.size());
    }
    r_1.clear();
    for (int j = 0; j < 1000; ++j) {
        r_1.push_back(rand() % ones + 1);
    }

    sd_v = sdsl::sd_vector<> (X);
    rrr_v = sdsl::rrr_vector<> (X);
    vlc_v = sdsl::vlc_vector<> (X);

    rrr_v_rk = sdsl::rrr_vector<>::rank_1_type(&rrr_v);
    rrr_v_sl = sdsl::rrr_vector<>::select_1_type(&rrr_v);
    v_rk = sdsl::rank_support_v<1>(&X);
    v5_rk = sdsl::rank_support_v5<1>(&X);


    sd_v_rk = sdsl::sd_vector<>::rank_1_type(&sd_v);
    sd_v_sl = sdsl::sd_vector<>::select_1_type(&sd_v);
    mcl_sl1 = sdsl::select_support_mcl<1>(&X);
    mcl_sl0 = sdsl::select_support_mcl<0>(&X);

    std::cout<<"size of vectors:\n";
    std::cout<<"size of X   :"<<sdsl::size_in_bytes(X)<<std::endl;
    std::cout<<"size of sd_v   :"<<sdsl::size_in_bytes(sd_v)<<std::endl;
    std::cout<<"size of rrr_v  :"<<sdsl::size_in_bytes(rrr_v)<<std::endl;
    std::cout<<"size of vlc_v  :"<<sdsl::size_in_bytes(vlc_v)<<std::endl;


    for (auto _ : st)
    {
        sleep(3);

    }

    delete idx;

};

auto l_test = [](benchmark::State &st, const int &c) {

    unsigned int ones = 0;
    unsigned int zeros = 0;
    code = c;
    SelfGrammarIndex *idx = load_idx(1, 0, 1, 2, 1, code);

    auto temp = idx->get_grammar().get_L();

    X = sdsl::bit_vector(temp.size(),0);


//    std::cout<<"size of L in structure "<<sdsl::size_in_bytes(idx->get_grammar().get_L())<<std::endl;


    for ( int i = 0 ; i < X.size(); ++i) {
        if(temp[i] == 1){
            X[i] = 1;
            ++ones;
        }
        else{
            ++zeros;
        }
    }

//    std::cout<<"unos: "<<ones<<std::endl;
//    std::cout<<"len of X: "<<X.size()<<std::endl;

    std::srand(std::time(nullptr));

    r_pos.clear();
    for (int j = 0; j < 1000; ++j) {
        r_pos.push_back(rand() % X.size());
    }
    r_1.clear();
    for (int j = 0; j < 1000; ++j) {
        r_1.push_back(rand() % ones + 1);
    }

    sd_v = sdsl::sd_vector<> (X);
    rrr_v = sdsl::rrr_vector<> (X);

    rrr_v_rk = sdsl::rrr_vector<>::rank_1_type(&rrr_v);
    rrr_v_sl = sdsl::rrr_vector<>::select_1_type(&rrr_v);
    v_rk = sdsl::rank_support_v<1>(&X);
    v5_rk = sdsl::rank_support_v5<1>(&X);


    sd_v_rk = sdsl::sd_vector<>::rank_1_type(&sd_v);
    sd_v_sl = sdsl::sd_vector<>::select_1_type(&sd_v);
    mcl_sl1 = sdsl::select_support_mcl<1>(&X);
    mcl_sl0 = sdsl::select_support_mcl<0>(&X);

    std::cout<<"size of vectors:\n";
    std::cout<<"size of X   :"<<sdsl::size_in_bytes(X)<<std::endl;
    std::cout<<"size of sd_v   :"<<sdsl::size_in_bytes(sd_v)<<std::endl;
    std::cout<<"size of rrr_v  :"<<sdsl::size_in_bytes(rrr_v)<<std::endl;


    for (auto _ : st)
    {
        sleep(3);

    }

    delete idx;

};

int main (int argc, char *argv[] ){

   /* if(argc < 1){
        std::cout<<"bad parameters....";
        return 0;
    }*/



    benchmark::RegisterBenchmark("Z einstein.en.txt"   ,z_test       ,4);
    benchmark::RegisterBenchmark("rank sd"                ,sd_rank          );
    benchmark::RegisterBenchmark("rank rrr"                ,rrr_rank         );
    benchmark::RegisterBenchmark("rank v"                ,v_rank         );
    benchmark::RegisterBenchmark("rank v5"                ,v5_rank         );
    benchmark::RegisterBenchmark("select sd"              ,sd_select        );
    benchmark::RegisterBenchmark("select rrr"              ,rrr_select       );
    benchmark::RegisterBenchmark("select mcl"              ,mcl_select       );

    benchmark::RegisterBenchmark("Z E-Coli"   ,z_test       ,5);
    benchmark::RegisterBenchmark("rank sd"                ,sd_rank          );
    benchmark::RegisterBenchmark("rank rrr"                ,rrr_rank         );
    benchmark::RegisterBenchmark("rank v"                ,v_rank         );
    benchmark::RegisterBenchmark("rank v5"                ,v5_rank         );
    benchmark::RegisterBenchmark("select sd"              ,sd_select        );
    benchmark::RegisterBenchmark("select rrr"              ,rrr_select       );
    benchmark::RegisterBenchmark("select mcl"              ,mcl_select       );

    benchmark::RegisterBenchmark("Z kernel"   ,z_test       ,7);
    benchmark::RegisterBenchmark("rank sd"                ,sd_rank          );
    benchmark::RegisterBenchmark("rank rrr"                ,rrr_rank         );
    benchmark::RegisterBenchmark("rank v"                ,v_rank         );
    benchmark::RegisterBenchmark("rank v5"                ,v5_rank         );
    benchmark::RegisterBenchmark("select sd"              ,sd_select        );
    benchmark::RegisterBenchmark("select rrr"              ,rrr_select       );
    benchmark::RegisterBenchmark("select mcl"              ,mcl_select       );

    benchmark::RegisterBenchmark("L einstein.en.txt"  ,l_test       ,4);

    benchmark::RegisterBenchmark("rank sd   "                ,sd_rank          );
    benchmark::RegisterBenchmark("rank rrr"                ,rrr_rank         );
    benchmark::RegisterBenchmark("rank v"                ,v_rank         );
    benchmark::RegisterBenchmark("rank v5"                ,v5_rank         );
    benchmark::RegisterBenchmark("select sd"              ,sd_select        );
    benchmark::RegisterBenchmark("select rrr"              ,rrr_select       );
    benchmark::RegisterBenchmark("select mcl"              ,mcl_select       );

    benchmark::RegisterBenchmark("L E-Coli"   ,l_test       ,5);
    benchmark::RegisterBenchmark("rank sd"                ,sd_rank          );
    benchmark::RegisterBenchmark("rank rrr"                ,rrr_rank         );
    benchmark::RegisterBenchmark("rank v"                ,v_rank         );
    benchmark::RegisterBenchmark("rank v5"                ,v5_rank         );
    benchmark::RegisterBenchmark("select sd"              ,sd_select        );
    benchmark::RegisterBenchmark("select rrr"              ,rrr_select       );
    benchmark::RegisterBenchmark("select mcl"              ,mcl_select       );

    benchmark::RegisterBenchmark("L kernel"  ,l_test       ,7);
    benchmark::RegisterBenchmark("rank sd"                ,sd_rank          );
    benchmark::RegisterBenchmark("rank rrr"                ,rrr_rank         );
    benchmark::RegisterBenchmark("rank v"                ,v_rank         );
    benchmark::RegisterBenchmark("rank v5"                ,v5_rank         );
    benchmark::RegisterBenchmark("select sd"              ,sd_select        );
    benchmark::RegisterBenchmark("select rrr"              ,rrr_select       );
    benchmark::RegisterBenchmark("select mcl"              ,mcl_select       );

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


    //return 0;

}