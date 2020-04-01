//
// Created by alejandro on 14-05-19.
//

#include "../SelfGrammarIndex.h"
#include "../SelfGrammarIndexBS.h"
#include "../SelfGrammarIndexPT.h"
#include "../SelfGrammarIndexPTS.h"
//#include "../SelfGrammarIndexBSQ.h"



#ifndef IMPROVED_GRAMMAR_INDEX_UTILS_H
#define IMPROVED_GRAMMAR_INDEX_UTILS_H

#endif //IMPROVED_GRAMMAR_INDEX_UTILS_H

std::string files_dir = "../";
std::string path = files_dir+"/files/indices/";
std::string read_path = files_dir+"/files/new_divided_indices/";
std::string read_path_div = files_dir+"/files/new_divided_indices/";

void load_grammar(grammar& g, const uint &code, const bool &bal){
    if(bal){
        fstream f(read_path+std::to_string(code)+"bal_repair_g",std::ios::in|std::ios::binary);
        g.load(f);
    }else{
        fstream f(files_dir+"/files/"+std::to_string(code)+"repair_g",std::ios::in|std::ios::binary);
        g.load(f);
    }
}

void load_data(const size_t &_max,std::string& data, const std::string &coll) {

    data = "";
    std::fstream f(coll, std::ios::in | std::ios::binary);
//    std::cout<<coll<<std::endl;
    //std::string data;
    if (!f.is_open()) {
        std::cout << "Error the coll file could not opened!!\n";
        return;
    }
    std::string buff;
    unsigned char buffer[1000];
    while (!f.eof()){
        f.read((char *) buffer, 1000);
        data.append((char *) buffer, f.gcount());
    }
    for (int i = 0; i < data.size(); ++i) {
        if (data[i] == 0 || data[i] == 1 || data[i] == 2)
            data[i] = 3;
    }


//    std::cout<<"[DEBUG]:DATA SIZE " <<data.size()<<std::endl;

    f.close();
}


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
SelfGrammarIndex* load_idx (const int& op_i,const int& sampling, const int& code ){

    SelfGrammarIndex* idx;
    //LOADING INDICE TYPE
    switch (op_i){ // indice type
        case 1:{ //binary-search
            idx = new SelfGrammarIndexBS();
            fstream f(read_path+std::to_string(code)+".balgbsidx",std::ios::in|std::ios::binary);
            idx->load(f);
            break;
        }
        case 2:{ //full-pt
            idx = new SelfGrammarIndexPT();
            fstream f(read_path+std::to_string(code)+".gidx",std::ios::in|std::ios::binary);
            idx->load(f);
            break;
        }
        case 3:{ //sampled-pt
            idx = new SelfGrammarIndexPTS(sampling);
            fstream f(read_path+std::to_string(code)+".gpts"+std::to_string(sampling)+"idx",std::ios::in|std::ios::binary);
            idx->load(f);
            break;
        }
//        case 4:{ //q-gram-sampled-pt
//            idx = new SelfGrammarIndexBSQ();
////            fstream f(read_path+std::to_string(code)+".gpts"+std::to_string(sampling)+"idx",std::ios::in|std::ios::binary);
////            idx->load(f);
//            break;
//        }
        default:{ //binary-search
            idx = new SelfGrammarIndexBS();
            break;
        }
    }
    return idx;
}

SelfGrammarIndex* load_idx (const int& op_i,const int& sampling,const int& op_x_p,const int& op_z, const int& op_trie, const int& code )
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
//        case 4:{ //binary-search
//            idx = new SelfGrammarIndexBSQ();
//            break;
//        }
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
//        fstream f (read_path+std::to_string(code)+"_l_rrr",std::ios::in|std::ios::binary);
//        idx->get_grammar().load_l(f);
    }

    ////////Loading Y
    {
        fstream f (read_path+std::to_string(code)+"_y_sd",std::ios::in|std::ios::binary);
        idx->get_grammar().load_y(f);
        //std::cout<<"Y loaded.....\n";
//        fstream f (read_path+std::to_string(code)+"_y_rrr",std::ios::in|std::ios::binary);
//        idx->get_grammar().load_y(f);

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
//        fstream f (read_path+std::to_string(code)+"_grid",std::ios::in|std::ios::binary);
//        idx->get_grid().load(f);
        //std::cout<<"_grid loaded.....\n";
        fstream f (read_path+std::to_string(code)+"_int_vec_sl",std::ios::in|std::ios::binary);
        idx->get_grid().load_SL(f);
    }
    {
        fstream f (read_path+std::to_string(code)+"_wt_int_sb",std::ios::in|std::ios::binary);
        idx->get_grid().load_SB(f);
    }
    {
        fstream f (read_path+std::to_string(code)+"_grid_xb_rrr",std::ios::in|std::ios::binary);
        idx->get_grid().load_XB(f);
    }
    {
        fstream f (read_path+std::to_string(code)+"_grid_xa_rrr",std::ios::in|std::ios::binary);
        idx->get_grid().load_XA(f);
    }


    return idx;
};

