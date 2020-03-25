//
// Created by alpachec on 19-08-18.
//


#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "collections.cpp"
#include "../compressed_grammar.h"




static void grammar_big_size(benchmark::State& state)
{
    size_t points = state.range(0);
    for (auto _ : state)
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

        std::fstream f_ncg("not_compress_grammar" + dircollection[coll], std::ios::out | std::ios::binary);
        grammar G;
        ////////////data = "panamabanana";
        G.buildRepair(data);
        G.save(f_ncg);
        f_ncg.close();


        std::cout<<"Number of rules \t"<<G.n_rules()<<std::endl;
        std::cout<<"Total size of the grammar \t"<<G.get_size()<<std::endl;
        std::cout<<"Total in megabytes of the grammar \t"<<(G.size_in_bytes()*1.0/1024*1024)<<std::endl;

        std::fstream f_cg("compress_grammar" + dircollection[coll], std::ios::out| std::ios::binary);
        compressed_grammar c_grammar;
        c_grammar.build(G);
        std::cout<<"Total in megabytes of the grammar \t"<<(c_grammar.size_in_bytes()*1.0/1024*1024)<<""<<std::endl;
        c_grammar.print_size_in_bytes();
        c_grammar.save(f_cg);
        f_cg.close();


    }
}

BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA100})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA200})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA50})->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
