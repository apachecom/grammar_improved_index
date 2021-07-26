//
// Created by alejandro on 25-07-21.
//


//
// Created by via56 on 02-04-20.
//
#include <iostream>
#include <fstream>
#include <benchmark/benchmark.h>

#include "../SelfGrammarIndexBS.h"
#include "../SelfGrammarIndexPTS.h"
#include "../SelfGrammarIndexBSQ.h"
#include "sdsl/io.hpp"

#define MAX_LEN_PATT 100
#define MAX_OCC 1e9
#define MAX_SAMPLES 1000

std::vector<std::string> patterns;

void load_patterns(const std::string& pattern_file){
    std::fstream in(pattern_file,std::ios::in|std::ios::binary);
    if(in.good()){
        uint32_t len, samples;
        in.read((char *)&len,sizeof (uint32_t));
        in.read((char *)&samples,sizeof (uint32_t));
        char *buff = new char[len];
        for (uint32_t i = 0; i < samples ; ++i) {
            in.read(buff,len);
            std::string ss;
            patterns.push_back(ss);
        }
        delete buff;
    }



    auto gibslocate = [](benchmark::State &st, const string &file_index, const uint& len, bool trie
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
                if(trie)
                    idx_gibs.locate(query,X);
                else
                    idx_gibs.locateNoTrie(query,X);
//
                std::sort(X.begin(),X.end());
                auto last = std::unique(X.begin(),X.end());
                X.erase(last,X.end());

                nocc += X.size();
                ptt++;

            }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

        }

        st.counters["pLen"] = len;
        st.counters["queries"] = ptt;
        st.counters["nOcc"] = nocc;

        if(trie){
            st.counters["size"] = idx_gibs.size_in_bytes();
        }else{
            st.counters["size"] = idx_gibs.size_in_bytes() - idx_gibs.get_grammar().get_right_trie().size_in_bytes() - idx_gibs.get_grammar().get_left_trie().size_in_bytes();
        }

    };

    auto gibslocate_split = [](benchmark::State &st, const string &file_index, const uint& len, bool trie
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
        uint64_t total_p = 0, total_s = 0;

        for (auto _ : st)
        {
            nocc = 0;
            ptt = 0;
//#ifdef MEM_MONITOR
//        mm.event("R-INDEX-BUILD");
//#endif

            total_p = 0, total_s = 0;
            for (uint ii=  0; ii < MAX_SAMPLES &&  ii < patterns.size();++ii) {
//            std::cout<<"query:"<<ii<<std::endl;
                std::string query;
                query.resize(len);
                uint64_t time_p =0 , time_s = 0;
                std::copy(patterns[ii].begin(),patterns[ii].begin()+len,query.begin());
                std::vector<uint> X;
                idx_gibs.locateNoTrie(query,X,time_p,time_s);
                std::sort(X.begin(),X.end());
                auto last = std::unique(X.begin(),X.end());
                X.erase(last,X.end());
                total_p +=  time_p;
                total_s +=  time_s;
                nocc += X.size();
                ptt++;

            }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

        }

        st.counters["pLen"] = len;
        st.counters["queries"] = ptt;
        st.counters["nOcc"] = nocc;
        st.counters["time_primary"] = total_p;
        st.counters["time_secondary"] = total_s;
        st.counters["size"] = idx_gibs.size_in_bytes() - idx_gibs.get_grammar().get_right_trie().size_in_bytes() - idx_gibs.get_grammar().get_left_trie().size_in_bytes();

    };


    auto giptslocate = [](benchmark::State &st, const string &file_index, const uint& len, const uint& sampling,bool trie
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

                if(trie)
                    idx_gipts.locate(query,X);
                else
                    idx_gipts.locateNoTrie(query,X);

                std::sort(X.begin(),X.end());
                auto last = std::unique(X.begin(),X.end());
                X.erase(last,X.end());

                nocc += X.size();
                ptt++;

            }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

        }


        st.counters["pLen"] = len;
        st.counters["queries"] = ptt;
        st.counters["nOcc"] = nocc;
        if(trie){
            st.counters["size"] = idx_gipts.size_in_bytes();
        }else{
            st.counters["size"] = idx_gipts.size_in_bytes() - idx_gipts.get_grammar().get_right_trie().size_in_bytes() - idx_gipts.get_grammar().get_left_trie().size_in_bytes();
        }

//    st.counters["size"] = idx_gipts.size_in_bytes();

    };


    auto giptslocate_split = [](benchmark::State &st, const string &file_index, const uint& len, const uint& sampling,bool trie
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
        uint64_t total_p = 0, total_s = 0;


        for (auto _ : st)
        {
            nocc = 0;
            ptt = 0;
//#ifdef MEM_MONITOR
//        mm.event("R-INDEX-BUILD");
//#endif
            total_p = 0, total_s = 0;
            for (uint ii=  0; ii < MAX_SAMPLES &&  ii < patterns.size();++ii) {

                std::string query;
                query.resize(len);
                std::copy(patterns[ii].begin(),patterns[ii].begin()+len,query.begin());
                std::vector<uint> X;

                uint64_t time_p = 0, time_s = 0;
                idx_gipts.locateNoTrie(query,X, time_p, time_s);
                std::sort(X.begin(),X.end());
                auto last = std::unique(X.begin(),X.end());
                X.erase(last,X.end());

                nocc += X.size();
                ptt++;
                total_p += time_p;
                total_s += time_s;

            }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

        }


        st.counters["pLen"] = len;
        st.counters["queries"] = ptt;
        st.counters["nOcc"] = nocc;
        st.counters["time_primary"] = total_p;
        st.counters["time_secondary"] = total_s;
        st.counters["size"] = idx_gipts.size_in_bytes() - idx_gipts.get_grammar().get_right_trie().size_in_bytes() - idx_gipts.get_grammar().get_left_trie().size_in_bytes();

//    st.counters["size"] = idx_gipts.size_in_bytes();

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
        std::string pattern_file = argv[2];

     std::cout<<"INV_PI_T_TRIE: "<<INV_PI_T_TRIE<<std::endl;
        std::cout<<"INV_PI_T_QGRAM: "<<INV_PI_T_QGRAM<<std::endl;

#ifdef BUILD_EXTERNAL_INDEXES
        std::cout<<"RUNING EXTERNAL INDEXES: ON"<<std::endl;
#endif
#ifdef MEM_MONITOR
        std::cout<<"USING MEM_MONITOR: ON"<<std::endl;
#endif

#ifdef PRINT_LOGS
        std::cout<<"MODE LOG ACTIVE:ON"<<std::endl;
#endif


        load_patterns(pattern_file);
        std::cout<<"PATTERNS LOADED FROM: "<<pattern_file<<std::endl;

        benchmark::RegisterBenchmark("G-INDEX-BINARY_SEARCH-NOTRIE",gibslocate_split,index_prefix,i,0)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<4>-NOTRIE",giptslocate_split,index_prefix,i,4,0)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<16>-NOTRIE",giptslocate_split,index_prefix,i,16,0)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<64>-NOTRIE",giptslocate_split,index_prefix,i,64,0)->Unit(benchmark::kMicrosecond);



        benchmark::Initialize(&argc, argv);
        benchmark::RunSpecifiedBenchmarks();

        return 0;

    }
