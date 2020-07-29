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
//        std::cout<<i<<" "<<pos[i]<<std::endl;
        ++i;
    }
    f.close();
}


auto slpdisplay = [](benchmark::State &st, const string &file_index, const uint& len, const uint& sampling, bool bal = false
//#ifdef MEM_MONITOR
//        , const std::string file_mem_monitor
//#endif
){
    /**
     * load slpindex
     * */


    cds_static::RePairSLPIndex* idx_slp;


    std::string filename;
    if(bal)
        filename = file_index+"-bal-q"+std::to_string(sampling);
    else
        filename = file_index+"-q"+std::to_string(sampling);

    char* _f = (char *)filename.c_str();
    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);



    uint nocc,ptt;

    for (auto _ : st)
    {
        ptt = 0;
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < pos.size();++ii) {
           unsigned char * s = idx_slp->RePairSLPIndex::extract(pos[ii],pos[ii]+len);
            delete s;
            ++ptt;
        }

    }

    st.counters["pLen"] = len;
    st.counters["queries"] = ptt;

};

auto gibsdisplay = [](benchmark::State &st, const string &file_index, const uint& len, bool trie = false
){


    std::fstream  f  (file_index+"-bs.gi",std::ios::in|std::ios::binary);
    SelfGrammarIndexBS idx_gibs;
    idx_gibs.load(f);

    uint queries;

    std::string ss = "";

    for (auto _ : st)
    {
        queries = 0;
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < pos.size();++ii) {

            if(trie)
                idx_gibs.display_L_trie(pos[ii],pos[ii]+len,ss);
            else
                idx_gibs.display_L(pos[ii],pos[ii]+len,ss);
            queries++;
        }

    }

    st.counters["pLen"] = len;
    st.counters["queries"] = queries;

};



auto gibsdisplaybs = [](benchmark::State &st, const string &file_index, const uint& len, bool trie = false
){

    std::fstream  f  (file_index+"-bs.gi",std::ios::in|std::ios::binary);
    SelfGrammarIndexBS idx_gibs;
    idx_gibs.load(f);
    uint queries;

    std::string ss = "";

    for (auto _ : st)
    {
        queries = 0;
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < pos.size();++ii) {
            if(trie)
                idx_gibs.display_trie(pos[ii],pos[ii]+len,ss);
            else
                idx_gibs.display(pos[ii],pos[ii]+len,ss);
            queries++;
        }

    }

    st.counters["pLen"] = len;
    st.counters["queries"] = queries;

};



auto giqgramdisplay = [](benchmark::State &st, const string &file_index, const uint& len, const uint& sampling, int op = 0
){

    std::fstream  fbs (file_index+"-bs.gi",std::ios::in|std::ios::binary);

    std::ifstream  f  (file_index+"-gram-"+std::to_string(sampling)+"-smp.gi",std::ios::in|std::ios::binary);
    std::ifstream  fg  (file_index+"-gram-"+std::to_string(sampling)+"-smp-g.gi",std::ios::in|std::ios::binary);
    std::ifstream  frev  (file_index+"-gram-"+std::to_string(sampling)+"-smp-rev.gi",std::ios::in|std::ios::binary);
    std::ifstream  fseq  (file_index+"-gram-"+std::to_string(sampling)+"-smp-seq.gi",std::ios::in|std::ios::binary);

    SelfGrammarIndexBSQ idx_giqbs;
    if(!f.is_open() || !fg.is_open() || !frev.is_open() || !fseq.is_open()){
        std::cout<<file_index+"-gram-"+std::to_string(sampling)+"-smp.gi"<<std::endl;
        throw "ERROR OPENING FILES QGRAM";
    }
    idx_giqbs.load_basics(fbs);
    idx_giqbs.loadSampling(f,fg,frev,fseq);
    uint queries;

    std::string ss = "";

    for (auto _ : st)
    {
        queries = 0;
//#ifdef MEM_MONITOR
//        mm.event("R-INDEX-BUILD");
//#endif
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < pos.size();++ii) {

            if(op == 1)
                idx_giqbs.display_qgram_rec(pos[ii],pos[ii]+len,ss);
            else
                idx_giqbs.display_qgram(pos[ii],pos[ii]+len,ss);

            queries++;
        }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

    }


    st.counters["pLen"] = len;
    st.counters["queries"] = queries;
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


//#ifdef MEM_MONITOR
//    /**
//     * mem monitor file out path
//     */
//    std::string mem_out = argv[6];
//#endif

    uint min_len_patten = std::atoi(argv[3]);
    uint max_len_patten = std::atoi(argv[4]);
    uint gap_len_patten = std::atoi(argv[5]);

    std::string ptos_file = argv[2];



    load_ptos(ptos_file+"-"+std::to_string(max_len_patten)+".pos");
    std::cout<<"POS LOADED FROM: "<<ptos_file+"-"+std::to_string(max_len_patten)+".pos"<<std::endl;
    std::cout<<"POS LEN: "<<pos.size()<<std::endl;
    std::cout<<"POS EX: "<<pos[0]<<std::endl;

    for (uint i = min_len_patten; i <= max_len_patten; i+=gap_len_patten)
    {

//        std::cout<<"Searching patterns len:"<<i<<std::endl;

//#ifdef BUILD_EXTERNAL_INDEXES
        benchmark::RegisterBenchmark("SLP-INDEX<2>" ,slpdisplay,index_prefix,i,2)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("SLP-INDEX<4>" ,slpdisplay,index_prefix,i,4)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("SLP-INDEX<6>" ,slpdisplay,index_prefix,i,6)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("SLP-INDEX<8>" ,slpdisplay,index_prefix,i,8)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("SLP-INDEX<10>",slpdisplay,index_prefix,i,10)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("SLP-INDEX<12>",slpdisplay,index_prefix,i,12)->Unit({benchmark::kMicrosecond});;

        benchmark::RegisterBenchmark("SLP-INDEX-BAL<2>" ,slpdisplay,index_prefix,i,2)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("SLP-INDEX-BAL<4>" ,slpdisplay,index_prefix,i,4)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("SLP-INDEX-BAL<6>" ,slpdisplay,index_prefix,i,6)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("SLP-INDEX-BAL<8>" ,slpdisplay,index_prefix,i,8)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("SLP-INDEX-BAL<10>",slpdisplay,index_prefix,i,10)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("SLP-INDEX-BAL<12>",slpdisplay,index_prefix,i,12)->Unit({benchmark::kMicrosecond});;
//#endif
        benchmark::RegisterBenchmark("G-INDEX-RANK-PHRASES-NOTRIE",gibsdisplay,index_prefix,i)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G-INDEX-RANK-PHRASES-TRIE",gibsdisplay,index_prefix,i,true)->Unit({benchmark::kMicrosecond});;

        benchmark::RegisterBenchmark("G_INDEX_BINARY_SEARCH_LEAF-NOTRIE",gibsdisplaybs,index_prefix,i)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G_INDEX_BINARY_SEARCH_LEAF-TRIE",gibsdisplaybs,index_prefix,i,true)->Unit({benchmark::kMicrosecond});;

        benchmark::RegisterBenchmark("G-INDEX-QGRAM<2>_RANK_PHRASES-DFS",giqgramdisplay,index_prefix,i,2)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<4>_RANK_PHRASES-DFS",giqgramdisplay,index_prefix,i,4)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<6>_RANK_PHRASES-DFS",giqgramdisplay,index_prefix,i,6)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<8>_RANK_PHRASES-DFS",giqgramdisplay,index_prefix,i,8)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<10>_RANK_PHRASES-DFS",giqgramdisplay,index_prefix,i,10)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<12>_RANK_PHRASES-DFS",giqgramdisplay,index_prefix,i,12)->Unit({benchmark::kMicrosecond});;

        benchmark::RegisterBenchmark("G-INDEX-QGRAM<2>_RANK_PHRASES-REC",giqgramdisplay,index_prefix,i,2,1)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<4>_RANK_PHRASES-REC",giqgramdisplay,index_prefix,i,4,1)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<6>_RANK_PHRASES-REC",giqgramdisplay,index_prefix,i,6,1)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<8>_RANK_PHRASES-REC",giqgramdisplay,index_prefix,i,8,1)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<10>_RANK_PHRASES-REC",giqgramdisplay,index_prefix,i,10,1)->Unit({benchmark::kMicrosecond});;
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<12>_RANK_PHRASES-REC",giqgramdisplay,index_prefix,i,12,1)->Unit({benchmark::kMicrosecond});;

    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;

}
