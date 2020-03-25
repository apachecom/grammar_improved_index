//
// Created by inspironXV on 8/17/2018.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "../collections.cpp"
#include "../../utils/grammar.h"

static void grammar_big_size(benchmark::State& state){

    for(auto _ : state)
    {
        uint folder = state.range(0);
        uint coll   = state.range(1);
        std::string collection = "../" + dirFolder[folder]+dircollection[coll];
        std::fstream f(collection, std::ios::in| std::ios::binary);
        std::string data;
        std::cout << "collection: "<< collection << std::endl;
        if(!f.is_open()){
            std::cout<<"Error the file could not opened!!\n";
            return;
        }
        std::string buff;
        while (!f.eof() && std::getline(f, buff)) {
            data += buff;
        }
        std::cout << "size of the string: " << data.length() << std::endl;
        f.close();
        fstream fg("grp_"+dircollection[coll],std::ios::out|std::ios::binary);
        grammar G;
        /////data = "abracadabra";
        G.buildRepair(data);

        std::cout<<"Number of rules"<<G.n_rules()<<std::endl;
        std::cout<<"Total size of the grammar"<<G.get_size()<<std::endl;
        std::cout<<"Total in megabytes of the grammar "<<(G.size_in_bytes()*1.0/1024*1024)<<std::endl;


        G.save(fg);
        fg.close();


    }
}

static void grammar_load(benchmark::State& state){

    for(auto _ : state)
    {
        uint folder = state.range(0);
        uint coll   = state.range(1);
        std::string collection = "../" + dirFolder[folder]+dircollection[coll];
        std::fstream f(collection, std::ios::in| std::ios::binary);
        std::string data;
        std::cout << "collection: "<< collection << std::endl;
        if(!f.is_open()){
            std::cout<<"Error the file could not opened!!\n";
            return;
        }
        std::string buff;
        while (!f.eof() && std::getline(f, buff)) {
            data += buff;
        }
        std::cout << "size of the string: " << data.length() << std::endl;
        f.close();
        fstream fg("grp_"+dircollection[coll],std::ios::in|std::ios::binary);
        grammar G;
        G.load(fg);

        std::cout<<"Number of rules"<<G.n_rules()<<std::endl;
        std::cout<<"Total size of the grammar"<<G.get_size()<<std::endl;
        std::cout<<"Total in megabytes of the grammar "<<(G.size_in_bytes()*1.0/1024*1024)<<std::endl;

        fstream wf("grammar222_"+dircollection[coll],std::ios::out|std::ios::binary);
        G.write_text(wf);


    }
}

BENCHMARK(grammar_big_size)->Args({dataDir::dir_pitches, dataCollection::pitches50})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_sources, dataCollection::sources50})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA50})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_english, dataCollection::english50})->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();
