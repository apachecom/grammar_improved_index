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
//#include <rlcsa/rlcsa.h>


using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;
std::string path = "../files/indices/";

std::vector<std::string> patt;
int code = 0;
size_t lenght;
std::string data_bs;



HybridSelfIndex*            idx_hyb;
ri::r_index<>*              idx_r;
cds_static::RePairSLPIndex* idx_slp;
SelfGrammarIndexPT*         idx_gimp;
SelfGrammarIndexPTS*        idx_gimp_pts;
SelfGrammarIndexBS*         idx_gimpbs;




/*std::fstream rlcsa_f(   "rlcsa",std::ios::out|std::ios::binary);
std::fstream b_f(       "br",std::ios::out|std::ios::binary);
std::fstream slp_f(     "slpr",std::ios::out|std::ios::binary);
std::fstream r_f(       "rr",std::ios::out|std::ios::binary);
std::fstream hyb_f(     "hybr",std::ios::out|std::ios::binary);
std::fstream g_fpt(     "grpt",std::ios::out|std::ios::binary);
std::fstream g_fpt8(    "grpt8",std::ios::out|std::ios::binary);
std::fstream g_fpt16(   "grpt16",std::ios::out|std::ios::binary);
std::fstream g_fpt32(   "grpt32",std::ios::out|std::ios::binary);
std::fstream g_fpt64(   "grpt64",std::ios::out|std::ios::binary);
std::fstream g_fbs(     "grbs",std::ios::out|std::ios::binary);*/




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
       // sleep(5);

    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_gimp->size_in_bytes();
    //st.counters["g"] = idx_gimp->get_grammar().n_rules()-1;
    delete idx_gimp;

};



auto g_imp_locate_no_Trie = [](benchmark::State &st, const std::string& collection)
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
            idx_gimp->locateNoTrie(i, X);

            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            //sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();
            nocc += X.size();
            //nocc += rrr.rank(_occ.size());
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }
        // sleep(5);

    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_gimp->size_in_bytes()
                           - idx_gimp->get_grammar().get_left_trie().size_in_bytes()
                           - idx_gimp->get_grammar().get_right_trie().size_in_bytes();
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
            idx_gimp_pts->locate(i, X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            //sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();

            nocc += X.size();//rrr.rank(_occ.size());
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }
        //sleep(5);

    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_gimp_pts->size_in_bytes();
    //st.counters["g"] = idx_gimp_pts->get_grammar().n_rules()-1;
    delete idx_gimp_pts;

};



auto g_imp_pts_locate_no_Trie = [](benchmark::State &st, const std::string & collection, const uint8_t& sampling)
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
            idx_gimp_pts->locateNoTrie(i, X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            //sdsl::bit_vector::rank_1_type rrr(&_occ);
            auto stop = timer::now();

            nocc += X.size();//rrr.rank(_occ.size());
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }
        //sleep(5);

    }

    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;

    st.counters["space"] = idx_gimp_pts->size_in_bytes()
                           - idx_gimp_pts->get_grammar().get_left_trie().size_in_bytes()
                           - idx_gimp_pts->get_grammar().get_right_trie().size_in_bytes();
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
        //sleep(5);
    }
    //g_fbs.close();
    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_gimpbs->size_in_bytes();
    //st.counters["g"] = idx_gimpbs->get_grammar().n_rules()-1;

    delete idx_gimpbs;

};



auto g_imp_bs_locate_no_Trie = [](benchmark::State &st, const std::string & collection)
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

            idx_gimpbs->locateNoTrie(i, X);
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
        //sleep(5);
    }
    //g_fbs.close();
    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_gimpbs->size_in_bytes() - idx_gimpbs->get_grammar().get_left_trie().size_in_bytes() - idx_gimpbs->get_grammar().get_right_trie().size_in_bytes();
    //st.counters["g"] = idx_gimpbs->get_grammar().n_rules()-1;

    delete idx_gimpbs;

};





auto hyb_locate = [](benchmark::State &st)
{
    size_t nocc = 0;
    double mean = 0;

    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {
            unsigned long _Occ;
            unsigned long* _occ;
            unsigned int m = (unsigned int) i.size();

            auto start = timer::now();

            idx_hyb->locate((uchar*)(i.c_str()),m,&_Occ,&_occ);


            auto stop = timer::now();

            nocc += _Occ;

            //std::sort(_occ,_occ+(_Occ*sizeof(long)));


            /*for (int j = 0; j < _Occ; ++j) {
                hyb_f << _occ[j] << "\n";
            }*/

            ///delete _occ;
            mean += (duration_cast<nanoseconds>(stop - start).count());
        }

     //   sleep(5);

    }
    //hyb_f.close();
    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_hyb->sizeDS;
   // st.counters["z"] = idx_hyb->orig_z;



};

auto r_locate = [](benchmark::State &st)
{
    size_t nocc = 0;
    double mean =0;

    for (auto _ : st)
    {

        for (auto &&  i : patt )
        {

            auto start = timer::now();

            auto occ = idx_r->locate_all(i);

            auto stop = timer::now();
            nocc += occ.size();

            //std::sort(occ.begin(),occ.end());

            /*for (auto &&  jj: occ) {
                r_f << jj <<"\n";
            }*/

            mean += (duration_cast<nanoseconds>(stop - start).count());

        }
     //   sleep(5);

    }
    //r_f.close();
    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = sdsl::size_in_bytes(*idx_r);
    //st.counters["r"] = idx_r->number_of_runs();
    //delete idx_r;

};

auto slp_locate = [](benchmark::State &st, const std::string & collection, const int& qgram)
{

    std::string filename = "../files/indices/"+std::to_string(collections_code[collection])+"_"+std::to_string(qgram);
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

            //std::sort(pos->begin(),pos->end());

            /*for (uint jj = 0; jj < pos->size(); ++jj)
            {
                slp_f << (*pos)[jj] -1 <<"\n";
            }*/

            delete pos;
            nocc += occs;
            mean += (duration_cast<nanoseconds>(stop - start).count());

        }
      //  sleep(5);
    }
    //slp_f.close();
    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = idx_slp->size();
    //st.counters["g"] = idx_slp->length();

//    slp_f.close();

    delete idx_slp;



};



auto bt_locate = [](benchmark::State &st)
{

    size_t nocc = 0;
    double mean =0;

    for (auto _ : st)
    {
        nocc = 0;
        for (auto &&  i : patt )
        {
            uint occs = 0;
            auto start = timer::now();

                size_t pos = data_bs.find(i, 0);
                while(pos != string::npos)
                {
                    ++nocc;
                    ++occs;
                    pos = data_bs.find(i,pos+1);
                }

            auto stop = timer::now();
           // b_f << i << " " << occs <<"\n";
            mean += (duration_cast<nanoseconds>(stop - start).count());


        }

    }


    //b_f.close();
    st.counters["n_occ"] = nocc;
    st.counters["time"] = mean;
    st.counters["mean time"] = mean*1.0/nocc;
    st.counters["space"] = data_bs.size();


};



auto secondOcc = [](benchmark::State &st,const std::string & collection, const uint8_t& sampling)
{
    fstream f(std::to_string(sampling)+"bf_ptos",std::ios::in|std::ios::binary);
    vector<long> L;
    vector<pair<uint,uint>> X;
    vector<pair<uint,uint>> Y;
    //std::vector< std::pair<size_t,size_t> > pairs;

    while(!f.eof())
    {
        /*long len;
        size_t n_ptos;
        sdsl::load(n_ptos,f);
        std::cout<<"ptos"<<n_ptos<<std::endl;
        pairs.resize(n_ptos);

        for (int i = 0; i < n_ptos ; ++i) {
            size_t kk ;

            sdsl::load(kk,f);
           // std::cout<<kk<<std::endl;
            pairs[i].second = kk;

        }
        sdsl::load(len,f);*/

        uint x1,x2,y1,y2;
        long len;

        sdsl::load(x1,f);
        sdsl::load(x2,f);
        sdsl::load(y1,f);
        sdsl::load(y2,f);
        sdsl::load(len,f);


        X.emplace_back(x1,x2);
        Y.emplace_back(y1,y2);
        L.emplace_back(len);


    }
    f.close();
    ///std::cout<<"len: "<<L.size()<<std::endl;
    /*std::fstream g_f(path+std::to_string(collections_code[collection])+".gpts"+std::to_string(sampling)+"idx", std::ios::in | std::ios::binary);
    idx_gimp_pts = new SelfGrammarIndexPTS(sampling);
    idx_gimp_pts->load(g_f);
    g_f.close();*/


    idx_gimpbs = new SelfGrammarIndexBS();
    std::fstream gbs_f(path+std::to_string(collections_code[collection])+".gbsidx", std::ios::in | std::ios::binary);

    //std::cout<<path+std::to_string(collections_code[collection])+".gbsidx"<<std::endl;

    idx_gimpbs->load(gbs_f);
    gbs_f.close();


    std::vector<uint> occ;
    size_t bocc = 0;
    std::cout<<L.size()<<std::endl;
    for (auto _ : st)
    {
        for (int i = 0; i < L.size() ; ++i) {

            occ.clear();
            ///std::cout<<pairs[i].second<<std::endl;
            idx_gimpbs->test_findSecondOcc(L[i],X[i].first,X[i].second,Y[i].second,Y[i].second,occ);
            // std::cout<<occ.size()<<std::endl;
            bocc += occ.size();

        }
        //sleep(5);

    }
    st.counters["n_occ"] = occ.size();
};

auto secondOcc2 = [](benchmark::State &st,const std::string & collection, const uint8_t& sampling)
{
    fstream f(std::to_string(sampling)+"bf_ptos",std::ios::in|std::ios::binary);
    vector<long> L;
    vector<pair<uint,uint>> X;
    vector<pair<uint,uint>> Y;
    //std::vector< std::pair<size_t,size_t> > pairs;

    while(!f.eof())
    {
        /*long len;
        size_t n_ptos;
        sdsl::load(n_ptos,f);
        std::cout<<"ptos"<<n_ptos<<std::endl;
        pairs.resize(n_ptos);

        for (int i = 0; i < n_ptos ; ++i) {
            size_t kk ;

            sdsl::load(kk,f);
           // std::cout<<kk<<std::endl;
            pairs[i].second = kk;

        }
        sdsl::load(len,f);*/

        uint x1,x2,y1,y2;
        long len;

        sdsl::load(x1,f);
        sdsl::load(x2,f);
        sdsl::load(y1,f);
        sdsl::load(y2,f);
        sdsl::load(len,f);


        X.emplace_back(x1,x2);
        Y.emplace_back(y1,y2);
        L.emplace_back(len);


    }
    f.close();
    ///std::cout<<"len: "<<L.size()<<std::endl;
    /*std::fstream g_f(path+std::to_string(collections_code[collection])+".gpts"+std::to_string(sampling)+"idx", std::ios::in | std::ios::binary);
    idx_gimp_pts = new SelfGrammarIndexPTS(sampling);
    idx_gimp_pts->load(g_f);
    g_f.close();*/


    idx_gimpbs = new SelfGrammarIndexBS();
    std::fstream gbs_f(path+std::to_string(collections_code[collection])+".gbsidx", std::ios::in | std::ios::binary);

    //std::cout<<path+std::to_string(collections_code[collection])+".gbsidx"<<std::endl;

    idx_gimpbs->load(gbs_f);
    gbs_f.close();


    std::vector<uint> occ;
    size_t bocc = 0;
    for (auto _ : st)
    {
        for (int i = 0; i < L.size() ; ++i) {

            occ.clear();
            ///std::cout<<pairs[i].second<<std::endl;
            idx_gimpbs->test_findSecondOcc2(L[i],X[i].first,X[i].second,Y[i].second,Y[i].second,occ);
            // std::cout<<occ.size()<<std::endl;
            bocc += occ.size();

        }
       // sleep(5);

    }
    st.counters["n_occ"] = occ.size();
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
   /* unsigned char buffer[1000];
    while(!f_c.eof()){
        f_c.read((char*)buffer,1000);
        data_bs.append((char*) buffer,f_c.gcount());
    }

    for (int i = 0; i < data_bs.size(); ++i) {
        if(data_bs[i] == 0 || data_bs[i] == 1)
            data_bs[i] = 2;
    }*/
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

    std::string pattern_file = "../files/patrones/" + std::to_string(code)+"_len_200.ptt";///(argv[3])+ std::to_string(code)+"_len_200.ptt";
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);
    std::cout<<pattern_file<<std::endl;

    if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
      //  return 0;
    }
    std::string buff; uint i = 0;

    while (!f.eof() && std::getline(f, buff) && i < num_patterns) {
        std::string sss;
        sss.resize(len_patterns);
        std::copy(buff.begin(),buff.begin()+len_patterns,sss.begin());
        ///std::cout<<i<<"\t"<<sss<<std::endl;

        if(sss == "NNNNNNNNNN"){
            std::cout<<"patron malo malo \t"<<sss<<std::endl;
            continue;
        }

        for (int k = 0; k < sss.size(); ++k){
            if(int(sss[k]) == 0 || int(sss[k]) == 1 || int(sss[k]) == 2 ){
                sss[k] = 65;
                //return 0;

            }

        }
        patt.push_back(sss);
        i++;
    }
    f.close();

    //std::cout<<patt.size()<<std::endl;
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

 /*   idx_gimpbs = new SelfGrammarIndexBS();
    std::fstream gbs_f(path+std::to_string(collections_code[collection])+".gbsidx", std::ios::in | std::ios::binary);
    //std::cout<<path+std::to_string(collections_code[collection])+".gbsidx"<<std::endl;
    idx_gimpbs->load(gbs_f);*/


    /////////R-Index
    std::string filename = "../files/new_indices/"+std::to_string(collections_code[collection]);
    std::cout<<filename<<std::endl;
    fstream rf(filename+".ri",std::ios::in|std::ios::binary);
    bool fast;

    //fast or small index?
    rf.read((char*)&fast,sizeof(fast));
    idx_r = new ri::r_index<>();
    idx_r->load(rf);
    benchmark::RegisterBenchmark("R-Index", r_locate);


    ///////Hyb Index

    char* _f = (char *)filename.c_str();
    idx_hyb = new HybridSelfIndex(_f);


   /* std::fstream frepair(path+std::to_string(collections_code[collection])+"repair_g", std::ios::out| std::ios::binary);
    grammar not_compressed_grammar;
    not_compressed_grammar.load(frepair);
*/
    /*std::cout<<collections_name[collection]<<" (hyb z) "<<idx_hyb->orig_z<<std::endl;
    std::cout<<collections_name[collection]<<" (hyb z) "<<idx_hyb->z<<std::endl;
    std::cout<<collections_name[collection]<<" (ri r) "<<idx_r->number_of_runs()<<std::endl;
*/

    /*std::cout<<collections_name[collection]<<" (gidx G) "<<idx_gimpbs->get_grammar().size_of_grammar()<<std::endl;
    std::cout<<collections_name[collection]<<" (gidx g) "<<idx_gimpbs->get_grammar().n_rules()<<std::endl;
    std::cout<<collections_name[collection]<<" (repair g) "<<not_compressed_grammar.n_rules()<<std::endl;
    std::cout<<collections_name[collection]<<" (repair G) "<<not_compressed_grammar.get_size()<<std::endl;
*/




     benchmark::RegisterBenchmark("Hyb-Index", hyb_locate);

     benchmark::RegisterBenchmark("SLP-Index<q-4>", slp_locate, collection,4);
    benchmark::RegisterBenchmark("SLP-Index<q-6>", slp_locate, collection,6);
    benchmark::RegisterBenchmark("SLP-Index<q-8>", slp_locate, collection,8);
    benchmark::RegisterBenchmark("SLP-Index<q-10>", slp_locate, collection,10);
    benchmark::RegisterBenchmark("SLP-Index<q-12>", slp_locate, collection,12);


     //benchmark::RegisterBenchmark("SLP-Index<q-4>", slp_locate, collection,4);
     //benchmark::RegisterBenchmark("SLP-Index<q-8>", slp_locate, collection,8);
     //benchmark::RegisterBenchmark("SLP-Index<q-16>", slp_locate, collection,16);
     //benchmark::RegisterBenchmark("SLP-Index<q-32>", slp_locate, collection,32);
     //benchmark::RegisterBenchmark("SLP-Index<q-64>", slp_locate, collection,64);



    ////GRAMMAR IDEXES



    //benchmark::RegisterBenchmark("Grammar-Binary-Search-Index"     , g_imp_bs_locate, collection);
    //benchmark::RegisterBenchmark("Grammar-Improved-Index<s-1>"     , g_imp_locate, collection);
    //benchmark::RegisterBenchmark("Grammar-Improved-Index<s-4>"     , g_imp_pts_locate,collection,4);
    //benchmark::RegisterBenchmark("Grammar-Improved-Index<s-8>"     , g_imp_pts_locate,collection,8);
    //benchmark::RegisterBenchmark("Grammar-Improved-Index<s-16>"    , g_imp_pts_locate,collection,16);
    //benchmark::RegisterBenchmark("Grammar-Improved-Index<s-32>"    , g_imp_pts_locate,collection,32);
    //benchmark::RegisterBenchmark("Grammar-Improved-Index<s-64>"    , g_imp_pts_locate,collection,64);



    //benchmark::RegisterBenchmark("Grammar no Trie - Binary-Search-Index"     , g_imp_bs_locate_no_Trie, collection);
    //benchmark::RegisterBenchmark("Grammar no Trie - Improved-Index<s-1>"     , g_imp_locate_no_Trie, collection);
    //benchmark::RegisterBenchmark("Grammar no Trie - Improved-Index<s-4>"     , g_imp_pts_locate_no_Trie,collection,4);
    //benchmark::RegisterBenchmark("Grammar no Trie - Improved-Index<s-8>"     , g_imp_pts_locate_no_Trie,collection,8);
    //benchmark::RegisterBenchmark("Grammar no Trie - Improved-Index<s-16>"    , g_imp_pts_locate_no_Trie,collection,16);
    //benchmark::RegisterBenchmark("Grammar no Trie - Improved-Index<s-32>"    , g_imp_pts_locate_no_Trie,collection,32);
    //benchmark::RegisterBenchmark("Grammar no Trie - Improved-Index<s-64>"    , g_imp_pts_locate_no_Trie,collection,64);



/*    benchmark::RegisterBenchmark("secondOcc Grammar-Improved-Index<s-4>" , secondOcc,collection,4);
    benchmark::RegisterBenchmark("secondOcc Grammar-Improved-Index<s-8>" , secondOcc,collection,8);
    benchmark::RegisterBenchmark("secondOcc Grammar-Improved-Index<s-16>", secondOcc,collection,16);
    benchmark::RegisterBenchmark("secondOcc Grammar-Improved-Index<s-32>", secondOcc,collection,32);
    benchmark::RegisterBenchmark("secondOcc Grammar-Improved-Index<s-64>", secondOcc,collection,64);


    benchmark::RegisterBenchmark("secondOcc2 Grammar-Improved-Index<s-4>" , secondOcc2,collection,4);
    benchmark::RegisterBenchmark("secondOcc2 Grammar-Improved-Index<s-8>" , secondOcc2,collection,8);
    benchmark::RegisterBenchmark("secondOcc2 Grammar-Improved-Index<s-16>", secondOcc2,collection,16);
    benchmark::RegisterBenchmark("secondOcc2 Grammar-Improved-Index<s-32>", secondOcc2,collection,32);
    benchmark::RegisterBenchmark("secondOcc2 Grammar-Improved-Index<s-64>", secondOcc2,collection,64);*/


    //benchmark::RegisterBenchmark("Brute Force-Index"    , bt_locate);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

/*


    delete idx_slp;
    delete idx_r;
    delete idx_hyb;
*/


}
