//
// Created by alpachec on 19-08-18.
//


#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "../collections.cpp"
#include "../../trees/patricia_tree/patricia_tree.h"
#include "../../trees/patricia_tree/compact_patricia_tree.h"


using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;


using namespace m_patricia;

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




        ///std::fstream f_trie("not_compress_patricia_tree" + dircollection[coll], std::ios::out | std::ios::binary);
        patricia_tree<string_pairs> T;
        uint id = 0;
        for (int i = 0; i < data.size(); ++i) {
            string_pairs s(data,++id);
            s.set_left(i);
            s.set_right(size);
            //std::string s;
            //s.resize(size-i);
            //std::copy(data.begin()+i,data.begin()+size,s.begin());
            T.insert(s);
            ////std::cout<<id<<std::endl;
        }
        std::cout<<"Number of internal nodes "<<T.num_internal_nodes()<<std::endl;
        std::cout<<"Number of leaves nodes "<<T.num_leaves()<<std::endl;
        std::cout<<"Number of total nodes "<<T.num_total_nodes()<<std::endl;
        //T.save(f_trie);
        ///f_trie.close();
        T.print();

       // std::fstream f_cg("compress_patricia_tree" + dircollection[coll], std::ios::out| std::ios::binary);
        compact_patricia_tree c_trie;
        c_trie.build(T);

       // c_trie.save(f_cg);
      //  f_cg.close();

    }
}

BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA50,  10000000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA100, 10000000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA200, 10000000})->Unit(benchmark::kMillisecond);
BENCHMARK(grammar_big_size)->Args({dataDir::dir_DNA, dataCollection::DNA,    10000000})->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();