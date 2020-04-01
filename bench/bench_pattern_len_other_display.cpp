//
// Created by alejandro on 24-05-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include <slp/RePairSLPIndex.h>
#include <ri/r_index.hpp>
#include <chrono>
//#include <hyb/HybridSelfIndex.h>
#include "../bench/repetitive_collections.h"
//#include "../bench/utils.h"

#define MAX_DATA 1e6
#define  MAX_Q 1e4
#define CODE_DATA 4

#define R_I 1
#define H_I 2
#define SLP_I 3


using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

int len_q = 0;
int code = 0;

//HybridSelfIndex*            idx_hyb;
//ri::r_index<>*              idx_r;
cds_static::RePairSLPIndex* idx_slp;

std::string data;
std::vector<std::pair<size_t ,size_t >>R;

void generate_random_queries(const size_t &n, const size_t &max_q) {

    R.clear();

    std::string rg_file = "../files/";

    std::fstream f(rg_file + "rangos/"+std::to_string(code)+"_"+std::to_string(len_q)+".rg", std::ios::in| std::ios::binary);


    if(!f.is_open()){
        std::cout<<"Error the ranges file could not opened!!\n";
        std::cout<<rg_file + "rangos/"+std::to_string(code)+"_"+std::to_string(len_q)+".rg"<<std::endl;
        return;
    }
    std::string buff; uint i = 0;

    while (!f.eof() && std::getline(f, buff) && i < MAX_Q) {
        if(buff.size() > 1){
            uint l,r;
            sscanf(buff.c_str(),"%u %u",&l,&r);
            R.emplace_back(l,r);
            i++;
        }
    }
    f.close();


}

static void  load_indice(benchmark::State &state){

    int64_t code_coll = state.range(0);
    int64_t len       = state.range(1);
    int64_t op_i      = state.range(2);
    int64_t samp      = state.range(3);
    code = code_coll;
    len_q = len;
    data = "";
//    load_data(MAX_DATA,data,collections_code_inv_dir[code]);
    generate_random_queries(data.size(), MAX_Q);

    for (auto _ : state) {

    }



//    if(op_i == R_I){
//
//        std::string filename = "../files/new_indices/"+std::to_string(code_coll)+".ri";
//        fstream rf(filename,std::ios::in|std::ios::binary);
//        bool fast;
//        rf.read((char*)&fast,sizeof(fast));
//        idx_r = new ri::r_index<>();
//        idx_r->load(rf);
//        return;
//    }
//    if(op_i == H_I){
//
//        std::string filename = "../files/new_indices/"+std::to_string(code_coll);
//        char* _f = (char *)filename.c_str();
//        idx_hyb = new HybridSelfIndex(_f);
//
//        return;
//    }
    if(op_i == SLP_I){

        std::string filename = "../files/indices/"+std::to_string(code_coll)+"_"+std::to_string(samp);
        char* _f = (char *)filename.c_str();
        int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);

        return;
    }


    state.counters["space"] = 0;
    state.counters["data_len"] = data.size();
    state.counters["bps"] = 0;


}
//static void display_h_index(benchmark::State &state) {
//
//    int64_t len = state.range(0);
//
//    size_t nocc;
//    for (auto _ : state) {
//        nocc = 0;
//        for (uint ii=  0; ii < MAX_Q;++ii) {
//
//            uint m = R[ii].second-R[ii].first+1;
//            unsigned char *s;
//            //idx_hyb->locate((uchar*)(i.c_str()),m,&_Occ,&_occ);
//            idx_hyb->extract(R[ii].first,m,&s);
//            delete []s;
//
//        }
//    }
//
//    state.counters["space"] = idx_hyb->sizeDS;
//    state.counters["data_len"] = data.size();
//    state.counters["bps"] = idx_hyb->sizeDS*(8.0/data.size());
//
//}
static void display_slp_index(benchmark::State &state) {

    int64_t code_coll = state.range(0);
    int64_t len = state.range(1);
    int64_t samp = state.range(3);

    std::string filename = "../files/indices/"+std::to_string(code_coll)+"_"+std::to_string(samp);
    char* _f = (char *)filename.c_str();
    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);

    size_t nocc;
    for (auto _ : state) {
        nocc = 0;
        for (uint ii=  0; ii < MAX_Q &&  ii < R.size();++ii) {
            unsigned char * s = idx_slp->RePairSLPIndex::extract(R[ii].first,R[ii].second-1);
//            if(nocc > 1e5)break;
        }
    }

    state.counters["space"] = idx_slp->size();
    state.counters["data_len"] = data.size();
    state.counters["bps"] = idx_slp->size()*(8.0/data.size());
    delete idx_slp;
}
//static void display_balslp_index(benchmark::State &state) {
//
//    int64_t code_coll = state.range(0);
//    int64_t len = state.range(1);
//    int64_t samp = state.range(3);
//
//    std::string filename = "../files/new_divided_indices/bal_slp"+std::to_string(code_coll)+"_"+std::to_string(samp);
//    char* _f = (char *)filename.c_str();
//    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);
//    if(!q){
//
//        std::cout<<"Error en fichero slp\n";
//        std::cout<<filename<<"\n";
//        return;
//
//    }
//
//    size_t nocc;
//    for (auto _ : state) {
//        nocc = 0;
//        for (uint ii=  0; ii < MAX_Q &&  ii < R.size();++ii) {
//            unsigned char * s = idx_slp->RePairSLPIndex::extract(R[ii].first,R[ii].second-1);
////            if(nocc > 1e5)break;
//        }
//    }
//
//    state.counters["space"] = idx_slp->size();
//    state.counters["data_len"] = data.size();
//    state.counters["bps"] = idx_slp->size()*(8.0/data.size());
//    delete idx_slp;
//}
static void free_indexes(benchmark::State &state) {

    for (auto _ : state) {

    }

    state.counters["occ"]       = 0;
    state.counters["space"]     = 0;
    state.counters["bps"]       = 0;
    state.counters["len_coll"]  = 0;

//    delete idx_slp;
//    delete idx_r;
//    delete idx_hyb;
}

//BENCHMARK(load_indice)     ->Args({1,H_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_r_index)     ->Args({4,5         })->Unit(benchmark::kMicrosecond);
BENCHMARK(display_slp_index)   ->Args({4,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(display_slp_index)   ->Args({4,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(display_slp_index)   ->Args({4,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,5,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,5,SLP_I,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,5,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,5,SLP_I,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(free_indexes)       ->Args({4,5         })->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({4,10,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_r_index)     ->Args({4,10         })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,10,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,10,SLP_I,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_balslp_index)->Args({4,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,10,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,10,SLP_I,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(free_indexes)       ->Args({4,10         })->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({4,20,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_r_index)     ->Args({4,20         })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,20,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,20,SLP_I,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_balslp_index)->Args({4,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,20,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,20,SLP_I,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(free_indexes)       ->Args({4,20         })->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({4,30,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_r_index)     ->Args({4,30         })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,30,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,30,SLP_I,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_balslp_index)->Args({4,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,30,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,30,SLP_I,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(free_indexes)       ->Args({4,30         })->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({4,50,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_r_index)     ->Args({4,50         })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,50,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,50,SLP_I,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_balslp_index)->Args({4,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,50,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,50,SLP_I,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(free_indexes)       ->Args({4,50         })->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)         ->Args({4,100,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_r_index)     ->Args({4,100         })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,100,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({4,100,SLP_I,64})->Unit(benchmark::kMicrosecond);
BENCHMARK(display_balslp_index)->Args({4,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,100,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({4,100,SLP_I,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(free_indexes)        ->Args({4,100         })->Unit(benchmark::kMicrosecond);
//BENCHMARK(load_indice)        ->Args({5,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_r_index)     ->Args({5,5         })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({5,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({5,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({5,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({5,5,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({5,5,SLP_I,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({5,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({5,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({5,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({5,5,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({5,5,SLP_I,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(free_indexes)       ->Args({5,5         })->Unit(benchmark::kMicrosecond);
//BENCHMARK(load_indice)        ->Args({5,10,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_r_index)     ->Args({5,10         })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({5,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({5,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({5,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({5,10,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_slp_index)   ->Args({5,10,SLP_I,64})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({5,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({5,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({5,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({5,10,SLP_I,32})->Unit(benchmark::kMicrosecond);
//BENCHMARK(display_balslp_index)->Args({5,10,SLP_I,64})->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
