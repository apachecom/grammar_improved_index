//
// Created by alejandro on 23-05-19.
//


#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include <slp/RePairSLPIndex.h>
#include <ri/r_index.hpp>
#include <hyb/HybridSelfIndex.h>

#include "../bench/repetitive_collections.h"

#define  MAX_Q 1e3
#define MAX_OCC 1e19
#define len_q 10
#define R_I 1
#define H_I 2
#define SLP_I 3

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;


HybridSelfIndex*            idx_hyb;
ri::r_index<>*              idx_r;
cds_static::RePairSLPIndex* idx_slp;
std::string data;

std::vector<std::string> R(MAX_Q);


void load_data(const size_t &_max,std::string& data, const std::string &coll) {

    std::fstream f(coll, std::ios::in | std::ios::binary);
    //std::string data;
    if (!f.is_open()) {
        std::cout << "Error the file could not opened!!\n";
        return;
    }
    std::string buff;
    unsigned char buffer[1000];
    while (!f.eof()) {
        f.read((char *) buffer, 1000);
        data.append((char *) buffer, f.gcount());
    }
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] == 0 || data[i] == 1 || data[i] == 2)
            data[i] = 65;
    }

    f.close();
    //std::cout << "DATA LOADED " <<data.size()<<std::endl;
}


void generate_random_queries(const int & c,const size_t &n, const size_t &max_q) {
    std::srand(std::time(nullptr));
    R.clear();
    ////////////////reading patterns/////////////

    std::string pattern_file = "../files/patrones/" + std::to_string(c)+"_len_200.ptt";
    //std::cout<<pattern_file<<std::endl;
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);
    if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
    }
    std::string buff; uint i = 0;
    std::string N = "";
    for (int j = 0; j < len_q; ++j) {
        N+='N';
    }
    while (!f.eof() && std::getline(f, buff) && i < max_q) {
        std::string sss;
        sss.resize(len_q);
        std::copy(buff.begin(),buff.begin()+len_q,sss.begin());
        ///std::cout<<i<<"\t"<<sss<<std::endl;

        if(sss == N){
//            std::cout<<"patron malo malo \t"<<sss<<std::endl;
            continue;
        }

        for (int k = 0; k < sss.size(); ++k){
            if(int(sss[k]) == 0 || int(sss[k]) == 1 || int(sss[k]) == 2 ){
                sss[k] = 65;
                //return 0;
            }
        }

        R.push_back(sss);
        i++;
    }

    std::cout<<R[R.size()-1]<<std::endl;
    f.close();


}

void load_indice(int op_i,int samp,int code_coll){

    if(op_i == R_I){

        std::string filename = "../files/new_indices/"+std::to_string(code_coll)+".ri";
        fstream rf(filename,std::ios::in|std::ios::binary);
        bool fast;
        rf.read((char*)&fast,sizeof(fast));
        idx_r = new ri::r_index<>();
        idx_r->load(rf);
        return;
    }

    if(op_i == H_I){

        std::string filename = "../files/new_indices/"+std::to_string(code_coll);
        char* _f = (char *)filename.c_str();
        idx_hyb = new HybridSelfIndex(_f);

        return;
    }

    if(op_i == SLP_I){

        std::string filename = "../files/indices/"+std::to_string(code_coll)+"_"+std::to_string(samp);
        char* _f = (char *)filename.c_str();
        int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);

        return;
    }



}

static void load_idx(benchmark::State &state) {

    int64_t code = state.range(0);
    data.clear();

    load_data(1e19, data, collections_code_inv_dir[code]);

    for (auto _ : state) {
//        load_indice(ind,s,code);
        sleep(1);
    }

    generate_random_queries(code,data.size(), MAX_Q);


    state.counters["occ"] = 0;
    state.counters["space"] = 0;
    state.counters["bps"] = 0;
    state.counters["len_coll"] = data.size();
}

static void locate_r_index(benchmark::State &state) {

    int64_t code = state.range(0);

    std::string filename = "../files/indices/"+std::to_string(code)+".ri";
    fstream rf(filename,std::ios::in|std::ios::binary);
    bool fast;
    rf.read((char*)&fast,sizeof(fast));
    idx_r = new ri::r_index<>();
    idx_r->load(rf);

    size_t nocc;
    for (auto _ : state) {
        nocc = 0;
        for (uint ii=  0; ii < MAX_Q &&  ii < R.size();++ii) {
            auto occ = idx_r->locate_all(R[ii]);
            nocc += occ.size();
            if(nocc > MAX_OCC)break;
        }
    }

    state.counters["occ"] = nocc;
    state.counters["space"] = sdsl::size_in_bytes(*idx_r);
    state.counters["bps"] = sdsl::size_in_bytes(*idx_r)*(8.0/data.size());
    state.counters["len_coll"] = data.size();
    delete idx_r;
}

static void locate_h_index(benchmark::State &state) {

    int64_t code = state.range(0);

    std::string filename = "../files/indices/"+std::to_string(code);
    char* _f = (char *)filename.c_str();
    idx_hyb = new HybridSelfIndex(_f);

    size_t nocc;
    for (auto _ : state) {
        nocc = 0;
        for (uint ii=  0; ii < MAX_Q;++ii) {

            unsigned long _Occ;
            unsigned long* _occ;

            unsigned int m = (unsigned int) R[ii].size();
            idx_hyb->locate((uchar*)(R[ii].c_str()),m,&_Occ,&_occ);
            nocc += _Occ;
            if(nocc > MAX_OCC)break;
        }
    }

    state.counters["occ"] = nocc;
    state.counters["space"] = idx_hyb->sizeDS;
    state.counters["bps"] = idx_hyb->sizeDS*(8.0/data.size());
    state.counters["len_coll"] = data.size();
    delete idx_hyb;
}


static void locate_slp_index(benchmark::State &state) {

    int64_t code = state.range(0);
    int64_t s = state.range(1);

    std::string filename = "../files/indices/"+std::to_string(code)+"_"+std::to_string(s);
    char* _f = (char *)filename.c_str();
    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);

    size_t nocc;
    for (auto _ : state) {
        nocc = 0;
        for (uint ii=  0; ii < MAX_Q &&  ii < R.size();++ii) {

            uint occs;
            uchar *tt = (uchar * )(R[ii].c_str());
            std::vector<uint> *pos = idx_slp->RePairSLPIndex::locate(tt, R[ii].length(), &occs);
            delete pos;
            nocc += occs;
            if(nocc > MAX_OCC)break;
        }
    }

    state.counters["occ"] = nocc;
    state.counters["space"] = idx_slp->size();
    state.counters["bps"] = idx_slp->size()*(8.0/data.size());
    state.counters["len_coll"] = data.size();
    delete idx_slp;
}

static void locate_balslp_index(benchmark::State &state) {

    int64_t code_coll = state.range(0);
    int64_t samp = state.range(1);

    std::string filename = "../files/new_divided_indices/bal_slp"+std::to_string(code_coll)+"_"+std::to_string(samp);
    char* _f = (char *)filename.c_str();
    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);

    size_t nocc;
    for (auto _ : state) {
        nocc = 0;
        for (uint ii=  0; ii < MAX_Q &&  ii < R.size();++ii) {

            uint occs;
            uchar *tt = (uchar * )(R[ii].c_str());
            std::vector<uint> *pos = idx_slp->RePairSLPIndex::locate(tt, R[ii].length(), &occs);
            delete pos;
            nocc += occs;
            if(nocc > MAX_OCC)break;
        }
    }

    state.counters["occ"] = nocc;
    state.counters["space"] = idx_slp->size();
    state.counters["bps"] = idx_slp->size()*(8.0/data.size());
    state.counters["len_coll"] = data.size();
    delete idx_slp;
}


BENCHMARK(load_idx)->Args({1})->Unit(benchmark::kMicrosecond);

BENCHMARK(locate_balslp_index)->Args({1,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({1,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({1,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({1,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({1,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({1,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)  ->Args({1})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_h_index)  ->Args({1})->Unit(benchmark::kMicrosecond);

BENCHMARK(load_idx)->Args({2})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({2,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({2,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({2,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,16})->Unit(benchmark::kMicrosecond);
///BENCHMARK(locate_h_index)  ->Args({2})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({2})->Unit(benchmark::kMicrosecond);

BENCHMARK(load_idx)->Args({4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({4,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({4,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({4,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({4,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({4,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({4,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_h_index)  ->Args({4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)  ->Args({4})->Unit(benchmark::kMicrosecond);

BENCHMARK(load_idx)->Args({5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({5,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({5,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({5,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({5,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({5,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({5,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_h_index)  ->Args({5})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)  ->Args({5})->Unit(benchmark::kMicrosecond);

BENCHMARK(load_idx)->Args({6})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({6,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({6,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({6,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({6,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({6,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({6,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_h_index)  ->Args({6})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)  ->Args({6})->Unit(benchmark::kMicrosecond);

BENCHMARK(load_idx)->Args({7})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({7,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({7,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({7,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({7,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({7,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({7,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_h_index)  ->Args({7})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)  ->Args({7})->Unit(benchmark::kMicrosecond);

BENCHMARK(load_idx)->Args({8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({8,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({8,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_balslp_index)->Args({8,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({8,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({8,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)->Args({8,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_h_index)  ->Args({8})->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)  ->Args({8})->Unit(benchmark::kMicrosecond);


BENCHMARK_MAIN();
