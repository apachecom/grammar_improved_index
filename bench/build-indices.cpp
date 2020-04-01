//
// Created by via56 on 01-04-20.
//

#include <iostream>
#include <fstream>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include <slp/RePairSLPIndex.h>
#include "../SelfGrammarIndexBS.h"
#include <ri/r_index.hpp>
#include "../SelfGrammarIndexPTS.h"

#ifdef MEM_MONITOR
#include "../utils/memory/mem_monitor/mem_monitor.hpp"
#endif

using namespace cds_static;

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
    while(!f.eof()  ){
        f.read((char*)buffer,1000);
        data.append((char*) buffer,f.gcount());
    }
    for (int i = 0; i < data.size(); ++i) {
        if(data[i] == 0 || data[i] == 1 || data[i] == 2)
            data[i] = 3;
    }

    f.close();
    std::fstream f2("temp_collection",std::ios::out );

    for(size_t i = 0 ; i < data.size(); ++i)
    {
        f2 << data[i];
    }
    f2.close();
}

auto ribuild = [](benchmark::State &st, const string &file_collection, const std::string& file_out

#ifdef MEM_MONITOR
, const std::string file_mem_monitor
#endif
        ){


    fstream f_ridx(file_out+".ri", std::ios::out | std::ios::binary);
    bool fast = false;
    bool sais = false;
    f_ridx.write((char*)&fast,sizeof(fast));

//    string input;
//    {

//        temp_collection
//        std::ifstream fs(file_collection);
//        std::stringstream buffer;
//        buffer << fs.rdbuf();
//        input = buffer.str();

//    }

    ri::r_index<> idx;


#ifdef MEM_MONITOR
    mem_monitor mm(file_out+".csv");
#endif

    for (auto _ : st)
    {
#ifdef MEM_MONITOR
        mm.event("R-INDEX-BUILD");
#endif
//        idx = ri::r_index<>(input,sais);
        idx = ri::r_index<>(data,sais);
    }

    idx.serialize(f_ridx);
//    st.counters["Size"] = sdsl::size_in_bytes(idx);

};

auto slpbuild = [](benchmark::State &st, const string &file_collection, const std::string& file_out, const int& qgram

#ifdef MEM_MONITOR
        , const std::string file_mem_monitor
#endif
){

    std::string filename = file_out+"-q"+std::to_string(qgram);
    char* _f = (char *)filename.c_str();
    RePairSLPIndex *indexer = new RePairSLPIndex();

#ifdef MEM_MONITOR
    mem_monitor mm(file_out+".csv");
#endif

    ifstream in(file_collection, ifstream::in);
    in.seekg(0,ios_base::end);
    uint tsize = in.tellg();
    in.seekg(0,ios_base::beg);
    uchar *text = nullptr;

    std::fstream R(file_collection+".R",std::ios::in|std::ios::binary);
    std::fstream C(file_collection+".C",std::ios::in|std::ios::binary);

    if (in.good())
    {
        text = loadValue<uchar>(in, tsize);
        text[tsize] = 0;
        in.close();
    } else {
        std::cout<<"File Error\n";
        return;
    }

    for (auto _ : st)
    {
#ifdef MEM_MONITOR
        mm.event("SLP-INDEX-BAL-BUILD");
#endif
        indexer->build(text, tsize, qgram, _f,R,C);
    }

    indexer->save();
    st.counters["Size"] = indexer->size();

    delete indexer;

};

auto g_imp_build_basics = [](benchmark::State &st, const string &file_collection, const std::string& file_out

#ifdef MEM_MONITOR
        , const std::string file_mem_monitor
#endif
){
    std::fstream fbasics(file_out+"-basics", std::ios::out| std::ios::binary);
    std::fstream fsuffixes(file_out+"-suffixes", std::ios::out| std::ios::binary);
    std::fstream frepair(file_out+"-grepair", std::ios::out| std::ios::binary);

    std::fstream R(file_collection+".R",std::ios::in|std::ios::binary);
    std::fstream C(file_collection+".C",std::ios::in|std::ios::binary);

    SelfGrammarIndexBS* g_index = new SelfGrammarIndexBS();
//    g_index->set_code(collections_code[file_collection]);

#ifdef MEM_MONITOR
    mem_monitor mm(file_out+".csv");
#endif

    for (auto _ : st)
    {

#ifdef MEM_MONITOR
        mm.event("G-INDEX-BS-BUILD");
#endif
        g_index->build_basics_bal(data,R,C,fsuffixes,fbasics,frepair);
    }

    fstream f_gidx(file_out+"-bs.gi", std::ios::out | std::ios::binary);
    g_index->save(f_gidx);
    st.counters["Size"] = g_index->size_in_bytes();

    fsuffixes.close();
    frepair.close();
    fbasics.close();

    delete g_index;

};


auto g_imp_pts_build = [](
        benchmark::State &st, const string &file_collection, const std::string& file_out

#ifdef MEM_MONITOR
        , const std::string file_mem_monitor
#endif
        , const uint8_t &sampling){


    std::fstream fbasics(file_out+"-basics", std::ios::in| std::ios::binary);
    std::fstream fsuffixes(file_out+"-suffixes", std::ios::in| std::ios::binary);
    std::fstream frepair(file_out+"-grepair", std::ios::in| std::ios::binary);


#ifdef MEM_MONITOR
    mem_monitor mm(file_out+".csv");
#endif

    SelfGrammarIndexPTS* g_index = new SelfGrammarIndexPTS(sampling);
//    g_index->set_code(collections_code[file_collection]);

    for (auto _ : st)
    {
        //g_index->build(data);

#ifdef MEM_MONITOR
        mm.event("G-INDEX-PTS<"+std::to_string(sampling)+">-SAMPLING-BUILD");
#endif
        g_index->load_basics(fbasics);
        g_index->build_suffix(data,fsuffixes,frepair);
//        sleep(5);
    }

    std::fstream f_gidx(file_out+"-pts-<"+std::to_string(sampling)+">.gi", std::ios::out | std::ios::binary);
    g_index->save(f_gidx);
    st.counters["Size"] = g_index->size_in_bytes();


    fsuffixes.close();
    frepair.close();
    fbasics.close();
    delete g_index;
};


/**
 * leer la colleccion
 * */



auto g_imp_qgram_build = [](
        benchmark::State &st, const string &file_collection, const std::string& file_out

#ifdef MEM_MONITOR
        , const std::string file_mem_monitor
#endif
        , const uint8_t &sampling) {


}

int main (int argc, char *argv[] ){

    if(argc < 1){
        std::cout<<"bad parameters....";
        return 0;
    }
    std::string collection = argv[1];//"../tests/collections/repetitive/reals/para";
    std::string path_out = argv[2];

    load_data(collection);



    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


    return 0;

}