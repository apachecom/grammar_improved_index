
//
// Created by via56 on 02-04-20.
//
#include <iostream>
#include <benchmark/benchmark.h>
#include "../SelfGrammarIndexBS.h"
#define MAX_SAMPLES 1000


std::vector<std::string> patterns;
uint32_t load_patterns(const std::string& pattern_file){
    std::fstream in(pattern_file,std::ios::in|std::ios::binary);
    if(in.good()){
        uint32_t len, samples;
        in.read((char *)&len,sizeof (uint32_t));
        in.read((char *)&samples,sizeof (uint32_t));
        char *buff = new char[len];
        for (uint32_t i = 0; i < samples ; ++i) {
            in.read(buff,len);
            std::string ss;ss.resize(len);
	    std::copy(buff, buff+len,ss.begin());
//	    std::cout<<ss<<std::endl;
            patterns.push_back(ss);
        }
        delete buff;
	std::cout<<"Total patterns readed:"<<patterns.size()<<std::endl;
	return len;
	}
    std::cout<<"File not good"<<std::endl;
    return 0;
}

auto gibslocate = [](benchmark::State &st, const string &file_index, const uint& len){
    std::fstream  f  (file_index+"-bs.gi",std::ios::in|std::ios::binary);
    SelfGrammarIndexBS idx_gibs;
    idx_gibs.load(f);
    uint nocc = 0,ptt = 0;
    for (auto _ : st)
    {
        nocc = 0;
        ptt = 0;
        for (uint ii=  0; ii < MAX_SAMPLES &&  ii < patterns.size();++ii) {
                std::vector<uint> X;
                idx_gibs.locateNoTrie(patterns[ii],X);
                std::sort(X.begin(),X.end());
                auto last = std::unique(X.begin(),X.end());
                X.erase(last,X.end());
                nocc += X.size();
                ptt++;

        }

    }

    st.counters["pLen"] = len;
    st.counters["queries"] = ptt;
    st.counters["nOcc"] = nocc;
    st.counters["size"] = idx_gibs.size_in_bytes() - idx_gibs.get_grammar().get_right_trie().size_in_bytes() - idx_gibs.get_grammar().get_left_trie().size_in_bytes();


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


    uint32_t i = load_patterns(pattern_file);
    if(!i) return 0;

    std::cout<<"PATTERNS LOADED FROM: "<<pattern_file<<std::endl;
    std::cout<<"PATTERN LEN: "<<i<<std::endl;
    benchmark::RegisterBenchmark("G-INDEX-BINARY_SEARCH",gibslocate,index_prefix,i)->Unit(benchmark::kMicrosecond);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

return 0;

}
