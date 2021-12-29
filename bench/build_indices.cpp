//
// Created by via56 on 01-04-20.
//

#include <iostream>
#include <fstream>
#include <benchmark/benchmark.h>
#include "../SelfGrammarIndexBS.h"
#include "sdsl/io.hpp"

std::string data = "";
int load_data(const std::string &collection){
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

    f.close();
    std::fstream f2("./temp_collection",std::ios::out );
    for(size_t i = 0 ; i < data.size(); ++i)
        f2 << data[i];

    f2 << '\0';

    f2.close();
    std::cout<<"DATA LOADED\n";
    return 1;
}

auto g_imp_build_basics = [](benchmark::State &st, const string &file_collection, const std::string& file_out){

    std::fstream fbasics(file_out+"-basics", std::ios::out| std::ios::binary);
    std::fstream fsuffixes(file_out+"-suffixes", std::ios::out| std::ios::binary);
    std::fstream frepair(file_out+"-grepair", std::ios::out| std::ios::binary);

    SelfGrammarIndexBS g_index;
    grammar not_compressed_grammar;
    std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx;

    for (auto _ : st)
    {
        g_index.build_basics(data,not_compressed_grammar,grammar_sfx);

    }

    fstream f_gidx(file_out+"-bs.gi", std::ios::out | std::ios::binary);

    g_index.save(f_gidx);
    g_index.save(fbasics);
    not_compressed_grammar.save(frepair);
    unsigned long num_sfx = grammar_sfx.size();
    sdsl::serialize(num_sfx,fsuffixes);


    for (auto && e : grammar_sfx ) {

        size_t p = e.first.first;
        sdsl::serialize(p,fsuffixes);
        p = e.first.second;
        sdsl::serialize(p,fsuffixes);
        p = e.second.first;
        sdsl::serialize(p,fsuffixes);
        p = e.second.second;
        sdsl::serialize(p,fsuffixes);
    }
    st.counters["size"] = g_index.size_in_bytes();
};


int main (int argc, char *argv[] ){

        if(argc < 2){
        std::cout<<"bad parameters...."<<std::endl;
        return 0;
    }

    std::string collection  = argv[1];
    std::string path_out    = argv[2];
    load_data(collection);
    benchmark::RegisterBenchmark("G-INDEX",  g_imp_build_basics ,collection,path_out);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;

}
