//
// Created by via56 on 01-04-20.
//

#include <iostream>
#include <fstream>
#include <benchmark/benchmark.h>

#include <slp/RePairSLPIndex.h>
#include "../SelfGrammarIndexBS.h"
#include <ri/r_index.hpp>
#include "../SelfGrammarIndexPTS.h"
#include "../SelfGrammarIndexBSQ.h"

#include "sdsl/io.hpp"

//#ifdef MEM_MONITOR
#include "../utils/memory/mem_monitor/mem_monitor.hpp"
//#endif

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


auto first_test = [](benchmark::State &st){

    for (auto _ : st)
    {
        sleep(3);
    }

    st.counters["Size"] = 0;

};





auto ribuild = [](benchmark::State &st, const string &file_collection, const std::string& file_out

#ifdef MEM_MONITOR
, const std::string file_mem_monitor
#endif
        ){
    fstream f_ridx(file_out+".ri", std::ios::out | std::ios::binary);

    bool fast = false;
    bool sais = false;
    f_ridx.write((char*)&fast,sizeof(fast));


    ri::r_index<> idx;


#ifdef MEM_MONITOR
//    std::cout<<file_mem_monitor<<std::endl;
    mem_monitor mm(file_mem_monitor+"-ri.csv");
#endif

    for (auto _ : st)
    {
#ifdef MEM_MONITOR
        mm.event("R-INDEX-BUILD");
#endif
        idx = ri::r_index<>(data,sais);
    }
    idx.serialize(f_ridx);

    st.counters["size"] = idx.print_space();

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
    mem_monitor mm(file_mem_monitor+"-slp.csv");
#endif

    ifstream in("./temp_collection", ifstream::in);
    in.seekg(0,ios_base::end);
    uint tsize = in.tellg();
    in.seekg(0,ios_base::beg);
    uchar *text = nullptr;

    if (in.good())
    {
        text = loadValue<uchar>(in, tsize);
        text[tsize] = 0;
        std::cout<<tsize<<std::endl;
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
        indexer->build(text, tsize, qgram, _f);
        sleep(3);
    }

    indexer->save();
    st.counters["Size"] = indexer->size();

    delete indexer;

};

auto slpbalbuild = [](benchmark::State &st, const string &file_collection, const std::string& file_out, const int& qgram, bool bal

#ifdef MEM_MONITOR
        , const std::string file_mem_monitor
#endif
){


    std::string filename = (bal)?file_out+"-bal-q"+std::to_string(qgram):file_out+"-q"+std::to_string(qgram);
    char* _f = (char *)filename.c_str();
    RePairSLPIndex *indexer = new RePairSLPIndex();

#ifdef MEM_MONITOR
    mem_monitor mm(file_mem_monitor+"-balspl.csv");
#endif
    std::cout<<"File collection: "<<file_collection<<std::endl;
    ifstream in(file_collection, ifstream::in);
    in.seekg(0,ios_base::end);
    uint tsize = in.tellg();
    in.seekg(0,ios_base::beg);
    uchar *text = nullptr;

    std::string prefix = (bal)? file_collection+"-bal":file_collection;

    std::cout<<"prefix grammar: "<<prefix<<std::endl;
    std::fstream R(prefix+".R",std::ios::in|std::ios::binary);
    if(!R.is_open()){
        std::cout<<"[ERROR]:file " +file_collection+".R"<<std::endl;
        return;
    }

    std::fstream C(prefix+".C",std::ios::in|std::ios::binary);
    if(!C.is_open()){
        std::cout<<"[ERROR]:file " +file_collection+".C"<<std::endl;
        return;
    }

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
        sleep(3);
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
    mem_monitor mm(file_mem_monitor+"-gi.csv");
#endif

    for (auto _ : st)
    {

#ifdef MEM_MONITOR
        mm.event("G-INDEX-BS-BUILD");
#endif
        g_index->build_basics(data,fsuffixes,fbasics,frepair);
        sleep(3);
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
        benchmark::State &st, const string &file_collection, const std::string& file_out, const uint8_t &sampling

#ifdef MEM_MONITOR
        , const std::string file_mem_monitor
#endif
        ){


    std::fstream fbasics(file_out+"-basics", std::ios::in| std::ios::binary);
    std::fstream fsuffixes(file_out+"-suffixes", std::ios::in| std::ios::binary);
    std::fstream frepair(file_out+"-grepair", std::ios::in| std::ios::binary);


#ifdef MEM_MONITOR
    mem_monitor mm(file_mem_monitor+"-gi-pts.csv");
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
        sleep(5);
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
        , const uint8_t &sampling

#ifdef MEM_MONITOR
        , const std::string file_mem_monitor
#endif
)
{



#ifdef MEM_MONITOR

    mem_monitor mm(file_mem_monitor+"-gi-qgram.csv");
#endif

    SelfGrammarIndexBSQ idx;

    std::ofstream foutF(file_out+"-gram-<"+std::to_string(sampling)+">-smp.gi",std::ios::out|std::ios::binary);
    std::ofstream foutG(file_out+"-gram-<"+std::to_string(sampling)+">-smp-g.gi",std::ios::out|std::ios::binary);
    std::ofstream foutGR(file_out+"-gram-<"+std::to_string(sampling)+">-smp-rev.gi",std::ios::out|std::ios::binary);
    std::ofstream foutGS(file_out+"-gram-<"+std::to_string(sampling)+">-smp-seq.gi",std::ios::out|std::ios::binary);



    for (auto _ : st) {
        //g_index->build(data);

#ifdef MEM_MONITOR
        mm.event("G-INDEX-PTS<"+std::to_string(sampling)+">-SAMPLING-BUILD");
#endif
        idx.build_basics(data);
        idx.buildSamplings(sampling);
        sleep(3);

    }

    idx.saveSampling(foutF,foutG,foutGR,foutGS);

    st.counters["size"] = idx.size_in_bytes();
    st.counters["sampling size"] = idx.grammarSamp->getSize()+idx.sequenceSamp->getSize()+idx.reverseSamp->getSize();

};

int main (int argc, char *argv[] ){

//    std::cout<<argc<<std::endl;
//
//    std::cout<<argv[0]<<std::endl;
//
//    std::cout<<argv[1]<<std::endl;
//
//    std::cout<<argv[2]<<std::endl;

    if(argc < 2){
        std::cout<<"bad parameters...."<<std::endl;
        return 0;
    }

    std::string collection  = argv[1];

    //"../tests/collections/repetitive/reals/para";
    std::string path_out    = argv[2];
#ifdef MEM_MONITOR
    std::string mem_out = argv[3];
#endif

    load_data(collection);

//   benchmark::RegisterBenchmark("First Test",  first_test
////#ifdef MEM_MONITOR
////            ,mem_out
////#endif
//   );
     benchmark::RegisterBenchmark("R-INDEX",  ribuild,collection,path_out
 #ifdef MEM_MONITOR
             ,mem_out
 #endif
     );
////
////    benchmark::RegisterBenchmark("SLP-BAL-INDEX<4>",  slpbalbuild,collection,path_out, 4,false
////#ifdef MEM_MONITOR
////            ,mem_out
////#endif
////    );
////    benchmark::RegisterBenchmark("SLP-BAL-INDEX<8>",  slpbalbuild,collection,path_out, 8,false
////#ifdef MEM_MONITOR
////            ,mem_out
////#endif
////    );
////    benchmark::RegisterBenchmark("SLP-BAL-INDEX<12>",  slpbalbuild,collection,path_out, 12,false
////#ifdef MEM_MONITOR
////            ,mem_out
////#endif
////    );
////    benchmark::RegisterBenchmark("SLP-BAL-INDEX<16>",  slpbalbuild,collection,path_out, 16,false
////#ifdef MEM_MONITOR
////            ,mem_out
////#endif
////    );
//

    benchmark::RegisterBenchmark("SLP-INDEX<4>",  slpbuild ,collection,path_out,4
#ifdef MEM_MONITOR
            ,mem_out
#endif
    );

    benchmark::RegisterBenchmark("SLP-INDEX<8>",  slpbuild ,collection,path_out,8
#ifdef MEM_MONITOR
            ,mem_out
#endif
    );

    benchmark::RegisterBenchmark("SLP-INDEX<12>",  slpbuild ,collection,path_out,12
#ifdef MEM_MONITOR
            ,mem_out
#endif
    );

    benchmark::RegisterBenchmark("SLP-INDEX<16>",  slpbuild ,collection,path_out,16
#ifdef MEM_MONITOR
            ,mem_out
#endif
    );


         benchmark::RegisterBenchmark("G-INDEX",  g_imp_build_basics ,collection,path_out
     #ifdef MEM_MONITOR
                 ,mem_out
     #endif
         );
//
    benchmark::RegisterBenchmark("G-INDEX-PTS<2>",  g_imp_pts_build ,collection,path_out,2
#ifdef MEM_MONITOR
            ,mem_out
#endif
    );

     benchmark::RegisterBenchmark("G-INDEX-PTS<4>",  g_imp_pts_build ,collection,path_out,4
 #ifdef MEM_MONITOR
             ,mem_out
 #endif
     );

     benchmark::RegisterBenchmark("G-INDEX-PTS<8>",  g_imp_pts_build ,collection,path_out,8
 #ifdef MEM_MONITOR
             ,mem_out
 #endif
     );
     benchmark::RegisterBenchmark("G-INDEX-PTS<16>",  g_imp_pts_build ,collection,path_out,16
 #ifdef MEM_MONITOR
             ,mem_out
 #endif
     );
     benchmark::RegisterBenchmark("G-INDEX-PTS<32>",  g_imp_pts_build ,collection,path_out,32
 #ifdef MEM_MONITOR
             ,mem_out
 #endif
     );

     benchmark::RegisterBenchmark("G-INDEX-PTS<64>",  g_imp_pts_build ,collection,path_out,64
 #ifdef MEM_MONITOR
             ,mem_out
 #endif
     );
//     benchmark::RegisterBenchmark("G-INDEX-QGRAM<4>",  g_imp_qgram_build ,collection,path_out,4
// #ifdef MEM_MONITOR
//             ,mem_out
// #endif
//     );
//     benchmark::RegisterBenchmark("G-INDEX-QGRAM<8>",  g_imp_qgram_build ,collection,path_out,8
// #ifdef MEM_MONITOR
//             ,mem_out
// #endif
//     );
//     benchmark::RegisterBenchmark("G-INDEX-QGRAM<12>",  g_imp_qgram_build ,collection,path_out,12
// #ifdef MEM_MONITOR
//             ,mem_out
// #endif
//     );
//     benchmark::RegisterBenchmark("G-INDEX-QGRAM<16>",  g_imp_qgram_build ,collection,path_out,16
// #ifdef MEM_MONITOR
//             ,mem_out
// #endif
//     );



//

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;

}