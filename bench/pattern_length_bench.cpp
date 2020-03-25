//
// Created by alpachec on 22-11-18.
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
std::string path = "../files/indices/";

std::vector<std::string> patt;
int code = 0;
size_t lenght;
std::string data_bs;
HybridSelfIndex* idx_hyb;
ri::r_index<>* idx_r;
cds_static::RePairSLPIndex *idx_slp;
SelfGrammarIndexPT* idx_gimp;
SelfGrammarIndexPTS* idx_gimp_pts;
SelfGrammarIndexBS* idx_gimpbs;


std::fstream rlcsa_f(       "rlcsa",std::ios::out|std::ios::binary);
std::fstream b_f(       "br",std::ios::out|std::ios::binary);
std::fstream slp_f(     "slpr",std::ios::out|std::ios::binary);
std::fstream r_f(       "rr",std::ios::out|std::ios::binary);
std::fstream hyb_f(     "hybr",std::ios::out|std::ios::binary);
std::fstream g_fpt(     "grpt",std::ios::out|std::ios::binary);
std::fstream g_fpt8(    "grpt8",std::ios::out|std::ios::binary);
std::fstream g_fpt16(   "grpt16",std::ios::out|std::ios::binary);
std::fstream g_fpt32(   "grpt32",std::ios::out|std::ios::binary);
std::fstream g_fpt64(   "grpt64",std::ios::out|std::ios::binary);
std::fstream g_fbs(     "grbs",std::ios::out|std::ios::binary);




auto g_imp_locate = [](benchmark::State &st, const std::string& collection)
{

    size_t nocc = 0;
    double mean =0;

    std::fstream g_f(path+std::to_string(collections_code[collection])+".gidx", std::ios::in | std::ios::binary);
    idx_gimp = new SelfGrammarIndexPT();
    idx_gimp->load(g_f);
    g_f.close();


    for (auto _ : st)
    {
        for (auto &&  i : patt ) {

            auto start = timer::now();
            std::vector<uint> X;
            //sdsl::bit_vector _occ(data_bs.size(), 0);
            idx_gimp->locate(i, X);

            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            //sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();
            nocc += X.size();
            //nocc += rrr.rank(_occ.size());
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

    }
    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_gimp->size_in_bytes();
    //st.counters["g"] = idx_gimp->get_grammar().n_rules()-1;
    delete idx_gimp;

};




auto g_imp_pts_locate = [](benchmark::State &st, const std::string & collection, const uint8_t& sampling)
{

    size_t nocc = 0;
    double mean =0;

    std::fstream g_f(path+std::to_string(collections_code[collection])+".gpts"+std::to_string(sampling)+"idx", std::ios::in | std::ios::binary);

    idx_gimp_pts = new SelfGrammarIndexPTS(sampling);
    idx_gimp_pts->load(g_f);
    g_f.close();

    for (auto _ : st)
    {
        for (auto &&  i : patt ) {

            auto start = timer::now();
            std::vector<uint> X;
            //sdsl::bit_vector _occ(data_bs.size(), 0);
            idx_gimp_pts->locate2(i, X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            //sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();

            nocc += X.size();//rrr.rank(_occ.size());
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }
    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_gimp_pts->size_in_bytes();
    //st.counters["g"] = idx_gimp_pts->get_grammar().n_rules()-1;
    delete idx_gimp_pts;

};

auto g_imp_bs_locate = [](benchmark::State &st, const std::string & collection)
{

    size_t nocc = 0;
    double mean =0;

    idx_gimpbs = new SelfGrammarIndexBS();
    std::fstream gbs_f(path+std::to_string(collections_code[collection])+".gbsidx", std::ios::in | std::ios::binary);
    //std::cout<<path+std::to_string(collections_code[collection])+".gbsidx"<<std::endl;
    idx_gimpbs->load(gbs_f);
    gbs_f.close();


    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {

            //std::cout<<i<<std::endl;
            auto start = timer::now();
            std::vector<uint> X;
            //sdsl::bit_vector _occ(data_bs.size(),0);

            idx_gimpbs->locate(i, X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            //sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();

            nocc += X.size();
            //nocc += rrr.rank(_occ.size());
            //g_fbs << i << " " << X.size() <<"\n";
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

    }
    g_fbs.close();
    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_gimpbs->size_in_bytes();
    //st.counters["g"] = idx_gimpbs->get_grammar().n_rules()-1;

    delete idx_gimpbs;

};
/*
auto g_imp_locate = [](benchmark::State &st, const std::string& collection)
{

    size_t nocc = 0;
    double mean =0;

    std::fstream g_f(path+std::to_string(collections_code[collection])+".gidx", std::ios::in | std::ios::binary);
    idx_gimp = new SelfGrammarIndexPT();
    idx_gimp->load(g_f);
    g_f.close();
    int j = 0;


    for (auto _ : st)
    {
        //for (auto &&  i : patt ) {

            auto start = timer::now();
            std::vector<uint> X;
            //sdsl::bit_vector _occ(data_bs.size(), 0);
            //idx_gimp->locate(i, X);
            idx_gimp->locate(patt[j], X);
            ++j;
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            //sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();
            nocc += X.size();
            //nocc += rrr.rank(_occ.size());
            mean += (duration_cast<nanoseconds>(stop - start).count());
        //}

    }
    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_gimp->size_in_bytes();
    //st.counters["g"] = idx_gimp->get_grammar().n_rules()-1;
    delete idx_gimp;

};




auto g_imp_pts_locate = [](benchmark::State &st, const std::string & collection, const uint8_t& sampling)
{

    size_t nocc = 0;
    double mean =0;

    std::fstream g_f(path+std::to_string(collections_code[collection])+".gpts"+std::to_string(sampling)+"idx", std::ios::in | std::ios::binary);

    idx_gimp_pts = new SelfGrammarIndexPTS(sampling);
    idx_gimp_pts->load(g_f);
    g_f.close();
    int j = 0;

    for (auto _ : st)
    {
            auto start = timer::now();
            std::vector<uint> X;
            //sdsl::bit_vector _occ(data_bs.size(), 0);
            idx_gimp_pts->locate(patt[j], X);
            std::sort(X.begin(),X.end());
            std::unique(X.begin(),X.end());
            ++j;
            //sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();
            nocc += X.size();//rrr.rank(_occ.size());
            mean += (duration_cast<nanoseconds>(stop - start).count());
    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_gimp_pts->size_in_bytes();
    //st.counters["g"] = idx_gimp_pts->get_grammar().n_rules()-1;
    delete idx_gimp_pts;

};

auto g_imp_bs_locate = [](benchmark::State &st, const std::string & collection)
{

    size_t nocc = 0;
    double mean =0;

    idx_gimpbs = new SelfGrammarIndexBS();
    std::fstream gbs_f(path+std::to_string(collections_code[collection])+".gbsidx", std::ios::in | std::ios::binary);
    idx_gimpbs->load(gbs_f);
    gbs_f.close();

    int j = 0;
    for (auto _ : st)
    {

        //for (auto &&  i : patt ){

            //std::cout<<i<<std::endl;
            auto start = timer::now();
            std::vector<uint> X;
            //sdsl::bit_vector _occ(data_bs.size(),0);

            //idx_gimpbs->locate(i, X);
            idx_gimpbs->locate(patt[j], X);
            std::sort(X.begin(),X.end());
            std::unique(X.begin(),X.end());
            ++j;
            //sdsl::bit_vector::rank_1_type rrr(&_occ);

            auto stop = timer::now();
            nocc+=X.size();
            //nocc += rrr.rank(_occ.size());
            //g_fbs << i << " " << X.size() <<"\n";
            mean += (duration_cast<nanoseconds>(stop - start).count());
        //}

    }
    g_fbs.close();
    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_gimpbs->size_in_bytes();
    //st.counters["g"] = idx_gimpbs->get_grammar().n_rules()-1;

    delete idx_gimpbs;

};
*/


/*

auto slp_locate = [](benchmark::State &st, const std::string & collection, const int& qgram)
{
    cds_static::RePairSLPIndex *idx_slp;
    std::string filename = path+std::to_string(collections_code[collection])+"_"+std::to_string(qgram);
    char* _f = (char *)filename.c_str();
    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);

    size_t nocc = 0;
    double mean =0;
    int j =0 ;
    for (auto _ : st)
    {

            uint occs;
            uchar* tt = (uchar*)(patt[j].c_str());
          */
/*  for (auto &&  s: patt[j] ) {
                std::cout<<int(s)<<" ";
            }
            std::cout<<std::endl<<tt<<std::endl;*//*


            auto start = timer::now();
            std::vector<uint> *pos = idx_slp->RePairSLPIndex::locate(tt, patt[j].length(), &occs);
            auto stop = timer::now();

            std::sort(pos->begin(),pos->end());

            delete pos;
            nocc += occs;
            mean += (duration_cast<nanoseconds>(stop - start).count());
            ++j;


    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_slp->size();
    //st.counters["g"] = idx_slp->length();

//    slp_f.close();

    delete idx_slp;



};
*/


auto slp_locate = [](benchmark::State &st, const std::string & collection, const int& qgram)
{

    std::string filename = path+std::to_string(collections_code[collection])+"_"+std::to_string(qgram);
    char* _f = (char *)filename.c_str();
    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);

    size_t nocc = 0;
    double mean =0;

    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {
            uint occs;
            uchar* tt = (uchar*)(i.c_str());
            // std::cout<<tt<<std::endl;
            auto start = timer::now();
            std::vector<uint> *pos = idx_slp->RePairSLPIndex::locate(tt, i.length(), &occs);
            auto stop = timer::now();

            std::sort(pos->begin(),pos->end());

            /*for (uint jj = 0; jj < pos->size(); ++jj)
            {
                slp_f << (*pos)[jj] -1 <<"\n";
            }
            */
            delete pos;
            nocc += occs;
            mean += (duration_cast<nanoseconds>(stop - start).count());

        }

    }
    slp_f.close();
    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_slp->size();
    //st.counters["g"] = idx_slp->length();

//    slp_f.close();

    delete idx_slp;



};



int main (int argc, char *argv[] ){

/*
    if(argc < 2){
        std::cout<<"bad parameters....";
        return 0;
    }
*/

    std::string collection = (argv[1]);

    if(collections_code.find(collection) == collections_code.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
        return 0;
    }


    std::fstream f_c(collection, std::ios::in| std::ios::binary);

/*  std::string buff;
    while (!f_c.eof() && std::getline(f_c, buff)) {
        lenght += buff.size();
        data_bs+=buff;
    }
*/


    unsigned char buffer[1000];
    while(!f_c.eof()){
        f_c.read((char*)buffer,1000);
        data_bs.append((char*) buffer,f_c.gcount());
    }

    for (int i = 0; i < data_bs.size(); ++i) {
        if(int(data_bs[i]) == 0 || int(data_bs[i]) == 1)
            data_bs[i] = 2;
    }



    /*  std::string S;
      S.reserve(10);
      for (int j = 0; j < 10; ++j) {
          std::cout<<data_bs[92911778+j];
      }
      std::cout<<std::endl;
  */
    f_c.close();
    code = collections_code[collection];

    unsigned int num_patterns = atoi(argv[2]);
    unsigned int len_patterns = atoi(argv[4]);
    //patt.reserve(num_patterns);

    //std::string pattern_file = (argv[3])+ std::to_string(code)+"_len_"+ std::to_string(len_patterns) +".ptt";
    std::string pattern_file = (argv[3])+ std::to_string(code)+"_len_200.ptt";
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);
    std::cout<<pattern_file<<std::endl;

    if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
        return 0;
    }
    std::string buff; uint i =0;

    while (!f.eof() && std::getline(f, buff) && i < num_patterns) {
        if(buff.size() > 1){
            std::string sss;
            sss.resize(len_patterns);
            std::copy(buff.begin(),buff.begin()+len_patterns,sss.begin());
           // std::cout<<sss<<"\t"<<i<<std::endl;

            for (int k = 0; k < sss.size(); ++k){
                if(int(sss[k]) == 0 || int(sss[k]) == 1 )
                    sss[k] = 65;
            }


            patt.push_back(sss);
            i++;
        }
    }
    f.close();


    std::cout<<patt.size()<<std::endl;

    /*std::cout<<patt[0]<<std::endl;
//./idx_locate_performance ../tests/collections/repetitive/reals/einstein.en.txt  10000 ../files/ 5    --benchmark_out= ../results/5_loc_einstein_en_out.csv      --benchmark_out_format=csv

    return 0;*/

    /////////SLP

    /*std::string filename = path+std::to_string(collections_code[collection]);
    char* _f = (char *)filename.c_str();

    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);

    if (q != 0)
    {
        benchmark::RegisterBenchmark("SLP-Index", slp_locate, collection);
    }*/


    /////////R-Index
   /* std::string filename = path+std::to_string(collections_code[collection]);
    fstream rf(filename+".ri",std::ios::in|std::ios::binary);
    bool fast;

    //fast or small index?
    rf.read((char*)&fast,sizeof(fast));
    idx_r = new ri::r_index<>();
    idx_r->load(rf);
    benchmark::RegisterBenchmark("R-Index", r_locate);

*/
    ///////Hyb Index
/*

    char* _f = (char *)filename.c_str();


    idx_hyb = new HybridSelfIndex(_f);
    benchmark::RegisterBenchmark("Hyb-Index", hyb_locate);
    benchmark::RegisterBenchmark("SLP-Index<q-4>", slp_locate, collection,4);
    benchmark::RegisterBenchmark("SLP-Index<q-8>", slp_locate, collection,8);
    benchmark::RegisterBenchmark("SLP-Index<q-16>", slp_locate, collection,16);
    benchmark::RegisterBenchmark("SLP-Index<q-32>", slp_locate, collection,32);
    benchmark::RegisterBenchmark("SLP-Index<q-64>", slp_locate, collection,64);

*/



    /////GRAMMAR IDEXES


    benchmark::RegisterBenchmark("SLP-Index<q-4>", slp_locate, collection,4);
    benchmark::RegisterBenchmark("SLP-Index<q-8>", slp_locate, collection,8);
    benchmark::RegisterBenchmark("SLP-Index<q-16>", slp_locate, collection,16);
    benchmark::RegisterBenchmark("SLP-Index<q-32>", slp_locate, collection,32);
    benchmark::RegisterBenchmark("SLP-Index<q-64>", slp_locate, collection,64);


    benchmark::RegisterBenchmark("Grammar-Binary-Search-Index", g_imp_bs_locate, collection);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<s-1>"    , g_imp_locate, collection);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<s-4>" , g_imp_pts_locate,collection,4);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<s-8>" , g_imp_pts_locate,collection,8);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<s-16>", g_imp_pts_locate,collection,16);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<s-32>", g_imp_pts_locate,collection,32);
    benchmark::RegisterBenchmark("Grammar-Improved-Index<s-64>", g_imp_pts_locate,collection,64);
    ///benchmark::RegisterBenchmark("Brute Force-Index"    , bt_locate);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

  /*
   * delete idx_slp;
    delete idx_r;
    delete idx_hyb;
    */

}