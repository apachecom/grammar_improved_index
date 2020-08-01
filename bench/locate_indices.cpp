
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


    char *buff = new char[max_len]; uint i = 0;

    for (int k = 0; k < samples && !f.eof() ; ++k)
    {
        f.read(buff,max_len);
        std::string pp; pp.resize(max_len);
        std::copy(buff,buff+max_len,pp.begin());
        patterns.push_back(pp);

    }

    delete buff;
    
    f.close();
}


auto rilocate = [](benchmark::State &st, const string &file_index, const uint& len
//#ifdef MEM_MONITOR
//        , const std::string file_mem_monitor
//#endif
){
    /**
     * load rindex
     *
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
    st.counters["size"] = idx_r->print_space();

    delete idx_r;

};



auto slplocate = [](benchmark::State &st, const string &file_index, const uint& len, const uint& sampling , bool bal = false
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

            // std::sort(pos->begin(),pos->end());
            // auto last = std::unique(pos->begin(),pos->end());
            // pos->erase(last,pos->end());

            delete pos;
            // nocc += occs;
            nocc += occs;
            ptt++;
        }

//        std::cout<<nocc<<std::endl;
//        sleep(2);

    }

    st.counters["pLen"] = len;
    st.counters["queries"] = ptt;
    st.counters["nOcc"] = nocc;

    st.counters["size"] = idx_slp->size();

    delete idx_slp;

};




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


auto giqgramlocate = [](benchmark::State &st, const string &file_index, const uint& len, const uint& sampling,int op = 0
//#ifdef MEM_MONITOR
//        , const std::string file_mem_monitor
//#endif
){
    /**
     * load gibsindex
     * */
    std::fstream  fbs (file_index+"-bs.gi",std::ios::in|std::ios::binary);

    std::ifstream  f  (file_index+"-gram-"+std::to_string(sampling)+"-smp.gi",std::ios::in|std::ios::binary);
    std::ifstream  fg  (file_index+"-gram-"+std::to_string(sampling)+"-smp-g.gi",std::ios::in|std::ios::binary);
    std::ifstream  frev  (file_index+"-gram-"+std::to_string(sampling)+"-smp-rev.gi",std::ios::in|std::ios::binary);
    std::ifstream  fseq  (file_index+"-gram-"+std::to_string(sampling)+"-smp-seq.gi",std::ios::in|std::ios::binary);

    SelfGrammarIndexBSQ idx_giqbs;

    idx_giqbs.load_basics(fbs);
//    std::cout<<"basic-index-loaded\n";
    if(!f.is_open() || !fg.is_open() || !frev.is_open() || !fseq.is_open()){
        std::cout<<file_index+"-gram-"+std::to_string(sampling)+"-smp.gi"<<std::endl;
        throw "ERROR OPENING FILES QGRAM";
    }
    idx_giqbs.loadSampling(f,fg,frev,fseq);

//    std::cout<<"bsgi-index loaded"<<std::endl;
    uint queries,nocc;

    std::string ss;
//    ss.resize(len);

    for (auto _ : st)
    {
        queries = 0;
        nocc =0;
//#ifdef MEM_MONITOR
//        mm.event("R-INDEX-BUILD");
//#endif
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < patterns.size();++ii) {

//            std::cout<<"start query\n";
            std::string query;
            query.resize(len);
            std::copy(patterns[ii].begin(),patterns[ii].begin()+len,query.begin());
            std::vector<uint> X;

//            std::cout <<query<<"-"<< queries << std::endl;
            if(op == 2){
                idx_giqbs.qgram_trie_locate(query,X);
            }else{
                if(op == 3){
                    idx_giqbs.qgram_locate_prefix_search(query,X);
                } else{
                    idx_giqbs.qgram_dfs_locate(query,X);
                }
            }



            nocc += X.size();
            queries++;

        }

    }


    st.counters["pLen"] = len;
    st.counters["queries"] = queries;
    st.counters["nOcc"] = nocc;
    if(op == 2){
        st.counters["size"] = idx_giqbs.size_in_bytes();
    }else{
        st.counters["size"] = idx_giqbs.size_in_bytes() - idx_giqbs.get_grammar().get_right_trie().size_in_bytes() - idx_giqbs.get_grammar().get_left_trie().size_in_bytes();
    }

//
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


    uint min_len_patten = std::atoi(argv[3]);
    uint max_len_patten = std::atoi(argv[4]);
    uint gap_len_patten = std::atoi(argv[5]);


    std::cout<<"INV_PI_T: "<<INV_PI_T<<std::endl;
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



    load_patterns(pattern_file+"-"+std::to_string(max_len_patten)+".ptt",max_len_patten,1000);
    std::cout<<"PATTERNS LOADED FROM: "<<pattern_file+"-"+std::to_string(max_len_patten)+".ptt"<<std::endl;

    for (uint i = min_len_patten; i <= max_len_patten; i+=gap_len_patten)
    {
//        std::cout<<"PATTERN EXAMPLE: "<<patterns[0]<<std::endl;

        // std::cout<<"Searching patterns len:"<<i<<std::endl;

#ifdef BUILD_EXTERNAL_INDEXES
        benchmark::RegisterBenchmark("R-INDEX",rilocate,index_prefix,i)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("SLP-INDEX<2>" ,slplocate,index_prefix,i,2)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("SLP-INDEX<4>" ,slplocate,index_prefix,i,4)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("SLP-INDEX<6>" ,slplocate,index_prefix,i,6)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("SLP-INDEX<8>" ,slplocate,index_prefix,i,8)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("SLP-INDEX<10>",slplocate,index_prefix,i,10)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("SLP-INDEX<12>",slplocate,index_prefix,i,12)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("SLP-INDEX-BAL<2>" ,slplocate,index_prefix,i,2,true)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("SLP-INDEX-BAL<4>" ,slplocate,index_prefix,i,4,true)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("SLP-INDEX-BAL<6>" ,slplocate,index_prefix,i,6,true)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("SLP-INDEX-BAL<8>" ,slplocate,index_prefix,i,8,true)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("SLP-INDEX-BAL<10>",slplocate,index_prefix,i,10,true)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("SLP-INDEX-BAL<12>",slplocate,index_prefix,i,12,true)->Unit(benchmark::kMicrosecond);
#endif
        benchmark::RegisterBenchmark("G-INDEX-BINARY_SEARCH-TRIE",gibslocate,index_prefix,i,1)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-BINARY_SEARCH-NOTRIE",gibslocate,index_prefix,i,0)->Unit(benchmark::kMicrosecond);


        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<2>-TRIE",giptslocate,index_prefix,i,2,1)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<4>-TRIE",giptslocate,index_prefix,i,4,1)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<8>-TRIE",giptslocate,index_prefix,i,8,1)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<16>-TRIE",giptslocate,index_prefix,i,16,1)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<32>-TRIE",giptslocate,index_prefix,i,32,1)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<64>-TRIE",giptslocate,index_prefix,i,64,1)->Unit(benchmark::kMicrosecond);

        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<2>-NOTRIE",giptslocate,index_prefix,i,2,0)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<4>-NOTRIE",giptslocate,index_prefix,i,4,0)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<8>-NOTRIE",giptslocate,index_prefix,i,8,0)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<16>-NOTRIE",giptslocate,index_prefix,i,16,0)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<32>-NOTRIE",giptslocate,index_prefix,i,32,0)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<64>-NOTRIE",giptslocate,index_prefix,i,64,0)->Unit(benchmark::kMicrosecond);

        benchmark::RegisterBenchmark("G-INDEX-QGRAM<2>-CMP_DFS" ,giqgramlocate,index_prefix,i,2,1)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<4>-CMP_DFS" ,giqgramlocate,index_prefix,i,4,1)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<6>-CMP_DFS" ,giqgramlocate,index_prefix,i,6,1)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<8>-CMP_DFS" ,giqgramlocate,index_prefix,i,8,1)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<10>-CMP_DFS",giqgramlocate,index_prefix,i,10,1)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<12>-CMP_DFS",giqgramlocate,index_prefix,i,12,1)->Unit(benchmark::kMicrosecond);

        benchmark::RegisterBenchmark("G-INDEX-QGRAM<2>-CMP_TRIE" ,giqgramlocate,index_prefix,i,2,2)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<4>-CMP_TRIE" ,giqgramlocate,index_prefix,i,4,2)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<6>-CMP_TRIE" ,giqgramlocate,index_prefix,i,6,2)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<8>-CMP_TRIE" ,giqgramlocate,index_prefix,i,8,2)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<10>-CMP_TRIE",giqgramlocate,index_prefix,i,10,2)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<12>-CMP_TRIE",giqgramlocate,index_prefix,i,12,2)->Unit(benchmark::kMicrosecond);

        benchmark::RegisterBenchmark("G-INDEX-QGRAM<2>-CMP_SMP" ,giqgramlocate,index_prefix,i,2,3)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<4>-CMP_SMP" ,giqgramlocate,index_prefix,i,4,3)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<6>-CMP_SMP" ,giqgramlocate,index_prefix,i,6,3)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<8>-CMP_SMP" ,giqgramlocate,index_prefix,i,8,3)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<10>-CMP_SMP",giqgramlocate,index_prefix,i,10,3)->Unit(benchmark::kMicrosecond);
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<12>-CMP_SMP",giqgramlocate,index_prefix,i,12,3)->Unit(benchmark::kMicrosecond);

    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

return 0;

}
