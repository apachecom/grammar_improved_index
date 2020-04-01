//
// Created by alejandro on 24-05-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include <slp/RePairSLPIndex.h>
#include <ri/r_index.hpp>
//#include <hyb/HybridSelfIndex.h>


#include "../bench/repetitive_collections.h"
#define  MAX_DATA 1e19
#define  MAX_Q 1e3
#define MAX_OCC 1e7
#define R_I 1
#define H_I 0
#define SLP_I 2
#define BAL_SLP_I 3
#define BF -1



using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;


//HybridSelfIndex*            idx_hyb;
ri::r_index<>*              idx_r;
cds_static::RePairSLPIndex* idx_slp;
std::string data;
std::vector<std::string> R;
int len_q = 0;
int code = 0;

void load_data(const size_t &_max,std::string& data, const std::string &coll) {

    std::fstream f(coll, std::ios::in | std::ios::binary);
    std::cout<<coll<<std::endl;
    //std::string data;
    if (!f.is_open()) {
        std::cout << "Error the coll file could not opened!!\n";
        return;
    }
    std::string buff;
    unsigned char buffer[1000];
    while (!f.eof() ) {
        f.read((char *) buffer, 1000);
        data.append((char *) buffer, f.gcount());
    }
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] == 0 || data[i] == 1 || data[i] == 2)
            data[i] = 65;
    }

    f.close();
}

void generate_random_queries(const int & c) {
    std::srand(std::time(nullptr));
    R.clear();
    ////////////////reading patterns/////////////

    std::string pattern_file = "../files/patrones/" + std::to_string(c)+"_len_200.ptt";
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);
    if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
    }
    std::string buff; uint i = 0;
    std::string N = "";
    for (int j = 0; j < len_q; ++j) {
        N+='N';
    }
    while (!f.eof() && std::getline(f, buff)) {
        std::string sss;
        sss.resize(len_q);
        std::copy(buff.begin(),buff.begin()+len_q,sss.begin());

        if(sss == N)continue;

        for (int k = 0; k < sss.size(); ++k){
            if(int(sss[k]) == 0 || int(sss[k]) == 1 || int(sss[k]) == 2 ){
                sss[k] = 65;
            }
        }

        R.push_back(sss);
        i++;
        if(i >= MAX_Q) break;
    }
    std::cout<<len_q<<"|\t"<<R[R.size()-1]<<std::endl;

    f.close();
}
static void  load_indice(benchmark::State &state){

    int64_t code_coll = state.range(0);
    int64_t len       = state.range(1);
    int64_t op_i      = state.range(2);
    int64_t samp      = state.range(3);
    data = "";
    code = code_coll;

    len_q = len;

    load_data(MAX_DATA,data,collections_code_inv_dir[code_coll]);


    generate_random_queries(code_coll);

    for (auto _ : state) {
        sleep(3);
    }



    if(op_i == R_I){

        std::string filename = "../files/indices/"+std::to_string(code_coll)+".ri";
        fstream rf(filename,std::ios::in|std::ios::binary);
        bool fast;
        rf.read((char*)&fast,sizeof(fast));
        idx_r = new ri::r_index<>();
        idx_r->load(rf);

    }
//    if(op_i == H_I){
//
//        std::string filename = "../files/new_indices/"+std::to_string(code_coll);
//        char* _f = (char *)filename.c_str();
//        idx_hyb = new HybridSelfIndex(_f);
//
//
//    }
    if(op_i == SLP_I){

        std::string filename = "../files/indices/"+std::to_string(code_coll)+"_"+std::to_string(samp);
        char* _f = (char *)filename.c_str();
        int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);


    }

    state.counters["1 n_ptt"]     = 0;
    state.counters["2 n_occ"]     = 0;
    state.counters["3 ptt_len"]   = 0;
    state.counters["4 coll_size"] = 0;
    state.counters["5 coll_id"]   = 0;
    state.counters["6 size"]      = 0;
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = 0;
    state.counters["9 valid"]     = 0;


}
static void locate_r_index(benchmark::State &state) {

    int64_t len = state.range(1);

    size_t nocc,ptt = 0;
    for (auto _ : state) {
        nocc = 0;
 ptt = 0;

        for (uint ii=  0; ii < MAX_Q &&  ii < R.size();++ii) {
//            std::string ss;
//            ss.resize(len);
//            std::copy(R[ii].begin(),R[ii].begin()+len,ss.begin());

            auto occ = idx_r->locate_all(R[ii]);
            nocc += occ.size();++ptt;
            if(nocc > MAX_OCC)break;
        }
    }

//    state.counters["occ"] = nocc;
//    state.counters["space"] = sdsl::size_in_bytes(*idx_r);
//    state.counters["bps"] = sdsl::size_in_bytes(*idx_r)*(8.0/data.size());
//    state.counters["len_coll"] = data.size();



    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = nocc;
    state.counters["3 ptt_len"]   = len;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
//    state.counters["6 size"]      = sdsl::size_in_bytes(*idx_r);
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = R_I;
    state.counters["9 valid"]     = 1;


//    delete idx_r;

}
//static void locate_h_index(benchmark::State &state) {
//
//    int64_t len = state.range(0);
//
//    size_t nocc,ptt = 0;
//    for (auto _ : state) {
//        nocc = 0; ptt = 0;
//
//        for (uint ii=  0; ii < MAX_Q &&  ii < R.size();++ii) {
//
////            std::string ss;
////            ss.resize(len);
////            std::copy(R[ii].begin(),R[ii].begin()+len,ss.begin());
//
//            unsigned long _Occ;
//            unsigned long* _occ;
//
//            unsigned int m = len;
//            idx_hyb->locate((uchar*)R[ii].c_str(),m,&_Occ,&_occ);
//            nocc += _Occ;++ptt;
//            if(nocc > MAX_OCC)break;
//        }
//    }
//
////    state.counters["occ"] = nocc;
////    state.counters["space"] = idx_hyb->sizeDS;
////    state.counters["bps"] = idx_hyb->sizeDS*(8.0/data.size());
////    state.counters["len_coll"] = data.size();
////
//
//    state.counters["1 n_ptt"]     = ptt;
//    state.counters["2 n_occ"]     = nocc;
//    state.counters["3 ptt_len"]   = len;
//    state.counters["4 coll_size"] = data.size();
//    state.counters["5 coll_id"]   = code;
//    state.counters["6 size"]      = idx_hyb->sizeDS;
//    state.counters["7 samp"]      = 0;
//    state.counters["8 name"]      = H_I;
//    state.counters["9 valid"]     = 1;
//
//}
static void locate_slp_index(benchmark::State &state) {

    int64_t code_coll = state.range(0);
    int64_t len = state.range(1);
    int64_t samp = state.range(3);

    std::string filename = "../files/indices/"+std::to_string(code_coll)+"_"+std::to_string(samp);
    char* _f = (char *)filename.c_str();
    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);

    size_t nocc,ptt = 0;
    for (auto _ : state) {
        nocc = 0; ptt = 0;

        for (uint ii=  0; ii < MAX_Q &&  ii < R.size();++ii) {
//            std::string ss;
//            ss.resize(len);
//            std::copy(R[ii].begin(),R[ii].begin()+len,ss.begin());
            uint occs;
            uchar *tt = (uchar * )(R[ii].c_str());
            std::vector<uint> *pos = idx_slp->RePairSLPIndex::locate(tt,len, &occs);
            delete pos;
            nocc += occs;ptt++;
            if(nocc > MAX_OCC)break;
        }
    }

//    state.counters["occ"] = nocc;
//    state.counters["space"] = idx_slp->size();
//    state.counters["bps"] = idx_slp->size()*(8.0/data.size());
//    state.counters["len_coll"] = data.size();

    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = nocc;
    state.counters["3 ptt_len"]   = len;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = idx_slp->size();
    state.counters["7 samp"]      = samp;
    state.counters["8 name"]      = SLP_I;
    state.counters["9 valid"]     = 1;

    delete idx_slp;

}
//static void locate_balslp_index(benchmark::State &state) {
//
//    int64_t code_coll = state.range(0);
//    int64_t len = state.range(1);
//    int64_t samp = state.range(3);
//
//    std::string filename = "../files/new_divided_indices/bal_slp"+std::to_string(code_coll)+"_"+std::to_string(samp);
//    char* _f = (char *)filename.c_str();
////    std::cout<<_f<<std::endl;
//
//    int q = cds_static::RePairSLPIndex::load(0,_f, &idx_slp);
//
//    size_t nocc,ptt = 0;
//    for (auto _ : state) {
//        nocc = 0; ptt = 0;
//
//        for (uint ii=  0; ii < R.size() && ii < MAX_Q;++ii) {
////            std::string ss;
////            ss.resize(len);
////            std::copy(R[ii].begin(),R[ii].begin()+len,ss.begin());
//            uint occs;
//            uchar *tt = (uchar * )(R[ii].c_str());
//            std::vector<uint> *pos = idx_slp->RePairSLPIndex::locate(tt,len, &occs);
//            delete pos;
//            nocc += occs;ptt++;
//            if(nocc > MAX_OCC)break;
//        }
//    }
//
////    state.counters["occ"] = nocc;
////    state.counters["space"] = idx_slp->size();
////    state.counters["bps"] = idx_slp->size()*(8.0/data.size());
////    state.counters["len_coll"] = data.size();
//
//    state.counters["1 n_ptt"]     = ptt;
//    state.counters["2 n_occ"]     = nocc;
//    state.counters["3 ptt_len"]   = len;
//    state.counters["4 coll_size"] = data.size();
//    state.counters["5 coll_id"]   = code;
//    state.counters["6 size"]      = idx_slp->size();
//    state.counters["7 samp"]      = samp;
//    state.counters["8 name"]      = BAL_SLP_I;
//    state.counters["9 valid"]     = 1;
//
//
//    delete idx_slp;
//}
static void free_indexes(benchmark::State &state) {

    for (auto _ : state) {

    }

    state.counters["1 n_ptt"]     = 0;
    state.counters["2 n_occ"]     = 0;
    state.counters["3 ptt_len"]   = 0;
    state.counters["4 coll_size"] = 0;
    state.counters["5 coll_id"]   = 0;
    state.counters["6 size"]      = 0;
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = 0;
    state.counters["9 valid"]     = 0;

//    delete idx_slp;
//    delete idx_r;
//    delete idx_hyb;
}
static void bt_locate(benchmark::State &state){

    size_t nocc = 0;
    double mean =0;

    for (auto _ : state)
    {
        nocc = 0;
        for (auto &&  i : R )
        {
            uint occs = 0;
//            auto start = timer::now();

            size_t pos = data.find(i, 0);
            while(pos != string::npos)
            {
                ++nocc;
                ++occs;
                pos = data.find(i,pos+1);
            }

//            auto stop = timer::now();
            // b_f << i << " " << occs <<"\n";
//            mean += (duration_cast<nanoseconds>(stop - start).count());

            if(nocc > MAX_OCC)break;

        }

    }


//    //b_f.close();
//    state.counters["occ"]       = nocc;
//    state.counters["space"]     = data.size();
//    state.counters["bps"]       = 8;
//    state.counters["len_coll"]  = data.size();

    state.counters["1 n_ptt"]     = R.size();
    state.counters["2 n_occ"]     = nocc;
    state.counters["3 ptt_len"]   = len_q;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = data.size();
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = BF;
    state.counters["9 valid"]     = 1;


}


////BENCHMARK(load_indice)     ->Args({1,H_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)          ->Args({4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({4,5         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({4,10,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({4,10         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({4,20,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({4,20         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({4,30,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({4,30         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({4,40,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({4,40         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({4,50,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({4,50         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({4,100,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({4,100         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({4,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({4,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(free_indexes)       ->Args({4,50         })->Unit(benchmark::kMicrosecond);



BENCHMARK(load_indice)        ->Args({5,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)          ->Args5{4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({5,5         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({5,10,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({5,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({5,10         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({5,20,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({5,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({5,20         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({5,30,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({5,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({5,30         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({5,40,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({5,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({5,40         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({5,50,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({5,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({5,50         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({5,100,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({5,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({5,100         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({5,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({5,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(free_indexes)       ->Args({5,50         })->Unit(benchmark::kMicrosecond);



////BENCHMARK(load_indice)     ->Args({1,H_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({1,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)          ->Args1{4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({1,5         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({1,10,R_I  ,0 })->Unit(benchmark::kMicrosecond);
////BENCHMARK(bt_locate)        ->Args({1,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({1,10         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({1,20,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({1,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({1,20         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({1,30,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({1,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({1,30         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({1,40,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({1,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({1,40         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({1,50,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({1,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({1,50         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({1,100,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({1,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({1,100         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({1,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({1,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(free_indexes)       ->Args({1,50         })->Unit(benchmark::kMicrosecond);




////BENCHMARK(load_indice)     ->Args({1,H_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({2,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)          ->Args2{4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({2,5         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({2,10,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({2,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({2,10         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({2,20,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({2,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({2,20         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({2,30,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({2,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({2,30         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({2,40,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({2,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({2,40         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({2,50,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({2,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({2,50         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({2,100,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({2,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({2,100         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({2,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({2,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(free_indexes)       ->Args({2,50         })->Unit(benchmark::kMicrosecond);




////BENCHMARK(load_indice)     ->Args({1,H_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({6,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)          ->Args6{4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({6,5         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({6,10,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({6,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({6,10         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({6,20,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({6,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({6,20         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({6,30,R_I  ,0 })->Unit(benchmark::kMicrosecond);
////BENCHMARK(bt_locate)        ->Args({6,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({6,30         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({6,40,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({6,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({6,40         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({6,50,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({6,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({6,50         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({6,100,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({6,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({6,100         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({6,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({6,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(free_indexes)       ->Args({6,50         })->Unit(benchmark::kMicrosecond);




////BENCHMARK(load_indice)     ->Args({1,H_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({7,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)          ->Args7{4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({7,5         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({7,10,R_I  ,0 })->Unit(benchmark::kMicrosecond);
////BENCHMARK(bt_locate)        ->Args({7,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({7,10         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({7,20,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({7,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({7,20         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({7,30,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({7,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({7,30         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({7,40,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({7,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({7,40         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({7,50,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({7,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({7,50         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({7,100,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({7,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({7,100         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({7,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({7,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(free_indexes)       ->Args({7,50         })->Unit(benchmark::kMicrosecond);




////BENCHMARK(load_indice)     ->Args({1,H_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({8,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)          ->Args8{4,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({8,5         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,5,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,5,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,5,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({8,10,R_I  ,0 })->Unit(benchmark::kMicrosecond);
////BENCHMARK(bt_locate)        ->Args({8,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({8,10         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,10,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,10,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,10,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({8,20,R_I  ,0 })->Unit(benchmark::kMicrosecond);
////BENCHMARK(bt_locate)        ->Args({8,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({8,20         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,20,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,20,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,20,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({8,30,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({8,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({8,30         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,30,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,30,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,30,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({8,40,R_I  ,0 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)        ->Args({8,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({8,40         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,40,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,40,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,40,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({8,50,R_I  ,0 })->Unit(benchmark::kMicrosecond);
////BENCHMARK(bt_locate)        ->Args({8,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({8,50         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,50,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,50,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,50,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(load_indice)        ->Args({8,100,R_I  ,0 })->Unit(benchmark::kMicrosecond);
////BENCHMARK(bt_locate)        ->Args({8,5,R_I  ,0 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_r_index)     ->Args({8,100         })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
BENCHMARK(locate_slp_index)   ->Args({8,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,100,SLP_I,4 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,100,SLP_I,8 })->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_balslp_index)->Args({8,100,SLP_I,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(free_indexes)       ->Args({8,50         })->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();
