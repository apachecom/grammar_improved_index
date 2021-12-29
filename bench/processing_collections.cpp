//
// Created by alpachec on 30-09-18.
//


#include <iostream>
#include <fstream>
//#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include "repetitive_collections.h"




int main (int argc, char *argv[] ){

    if(argc < 1){
        std::cout<<"bad parameters....";
        return 0;
    }
    ///slp_build2();
    //// build improve grammar index patrcia tree /////
    std::string collection(argv[1]);

    if(collections_code.find(collection) == collections_code.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
        return 0;
    }

    code = collections_code[collection];

    std::fstream f(collection, std::ios::in| std::ios::binary);
    std::string data;

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
        if(data[i] == 0 || data[i] == 1)
            data[i] = 2;
    }

    uint len = atoi(argv[2]);
    uint n = atoi(argv[3]);
    uint size_ranges = atoi(argv[4]);

    if(len >= data.size()/2)
    {
        std::cout<<"error len > data.size()/2\n";
    }

    std::string name_test_patt = "patterns "+ collection;
    std::string name_test_rg = "ranges "+ collection;

    benchmark::RegisterBenchmark(name_test_patt.c_str(), create_patterns, data, len , n);
    benchmark::RegisterBenchmark(name_test_rg.c_str()  , create_ranges, data, size_ranges , n);


    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


    return 0;

}