//
// Created by alejandro on 14-01-19.
//



#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include "repetitive_collections.h"


#include <slp/RePairSLPIndex.h>
#include "../SelfGrammarIndexPT.h"
#include <ri/r_index.hpp>
#include "../utils/build_hyb_lz77.h"
#include <hyb/HybridSelfIndex.h>
#include "../SelfGrammarIndexBS.h"
#include "../SelfGrammarIndexPTS.h"


using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

std::string path = "../files/indices/";
std::string read_path = "../files/divided_indices/";

std::vector<std::string> patt;
std::vector<std::pair<uint,uint>> rg;



void load_X_p(SelfGrammarIndex* idx, const int& code, int op)
{
    fstream f;
    switch (op){
        case 1:{
            f.open(read_path+std::to_string(code)+"_X_p_gmr",std::ios::in|std::ios::binary);
            std::cout<<"X_p_gmr"<<std::endl;
            break;
        }
        case 2:{
            f.open(read_path+std::to_string(code)+"_X_p_gmr_rs",std::ios::in|std::ios::binary);
            std::cout<<"X_p_gmr_rs"<<std::endl;
            break;
        }
        case 3:{
            f.open(read_path+std::to_string(code)+"_X_p_ap",std::ios::in|std::ios::binary);
            std::cout<<"X_p_ap"<<std::endl;
            break;
        }
        default:{
            f.open(read_path+std::to_string(code)+"_X_p_gmr_rs",std::ios::in|std::ios::binary);
            std::cout<<"X_p_gmr_rs"<<std::endl;
            break;
        }
    }
    idx->get_grammar().load_X_p(f);
}

void load_z(SelfGrammarIndex* idx, const int& code, int op)
{
    fstream f;
    switch (op){
        case 1:{
            f.open(read_path+std::to_string(code)+"_z_sd",std::ios::in|std::ios::binary);
            std::cout<<"Z sd vector"<<std::endl;
        }break;
        case 2:{
            f.open(read_path+std::to_string(code)+"_z_rrr",std::ios::in|std::ios::binary);
            std::cout<<"Z rrr vector"<<std::endl;
        }break;
        default:{
            f.open(read_path+std::to_string(code)+"_z_sd",std::ios::in|std::ios::binary);
            std::cout<<"Z sd vector"<<std::endl;
        }break;
    }
    idx->get_grammar().load_z(f);
}



auto idx_div = [](benchmark::State &st, const int& op_i,const int& sampling,const int& op_x_p,const int& op_z, const int& op_trie, const int& code )
{

    SelfGrammarIndex* idx;
    //LOADING INDICE TYPE
    switch (op_i){ // indice type
        case 1:{ //binary-search
            idx = new SelfGrammarIndexBS();
            break;
        }
        case 2:{ //full-pt
            idx = new SelfGrammarIndexPT();
            {
                fstream f(read_path+std::to_string(code)+"_g_rules_pt",std::ios::in|std::ios::binary);
                idx->load_rules_pt(f);
            }
            {
                fstream f (read_path+std::to_string(code)+"_g_sfx_pt",std::ios::in|std::ios::binary);
                idx->load_sfx_pt(f);
            }
            break;
        }
        case 3:{ //sampled-pt
            idx = new SelfGrammarIndexPTS(sampling);
            {
                fstream f(read_path+std::to_string(code)+"_g_rules_pts_"+std::to_string(sampling),std::ios::in|std::ios::binary);
                idx->load_rules_pt(f);
            }

            {
                fstream f (read_path+std::to_string(code)+"_g_sfx_pts_"+std::to_string(sampling),std::ios::in|std::ios::binary);
                idx->load_sfx_pt(f);
            }
            break;
        }
        default:{ //binary-search
            idx = new SelfGrammarIndexBS();
            break;
        }
    }

    //std::cout<<"index loaded.....\n";

    load_X_p(idx,code,op_x_p);
    //std::cout<<"X_p loaded.....\n";
    load_z(idx,code,op_z);
    //std::cout<<"z loaded.....\n";

    ////////Loading F
    {

        fstream f (read_path+std::to_string(code)+"_F",std::ios::in|std::ios::binary);
        idx->get_grammar().load_F(f);
        //std::cout<<"F loaded.....\n";
    }

    ////////Loading Parser Tree
    {

        fstream f (read_path+std::to_string(code)+"_tree_dfuds",std::ios::in|std::ios::binary);
        idx->get_grammar().load_mtree(f);
        //std::cout<<"parser tree loaded.....\n";
    }

    ////////Loading L
    {
        fstream f (read_path+std::to_string(code)+"_L_sd",std::ios::in|std::ios::binary);
        idx->get_grammar().load_l(f);
        //std::cout<<"L loaded.....\n";
    }

    ////////Loading Y
    {
        fstream f (read_path+std::to_string(code)+"_y_sd",std::ios::in|std::ios::binary);
        idx->get_grammar().load_y(f);
        //std::cout<<"Y loaded.....\n";
    }


    ////////Loading left/right trie
    if(op_trie == 1 || op_trie == 3){

        {
            fstream f (read_path+std::to_string(code)+"_left_trie",std::ios::in|std::ios::binary);
            idx->get_grammar().load_ltrie(f);

            //std::cout<<"_left_trie loaded.....\n";
        }

        {
            fstream f (read_path+std::to_string(code)+"_right_trie",std::ios::in|std::ios::binary);
            idx->get_grammar().load_rtrie(f);
            //std::cout<<"_right_trie loaded.....\n";
        }

    }

    //////////loading alp
    {
        fstream f (read_path+std::to_string(code)+"_alp",std::ios::in|std::ios::binary);
        idx->get_grammar().load_alp(f);
        //std::cout<<"_alp loaded.....\n";

    }

    //////////loading grid
    {
        fstream f (read_path+std::to_string(code)+"_grid",std::ios::in|std::ios::binary);
        idx->get_grid().load(f);
        //std::cout<<"_grid loaded.....\n";
    }

    double mean =0;
    std::string s;
    s.reserve(rg[0].second-rg[0].first+1);
    sleep(3);

    if(op_trie == 1){

        for (auto _ : st)
        {
            for (auto &&  i : rg )
            {
                //std::cout<<i<<std::endl;
                idx->display_trie(i.first,i.second-1,s);
            }
        }
    }
    if(op_trie == 2){

        for (auto _ : st)
        {
            for (auto &&  i : rg )
            {
                //std::cout<<i<<std::endl;
                idx->display(i.first,i.second-1,s);
            }
        }
    }

    if(op_trie == 3){

        for (auto _ : st)
        {
            for (auto &&  i : rg )
            {
                //std::cout<<i<<std::endl;
                idx->display_L_trie(i.first,i.second-1,s);
            }
        }
    }

    if(op_trie == 4){
        for (auto _ : st)
        {
            for (auto &&  i : rg )
            {
                //std::cout<<i<<std::endl;
                idx->display_L(i.first,i.second-1,s);
            }
        }
    }

    if(op_trie == 5){
        for (auto _ : st)
        {
            for (auto &&  i : rg )
            {
                //std::cout<<i<<std::endl;
                idx->display_L_rec(i.first,i.second-1,s);
            }
        }
    }
    st.counters["space"] = idx->size_in_bytes();
    st.counters["space-tries"] = idx->size_tries_in_bytes();

    delete idx;
};



int main (int argc, char *argv[] ){


    if(argc < 2){
        std::cout<<"bad parameters....";
        // return 0;
    }


    std::string collection = "";//(argv[1]);

    if(collections_code.find(collection) == collections_code.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
        //   return 0;
    }

    auto code = collections_code[collection];


    unsigned int num_ranges =  atoi(argv[2]);
    unsigned int rg_len = atoi(argv[4]);
    //patt.reserve(num_patterns);

    std::string rg_file = "../files/";//argv[3];

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

    benchmark::RegisterBenchmark("BS [X_p_gmr, Z_rrr] + NoTries"      , idx_div, 1,0,1,1,2,code)->Unit(benchmark::kMicrosecond);
    benchmark::RegisterBenchmark("BS [X_p_gmr, Z_rrr] + Tries"        , idx_div, 1,0,1,1,1,code)->Unit(benchmark::kMicrosecond);
    benchmark::RegisterBenchmark("BS [X_p_gmr, Z_rrr] + Trie + L"     , idx_div, 1,0,1,1,3,code)->Unit(benchmark::kMicrosecond);
    benchmark::RegisterBenchmark("BS [X_p_gmr, Z_rrr] + L"            , idx_div, 1,0,1,1,4,code)->Unit(benchmark::kMicrosecond);
    benchmark::RegisterBenchmark("BS [X_p_gmr, Z_rrr] + L rec"        , idx_div, 1,0,1,1,5,code)->Unit(benchmark::kMicrosecond);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();
}