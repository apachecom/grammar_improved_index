
//
// Created by via56 on 02-04-20.
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

std::vector<std::string> patterns;

void load_patterns(const std::string& pattern_file,uint max_len, uint samples){
    std::cout<<pattern_file<<std::endl;
//    patterns.resize(MAX_SAMPLES);
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);
    if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
    }

    std::string forbb  = "";
    for (size_t i = 0; i < max_len; i++)
    {
        forbb+="N";
    }
    



    char *buff = new char[max_len]; uint i = 0;
    for (int k = 0; k < samples && !f.eof() ; ++k)
    {
        f.read(buff,max_len);
        std::string pp; pp.resize(max_len);
        std::copy(buff,buff+max_len,pp.begin());
        if(pp != forbb){
            patterns.push_back(pp);
            std::cout<<pp<<std::endl;
        }
    }
//
//    while (i < MAX_SAMPLES && !f.eof() && std::getline(f, buff)) {
//        bool obv = 0;
//        for (int j = 0; j <buff.size() ; ++j) {
//            int c = buff[j];
//            if(buff[j] == 0 || buff[j] == 1 || buff[j] == 2  )
//            {
//                std::cout<<"Invalid symbol\n";
//                obv = true;
//                break;
//            }
//        }
//        if(!obv){
//
//            patterns.push_back(buff);
////            std::cout<<i<<" "<<patterns[i]<<std::endl;
//
//            std::cout<<i<<std::endl;
//
//            for (int j = 0; j <buff.size() ; ++j) {
//                std::cout<<(int)buff[j]<<" ";
//            }
//            std::cout<<std::endl;
//
//        }
//
//        ++i;
//    }
    f.close();
}


auto rilocate = [](benchmark::State &st, const string &file_index, const uint& len
//#ifdef MEM_MONITOR
//        , const std::string file_mem_monitor
//#endif
){
    /**
     * load rindex
     * */

    ri::r_index<> *idx_r;

    fstream rf(file_index + ".ri",std::ios::in|std::ios::binary);

    bool fast;
    rf.read((char*)&fast,sizeof(fast));
    idx_r = new ri::r_index<>();
    idx_r->load(rf);


//    std::cout<<"r -index loaded"<<std::endl;
    uint nocc,ptt;
    for (auto _ : st)
    {
        nocc = 0;
        ptt = 0;
//#ifdef MEM_MONITOR
//        mm.event("R-INDEX-BUILD");
//#endif
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < patterns.size();++ii) {
            std::string query;
            query.resize(len);
            std::copy(patterns[ii].begin(),patterns[ii].begin()+len,query.begin());

//            std::cout<<"r -index query:"<<query<<std::endl;
            auto occ = idx_r->locate_all(query);
            nocc += occ.size(); ptt++;
        }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

      }

    st.counters["pLen"] = len;
    st.counters["queries"] = ptt;
    st.counters["nOcc"] = nocc;

    delete idx_r;

};



auto slplocate = [](benchmark::State &st, const string &file_index, const uint& len, const uint& sampling
//#ifdef MEM_MONITOR
//        , const std::string file_mem_monitor
//#endif
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
        nocc = 0;
        ptt = 0;
//#ifdef MEM_MONITOR
//        mm.event("R-INDEX-BUILD");
//#endif
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < patterns.size();++ii) {
            std::string query;
            query.resize(len);
            std::copy(patterns[ii].begin(),patterns[ii].begin()+len,query.begin());
            uint occs;
            uchar *tt = (uchar * )(query.c_str());
//            std::cout<<"slp -index query:"<<query<<std::endl;
            std::vector<uint> *pos = idx_slp->RePairSLPIndex::locate(tt,len, &occs);
            delete pos;
            nocc += occs;ptt++;
        }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

    }

    st.counters["pLen"] = len;
    st.counters["queries"] = ptt;
    st.counters["nOcc"] = nocc;

};




auto gibslocate = [](benchmark::State &st, const string &file_index, const uint& len
//#ifdef MEM_MONITOR
//        , const std::string file_mem_monitor
//#endif
){
    /**
     * load gibsindex
     * */


    std::fstream  f  (file_index+"-bs.gi",std::ios::in|std::ios::binary);
    SelfGrammarIndexBS idx_gibs;
    idx_gibs.load(f);

//    std::cout<<"bsgi-index loaded"<<std::endl;
    uint nocc,ptt;

    for (auto _ : st)
    {
        nocc = 0;
        ptt = 0;
//#ifdef MEM_MONITOR
//        mm.event("R-INDEX-BUILD");
//#endif
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < patterns.size();++ii) {
//            std::cout<<"query:"<<ii<<std::endl;
            std::string query;
            query.resize(len);
            std::copy(patterns[ii].begin(),patterns[ii].begin()+len,query.begin());
            std::vector<uint> X;
            idx_gibs.locateNoTrie(query,X);

            nocc += X.size();
            ptt++;

        }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

    }

    st.counters["pLen"] = len;
    st.counters["queries"] = ptt;
    st.counters["nOcc"] = nocc;
};



auto giptslocate = [](benchmark::State &st, const string &file_index, const uint& len, const uint& sampling
//#ifdef MEM_MONITOR
//        , const std::string file_mem_monitor
//#endif
){
    /**
     * load gibsindex
     * */


    std::fstream  fpts(file_index+"-pts-<"+std::to_string(sampling)+">.gi",std::ios::in|std::ios::binary);

    SelfGrammarIndexPTS idx_gipts(sampling);
    idx_gipts.load(fpts);


//    std::cout<<"bsgi-index loaded"<<std::endl;
    uint nocc,ptt;

    for (auto _ : st)
    {
        nocc = 0;
        ptt = 0;
//#ifdef MEM_MONITOR
//        mm.event("R-INDEX-BUILD");
//#endif
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < patterns.size();++ii) {

            std::string query;
            query.resize(len);
            std::copy(patterns[ii].begin(),patterns[ii].begin()+len,query.begin());
            std::vector<uint> X;
            idx_gipts.locateNoTrie(query,X);

            nocc += X.size();
            ptt++;

        }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

    }


    st.counters["pLen"] = len;
    st.counters["queries"] = ptt;
    st.counters["nOcc"] = nocc;

};


auto giqgramlocate = [](benchmark::State &st, const string &file_index, const uint& len, const uint& sampling
//#ifdef MEM_MONITOR
//        , const std::string file_mem_monitor
//#endif
){
    /**
     * load gibsindex
     * */
    std::fstream  fbs (file_index+"-bs.gi",std::ios::in|std::ios::binary);

    std::ifstream  f  (file_index+"-gram-<"+std::to_string(sampling)+">-smp.gi",std::ios::in|std::ios::binary);
    std::ifstream  fg  (file_index+"-gram-<"+std::to_string(sampling)+">-smp-g.gi",std::ios::in|std::ios::binary);
    std::ifstream  frev  (file_index+"-gram-<"+std::to_string(sampling)+">-smp-rev.gi",std::ios::in|std::ios::binary);
    std::ifstream  fseq  (file_index+"-gram-<"+std::to_string(sampling)+">-smp-seq.gi",std::ios::in|std::ios::binary);

    SelfGrammarIndexBSQ idx_giqbs;

    idx_giqbs.load_basics(fbs);
//    std::cout<<"basic-index-loaded\n";
    idx_giqbs.loadSampling(f,fg,frev,fseq);

//    std::cout<<"bsgi-index loaded"<<std::endl;
    uint queries,nocc;

    std::string ss = "";
//    ss.resize(len);

    for (auto _ : st)
    {
        queries = 0;
        nocc =0;
//#ifdef MEM_MONITOR
//        mm.event("R-INDEX-BUILD");
//#endif
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < patterns.size();++ii) {

            std::string query;
            query.resize(len);
            std::copy(patterns[ii].begin(),patterns[ii].begin()+len,query.begin());
            std::vector<uint> X;
            idx_giqbs.qgram_dfs_locate(query,X);

            nocc += X.size();
            queries++;

        }


//        std::cout<<nocc<<std::endl;
//        sleep(2);

    }


    st.counters["pLen"] = len;
    st.counters["queries"] = queries;
    st.counters["nOcc"] = nocc;
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

    std::string pattern_file = argv[2];



    load_patterns(pattern_file+"-"+std::to_string(max_len_patten)+".ptt",max_len_patten,1000);
    std::cout<<"PATTERNS LOADED FROM: "<<pattern_file+"-"+std::to_string(max_len_patten)+".ptt"<<std::endl;

    for (uint i = min_len_patten; i <= max_len_patten; i+=gap_len_patten)
    {

        // std::cout<<"Searching patterns len:"<<i<<std::endl;
        benchmark::RegisterBenchmark("R-Index",rilocate,index_prefix,i);
        benchmark::RegisterBenchmark("SLP-Index<4>" ,slplocate,index_prefix,i,4);
        benchmark::RegisterBenchmark("SLP-Index<8>" ,slplocate,index_prefix,i,8);
        benchmark::RegisterBenchmark("SLP-Index<12>",slplocate,index_prefix,i,12);
        benchmark::RegisterBenchmark("SLP-Index<16>",slplocate,index_prefix,i,16);

        benchmark::RegisterBenchmark("G-INDEX-BS",gibslocate,index_prefix,i);
        benchmark::RegisterBenchmark("G-INDEX-PTS<2>",giptslocate,index_prefix,i,2);
        benchmark::RegisterBenchmark("G-INDEX-PTS<4>",giptslocate,index_prefix,i,4);
        benchmark::RegisterBenchmark("G-INDEX-PTS<8>",giptslocate,index_prefix,i,8);
        benchmark::RegisterBenchmark("G-INDEX-PTS<16>",giptslocate,index_prefix,i,16);
        benchmark::RegisterBenchmark("G-INDEX-PTS<32>",giptslocate,index_prefix,i,32);
        benchmark::RegisterBenchmark("G-INDEX-PTS<64>",giptslocate,index_prefix,i,64);
//
//
//        benchmark::RegisterBenchmark("G-INDEX-QGRAM<4>" ,giqgramlocate,index_prefix,i,4);
//        benchmark::RegisterBenchmark("G-INDEX-QGRAM<8>" ,giqgramlocate,index_prefix,i,8);
//        benchmark::RegisterBenchmark("G-INDEX-QGRAM<12>",giqgramlocate,index_prefix,i,12);
//        benchmark::RegisterBenchmark("G-INDEX-QGRAM<16>",giqgramlocate,index_prefix,i,16);

    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

return 0;

}
