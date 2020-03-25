//
// Created by alejandro on 13-03-19.
//
#include <iostream>
#include <fstream>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include "../utils/repair/RePair.h"
#include <ri/r_index.hpp>
#include "../utils/build_hyb_lz77.h"
#include <hyb/HybridSelfIndex.h>

#include "repetitive_collections.h"

using namespace ri;

static const uchar TERMINATOR = 1;
ulint terminator_position = 0;

std::string data;

uint load_data(const string & collection, string& data){


    std::cout<<".............\n"<<collections_name[collection]<<".............\n"<<std::endl;

    std::fstream f(collection, std::ios::in| std::ios::binary);
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
    return 1;

}


auto lz77_parse_ph = [](benchmark::State &st, const string &collection, const uint & m)
        {
            std::string data;
            if(!load_data(collection,data))
                return ;

            std::fstream f2("temp_collection",std::ios::out );

            for(size_t i = 0 ; i < data.size(); ++i)
            {
                f2 << data[i];
            }

            f2.close();


            std::string temp = "temp_collection";
            char* input_file_parser = (char*)temp.c_str();
            char parser_file [] = "lz77_parser_file_hyb.lz";
            build_hyb_lz77(input_file_parser,parser_file);

            //build_hyb_lz77("temp_collection",parser_file);
            std::string filename = "hyb_"+ std::to_string(m) + std::to_string(collections_code[collection]);
            char* _f = (char *)filename.c_str();

            HybridSelfIndex* index;

            // saving the index ...

            for (auto _ : st)
            {
                index = new HybridSelfIndex(parser_file,m, _f);
            }

            //index->saveStructure();

            st.counters["Size in bytes"] = index->sizeDS;
            st.counters["number of phrases z"] = index->z;

            delete index;



        };


auto bwt_runs = [](benchmark::State &st, const string &collection)
        {
            std::string data;
            if(!load_data(collection,data))
                return ;


            ri::r_index<> idx;

            for (auto _ : st)
            {
                idx = ri::r_index<>(data/*input*/,true);
            }
            st.counters["number of runs"] = idx.number_of_runs();
            st.counters["Size of index in bytes"] = sdsl::size_in_bytes(idx);


        };



auto simple_repair_build = [](benchmark::State &st, const string &collection)
        {
            std::string data;
            if(!load_data(collection,data))
                return ;

            unsigned int max_key;
            auto  utext = (u_char *)data.c_str();
            int * ctext;
            unsigned int clength;
            unsigned int  length;
            length = sizeof(u_char) * data.length();
            unsigned char * symbols;
            unsigned int terminals;
            Tdiccarray *dicc; uint cdicc;
            RePair compressor;

            compressor.compress(utext, length, &ctext, &clength, &symbols, &terminals, &dicc, &cdicc);

            for (auto _ : st)
            {
                sleep(5);
            }

            uint rules = terminals+cdicc;

            st.counters["collection total size"] = data.length();
            st.counters["grammar total size"] = 2*rules + clength;
            st.counters["grammar first rule len"] = clength;
            st.counters["grammar terminals"] = terminals;
            st.counters["grammar variables"] = cdicc;

        };


int main (int argc, char *argv[] ){

    /*benchmark::RegisterBenchmark("Re-Pair cere",             simple_repair_build,"../tests/collections/repetitive/reals/cere");
    benchmark::RegisterBenchmark("Re-Pair coreutils",        simple_repair_build,"../tests/collections/repetitive/reals/coreutils");
    benchmark::RegisterBenchmark("Re-Pair einstein.en.txt",  simple_repair_build,"../tests/collections/repetitive/reals/einstein.en.txt");
    benchmark::RegisterBenchmark("Re-Pair Escherichia_Coli", simple_repair_build,"../tests/collections/repetitive/reals/Escherichia_Coli");
    benchmark::RegisterBenchmark("Re-Pair influenza",        simple_repair_build,"../tests/collections/repetitive/reals/influenza");
    benchmark::RegisterBenchmark("Re-Pair kernel",           simple_repair_build,"../tests/collections/repetitive/reals/kernel");
    benchmark::RegisterBenchmark("Re-Pair para",             simple_repair_build,"../tests/collections/repetitive/reals/para");

    benchmark::RegisterBenchmark("Bwt cere",             bwt_runs,"../tests/collections/repetitive/reals/cere");
    benchmark::RegisterBenchmark("Bwt coreutils",        bwt_runs,"../tests/collections/repetitive/reals/coreutils");
    benchmark::RegisterBenchmark("Bwt einstein.en.txt",  bwt_runs,"../tests/collections/repetitive/reals/einstein.en.txt");
    benchmark::RegisterBenchmark("Bwt Escherichia_Coli", bwt_runs,"../tests/collections/repetitive/reals/Escherichia_Coli");
    benchmark::RegisterBenchmark("Bwt influenza",        bwt_runs,"../tests/collections/repetitive/reals/influenza");
    benchmark::RegisterBenchmark("Bwt kernel",           bwt_runs,"../tests/collections/repetitive/reals/kernel");
    benchmark::RegisterBenchmark("Bwt para",             bwt_runs,"../tests/collections/repetitive/reals/para");
*/
    benchmark::RegisterBenchmark("lz77 cere",             lz77_parse_ph,"../tests/collections/repetitive/reals/cere",30);
    benchmark::RegisterBenchmark("lz77 coreutils",        lz77_parse_ph,"../tests/collections/repetitive/reals/coreutils",30);
    benchmark::RegisterBenchmark("lz77 einstein.en.txt",  lz77_parse_ph,"../tests/collections/repetitive/reals/einstein.en.txt",30);
    benchmark::RegisterBenchmark("lz77 Escherichia_Coli", lz77_parse_ph,"../tests/collections/repetitive/reals/Escherichia_Coli",30);
    benchmark::RegisterBenchmark("lz77 influenza",        lz77_parse_ph,"../tests/collections/repetitive/reals/influenza",30);
    benchmark::RegisterBenchmark("lz77 kernel",           lz77_parse_ph,"../tests/collections/repetitive/reals/kernel",30);
    benchmark::RegisterBenchmark("lz77 para",             lz77_parse_ph,"../tests/collections/repetitive/reals/para",30);





    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;
}
