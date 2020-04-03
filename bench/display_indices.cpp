//
// Created by via56 on 03-04-20.
//

#include <iostream>
#include <fstream>
#include <benchmark/benchmark.h>

#include <slp/RePairSLPIndex.h>
#include "../SelfGrammarIndexBS.h"
#include <ri/r_index.hpp>
#include "../SelfGrammarIndexPTS.h"
#include "../SelfGrammarIndexBSQ.h"
#include "sdsl/io.hpp"

#define MAX_LEN_PATT 100

#define MAX_OCC 1e9
#define MAX_SAMPLES 1000

std::vector<size_t> pos;

void load_ptos(const std::string& pos_file){
    std::cout<<pos_file<<std::endl;
    pos.resize(MAX_SAMPLES);
    std::fstream f(pos_file, std::ios::in| std::ios::binary);
    if(!f.is_open()){
        std::cout<<"Error the pos file could not opened!!\n";
    }
    std::string buff; uint i = 0;
    while (i < MAX_SAMPLES && !f.eof() && std::getline(f, buff)) {

        pos[i] = atoi(buff.c_str());
        std::cout<<i<<" "<<pos[i]<<std::endl;
        ++i;
    }
    f.close();
}


auto slpdisplay = [](benchmark::State &st, const string &file_index, const uint& len, const uint& sampling
#ifdef MEM_MONITOR
        , const std::string file_mem_monitor
#endif
){
    /**
     * load slpindex
     * */


    cds_static::RePairSLPIndex* idx_slp;
    std::string filename = file_index+"-q"+std::to_string(sampling);
    char* _f = (char *)filename.c_str();
    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);


//    std::cout<<"slp-index loaded"<<std::endl;

    uint nocc,ptt;

    for (auto _ : st)
    {
        ptt = 0;
#ifdef MEM_MONITOR
        mm.event("R-INDEX-BUILD");
#endif
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < pos.size();++ii) {
           unsigned char * s = idx_slp->RePairSLPIndex::extract(pos[ii],pos[ii]+len);
            delete s;
            ++ptt;
        }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

    }

    st.counters["pLen"] = len;
    st.counters["queies"] = ptt;

};

auto gibsdisplay = [](benchmark::State &st, const string &file_index, const uint& len
#ifdef MEM_MONITOR
        , const std::string file_mem_monitor
#endif
){
    /**
     * load gibsindex
     * */


    std::fstream  f  (file_index+"-bs.gi",std::ios::in|std::ios::binary);
    SelfGrammarIndexBS idx_gibs;
    idx_gibs.load(f);

//    std::cout<<"bsgi-index loaded"<<std::endl;
    uint queries;

    std::string ss = "";
//    ss.resize(len);

    for (auto _ : st)
    {
        queries = 0;
#ifdef MEM_MONITOR
        mm.event("R-INDEX-BUILD");
#endif
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < pos.size();++ii) {

            idx_gibs.display_L(pos[ii],pos[ii]+len,ss);
        }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

    }

    st.counters["pLen"] = len;
    st.counters["nOcc"] = queries;

};



auto gibsdisplaybs = [](benchmark::State &st, const string &file_index, const uint& len
#ifdef MEM_MONITOR
        , const std::string file_mem_monitor
#endif
){
    /**
     * load gibsindex
     * */


    std::fstream  f  (file_index+"-bs.gi",std::ios::in|std::ios::binary);
    SelfGrammarIndexBS idx_gibs;
    idx_gibs.load(f);

//    std::cout<<"bsgi-index loaded"<<std::endl;
    uint queries;

    std::string ss = "";
//    ss.resize(len);

    for (auto _ : st)
    {
        queries = 0;
#ifdef MEM_MONITOR
        mm.event("R-INDEX-BUILD");
#endif
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < pos.size();++ii) {

            idx_gibs.display(pos[ii],pos[ii]+len,ss);
        }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

    }

    st.counters["pLen"] = len;
    st.counters["nOcc"] = queries;

};


int main (int argc, char *argv[] ){

    if(argc < 2){
        std::cout<<"bad parameters...."<<std::endl;
        return 0;
    }
    /**
     * collection-name
     */
    std::string index_prefix  = argv[1];


#ifdef MEM_MONITOR
    /**
     * mem monitor file out path
     */
    std::string mem_out = argv[6];
#endif

    uint min_len_patten = std::atoi(argv[3]);
    uint max_len_patten = std::atoi(argv[4]);
    uint gap_len_patten = std::atoi(argv[5]);

    std::string ptos_file = argv[2];



    load_ptos(ptos_file+"-"+std::to_string(max_len_patten)+".pos");
    std::cout<<"PATTERNS LOADED FROM: "<<ptos_file+"-"+std::to_string(max_len_patten)+".pos"<<std::endl;

    for (uint i = min_len_patten; i <= max_len_patten; i+=gap_len_patten)
    {

        std::cout<<"Searching patterns len:"<<i<<std::endl;
//        benchmark::RegisterBenchmark("R-Index",rilocate,index_prefix,i);
        benchmark::RegisterBenchmark("SLP-Index<4>" ,slpdisplay,index_prefix,i,4);
        benchmark::RegisterBenchmark("SLP-Index<8>" ,slpdisplay,index_prefix,i,8);
        benchmark::RegisterBenchmark("SLP-Index<12>",slpdisplay,index_prefix,i,12);
        benchmark::RegisterBenchmark("SLP-Index<16>",slpdisplay,index_prefix,i,16);
        benchmark::RegisterBenchmark("G-INDEX-RANK-PHRASES",gibsdisplay,index_prefix,i);
        benchmark::RegisterBenchmark("G-INDEX-BS",gibsdisplay,index_prefix,i);

    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;

}
