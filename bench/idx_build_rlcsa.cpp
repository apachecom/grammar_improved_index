//
// Created by alpachec on 19-12-18.
//

//
// Created by alpachec on 16-09-18.
//


#include <iostream>
#include <fstream>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>

#include "../SelfGrammarIndexPT.h"
#include "../SelfGrammarIndexBS.h"
#include "repetitive_collections.h"
#include "../SelfGrammarIndexPTS.h"
#include <rlcsa/rlcsa.h>
#include <rlcsa/rlcsa_builder.h>
//
//
// #include <lzend/static_selfindex_lzend.h>
//#include <lzend/static_selfindex_lz77.h>



using namespace CSA;

#define QGRAM_DEF 4
#define MAX_LENGTH_PATTERN 30

std::string path = "../files/gmr_rs_index_rlcsa/";
std::string data;



/*

auto lz77build = [](benchmark::State &st, const string &file_collection){


    unsigned char br=1;
    unsigned char bs=1;
    unsigned char ss=1;
    lz77index::static_selfindex* idx ;
    std::string filename = std::to_string(collections_code[file_collection])+"lz77";
    char* _f = (char *)filename.c_str();
    char* file = (char *) file_collection.c_str();
    for (auto _ : st)
    {
         idx = lz77index::static_selfindex_lz77::build(file,_f, br, bs, ss);
    }
    st.counters["Size"] = idx->size();
    delete idx;

};


auto lzEndbuild = [](benchmark::State &st, const string &file_collection){


    unsigned char br=1;
    unsigned char bs=1;
    unsigned char ss=1;
    lz77index::static_selfindex_lzend* idx ;
    std::string filename = std::to_string(collections_code[file_collection])+"lzEnd";
    char* _f = (char *)filename.c_str();
    char* file = (char *) file_collection.c_str();
    for (auto _ : st)
    {
        idx = lz77index::static_selfindex_lzend::build(file,_f, br, bs, ss);
    }
    st.counters["Size"] = idx->size();
    delete idx;

};

*/

auto g_imp_build_basics = [](benchmark::State &st, const string &file_collection){

    std::fstream fbasics(path+std::to_string(collections_code[file_collection])+"basics", std::ios::out| std::ios::binary);
    std::fstream fsuffixes(path+std::to_string(collections_code[file_collection])+"suffixes", std::ios::out| std::ios::binary);
    std::fstream frepair(path+std::to_string(collections_code[file_collection])+"repair_g", std::ios::out| std::ios::binary);


    SelfGrammarIndexBS* g_index = new SelfGrammarIndexBS();
    for (auto _ : st)
    {
        g_index->build_basics(data,fsuffixes,fbasics,frepair);
    }

    fstream f_gidx(path+std::to_string(collections_code[file_collection])+".gbsidx", std::ios::out | std::ios::binary);
    g_index->save(f_gidx);
    st.counters["Size"] = g_index->size_in_bytes();

    fsuffixes.close();
    frepair.close();
    fbasics.close();

    delete g_index;

};


auto g_imp_build = [](benchmark::State &st, const string &file_collection){


    SelfGrammarIndexPT* g_index = new SelfGrammarIndexPT();

    std::fstream fsuffixes(path+std::to_string(collections_code[file_collection])+"suffixes", std::ios::in | std::ios::binary);
    std::fstream frepair(path+std::to_string(collections_code[file_collection])+"repair_g", std::ios::in | std::ios::binary);
    std::fstream fbasics(path+std::to_string(collections_code[file_collection])+"basics", std::ios::in| std::ios::binary);

    for (auto _ : st)
    {
        //g_index->build(data);
        g_index->load_basics(fbasics);
        g_index->build_suffix(data,fsuffixes,frepair);
        sleep(5);

    }

    std::fstream f_gidx(path+std::to_string(collections_code[file_collection])+".gidx", std::ios::out | std::ios::binary);
    g_index->save(f_gidx);
    st.counters["Size"] = g_index->size_in_bytes();

    fsuffixes.close();
    frepair.close();
    fbasics.close();

    delete g_index;
};

auto g_imp_pts_build = [](benchmark::State &st, const string &file_collection, const uint8_t &sampling){


    std::fstream fsuffixes(path+std::to_string(collections_code[file_collection])+"suffixes", std::ios::in | std::ios::binary);
    std::fstream frepair(path+std::to_string(collections_code[file_collection])+"repair_g", std::ios::in | std::ios::binary);
    std::fstream fbasics(path+std::to_string(collections_code[file_collection])+"basics", std::ios::in| std::ios::binary);

    SelfGrammarIndexPTS* g_index = new SelfGrammarIndexPTS(sampling);
    for (auto _ : st)
    {
        //g_index->build(data);
        g_index->load_basics(fbasics);
        g_index->build_suffix(data,fsuffixes,frepair);
        sleep(5);
    }

    std::fstream f_gidx(path+std::to_string(collections_code[file_collection])+".gpts"+std::to_string(sampling)+"idx", std::ios::out | std::ios::binary);
    g_index->save(f_gidx);
    st.counters["Size"] = g_index->size_in_bytes();


    fsuffixes.close();
    frepair.close();
    fbasics.close();
    delete g_index;
};



auto g_imp_bs_build = [](benchmark::State &st, const string &file_collection){


    SelfGrammarIndexBS* g_index = new SelfGrammarIndexBS();
    for (auto _ : st)
    {
        g_index->build(data);
    }

    fstream f_gidx(path+std::to_string(collections_code[file_collection])+".gbsidx", std::ios::out | std::ios::binary);
    g_index->save(f_gidx);
    st.counters["Size"] = g_index->size_in_bytes();
};



auto rlcsa_build = [](benchmark::State &st, const string &file_collection) {


    std::cout<<std::to_string(collections_code[file_collection])<< "collection"<<std::endl;
    std::string parameters_name = std::to_string(collections_code[file_collection]) + PARAMETERS_EXTENSION;


    ifstream input("temp_collection_rlcsa",ifstream::in);


    if(!input)
    {
        std::cerr << "Error opening input file " << collections_name[file_collection]   << "!" << std::endl;
        return 2;
    }
    CSA::usint size = CSA::fileSize(input);
    if(size <= 0)
    {
        std::cerr << "Error: Input file is empty!" << std::endl;
        input.close();
        return 3;
    }
    uchar* data = new uchar[size];
    input.read((char*)data, size);
    input.close();


    CSA::usint block = 32;
    CSA::usint sample = 128;
    for (auto _ : st)
    {
        CSA::RLCSA rlcsa(data, size, block, sample, 1, true);

        data = 0; // RLCSA constructor deleted the data!
        if(!(rlcsa.isOk()))
        {
            std::cout<<"Error en la construccion del indice 4\n";
            return 4;
        }
        rlcsa.printInfo();
        rlcsa.reportSize(true);
        rlcsa.writeTo("../files/rlcsa/"+std::to_string(collections_code[file_collection]));
    }

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
    std::fstream f2("temp_collection_rlcsa",std::ios::out );

    for(size_t i = 0 ; i < data.size(); ++i)
    {
        f2 << data[i];
    }
    char cc = 0;
    f2 << cc;

    f2.close();


    benchmark::RegisterBenchmark("RLCSA", rlcsa_build, collection);
    /*benchmark::RegisterBenchmark("Grammar-Improved-Index Binary Search", g_imp_build_basics, collection);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<1>", g_imp_build, collection);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<4>" ,  g_imp_pts_build, collection, 4);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<8>" ,  g_imp_pts_build, collection, 8);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<16>",  g_imp_pts_build, collection, 16);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<32>",  g_imp_pts_build, collection, 32);
    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<64>",  g_imp_pts_build, collection, 64);*/
    //benchmark::RegisterBenchmark("LZ77-Index", lz77build, collection);
    //benchmark::RegisterBenchmark("LZEnd-Index", lzEndbuild, collection);


    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


    return 0;

}
