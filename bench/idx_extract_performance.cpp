//
// Created by alpachec on 24-09-18.
//

//
// Created by alpachec on 16-09-18.
//



#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include <slp/RePairSLPIndex.h>
#include "../SelfGrammarIndexPT.h"
#include <ri/r_index.hpp>
#include "../utils/build_hyb_lz77.h"
#include <hyb/HybridSelfIndex.h>
#include "repetitive_collections.h"
#include "../SelfGrammarIndexBS.h"
#include "../SelfGrammarIndexPTS.h"

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;


std::vector<std::pair<uint,uint>> rg;
int code = 0;

HybridSelfIndex* idx_hyb;
ri::r_index<>* idx_r;
cds_static::RePairSLPIndex *idx_slp;
SelfGrammarIndexBS* idx_gimp;
SelfGrammarIndexPTS* idx_gimp_pts;

std::string path = "../files/new_divided_indices/";






auto g_imp_display = [](benchmark::State &st, const std::string& collection, const int& sampling)
{


    double mean =0;

    std::fstream g_f(path+std::to_string(collections_code[collection])+".gpts"+std::to_string(sampling)+"idx", std::ios::in | std::ios::binary);
    std::cout<<path+std::to_string(collections_code[collection])+".gpts"+std::to_string(sampling)+"idx"<<std::endl;


    if(!g_f.is_open())
        std::cout<<"Error al abrir el fichero"<<std::endl;

    idx_gimp_pts = new SelfGrammarIndexPTS(sampling);
    idx_gimp_pts->load(g_f);
    g_f.close();


    for (auto _ : st)
    {

        for (auto &&  i : rg )
        {
            std::string s;
            s.reserve(i.second-i.first+1);
            idx_gimp_pts->display(i.first,i.second,s);


        }

    }

    delete idx_gimp_pts;

};



auto g_imp_bs_display = [](benchmark::State &st, const std::string& collection)
{


    std::fstream g_f(path+std::to_string(collections_code[collection])+".gbsidx", std::ios::in | std::ios::binary);

    if(!g_f.is_open())
        std::cout<<"Error al abrir el fichero"<<std::endl;

    idx_gimp = new SelfGrammarIndexBS();
    idx_gimp->load(g_f);
    g_f.close();


    for (auto _ : st)
    {

        for (auto &&  i : rg )
        {
            std::string s;
            s.reserve(i.second-i.first+1);
            idx_gimp->display(i.first,i.second,s);

        }

    }




    delete idx_gimp;

};




auto hyb_display = [](benchmark::State &st,const std::string & collection)
{

    double mean;
    std::string filename = path+std::to_string(collections_code[collection]);
    char* _f = (char *)filename.c_str();
    idx_hyb = new HybridSelfIndex(_f);

    for (auto _ : st)
    {

        for (auto &&  i : rg )
        {

            uint m = i.second-i.first+1;
            unsigned char *s;
            //idx_hyb->locate((uchar*)(i.c_str()),m,&_Occ,&_occ);
            idx_hyb->extract(i.first,m,&s);
            delete []s;
        }

    }

    delete idx_hyb;

};

/*auto r_display = [](benchmark::State &st)
{
    double mean =0;

    for (auto _ : st)
    {

        for (auto &&  i : rg )
        {

            auto start = timer::now();

            //auto occ = idx_r->locate_all(i);

            auto stop = timer::now();



        }

    }

    st.counters["time"] = mean;
    st.counters["time/c"] = mean*1.0/rg.size();

    delete idx_r;

};*/

auto slp_display = [](benchmark::State &st, const std::string & collection,const int& qgram)
{

    double mean ;
    std::string filename = path+std::to_string(collections_code[collection])+"_"+std::to_string(qgram);
    char* _f = (char *)filename.c_str();

    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);

    if (q == 0)
    {
        std::cout<<"Error loading slp file\n";
        return;
    }

    for (auto _ : st)
    {
        //mean =0;
        for (auto &&  i : rg )
        {

            unsigned char * s = idx_slp->RePairSLPIndex::extract(i.first,i.second-1);
            //delete [] s;


        }

    }



    delete idx_slp;

};


int main (int argc, char *argv[] ){


    if(argc < 2){
        std::cout<<"bad parameters....";
       // return 0;
    }


    std::string collection = (argv[1]);

    if(collections_code.find(collection) == collections_code.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
     //   return 0;
    }

    code = collections_code[collection];


    unsigned int num_ranges = atoi(argv[2]);
    unsigned int rg_len = atoi(argv[4]);
    //patt.reserve(num_patterns);

    std::string rg_file = argv[3];

    std::fstream f(rg_file + "rangos/"+std::to_string(code)+"_"+std::to_string(rg_len)+".rg", std::ios::in| std::ios::binary);

    std::cout<<rg_file + "rangos/"+std::to_string(code)+"_"+std::to_string(rg_len)+".rg"<<std::endl;

    if(!f.is_open()){
        std::cout<<"Error the ranges file could not opened!!\n";
        return 0;
    }
    std::string buff; uint i = 0;
    while (!f.eof() && std::getline(f, buff), i < num_ranges) {
        if(buff.size() > 1){
            uint l,r;
            sscanf(buff.c_str(),"%u %u",&l,&r);
            rg.emplace_back(l,r);
            i++;
        }
    }
    //std::cout<<"Number of ranges "<<rg.size()<<std::endl;
    f.close();

    //benchmark::RegisterBenchmark("Grammar-Improved-Index Binary Search"    , g_imp_bs_display, collection);
   // benchmark::RegisterBenchmark("Grammar-Improved-Index<1>"    , g_imp_display, collection,1);
   /* benchmark::RegisterBenchmark("Grammar-Improved-Index<4>"    , g_imp_display, collection,4);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<8>"    , g_imp_display, collection,8);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<16>"    , g_imp_display, collection,16);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<32>"    , g_imp_display, collection,32);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<64>"    , g_imp_display, collection,64);
*/

    /*benchmark::RegisterBenchmark("Grammar-Binary-Search-Index", g_imp_bs_locate, collection);
    benchmark::RegisterBenchmark("Grammar-Improved-Index"    , g_imp_locate, collection);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<8>" , g_imp_pts_locate,collection,4);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<8>" , g_imp_pts_locate,collection,8);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<16>", g_imp_pts_locate,collection,16);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<32>", g_imp_pts_locate,collection,32);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<64>", g_imp_pts_locate,collection,64);*/




    benchmark::RegisterBenchmark("SLP-Index<4>", slp_display,collection,4);
//    benchmark::RegisterBenchmark("SLP-Index<6>", slp_display,collection,6);
    benchmark::RegisterBenchmark("SLP-Index<8>", slp_display,collection,8);
//    benchmark::RegisterBenchmark("SLP-Index<10>", slp_display,collection,10);
//    benchmark::RegisterBenchmark("SLP-Index<12>", slp_display,collection,12);
//    benchmark::RegisterBenchmark("SLP-Index<8>", slp_display,collection,8);
    benchmark::RegisterBenchmark("SLP-Index<16>", slp_display,collection,16);
    benchmark::RegisterBenchmark("SLP-Index<32>", slp_display,collection,32);
    benchmark::RegisterBenchmark("SLP-Index<64>", slp_display,collection,64);
//    //benchmark::RegisterBenchmark("Hyb", hyb_display,collection);


    /*
     * benchmark::RegisterBenchmark("SLP-Index", slp_locate, collection,4);
    benchmark::RegisterBenchmark("SLP-Index", slp_locate, collection,8);
    benchmark::RegisterBenchmark("SLP-Index", slp_locate, collection,16);
    benchmark::RegisterBenchmark("SLP-Index", slp_locate, collection,32);
    benchmark::RegisterBenchmark("SLP-Index", slp_locate, collection,64);*/


    //benchmark::RegisterBenchmark("Hyb-Index", hyb_display,collection);

   /* /////////R-Index
    fstream rf(filename+".ri",std::ios::in|std::ios::binary);
    bool fast;

    //fast or small index?
    rf.read((char*)&fast,sizeof(fast));
    idx_r = new ri::r_index<>();
    idx_r->load(rf);
    benchmark::RegisterBenchmark("R-Index", r_display);*/



    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


}