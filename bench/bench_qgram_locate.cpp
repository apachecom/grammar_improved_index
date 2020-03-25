//
// Created by root on 12-06-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "../SelfGrammarIndexBSQ.h"

#include "../bench/utils.h"
#include "../bench/repetitive_collections.h"


#define MAX_DATA 1e19
#define MAX_Q 1e3
#define MAX_OCC 1e19
#define len_q 10

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

SelfGrammarIndexBSQ idx;
SelfGrammarIndex* pidx;
std::string data;

std::vector<std::string> R(MAX_Q);
std::vector<std::vector<uint>> P(MAX_Q);

void generate_random_queries(const int & c,const size_t &n, const size_t &max_q) {

//    std::srand(std::time(nullptr));
//    P.clear();
//
//    for (int i = 0; i < max_q; ++i) {
//
//        size_t r, l = rand() % n;
//        if (l > n / 2)
//            r = l - len_q;
//        else
//            r = l + len_q;
//        //size_t r = rand()%n;
//
//        if (l > r) swap(l, r);
//
//        std::string patt;
//
//        patt.resize(r - l + 1);
//
//        std::copy(data.begin() + l, data.begin() + r + 1, patt.begin());
//
//        R[i]= patt;
//
//    }

    std::srand(std::time(nullptr));
    R.clear();
//    std::string N = "";
//    for (int j = 0; j < len_q; ++j) {
//        N +="N";
//    }
    ////////////////reading patterns/////////////

    std::string pattern_file = "../files/patrones/" + std::to_string(c)+"_len_200.ptt";
    std::cout<<pattern_file<<std::endl;
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
//        std::cout<<i<<"\t"<<sss<<std::endl;

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
//    sleep(30);


}
void build_idx(benchmark::State &state) {


    int code = state.range(0);

    data.clear();

    load_data(MAX_DATA, data, collections_code_inv_dir[code]);
    for (auto _ : state) {

        sleep(4);
    }

    generate_random_queries(code,data.size(), MAX_Q);

    state.counters["occ"] = 0;
    state.counters["space"] = 0;
    state.counters["data_len"] = 0;
    state.counters["bps"] = 0;
}
static void print_samplings(benchmark::State &state){

    int code = state.range(0);
    int s    = state.range(1);

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR, fGS);

    for (auto _ : state) {

        sleep(3);

    }

    state.counters["total size"]                   =         idx.size_in_bytes();
    state.counters["size basics with tries"]       =         idx.get_grammar().size_in_bytes()+idx.get_grid().size_in_bytes();
    state.counters["size basics without tries"]    =         idx.get_grammar().size_in_bytes()+idx.get_grid().size_in_bytes() - (idx.get_grammar().left_path.size_in_bytes()+idx.get_grammar().right_path.size_in_bytes());
    state.counters["size grid"]                    =         idx.get_grid().size_in_bytes();
    state.counters["size compressed grammar"]      =         idx.get_grammar().size_in_bytes();
    state.counters["size left trie"]               =         idx.get_grammar().left_path.size_in_bytes();
    state.counters["size rigth trie"]              =         idx.get_grammar().right_path.size_in_bytes();
    state.counters["size samp G"]                  =         idx.reverseSamp->getSize();
    state.counters["size samp GR"]                 =         idx.grammarSamp->getSize();
    state.counters["size samp GS"]                 =         idx.sequenceSamp->getSize();



}
static void bs_no_trie(benchmark::State &state) {

    int code = state.range(0);
    int s    = state.range(1);

    pidx=load_idx(1,0,1,1,2,code);
//    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
//    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);
//
//    idx.load(f);
//    idx.loadSampling(fG,fGR, fGS);

    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint ii=  0; ii < R.size();++ii) {
            std::vector<uint> X;
            pidx->locateNoTrie(R[ii],X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            if(occ > MAX_OCC)break;
        }
    }


    size_t size_indx = pidx->get_grid().size_in_bytes()+(pidx->get_grammar().size_in_bytes()- pidx->get_grammar().right_path.size_in_bytes() - pidx->get_grammar().left_path.size_in_bytes());

    state.counters["occ"] = occ;
    state.counters["space"] = size_indx;
    state.counters["data_len"] = pidx->get_grammar().L.size();
    state.counters["bps"] = size_indx*8.0/pidx->get_grammar().L.size();
//    float lt = idx.get_grammar().left_path.size_in_bytes()*8.0/idx.get_grammar().L.size();
//    float rt = idx.get_grammar().right_path.size_in_bytes()*8.0/idx.get_grammar().L.size();
//    state.counters["tries"] = lt+rt;
//    //std::cout<<lt+rt<<std::endl;
//    state.counters["tries bytes"] = idx.get_grammar().left_path.size_in_bytes() + idx.get_grammar().right_path.size_in_bytes();




}
static void bs_trie(benchmark::State &state) {

    int code = state.range(0);
    int s    = state.range(1);

    pidx=load_idx(1,0,1,1,1,code);
//    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
//    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);
//
//    idx.load(f);
//    idx.loadSampling(fG,fGR, fGS);

    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint ii=  0; ii < R.size();++ii) {
            std::vector<uint> X;
            pidx->locate(R[ii],X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            if(occ > MAX_OCC)break;
        }
    }


    size_t size_indx = pidx->get_grid().size_in_bytes()+pidx->get_grammar().size_in_bytes();

    state.counters["occ"] = occ;
    state.counters["space"] = size_indx;
    state.counters["data_len"] = pidx->get_grammar().L.size();
    state.counters["bps"] = size_indx*8.0/pidx->get_grammar().L.size();
//    float lt = idx.get_grammar().left_path.size_in_bytes()*8.0/idx.get_grammar().L.size();
//    float rt = idx.get_grammar().right_path.size_in_bytes()*8.0/idx.get_grammar().L.size();
//    state.counters["tries"] = lt+rt;
//    //std::cout<<lt+rt<<std::endl;
//    state.counters["tries bytes"] = idx.get_grammar().left_path.size_in_bytes() + idx.get_grammar().right_path.size_in_bytes();




}
static void pts_no_trie(benchmark::State &state) {

    int code = state.range(0);
    int s    = state.range(1);

    pidx=load_idx(3,s,1,1,2,code);
//    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
//    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);
//
//    idx.load(f);
//    idx.loadSampling(fG,fGR, fGS);

    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint ii=  0; ii < R.size();++ii) {

            std::vector<uint> X;
            pidx->locateNoTrie(R[ii],X);

            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());

            occ += X.size();

            if(occ > MAX_OCC)
                break;
        }
    }


    size_t size_indx = pidx->get_grid().size_in_bytes()+(pidx->get_grammar().size_in_bytes()- pidx->get_grammar().right_path.size_in_bytes() - pidx->get_grammar().left_path.size_in_bytes())
            + pidx->get_pt_rules().size_in_bytes()+pidx->get_pt_suffixes().size_in_bytes();

    state.counters["occ"] = occ;
    state.counters["space"] = size_indx;
    state.counters["data_len"] = pidx->get_grammar().L.size();
    state.counters["bps"] = size_indx*8.0/pidx->get_grammar().L.size();
//    float lt = idx.get_grammar().left_path.size_in_bytes()*8.0/idx.get_grammar().L.size();
//    float rt = idx.get_grammar().right_path.size_in_bytes()*8.0/idx.get_grammar().L.size();
//    state.counters["tries"] = lt+rt;
//    //std::cout<<lt+rt<<std::endl;
//    state.counters["tries bytes"] = idx.get_grammar().left_path.size_in_bytes() + idx.get_grammar().right_path.size_in_bytes();




}
static void pts_trie(benchmark::State &state) {

    int code = state.range(0);
    int s    = state.range(1);

    pidx=load_idx(3,s,1,1,1,code);
//    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
//    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);
//
//    idx.load(f);
//    idx.loadSampling(fG,fGR, fGS);

    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint ii=  0; ii < R.size();++ii) {
            std::vector<uint> X;
            pidx->locate(R[ii],X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            if(occ > MAX_OCC)break;
        }
    }


    size_t size_indx = pidx->get_grid().size_in_bytes()+pidx->get_grammar().size_in_bytes()
                       + pidx->get_pt_rules().size_in_bytes()+pidx->get_pt_suffixes().size_in_bytes();

    state.counters["occ"] = occ;
    state.counters["space"] = size_indx;
    state.counters["data_len"] = pidx->get_grammar().L.size();
    state.counters["bps"] = size_indx*8.0/pidx->get_grammar().L.size();
//    float lt = idx.get_grammar().left_path.size_in_bytes()*8.0/idx.get_grammar().L.size();
//    float rt = idx.get_grammar().right_path.size_in_bytes()*8.0/idx.get_grammar().L.size();
//    state.counters["tries"] = lt+rt;
//    //std::cout<<lt+rt<<std::endl;
//    state.counters["tries bytes"] = idx.get_grammar().left_path.size_in_bytes() + idx.get_grammar().right_path.size_in_bytes();




}
static void qgram_nosmp_no_trie(benchmark::State &state) {

    int code = state.range(0);
    int s    = state.range(1);


    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR, fGS);

    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint ii=  0; ii < R.size();++ii) {
            std::vector<uint> X;
            idx.locateNoTrie(R[ii],X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            if(occ > MAX_OCC)break;
        }
    }


    size_t size_indx = idx.get_grid().size_in_bytes()+(idx.get_grammar().size_in_bytes()- idx.get_grammar().right_path.size_in_bytes() - idx.get_grammar().left_path.size_in_bytes());

    state.counters["occ"] = occ;
    state.counters["space"] = size_indx;
    state.counters["data_len"] = idx.get_grammar().L.size();
    state.counters["bps"] = size_indx*8.0/idx.get_grammar().L.size();
//    float lt = idx.get_grammar().left_path.size_in_bytes()*8.0/idx.get_grammar().L.size();
//    float rt = idx.get_grammar().right_path.size_in_bytes()*8.0/idx.get_grammar().L.size();
//    state.counters["tries"] = lt+rt;
//    //std::cout<<lt+rt<<std::endl;
//    state.counters["tries bytes"] = idx.get_grammar().left_path.size_in_bytes() + idx.get_grammar().right_path.size_in_bytes();




}
static void qgram_nosmp_trie(benchmark::State &state) {

    int code = state.range(0);
    int s    = state.range(1);

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR, fGS);

    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint ii=  0; ii < R.size();++ii) {
            std::vector<uint> X;
            idx.locate(R[ii],X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            if(occ > MAX_OCC)break;
        }
    }

    size_t size_indx = idx.get_grid().size_in_bytes() + idx.get_grammar().size_in_bytes();

    state.counters["occ"] = occ;
    state.counters["space"] = size_indx;
    state.counters["data_len"] = idx.get_grammar().L.size();
    state.counters["bps"] = size_indx*8.0/idx.get_grammar().L.size();



}
static void qgram_smp(benchmark::State &state) {

    int code = state.range(0);
    int s    = state.range(1);


    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR, fGS);

    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint ii=  0; ii < R.size();++ii) {
            std::vector<uint> X;
            idx.qgram_locate(R[ii],X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            if(occ > MAX_OCC)break;
        }
    }

    size_t size_indx = idx.get_grid().size_in_bytes()+(idx.get_grammar().size_in_bytes()- idx.get_grammar().right_path.size_in_bytes() - idx.get_grammar().left_path.size_in_bytes())
                       + idx.grammarSamp->getSize() + + idx.reverseSamp->getSize();


    state.counters["occ"] = occ;
    state.counters["space"] = size_indx;
    state.counters["data_len"] = idx.get_grammar().L.size();
    state.counters["bps"] = size_indx*8.0/idx.get_grammar().L.size();



}
static void qgram_smp_rules_search(benchmark::State &state) {

    int code = state.range(0);
    int s    = state.range(1);


    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR, fGS);

    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint ii=  0; ii < R.size();++ii) {
            std::vector<uint> X;
            idx.qgram_locate_prefix_search(R[ii],X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            if(occ > MAX_OCC)break;
        }
    }


    size_t size_indx = idx.get_grid().size_in_bytes()+(idx.get_grammar().size_in_bytes()- idx.get_grammar().right_path.size_in_bytes() - idx.get_grammar().left_path.size_in_bytes())
                       + idx.grammarSamp->getSize() + + idx.reverseSamp->getSize() + idx.sequenceSamp->getSize();


    state.counters["occ"] = occ;
    state.counters["space"] = size_indx;
    state.counters["data_len"] = idx.get_grammar().L.size();
    state.counters["bps"] = size_indx*8.0/idx.get_grammar().L.size();



}
static void qgram_smp_rules_sfx_search_trie_locate(benchmark::State &state) {

    int code = state.range(0);
    int s    = state.range(1);


    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR, fGS);

    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint ii=  0; ii < R.size();++ii) {
            std::vector<uint> X;
            idx.qgram_trie_locate(R[ii],X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            if(occ > MAX_OCC)break;
        }
    }

    size_t size_indx = idx.get_grid().size_in_bytes()+ idx.get_grammar().size_in_bytes()
                       + idx.grammarSamp->getSize() + + idx.reverseSamp->getSize() + idx.sequenceSamp->getSize();


    state.counters["occ"] = occ;
    state.counters["space"] = size_indx;
    state.counters["data_len"] = idx.get_grammar().L.size();
    state.counters["bps"] = size_indx*8.0/idx.get_grammar().L.size();


}
static void qgram_smp_rules_sfx_search_dfs_locate(benchmark::State &state){
    int code = state.range(0);
    int s    = state.range(1);


    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR, fGS);

    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint ii=  0; ii < R.size();++ii) {
            std::vector<uint> X;
            idx.qgram_dfs_locate(R[ii],X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            if(occ > MAX_OCC)break;
        }
    }

    size_t size_indx = idx.get_grid().size_in_bytes()+(idx.get_grammar().size_in_bytes()- idx.get_grammar().right_path.size_in_bytes() - idx.get_grammar().left_path.size_in_bytes())
                       + idx.grammarSamp->getSize() + + idx.reverseSamp->getSize() + idx.sequenceSamp->getSize();


    state.counters["occ"] = occ;
    state.counters["space"] = size_indx;
    state.counters["data_len"] = idx.get_grammar().L.size();
    state.counters["bps"] = size_indx*8.0/idx.get_grammar().L.size();
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


            size_t pos = data.find(i, 0);
            while(pos != string::npos)
            {
                ++nocc;
                ++occs;
                pos = data.find(i,pos+1);
            }
            if(nocc > MAX_OCC)break;

        }

    }


    //b_f.close();
    state.counters["occ"] = nocc;
    state.counters["space"] = data.size();
    state.counters["data_len"] = data.size();
    state.counters["bps"] = 8.0;


}

BENCHMARK(build_idx)                                   ->Args({1})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)                                   ->Args({1,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_no_trie)                                  ->Args({1,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_trie)                                     ->Args({1,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({1,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({1,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({1,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({1,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({1,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({1,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({1,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({1,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({1,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({1,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({1,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({1,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({1,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({1,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({1,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({1,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({1,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({1,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({1,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({1,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({1,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({1,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({1,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({1,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({1,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({1,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({1,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({1,12 })  ->Unit(benchmark::kMicrosecond);



//
BENCHMARK(build_idx)                                   ->Args({2})     ->Unit(benchmark::kMicrosecond);
////BENCHMARK(bt_locate)                                   ->Args({1,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_no_trie)                                  ->Args({2,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_trie)                                     ->Args({2,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({2,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({2,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({2,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({2,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({2,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({2,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({2,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({2,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({2,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({2,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({2,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({2,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({2,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({2,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({2,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({2,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({2,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({2,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({2,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({2,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({2,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({2,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({2,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({2,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({2,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({2,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({2,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({2,12 })  ->Unit(benchmark::kMicrosecond);

//
//
//
BENCHMARK(build_idx)                                   ->Args({4})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)                                   ->Args({1,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_no_trie)                                  ->Args({4,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_trie)                                     ->Args({4,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({4,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({4,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({4,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({4,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({4,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({4,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({4,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({4,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({4,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({4,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_nosmp_no_trie)                         ->Args({4,2})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_nosmp_trie)                            ->Args({4,2})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({4,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({4,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({4,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({4,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({4,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({4,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({4,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({4,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({4,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({4,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({4,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({4,12 })  ->Unit(benchmark::kMicrosecond);


//
BENCHMARK(build_idx)                                   ->Args({5})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)                                   ->Args({1,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_no_trie)                                  ->Args({5,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_trie)                                     ->Args({5,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({5,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({5,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({5,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({5,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({5,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({5,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({5,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({5,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({5,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({5,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({5,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({5,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({5,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({5,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({5,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({5,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({5,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({5,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({5,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({5,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({5,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({5,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({5,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({5,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({5,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({5,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({5,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({5,12 })  ->Unit(benchmark::kMicrosecond);



BENCHMARK(build_idx)                                   ->Args({6})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)                                   ->Args({1,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_no_trie)                                  ->Args({6,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_trie)                                     ->Args({6,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({6,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({6,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({6,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({6,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({6,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({6,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({6,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({6,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({6,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({6,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({6,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({6,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({6,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({6,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({6,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({6,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({6,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({6,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({6,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({6,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({6,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({6,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({6,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({6,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({6,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({6,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({6,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({6,12 })  ->Unit(benchmark::kMicrosecond);

BENCHMARK(build_idx)                                   ->Args({7})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)                                   ->Args({1,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_no_trie)                                  ->Args({7,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_trie)                                     ->Args({7,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({7,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({7,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({7,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({7,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({7,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({7,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({7,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({7,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({7,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({7,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({7,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({7,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({7,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({7,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({7,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({7,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({7,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({7,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({7,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({7,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({7,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({7,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({7,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({7,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({7,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({7,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({7,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({7,12 })  ->Unit(benchmark::kMicrosecond);




BENCHMARK(build_idx)                                   ->Args({8})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)                                   ->Args({1,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_no_trie)                                  ->Args({8,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(bs_trie)                                     ->Args({8,0 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({8,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({8,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({8,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({8,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_no_trie)                                 ->Args({8,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({8,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({8,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({8,16})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({8,32})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(pts_trie)                                    ->Args({8,64})  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({8,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({8,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({8,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({8,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({8,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_search)                      ->Args({8,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({8,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({8,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({8,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({8,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({8,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_trie_locate)      ->Args({8,12 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({8,2 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({8,4 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({8,6 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({8,8 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({8,10 })  ->Unit(benchmark::kMicrosecond);
BENCHMARK(qgram_smp_rules_sfx_search_dfs_locate)       ->Args({8,12 })  ->Unit(benchmark::kMicrosecond);

//
//BENCHMARK(print_samplings)           ->Args({1,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({1,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({1,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({1,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({1,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({1,12})  ->Unit(benchmark::kMicrosecond);
//
//
//BENCHMARK(print_samplings)           ->Args({2,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({2,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({2,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({2,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({2,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({2,12})  ->Unit(benchmark::kMicrosecond);
//
//BENCHMARK(print_samplings)           ->Args({4,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({4,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({4,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({4,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({4,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({4,12})  ->Unit(benchmark::kMicrosecond);
//
//BENCHMARK(print_samplings)           ->Args({5,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({5,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({5,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({5,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({5,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({5,12})  ->Unit(benchmark::kMicrosecond);
//
//BENCHMARK(print_samplings)           ->Args({6,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({6,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({6,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({6,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({6,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({6,12})  ->Unit(benchmark::kMicrosecond);
//
//BENCHMARK(print_samplings)           ->Args({7,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({7,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({7,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({7,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({7,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({7,12})  ->Unit(benchmark::kMicrosecond);
//
//BENCHMARK(print_samplings)           ->Args({8,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({8,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({8,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({8,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({8,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)           ->Args({8,12})  ->Unit(benchmark::kMicrosecond);

//
//
//
//BENCHMARK(build_idx)              ->Args({1})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)         ->Args({1,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)            ->Args({1,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({1,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({1,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({1,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({1,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({1,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({1,12})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({1,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({1,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({1,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({1,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({1,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({1,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({1,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({1,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({1,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({1,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({1,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({1,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({1,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({1,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({1,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({1,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({1,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({1,12 })  ->Unit(benchmark::kMicrosecond);
//
//
//
//BENCHMARK(build_idx)              ->Args({2})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)         ->Args({2,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)            ->Args({2,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({2,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({2,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({2,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({2,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({2,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({2,12})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({2,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({2,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({2,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({2,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({2,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({2,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({2,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({2,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({2,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({2,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({2,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({2,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({2,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({2,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({2,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({2,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({2,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({2,12 })  ->Unit(benchmark::kMicrosecond);
//
//
//BENCHMARK(build_idx)              ->Args({4})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)         ->Args({4,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)            ->Args({4,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({4,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({4,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({4,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({4,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({4,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({4,12})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({4,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({4,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({4,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({4,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({4,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({4,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({4,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({4,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({4,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({4,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({4,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({4,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({4,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({4,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({4,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({4,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({4,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({4,12 })  ->Unit(benchmark::kMicrosecond);
//
//
//
//BENCHMARK(build_idx)              ->Args({5})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)         ->Args({5,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)            ->Args({5,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({5,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({5,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({5,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({5,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({5,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({5,12})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({5,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({5,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({5,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({5,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({5,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({5,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({5,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({5,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({5,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({5,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({5,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({5,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({5,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({5,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({5,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({5,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({5,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({5,12 })  ->Unit(benchmark::kMicrosecond);
//
//
//BENCHMARK(build_idx)              ->Args({6})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)         ->Args({6,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)            ->Args({6,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({6,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({6,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({6,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({6,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({6,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({6,12})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({6,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({6,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({6,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({6,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({6,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({6,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({6,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({6,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({6,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({6,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({6,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({6,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({6,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({6,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({6,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({6,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({6,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({6,12 })  ->Unit(benchmark::kMicrosecond);
//
//
//
//
//
//BENCHMARK(build_idx)              ->Args({7})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)         ->Args({7,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)            ->Args({7,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({7,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({7,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({7,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({7,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({7,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({7,12})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({7,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({7,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({7,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({7,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({7,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({7,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({7,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({7,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({7,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({7,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({7,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({7,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({7,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({7,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({7,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({7,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({7,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({7,12 })  ->Unit(benchmark::kMicrosecond);
//
//
//
//
//BENCHMARK(build_idx)              ->Args({8})     ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_no_trie)         ->Args({8,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(locate_trie)            ->Args({8,2})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({8,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({8,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({8,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({8,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({8,10})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate)           ->Args({8,12})  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({8,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({8,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({8,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({8,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({8,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_locate_prefix)    ->Args({8,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({8,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({8,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({8,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({8,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({8,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_trie_locate)      ->Args({8,12 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({8,2 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({8,4 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({8,6 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({8,8 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({8,10 })  ->Unit(benchmark::kMicrosecond);
//BENCHMARK(qgram_dfs_locate)       ->Args({8,12 })  ->Unit(benchmark::kMicrosecond);
//
//
//
//


BENCHMARK_MAIN();
