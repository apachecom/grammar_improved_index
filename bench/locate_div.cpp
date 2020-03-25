//
// Created by alejandro on 09-01-19.
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
            ///std::cout<<"opening "+read_path+std::to_string(code)+"_X_p_gmr"<<std::endl;
            break;
        }
        case 2:{
            f.open(read_path+std::to_string(code)+"_X_p_gmr_rs",std::ios::in|std::ios::binary);
            ///std::cout<<"opening "+read_path+std::to_string(code)+"_X_p_gmr_rs"<<std::endl;
            break;
        }
        case 3:{
            f.open(read_path+std::to_string(code)+"_X_p_ap",std::ios::in|std::ios::binary);
            ///std::cout<<"opening "+read_path+std::to_string(code)+"_X_p_ap"<<std::endl;
            break;
        }
        default:{
            f.open(read_path+std::to_string(code)+"_X_p_gmr_rs",std::ios::in|std::ios::binary);
            ///std::cout<<"opening "+read_path+std::to_string(code)+"_X_p_gmr_rs"<<std::endl;
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
        }break;
        case 2:{
            f.open(read_path+std::to_string(code)+"_z_rrr",std::ios::in|std::ios::binary);
        }break;
        default:{
            f.open(read_path+std::to_string(code)+"_z_sd",std::ios::in|std::ios::binary);
        }break;
    }
    idx->get_grammar().load_z(f);
}


auto idx_div = [](benchmark::State &st, const int& op_i,const int& sampling,const int& op_x_p,const int& op_z, const bool& op_trie, const int& code )
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
    if(op_trie){

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

    size_t nocc = 0;
    double mean =0;

    for (auto _ : st)
    {

        if(op_trie){
            for (auto &&  i : patt )
            {
                //std::cout<<i<<std::endl;
                std::vector<uint> X;
                idx->locate(i, X);
                std::sort(X.begin(),X.end());
                auto last = std::unique(X.begin(),X.end());
                X.erase(last,X.end());
                nocc += X.size();
            }
        }else{
            for (auto &&  i : patt )
            {
                //std::cout<<i<<std::endl;
                std::vector<uint> X;
                idx->locateNoTrie(i, X);
                std::sort(X.begin(),X.end());
                auto last = std::unique(X.begin(),X.end());
                X.erase(last,X.end());
                nocc += X.size();
            }
        }

    }

    st.counters["n_occ"] = nocc;
    st.counters["space"] = idx->size_in_bytes();
    st.counters["space-tries"] = idx->size_tries_in_bytes();

    delete idx;
};

int main (int argc, char *argv[] ){


    std::string collection = (argv[1]);//"../tests/collections/repetitive/reals/Escherichia_Coli";//

    if(collections_code.find(collection) == collections_code.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
        return 0;
    }

    auto code = collections_code[collection];

    unsigned int num_patterns = atoi(argv[2]);
    unsigned int len_patterns = atoi(argv[4]);


    ////////////////reading patterns/////////////

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
            ////std::cout<<"patron malo malo \t"<<sss<<std::endl;
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


    ////////////////reading patterns/////////////



//const int& op_i,const int& sampling,const int& op_x_p,const int& op_z, const bool& op_trie, const int& code

/*
 * Opciones
 * Indices
 * 1-BS
 * 2-FPT
 * 3-SPT
 * X_p
 * 1-X_p_gmr
 * 2-X_p_gmr_rs
 * 3-X_p_ap
 * Z
 * 1-sd
 * 2-rrr
 *
 */
    /*benchmark::RegisterBenchmark("BS [X_p_gmr_rs, z_sd] + Tries"     , idx_div, 1,0,2,1,true,code);
    benchmark::RegisterBenchmark("BS [X_p_gmr_rs, z_sd] + NoTries"     , idx_div, 1,0,2,1,false,code);*/


    /*benchmark::RegisterBenchmark("BS [X_p_gmr, z_sd] + Tries"     , idx_div, 1,0,1,1,true,code);
    benchmark::RegisterBenchmark("BS [X_p_gmr, z_sd] + NoTries"     , idx_div, 1,0,1,1,false,code);*/

    /*benchmark::RegisterBenchmark("BS [X_p_ap, z_sd] + Tries"     , idx_div, 1,0,3,1,true,code);
    benchmark::RegisterBenchmark("BS [X_p_ap, z_sd] + NoTries"     , idx_div, 1,0,3,1,false,code);*/

    benchmark::RegisterBenchmark("BS [X_p_gmr, Z_rrr] + Tries"        , idx_div, 1,0,3,1,true,code);
    benchmark::RegisterBenchmark("BS [X_p_gmr, Z_rrr] + NoTries"      , idx_div, 1,0,3,1,false,code);
    //benchmark::RegisterBenchmark("FPT [X_p_gmr, rrr_sd] + Tries"       , idx_div, 2,0,1,2,true,code);
    //benchmark::RegisterBenchmark("FPT [X_p_gmr, rrr_sd] + NoTries"     , idx_div, 2,0,1,2,false,code);
    //benchmark::RegisterBenchmark("SPT [X_p_gmr, rrr_sd] + Tries"       , idx_div, 3,4,1,2,true,code);
    //benchmark::RegisterBenchmark("SPT [X_p_gmr, rrr_sd] + NoTries"     , idx_div, 3,4,1,2,false,code);


    //benchmark::RegisterBenchmark("FPT [X_p_ap, z_sd] + Tries"     , idx_div, 2,0,3,1,true,code);
    //benchmark::RegisterBenchmark("FPT [X_p_ap, rrr_sd] + NoTries"     , idx_div, 2,0,1,2,false,code);
    benchmark::RegisterBenchmark("SPT-8 [X_p_gmr, rrr_sd] + Tries"     , idx_div, 3,8,3,1,true,code);
    benchmark::RegisterBenchmark("SPT-8 [X_p_gmr, rrr_sd] + NoTries"     , idx_div, 3,8,3,1,true,code);
    //benchmark::RegisterBenchmark("SPT-16 [X_p_gmr_rs, rrr_sd] + Tries"     , idx_div, 3,16,2,2,true,code);
    benchmark::RegisterBenchmark("SPT-64 [X_p_gmr, rrr_sd] + Tries"     , idx_div, 3,64,3,1,false,code);
    benchmark::RegisterBenchmark("SPT-64 [X_p_gmr, rrr_sd] + NoTries"     , idx_div, 3,64,3,1,false,code);
    //benchmark::RegisterBenchmark("SPT [X_p_ap, z_sd] + NoTries"     , idx_div, 3,4,1,2,false,code);


    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

}
