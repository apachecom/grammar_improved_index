//
// Created by alpachec on 08-12-18.
//

#include <iostream>
#include <fstream>


#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include "repetitive_collections.h"
#include "../SelfGrammarIndexBS.h"
#include "../SelfGrammarIndexPT.h"
#include "../SelfGrammarIndexPTS.h"
#include "sdsl/coder_elias_delta.hpp"


std::string path = "../files/new_divided_indices/";
std::string read_path = "../files/indices/";



auto divide_patricia_tree = [](benchmark::State &st, const string &collection){

    uint code = collections_code[collection];
    std::fstream g_PT_no_smp_f(read_path+std::to_string(collections_code[collection])+".gidx", std::ios::in | std::ios::binary);

    SelfGrammarIndexPT idx_pt;
    idx_pt.load(g_PT_no_smp_f);
    g_PT_no_smp_f.close();

    {
        std::fstream grammar_rules_pt_f(path + std::to_string(collections_code[collection]) + "_g_rules_pt",
                                        std::ios::out | std::ios::binary);
        auto pt_rules = idx_pt.get_pt_rules();
        pt_rules.save(grammar_rules_pt_f);
        grammar_rules_pt_f.close();

        std::fstream grammar_sfx_pt_f(path + std::to_string(collections_code[collection]) + "_g_sfx_pt",
                                      std::ios::out | std::ios::binary);
        auto pt_sfx = idx_pt.get_pt_suffixes();
        pt_sfx.save(grammar_sfx_pt_f);
        grammar_sfx_pt_f.close();
    }


    int sampling[] = {4, 8, 16, 32, 64};

    for (int i = 0; i < 5; ++i)
    {

        std::fstream g_pts(read_path+std::to_string(collections_code[collection])+".gpts"+std::to_string(sampling[i])+"idx", std::ios::in | std::ios::binary);

        SelfGrammarIndexPTS idx_pts(sampling[i]);
        idx_pts.load(g_pts);

        {
            std::fstream grammar_rules_pt_f(path + std::to_string(collections_code[collection]) + "_g_rules_pts_"+std::to_string(sampling[i]),
                                            std::ios::out | std::ios::binary);
            auto pt_rules = idx_pts.get_pt_rules();
            pt_rules.save(grammar_rules_pt_f);
            grammar_rules_pt_f.close();

            std::fstream grammar_sfx_pt_f(path + std::to_string(collections_code[collection]) + "_g_sfx_pts_"+std::to_string(sampling[i]),
                                          std::ios::out | std::ios::binary);
            auto pt_sfx = idx_pts.get_pt_suffixes();
            pt_sfx.save(grammar_sfx_pt_f);
            grammar_sfx_pt_f.close();
        }



    }



    for (auto _ : st)
    {
        sleep(3);
    }


};


auto divide_basics = [](benchmark::State &st, const string &collection){

    uint code = collections_code[collection];

    std::fstream gbs_f(read_path+std::to_string(code)+".gbsidx", std::ios::in | std::ios::binary);
    SelfGrammarIndexBS idx;
    idx.load(gbs_f);
    std::cout<<"READ "+read_path+std::to_string(code)+".gbsidx"<<std::endl;


//    auto& alp = idx.get_grammar().get_alp();
//    std::fstream alp_f(path+std::to_string(code)+"_alp",std::ios::out|std::ios::binary);
//    sdsl::serialize(alp,alp_f);
//    alp_f.close();
//    std::cout<<"SAVE "+path+std::to_string(code)+"_alp"<<std::endl;
//
    auto& L = idx.get_grammar().get_L();
    std::fstream L_f(path+std::to_string(code)+"_L_sd",std::ios::out|std::ios::binary);
    sdsl::serialize(L,L_f);
    L_f.close();
    std::cout<<"SAVE "+path+std::to_string(code)+"_L_sd"<<std::endl;


    std::cout<<"CREATING L_bitvectors "<<std::endl;
    sdsl::bit_vector lT(L.size(),0);
    for (int j = 0; j < L.size(); ++j) {
        if(L[j] == 1) lT[j] = 1;
    }
    sdsl::rrr_vector<> L_rrr (lT);
    std::fstream l_rrr_f(path+std::to_string(code)+"_l_rrr",std::ios::out|std::ios::binary);
    sdsl::serialize(L_rrr,l_rrr_f);


//
//    auto& mtree = idx.get_grammar().get_parser_tree();
//    std::fstream tree_f(path+std::to_string(code)+"_tree_dfuds",std::ios::out|std::ios::binary);
//    mtree.save(tree_f);
//    tree_f.close();
//    std::cout<<"SAVE "+path+std::to_string(code)+"_tree_dfuds"<<std::endl;
//
//
//    auto& F_inv = idx.get_grammar().get_F_inv();
//    std::fstream F_inv_f(path+std::to_string(code)+"_F_inv",std::ios::out|std::ios::binary);
//    sdsl::serialize(F_inv,F_inv_f);
//    F_inv_f.close();
//    std::cout<<"SAVE "+path+std::to_string(code)+"_F_inv"<<std::endl;
//
//
//    auto& X_p_gmr = idx.get_grammar().get_Xp();
//    std::fstream X_p_gmr_f(path+std::to_string(code)+"_X_p_gmr",std::ios::out|std::ios::binary);
//    sdsl::serialize(X_p_gmr,X_p_gmr_f);
//    X_p_gmr_f.close();
//
//    std::cout<<"SAVE "+path+std::to_string(code)+"_X_p_gmr"<<std::endl;
//
//    auto& Z = idx.get_grammar().get_Z();
//    std::fstream z_sd_f(path+std::to_string(code)+"_z_sd",std::ios::out|std::ios::binary);
//    sdsl::serialize(Z,z_sd_f);
//    z_sd_f.close();
//
//    std::cout<<"SAVE "+path+std::to_string(code)+"_z_sd"<<std::endl;
//
    auto& Y = idx.get_grammar().get_Y();
    std::fstream y_sd_f(path+std::to_string(code)+"_y_sd",std::ios::out|std::ios::binary);
    sdsl::serialize(Y,y_sd_f);
    y_sd_f.close();

    std::cout<<"CREATING Y_bitvectors "<<std::endl;
    sdsl::bit_vector yT(Y.size(),0);
    for (int j = 0; j < Y.size(); ++j) {
        if(Y[j] == 1) yT[j] = 1;
    }
    sdsl::rrr_vector<> Y_rrr (lT);
    std::fstream Y_rrr_f(path+std::to_string(code)+"_y_rrr",std::ios::out|std::ios::binary);
    sdsl::serialize(Y_rrr,Y_rrr_f);


//
//    std::cout<<"SAVE "+path+std::to_string(code)+"_y_sd"<<std::endl;
//
//    compressed_grammar::compact_perm F = idx.get_grammar().get_F();
//    std::fstream F_sd_f(path+std::to_string(code)+"_F",std::ios::out|std::ios::binary);
//    sdsl::serialize(F,F_sd_f);
//    F_sd_f.close();
//
//    std::cout<<"SAVE "+path+std::to_string(code)+"_F"<<std::endl;
//
//    auto& left_trie = idx.get_grammar().get_left_trie();
//    std::fstream left_trie_f(path+std::to_string(code)+"_left_trie",std::ios::out|std::ios::binary);
//    left_trie.save(left_trie_f);
//    left_trie_f.close();
//    std::cout<<"SAVE "+path+std::to_string(code)+"_left_trie"<<std::endl;
//
//    auto& right_trie = idx.get_grammar().get_right_trie();
//    std::fstream right_trie_f(path+std::to_string(code)+"_right_trie",std::ios::out|std::ios::binary);
//    right_trie.save(right_trie_f);
//    right_trie_f.close();
//
//    std::cout<<"SAVE "+path+std::to_string(code)+"_right_trie"<<std::endl;
//    /////////CREATING NEW VARIANTS////////////////////
//    std::cout<<"CREATING NEW VARIANTS"<<std::endl;
//
//    /////////CREATING X_p_wavelet_tree////////////////////
//    std::cout<<"CREATING X_p_wavelet_tree"<<std::endl;
//    sdsl::wt_gmr_rs<> X_p_gmr_rs;
//    sdsl::wt_ap<> X_p_ap;
//
//    sdsl::int_vector<> T(X_p_gmr.size());
//    for (int i = 0; i < X_p_gmr.size() ; ++i) {
//        T[i] = X_p_gmr[i];
//    }
//
//    std::fstream xp_f("temp_xp_file",std::ios::out|std::ios::binary);
//    sdsl::serialize(T,xp_f);
//    sdsl::construct(X_p_gmr_rs, "temp_xp_file", 0);
//    sdsl::construct(X_p_ap, "temp_xp_file", 0);
//
//    xp_f.close();
//
//    xp_f.open(path+std::to_string(code)+"_X_p_gmr_rs",std::ios::out|std::ios::binary);
//    sdsl::serialize(X_p_gmr_rs,xp_f);
//    xp_f.close();
//
//    xp_f.open(path+std::to_string(code)+"_X_p_ap",std::ios::out|std::ios::binary);
//    sdsl::serialize(X_p_ap,xp_f);
//    xp_f.close();
//
//    /////////CREATING Z_bitvectors ////////////////////
//    std::cout<<"CREATING Z_bitvectors "<<std::endl;
//    sdsl::bit_vector bT(Z.size(),0);
//    for (int j = 0; j < Z.size(); ++j) {
//        if(Z[j] == 1) bT[j] = 1;
//    }
//    sdsl::rrr_vector<> Z_rrr (bT);
//    std::fstream z_rrr_f(path+std::to_string(code)+"_z_rrr",std::ios::out|std::ios::binary);
//    sdsl::serialize(Z_rrr,z_rrr_f);
//
//    std::fstream grid_f(path+std::to_string(code)+"_grid",std::ios::out|std::ios::binary);
//    idx.get_grid().save(grid_f);
//
//    //////////////GRID//////////////////
//    std::cout<<"CREATING GRID XA XB bitvectors "<<std::endl;
//
//    auto& xa = idx.get_grid().XA;
//    std::fstream xa_f(path+std::to_string(code)+"_grid_xa_rrr",std::ios::out|std::ios::binary);
//    sdsl::serialize(xa,xa_f);
//    alp_f.close();
//    std::cout<<"SAVE "+path+std::to_string(code)+"_grid_xa_rrr"<<std::endl;
//
//    sdsl::bit_vector xaT(xa.size(),0);
//    for (int j = 0; j < xa.size(); ++j) {
//        if(xa[j] == 1) xaT[j] = 1;
//    }
//
//    sdsl::sd_vector<> xa_sd (xaT);
//    std::fstream xa_sd_f(path+std::to_string(code)+"_grid_xa_sd",std::ios::out|std::ios::binary);
//    sdsl::serialize(xa_sd,xa_sd_f);
//
//    auto& xb = idx.get_grid().XB;
//    std::fstream xb_f(path+std::to_string(code)+"_grid_xb_rrr",std::ios::out|std::ios::binary);
//    sdsl::serialize(xb,xb_f);
//    alp_f.close();
//    std::cout<<"SAVE "+path+std::to_string(code)+"_grid_xb_rrr"<<std::endl;
//
//    sdsl::bit_vector xbT(xb.size(),0);
//    for (int j = 0; j < xb.size(); ++j) {
//        if(xb[j] == 1) xbT[j] = 1;
//    }
//
//    sdsl::sd_vector<> xb_sd (xaT);
//    std::fstream xb_sd_f(path+std::to_string(code)+"_grid_xb_sd",std::ios::out|std::ios::binary);
//    sdsl::serialize(xb_sd,xb_sd_f);
//
//    std::cout<<"CREATING wavelet_tree for SB"<<std::endl;
//    auto& wt_int_sb = idx.get_grid().SB;
//    std::fstream wt_int_sb_f(path+std::to_string(code)+"_wt_int_sb",std::ios::out|std::ios::binary);
//    sdsl::serialize(wt_int_sb,wt_int_sb_f);
//    wt_int_sb_f.close();
//    std::cout<<"SAVE "+path+std::to_string(code)+"_wt_int_sb"<<std::endl;
//
//    sdsl::wm_int<> SB_wm;
//
//    sdsl::int_vector<> sbT(wt_int_sb.size());
//    for (int i = 0; i < wt_int_sb.size() ; ++i) {
//        sbT[i] = wt_int_sb[i];
//    }
//
//    std::fstream sb_f("temp_xp_file",std::ios::out|std::ios::binary);
//    sdsl::serialize(sbT,sb_f);
//    sdsl::construct(SB_wm, "temp_xp_file", 0);
//    sb_f.close();
//
//    sb_f.open(path+std::to_string(code)+"_wm_int_sb",std::ios::out|std::ios::binary);
//    sdsl::serialize(SB_wm,sb_f);
//    sb_f.close();

    auto& int_vec_sl = idx.get_grid().SL;
    std::fstream int_vec_sl_f(path+std::to_string(code)+"_int_vec_sl",std::ios::out|std::ios::binary);
    sdsl::serialize(int_vec_sl,int_vec_sl_f);
    int_vec_sl_f.close();
    std::cout<<"SAVE "+path+std::to_string(code)+"_int_vec_sl"<<std::endl;

    sdsl::int_vector<> slT(int_vec_sl.size());
    for (int i = 0; i < int_vec_sl.size() ; ++i) {
        slT[i] = int_vec_sl[i];
    }

    sdsl::vlc_vector<sdsl::coder::elias_delta> _sl_vlc(slT);
    std::fstream int_vec_delta_sl_f(path+std::to_string(code)+"_int_vec_vlc_delta_sl",std::ios::out|std::ios::binary);
    sdsl::serialize(_sl_vlc,int_vec_delta_sl_f);
    int_vec_delta_sl_f.close();

    sdsl::vlc_vector<sdsl::coder::elias_gamma> _sl_vlc_delta(slT);
    std::fstream int_vec_gamma_sl_f(path+std::to_string(code)+"_int_vec_vlc_gama_sl",std::ios::out|std::ios::binary);
    sdsl::serialize(_sl_vlc_delta,int_vec_gamma_sl_f);
    int_vec_gamma_sl_f.close();

    sdsl::enc_vector<> _sl_enc_delta(slT);
    std::fstream _sl_enc_delta_f(path+std::to_string(code)+"_int_vec_enc_delta_sl",std::ios::out|std::ios::binary);
    sdsl::serialize(_sl_enc_delta,_sl_enc_delta_f);
    _sl_enc_delta_f.close();

    sdsl::enc_vector<sdsl::coder::elias_gamma> _sl_enc_gamma(slT);
    std::fstream _sl_enc_gamma_f(path+std::to_string(code)+"_int_vec_enc_gamma_sl",std::ios::out|std::ios::binary);
    sdsl::serialize(_sl_enc_gamma,_sl_enc_gamma_f);
    _sl_enc_gamma_f.close();

    std::cout<<"FINISHED"<<std::endl;

    for (auto _ : st)
    {
        sleep(3);
    }

};

auto basics = [](benchmark::State &st, const string &collection){

    auto code = collections_code[collection];

    std::fstream gbs_f(path+std::to_string(collections_code[collection])+".gbsidx", std::ios::in | std::ios::binary);
    std::fstream rg_f(path+std::to_string(collections_code[collection])+"repair_g", std::ios::in | std::ios::binary);

    grammar not_compressed_grammar;
    not_compressed_grammar.load(rg_f);

    SelfGrammarIndexBS idx;
    idx.load(gbs_f);

    for (auto _ : st) {

    }
        st.counters["G"] = not_compressed_grammar.get_size();
        st.counters["g"] = not_compressed_grammar.n_rules();
        st.counters["sigma"] = not_compressed_grammar.num_terminals();
        st.counters["LBR (Bytes)"] = idx.get_grid().size_in_bytes();
        st.counters["LBR SB (WT)(Bytes)"] = idx.get_grid().get_SB_size();
        st.counters["LBR SL (Int_Vector)(Bytes)"] = idx.get_grid().get_SL_size();
        st.counters["LBR XA (Bit_Vector)(Bytes)"] = idx.get_grid().get_XA_size();
        st.counters["LBR XB (Bit_Vector)(Bytes)"] = idx.get_grid().get_XB_size();
        st.counters["CGr (Bytes)"] = idx.get_grammar().size_in_bytes();
        st.counters["CGr Tree (DFUDS) (Bytes)"] = idx.get_grammar().get_tree_size();
        st.counters["CGr LeftTrie  (BP) (Bytes)"] = idx.get_grammar().get_compact_trie_left_size();
        st.counters["CGr RightTrie (BP) (Bytes)"] = idx.get_grammar().get_compact_trie_right_size();
        st.counters["CGr X (WT)(Bytes)"] = idx.get_grammar().get_X_size();
        st.counters["CGr Z (BitVector) (Bytes)"] = idx.get_grammar().get_Z_size();
        st.counters["CGr L (BitVector) (Bytes)"] = idx.get_grammar().get_L_size();
        st.counters["CGr Y (BitVector) (Bytes)"] = idx.get_grammar().get_Y_size();
        st.counters["CGr F (Permutation) (Bytes)"] = idx.get_grammar().get_F_size();


};



int main (int argc, char *argv[] ){

    /*benchmark::RegisterBenchmark("Grammar-Improved-Index Basic coreutils",          basics, "../tests/collections/repetitive/reals/coreutils" );
    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic einstein",           basics, "../tests/collections/repetitive/reals/einstein.en.txt" );
    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic Escherichia_Coli",   basics, "../tests/collections/repetitive/reals/Escherichia_Coli" );
    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic influenza",          basics, "../tests/collections/repetitive/reals/influenza" );
    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic kernel",             basics, "../tests/collections/repetitive/reals/kernel" );
    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic para",               basics, "../tests/collections/repetitive/reals/para" );
    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic cere",               basics, "../tests/collections/repetitive/reals/cere" );
*/

    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic coreutils",          divide_basics, "../tests/collections/repetitive/reals/coreutils" );
    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic einstein",           divide_basics, "../tests/collections/repetitive/reals/einstein.en.txt" );
    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic Escherichia_Coli",   divide_basics, "../tests/collections/repetitive/reals/Escherichia_Coli" );
    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic influenza",          divide_basics, "../tests/collections/repetitive/reals/influenza" );
    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic kernel",             divide_basics, "../tests/collections/repetitive/reals/kernel" );
    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic para",               divide_basics, "../tests/collections/repetitive/reals/para" );
    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic cere",               divide_basics, "../tests/collections/repetitive/reals/cere" );

//    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic coreutils",          divide_patricia_tree, "../tests/collections/repetitive/reals/coreutils" );
//    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic einstein",           divide_patricia_tree, "../tests/collections/repetitive/reals/einstein.en.txt" );
//    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic Escherichia_Coli",   divide_patricia_tree, "../tests/collections/repetitive/reals/Escherichia_Coli" );
//    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic influenza",          divide_patricia_tree, "../tests/collections/repetitive/reals/influenza" );
//    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic kernel",             divide_patricia_tree, "../tests/collections/repetitive/reals/kernel" );
//    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic para",               divide_patricia_tree, "../tests/collections/repetitive/reals/para" );
//    benchmark::RegisterBenchmark("Grammar-Improved-Index Basic cere",               divide_patricia_tree, "../tests/collections/repetitive/reals/cere" );



    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;

}
