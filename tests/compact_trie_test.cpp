//
// Created by alpachec on 19-08-18.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "collections.cpp"
#include "../trees/trie/Trie.h"
#include "../trees/trie/compact_trie.h"


using namespace _trie;

static void grammar_big_size(benchmark::State& state) {

    size_t points = state.range(0);
    for (auto _ : state)
    {

        uint folder = state.range(0);
        uint coll = state.range(1);
        std::string collection = "../" + dirFolder[folder] + dircollection[coll];
        std::fstream f(collection, std::ios::in | std::ios::binary);
        std::string data;
        std::cout << "collection: " << collection << std::endl;

        if (!f.is_open())
        {
            std::cout << "Error the file could not opened!!\n";
            return;
        }

        std::string buff;

        while (!f.eof() && std::getline(f, buff))
        {
            data += buff;
        }
        std::cout << "size of the string: " << data.length() << std::endl;
        f.close();
        uint size = state.range(2);


        std::fstream f_trie("not_compress_trie" + dircollection[coll], std::ios::out | std::ios::binary);
        Trie<std::string> T;
        for (int i = 0; i < size; ++i) {
            std::string s;
            s.resize(size-i);
            std::copy(data.begin()+i,data.begin()+size,s.begin());
            T.insert(s);
        }
        std::cout<<"Number of nodes "<<T.get_num_nodes()<<std::endl;
        T.save(f_trie);
        f_trie.close();
/*
        std::fstream f_cg("compress_trie" + dircollection[coll], std::ios::out| std::ios::binary);
        compact_trie c_trie;
        c_trie.build(T);
        c_trie.save(f_cg);
        f_cg.close();
*/
    }
}


BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA100, 10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA200, 10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA,    10000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA50,  10000})->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();