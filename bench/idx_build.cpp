//
// Created by alpachec on 16-09-18.
//


#include <iostream>
#include <fstream>


#include <gflags/gflags.h>
#include <benchmark/benchmark.h>

#include <slp/RePairSLPIndex.h>
#include "../SelfGrammarIndexPT.h"
#include "../SelfGrammarIndexBS.h"
#include <ri/r_index.hpp>
//#include "../utils/build_hyb_lz77.h"
//#include <hyb/HybridSelfIndex.h>

#include "repetitive_collections.h"
#include "../SelfGrammarIndexPTS.h"


#include "../utils/memory/mem_monitor/mem_monitor.hpp"

using namespace cds_static;

#define QGRAM_DEF 4
#define MAX_LENGTH_PATTERN 30

std::string path = "";
std::string data;
/*
 *
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
// auto hybbuild = [](benchmark::State &st, const string &file_collection){

//     std::string filename = path+std::to_string(collections_code[file_collection]);
//     char* _f = (char *)filename.c_str();
//     std::string temp = "temp_collection";
//     char* input_file_parser = (char*)temp.c_str();
//     char parser_file [] = "lz77_parser_file_hyb.lz";
//     build_hyb_lz77(input_file_parser,parser_file);

//     //build_hyb_lz77("temp_collection",parser_file);


//     HybridSelfIndex* index;

//     // saving the index ...
//     mem_monitor mm("mem-mon-out-"+collections_name[file_collection]+"-hyb-index.csv");

//     for (auto _ : st)
//     {
//         mm.event("HYB-INDEX-BUILD");
//         index = new HybridSelfIndex(parser_file,MAX_LENGTH_PATTERN, _f);
//     }

//     index->saveStructure();

//     st.counters["Size"] = index->sizeDS;

//     delete index;
// };

auto ribuild = [](benchmark::State &st, const string &file_collection){



    fstream f_ridx(path+std::to_string(collections_code[file_collection])+".ri", std::ios::out | std::ios::binary);
    bool fast = false;
    bool sais = false;
    f_ridx.write((char*)&fast,sizeof(fast));

    string input;
    {

        //std::ifstream fs(file_collection);
        std::ifstream fs("temp_collection");
        std::stringstream buffer;
        buffer << fs.rdbuf();
        input = buffer.str();

    }

    ri::r_index<> idx;

    mem_monitor mm("mem-mon-out-"+collections_name[file_collection]+"-r-index.csv");


    for (auto _ : st)
    {
        mm.event("R-INDEX-BUILD");
        idx = ri::r_index<>(data/*input*/,sais);
    }

    idx.serialize(f_ridx);
//    st.counters["Size"] = sdsl::size_in_bytes(idx);

};


auto slpbuild = [](benchmark::State &st, const string &file_collection, int qgram){

    std::string filename = path+std::to_string(collections_code[file_collection])+"_"+std::to_string(qgram);
    char* _f = (char *)filename.c_str();
    RePairSLPIndex *indexer = new RePairSLPIndex();

    mem_monitor mm("mem-mon-out-"+collections_name[file_collection]+"-slp-index.csv");

    for (auto _ : st)
    {

        ifstream in(file_collection, ifstream::in);
//	    ifstream in("temp_collection",ifstream::in);
        in.seekg(0,ios_base::end);
        uint tsize = in.tellg();
        in.seekg(0,ios_base::beg);

        if (in.good())
        {
            // Reading the input text
            uchar *text = loadValue<uchar>(in, tsize);
            //uchar* text = (unsigned char*)data.c_str();
            in.close();

            cout << endl;
            cout << " >> Building the SLP Index for '"<<file_collection<<"':" << endl;
            cout << "    # Input file size: " << tsize << " bytes" << endl;
            cout << "    # " << qgram << "-grams will be additionally indexed in the grammar" << endl;

            // New RePairSLPIndex
            mm.event("SLP-INDEX-BUILD");
            indexer->build(text, tsize, qgram, _f);

        }
        else cerr << endl << "Please, check your input file" << endl << endl;
        //uchar *text = (uchar *) "abracadabra\0";
    }

    indexer->save();
    st.counters["Size"] = indexer->size();
    delete indexer;

};


/***
 * Este metodo necesita los ficheros .C y .R de la gramatica
 * */

auto balslpbuild = [](benchmark::State &st, const string &file_collection, int qgram){

    std::string filename = path+std::to_string(collections_code[file_collection])+"_bal_"+std::to_string(qgram);
    char* _f = (char *)filename.c_str();
    RePairSLPIndex *indexer = new RePairSLPIndex();

    mem_monitor mm("mem-mon-out-"+collections_name[file_collection]+"-slp-index-bal.csv");
    for (auto _ : st)
    {

        ifstream in(file_collection, ifstream::in);
        //ifstream in("temp_collection",ifstream::in);
        in.seekg(0,ios_base::end);
        uint tsize = in.tellg();
        in.seekg(0,ios_base::beg);

        if (in.good())
        {
            // Reading the input text
            uchar *text = loadValue<uchar>(in, tsize);
            //uchar* text = (unsigned char*)data.c_str();
            in.close();

            cout << endl;
            cout << " >> Building the SLP Index for '"<<file_collection<<"':" << endl;
            cout << "    # Input file size: " << tsize << " bytes" << endl;
            cout << "    # " << qgram << "-grams will be additionally indexed in the grammar" << endl;

            // New RePairSLPIndex

            std::fstream R(file_collection+".R",std::ios::in|std::ios::binary);
            text[tsize] = 0;
            std::fstream C(file_collection+".C",std::ios::in|std::ios::binary);

            mm.event("SLP-INDEX-BAL-BUILD");

            indexer->build(text, tsize, qgram, _f,R,C);

        }
        else cerr << endl << "Please, check your input file" << endl << endl;
        //uchar *text = (uchar *) "abracadabra\0";
    }

    indexer->save();
    st.counters["Size"] = indexer->size();
    delete indexer;

};
auto g_imp_build_bal_basics = [](benchmark::State &st, const string &file_collection){

    std::fstream fbasics(path+std::to_string(collections_code[file_collection])+"bal_basics", std::ios::out| std::ios::binary);
    std::fstream fsuffixes(path+std::to_string(collections_code[file_collection])+"bal_suffixes", std::ios::out| std::ios::binary);
    std::fstream frepair(path+std::to_string(collections_code[file_collection])+"bal_repair_g", std::ios::out| std::ios::binary);

    std::fstream R(file_collection+".R",std::ios::in|std::ios::binary);
    std::fstream C(file_collection+".C",std::ios::in|std::ios::binary);

    SelfGrammarIndexBS* g_index = new SelfGrammarIndexBS();
    g_index->set_code(collections_code[file_collection]);
    mem_monitor mm("mem-mon-out-"+collections_name[file_collection]+"-g-index.csv");
    for (auto _ : st)
    {

        mm.event("G-INDEX-BUILD");
        g_index->build_basics_bal(data,R,C,fsuffixes,fbasics,frepair);
    }



    fstream f_gidx(path+std::to_string(collections_code[file_collection])+".balgbsidx", std::ios::out | std::ios::binary);
    g_index->save(f_gidx);
    st.counters["Size"] = g_index->size_in_bytes();

    fsuffixes.close();
    frepair.close();
    fbasics.close();

    delete g_index;

};
auto g_imp_build_basics = [](benchmark::State &st, const string &file_collection){

    std::fstream fbasics(path+std::to_string(collections_code[file_collection])+"basics", std::ios::out| std::ios::binary);
    std::fstream fsuffixes(path+std::to_string(collections_code[file_collection])+"suffixes", std::ios::out| std::ios::binary);
    std::fstream frepair(path+std::to_string(collections_code[file_collection])+"repair_g", std::ios::out| std::ios::binary);

    std::cout<<path+std::to_string(collections_code[file_collection])<<std::endl;


    SelfGrammarIndexBS* g_index = new SelfGrammarIndexBS();
    g_index->set_code(collections_code[file_collection]);
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
auto g_imp_build_bal = [](benchmark::State &st, const string &file_collection){

    /* std::fstream f(file_collection, std::ios::in| std::ios::binary);
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
 *//*
    while (!f.eof() && std::getline(f, buff)) {
        data += buff;
    }*//*

    for (int i = 0; i < data.size(); ++i) {
        if(data[i] == 0 || data[i] == 1)
            data[i] = 2;
    }*/

    SelfGrammarIndexPT* g_index = new SelfGrammarIndexPT();
    g_index->set_code(collections_code[file_collection]);


    std::fstream fsuffixes(path+std::to_string(collections_code[file_collection])+"bal_suffixes", std::ios::in | std::ios::binary);
    std::fstream frepair(path+std::to_string(collections_code[file_collection])+"bal_repair_g", std::ios::in | std::ios::binary);
    std::fstream fbasics(path+std::to_string(collections_code[file_collection])+"bal_basics", std::ios::in| std::ios::binary);

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



auto g_imp_build = [](benchmark::State &st, const string &file_collection){

   /* std::fstream f(file_collection, std::ios::in| std::ios::binary);
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
*//*
    while (!f.eof() && std::getline(f, buff)) {
        data += buff;
    }*//*

    for (int i = 0; i < data.size(); ++i) {
        if(data[i] == 0 || data[i] == 1)
            data[i] = 2;
    }*/

    SelfGrammarIndexPT* g_index = new SelfGrammarIndexPT();
    g_index->set_code(collections_code[file_collection]);


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

   /* std::fstream f(file_collection, std::ios::in| std::ios::binary);
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
    }*/

    std::fstream fsuffixes(path+std::to_string(collections_code[file_collection])+"suffixes", std::ios::in | std::ios::binary);
    std::fstream frepair(path+std::to_string(collections_code[file_collection])+"repair_g", std::ios::in | std::ios::binary);
    std::fstream fbasics(path+std::to_string(collections_code[file_collection])+"basics", std::ios::in| std::ios::binary);

    mem_monitor mm("mem-mon-out-"+collections_name[file_collection]+"-g-index-pts<"+std::to_string(sampling)+">.csv");
    SelfGrammarIndexPTS* g_index = new SelfGrammarIndexPTS(sampling);
    g_index->set_code(collections_code[file_collection]);

    for (auto _ : st)
    {
        //g_index->build(data);

        mm.event("G-INDEX-PTS<"+std::to_string(sampling)+">-BUILD");
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

auto g_imp_pts_build_bal = [](benchmark::State &st, const string &file_collection, const uint8_t &sampling){

    /* std::fstream f(file_collection, std::ios::in| std::ios::binary);
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
     }*/

    std::fstream fsuffixes(path+std::to_string(collections_code[file_collection])+"bal_suffixes", std::ios::in | std::ios::binary);
    std::fstream frepair(path+std::to_string(collections_code[file_collection])+"bal_repair_g", std::ios::in | std::ios::binary);
    std::fstream fbasics(path+std::to_string(collections_code[file_collection])+"bal_basics", std::ios::in| std::ios::binary);

    SelfGrammarIndexPTS* g_index = new SelfGrammarIndexPTS(sampling);
    g_index->set_code(collections_code[file_collection]);
    mem_monitor mm("mem-mon-out-"+collections_name[file_collection]+"-g-index-bal-pts<"+std::to_string(sampling)+">.csv");

    for (auto _ : st)
    {
        //g_index->build(data);
        mm.event("G-INDEX-BAL-PTS<"+std::to_string(sampling)+">-BUILD");
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

    /*std::fstream f(file_collection, std::ios::in| std::ios::binary);
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
    }*/
    SelfGrammarIndexBS* g_index = new SelfGrammarIndexBS();
    for (auto _ : st)
    {
        g_index->build(data);
    }

    fstream f_gidx(path+std::to_string(collections_code[file_collection])+".gbsidx", std::ios::out | std::ios::binary);
    g_index->save(f_gidx);
    st.counters["Size"] = g_index->size_in_bytes();
};




int main (int argc, char *argv[] ){

    if(argc < 1){
        std::cout<<"bad parameters....";
       /// return 0;
    }

    std::string collection = argv[1];//"../tests/collections/repetitive/reals/para";
    std::cout<<collection<<std::endl;
    std::cout<<".............\n"<<collections_name[collection]<<".............\n"<<std::endl;

    path = argv[2];
    std::cout<<"output directory:"<<path<<std::endl;
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

    ///slp_build2();
    //// build improve grammar index patrcia tree /////


    // New RePairSLPIndex
  // //cout << " >> Building the SLP Index:" << endl;
   benchmark::RegisterBenchmark("R-Index", ribuild, collection);
    //benchmark::RegisterBenchmark("Hyb-Index", hybbuild, collection);

//    benchmark::RegisterBenchmark("SLP-Index-2",  slpbuild,collection,2);
//    benchmark::RegisterBenchmark("SLP-Index-4",  slpbuild,collection,4);
//    benchmark::RegisterBenchmark("SLP-Index-6",  slpbuild,collection,6);
//    benchmark::RegisterBenchmark("SLP-Index-8",  slpbuild,collection,8);
//    benchmark::RegisterBenchmark("SLP-Index-10", slpbuild,collection,10);
//    benchmark::RegisterBenchmark("SLP-Index-12", slpbuild,collection,12);
//    benchmark::RegisterBenchmark("SLP-Index-16", slpbuild,collection,16);

    benchmark::RegisterBenchmark("SLP-Index-BAL-2",  balslpbuild,collection,2);
    benchmark::RegisterBenchmark("SLP-Index-BAL-4",  balslpbuild,collection,4);
    benchmark::RegisterBenchmark("SLP-Index-BAL-6",  balslpbuild,collection,6);
    benchmark::RegisterBenchmark("SLP-Index-BAL-8",  balslpbuild,collection,8);
    benchmark::RegisterBenchmark("SLP-Index-BAL-10", balslpbuild,collection,10);
    benchmark::RegisterBenchmark("SLP-Index-BAL-12", balslpbuild,collection,12);
    benchmark::RegisterBenchmark("SLP-Index-BAL-16", balslpbuild,collection,16);


//   benchmark::RegisterBenchmark("Grammar-Improved-Index Binary Search", g_imp_build_bal_basics, collection);
////   benchmark::RegisterBenchmark("Grammar-Improved-PT-Index"    , g_imp_build_bal, collection);
//   benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<2>" , g_imp_pts_build_bal, collection, 2);
//   benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<4>" , g_imp_pts_build_bal, collection, 4);
//   benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<8>" , g_imp_pts_build_bal, collection, 8);
//   benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<16>", g_imp_pts_build_bal, collection, 16);
//   benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<32>", g_imp_pts_build_bal, collection, 32);
//   benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<64>", g_imp_pts_build_bal, collection, 64);
////



//    benchmark::RegisterBenchmark("Grammar-Improved-Index Binary Search", g_imp_build_basics, collection);
//    benchmark::RegisterBenchmark("Grammar-Improved-Index<1>", g_imp_build, collection);
//    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<2>" ,  g_imp_pts_build, collection, 2);
//    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<4>" ,  g_imp_pts_build, collection, 4);
//    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<8>" ,  g_imp_pts_build, collection, 8);
//    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<16>",  g_imp_pts_build, collection, 16);
//    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<32>",  g_imp_pts_build, collection, 32);
//    benchmark::RegisterBenchmark("Grammar-Improved-PT-Index<64>",  g_imp_pts_build, collection, 64);

    /*cout << " >> Building the LZ77-Index:" << endl;
    benchmark::RegisterBenchmark("LZ77-Index", lz77build, collection);
    cout << " >> Building the LZEnd-Index:" << endl;
    benchmark::RegisterBenchmark("LZEnd-Index", lzEndbuild, collection);*/
    //cout << " >> Building R-Index:" << endl;

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


    return 0;

}
// 