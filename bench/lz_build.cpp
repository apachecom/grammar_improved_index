//
// Created by alpachec on 19-12-18.
//


#include <iostream>
#include <fstream>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include "repetitive_collections.h"

#include <lzend/static_selfindex_lzend.h>
#include <lzend/static_selfindex_lz77.h>

std::string data;

auto lz77build = [](benchmark::State &st, const string &file_collection){


    unsigned char br=1;
    unsigned char bs=1;
    unsigned char ss=1;
    lz77index::static_selfindex* idx ;
    std::string filename = std::to_string(collections_code[file_collection])+"lz77";

    std::string tmp_coll = "temp_collection_lz";
    char* _f = (char *)filename.c_str();
    char* file = (char *) tmp_coll.c_str();
    for (auto _ : st)
    {
        idx = lz77index::static_selfindex_lz77::build(file,_f, br, bs, ss);
    }
    st.counters["Size"] = idx->size();
    delete idx;

};




int main (int argc, char *argv[] ){

    if(argc < 1){
        std::cout<<"bad parameters....";
        return 0;
    }

    std::string collection = (argv[1]);

    std::fstream f(collection, std::ios::in| std::ios::binary);
    //std::string data;
    if(!f.is_open()){
        std::cout<<"Error the file could not opened!!\n";
        return 0;
    }
    std::string buff;
    unsigned char buffer[1000];
    while(!f.eof()){
        f.read((char*)buffer,1000);
        data.append((char*) buffer,f.gcount());
    }
    for (int i = 0; i < data.size(); ++i) {
        if(data[i] == 0 || data[i] == 1 || data[i] == 2)
            data[i] = 65;
    }

    f.close();
    std::fstream f2("temp_collection_lz",std::ios::out );

    for(size_t i = 0 ; i < data.size(); ++i)
    {
        f2 << data[i];
    }
    char cc = 0;
    f2 << cc;

    f2.close();


    benchmark::RegisterBenchmark("LZ77", lz77build, collection);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


}
