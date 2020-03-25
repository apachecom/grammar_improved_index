//
// Created by alpachec on 25-09-18.
//


#include <iostream>
#include <fstream>


#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include "repetitive_collections.h"
#include "../compressed_grammar.h"


compressed_grammar G;
compressed_grammar::parser_tree Tg;
compact_trie CTrie;
compact_trie::tree Tbp_tree;

uint nrules;
std::vector<uint> v_nocc;
uint z_size;

std::vector<uint> v_childs;
std::vector<uint> v_node_pos;

std::vector<uint> v_trie_node_pos;
std::vector<uint> v_trie_childs;
std::vector<uint> v_trie_level;


std::vector<unsigned char > alp;
std::vector<uint> r_al;



using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;




auto offText = [](benchmark::State &st){
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        compressed_grammar::g_long X = std::rand() % nrules + 1;
        auto p = G.offsetText(X);
    }

};

auto isTerminal = [](benchmark::State &st){
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        compressed_grammar::g_long X = std::rand() % nrules + 1;
        auto p = G.isTerminal(X);
    }

};

auto n_occ = [](benchmark::State &st){
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        compressed_grammar::g_long X = std::rand() % nrules + 1;
        auto p = G.n_occ(X);
    }

};

auto first_occ = [](benchmark::State &st){
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        compressed_grammar::g_long X = std::rand() % nrules + 1;
        auto p = G.select_occ(X,1);
    }

};


auto occ = [](benchmark::State &st){

    std::srand(std::time(nullptr));


    for (auto _ : st)
    {
        compressed_grammar::g_long X = std::rand() % nrules + 1;
        uint oc = (std::rand() % v_nocc[X-1]) + 1;
        auto p = G.select_occ(X,oc);
    }

};
auto _access_to_Z = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));


    for (auto _ : st)
    {
        compressed_grammar::g_long i = std::rand() % z_size + 1;

        auto p = G[i];
    }

};


auto tg_tree_access = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));
    for (auto _ : st)
    {
        auto i = std::rand() % z_size + 1;
        auto p = Tg[i];
    }

};

auto tg_child = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));
    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        if(v_childs[i] == 0)
            continue;
        auto ch = std::rand() % v_childs[i]+1;
            auto p = Tg.child(v_node_pos[i],ch);
    }

};

auto tg_children = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.children(v_node_pos[i]);
    }

};

auto tg_parent = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.parent(v_node_pos[i]);
    }

};


auto tg_leaf_rank = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.leafrank(v_node_pos[i]);
    }

};

auto tg_leaf_num = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.leafnum(v_node_pos[i]);
    }

};



auto tg_leaf_select = [](benchmark::State &st)
{

    uint leafs = Tg.leafnum(Tg.root());

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % leafs + 1;
        auto p = Tg.leafselect(i);
    }

};

auto tg_nsibling = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.nsibling(v_node_pos[i]);
    }

};

auto tg_child_rank= [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.childrank(v_node_pos[i]);
    }

};

auto tg_lchild = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.lchild(v_node_pos[i]);
    }

};

auto tg_preorder = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.pre_order(v_node_pos[i]);
    }

};

auto tg_isancestor = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto j = std::rand() % v_node_pos.size();
        auto p = Tg.is_ancestor(v_node_pos[i],v_node_pos[j]);
    }

};


auto tt_tree_access = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));
    for (auto _ : st)
    {
        auto i = std::rand() % v_trie_node_pos.size() + 1;
        auto p = Tbp_tree[i];
    }

};

auto tt_child = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));
    for (auto _ : st)
    {
        auto i = std::rand() % v_trie_node_pos.size();
        if(v_trie_childs[i] == 0)
            continue;
        auto ch = std::rand() % v_trie_childs[i]+1;
        auto p = Tbp_tree.child(v_trie_node_pos[i],ch);
    }

};

auto tt_children = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_trie_node_pos.size();
        auto p = Tbp_tree.children(v_trie_node_pos[i]);
    }

};

auto tt_parent = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_trie_node_pos.size();
        auto p = Tbp_tree.parent(v_trie_node_pos[i]);
    }

};


auto tt_leaf_rank = [](benchmark::State &st)
{
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_trie_node_pos.size();
        auto p = Tbp_tree.leafrank(v_trie_node_pos[i]);
    }

};

auto tt_leaf_num = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_trie_node_pos.size();
        auto p = Tbp_tree.leafnum(v_trie_node_pos[i]);
    }

};

auto tt_nsibling = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_trie_node_pos.size();
        auto p = Tbp_tree.nsibling(v_trie_node_pos[i]);
    }

};

auto tt_child_rank = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_trie_node_pos.size();
        auto p = Tbp_tree.childrank(v_trie_node_pos[i]);
    }

};

auto tt_lchild = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_trie_node_pos.size();
        auto p = Tbp_tree.lchild(v_trie_node_pos[i]);
    }

};

auto tt_preorder = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_trie_node_pos.size();
        auto p = Tbp_tree.pre_order(v_trie_node_pos[i]);
    }

};

auto tt_levelancestor = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_trie_node_pos.size();
        auto j = std::rand() % v_trie_level[i] + 1;
        auto p = Tbp_tree.levelancestor(i,j-1);
    }

};

auto tg_find_open = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.pre_order(v_node_pos[i]-1);
    }

};

auto tg_pred0 = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.pred0(v_node_pos[i]);
    }

};

auto tg_succ0 = [](benchmark::State &st)
{

    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        auto i = std::rand() % v_node_pos.size();
        auto p = Tg.succ0(v_node_pos[i]);
    }

};


auto wt_xp_rank = [](benchmark::State &st, const auto & wt)
{
    auto n =  wt.size();
    auto sigm = wt.sigma;
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        wt.rank(std::rand()%n,alp[std::rand() % sigm]);
    }

};



auto wt_xp_access = [](benchmark::State &st, const auto & wt)
{
    auto n =  wt.size();
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {
        wt[std::rand()%n];
    }

};


auto wt_xp_select = [](benchmark::State &st, const auto & wt)
{
    auto sigm = wt.sigma;
    std::srand(std::time(nullptr));

    for (auto _ : st)
    {   auto c = alp[std::rand() % sigm];
        if(r_al[c] == 0) continue;
        wt.select(std::rand()%r_al[c]+1,c);
    }

};

int main (int argc, char *argv[] ){

   /* if(argc < 1){
        std::cout<<"bad parameters....";
        return 0;
    }*/


    std::string collection = "../tests/collections/repetitive/reals/einstein.de.txt";//(argv[1]);

    if(collections_code.find(collection) == collections_code.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
        return 0;
    }
    std::string data;
    uint code = collections_code[collection];

    //std::string file_to_save = "../files/"+std::to_string(code)+".cg";
    std::string file_to_save = "../files/"+std::to_string(code)+".cg";
    fstream fstream1(file_to_save, std::ios::in|std::ios::binary);




    if(!fstream1.is_open())
    {
        std::fstream f(collection, std::ios::in| std::ios::binary);
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
        }

        auto *p_G = new grammar();
        p_G->buildRepair(data);
        G.build(*p_G);
        fstream fstream2(file_to_save, std::ios::out|std::ios::binary);
        G.save(fstream2);
        delete p_G;
    }
    else
    {
        G.load(fstream1);
    }



    nrules = G.n_rules()-1;

    const auto & Z = G.get_Z();

    z_size = Z.size();

    Tg = G.get_parser_tree();

    CTrie = G.get_left_trie();

    Tbp_tree = CTrie.get_tree();
    uint tnodes = Tbp_tree.subtree(Tbp_tree.root())+1;

    v_trie_node_pos.resize(tnodes);
    v_trie_childs.resize(tnodes);
    v_trie_level.resize(tnodes);
    for (int l = 0; l < tnodes ; ++l) {
        v_trie_node_pos[l] = Tbp_tree[l+1];
        v_trie_childs[l] = Tbp_tree.children(v_trie_node_pos[l]);
        v_trie_level[l] = Tbp_tree.depth(v_trie_node_pos[l]);
      //  std::cout<<v_trie_level[l]<<"\n";
    }



    v_nocc.resize(nrules);

    for (int j = 0; j < nrules ; ++j) {
        v_nocc[j] = G.n_occ(j+1);
    }

    v_node_pos.resize(z_size);
    v_childs.resize(z_size);



    for (int k = 0; k < z_size; ++k) {
        v_node_pos[k] = Tg[k+1];
        v_childs[k] = Tg.children(v_node_pos[k]);

    }

    const auto & X_p = G.get_Xp();
    alp = G.get_alp();
    r_al.resize(alp.size());
    for (int m = 0; m < alp.size(); ++m) {
        std::cout<<alp[m]<<std::endl;
        r_al[m] = X_p.rank(X_p.size(),alp[m]);
    }


    benchmark::RegisterBenchmark("G offset text operation",offText);
    benchmark::RegisterBenchmark("G is terminal operation",isTerminal);
    benchmark::RegisterBenchmark("G number of occurences operation",n_occ);
    benchmark::RegisterBenchmark("G first occurence operation",first_occ);
    benchmark::RegisterBenchmark("G random occurence operation",occ);
    benchmark::RegisterBenchmark("G operator [] ",_access_to_Z);
    benchmark::RegisterBenchmark("G wt rank ",wt_xp_rank,X_p);
    benchmark::RegisterBenchmark("G wt access ",wt_xp_access,X_p);
    //benchmark::RegisterBenchmark("G wt select ",wt_xp_select,X_p);

    benchmark::RegisterBenchmark("TG operator [] ",tg_tree_access);
    benchmark::RegisterBenchmark("TG preorder ",tg_preorder);
    benchmark::RegisterBenchmark("TG nsibling ",tg_nsibling);
    benchmark::RegisterBenchmark("TG childs ",tg_child);
    benchmark::RegisterBenchmark("TG children ",tg_children);
    benchmark::RegisterBenchmark("TG childrank ",tg_child_rank);
    benchmark::RegisterBenchmark("TG lchild ",tg_lchild);
    benchmark::RegisterBenchmark("TG parent ",tg_parent);
    benchmark::RegisterBenchmark("TG leaf rank ",tg_leaf_rank);
    benchmark::RegisterBenchmark("TG leaf num ",tg_leaf_num);
    benchmark::RegisterBenchmark("TG leaf select ",tg_leaf_select);
    benchmark::RegisterBenchmark("TG is ancestor ",tg_isancestor);
    /*benchmark::RegisterBenchmark("TG find open ",tg_find_open);
    benchmark::RegisterBenchmark("TG pred0 ",tg_pred0);
    benchmark::RegisterBenchmark("TG succ0 ",tg_succ0);*/


    benchmark::RegisterBenchmark("TT operator [] ",tt_tree_access);
    benchmark::RegisterBenchmark("TT preorder ",tt_preorder);
    benchmark::RegisterBenchmark("TT nsibling ",tt_nsibling);
    benchmark::RegisterBenchmark("TT childs ",tt_child);
    benchmark::RegisterBenchmark("TT children ",tt_children);
    benchmark::RegisterBenchmark("TT lchild ",tt_lchild);
    benchmark::RegisterBenchmark("TT parent ",tt_parent);
    benchmark::RegisterBenchmark("TT leaf rank ",tt_leaf_rank);
    benchmark::RegisterBenchmark("TT leaf num ",tt_leaf_num);
    benchmark::RegisterBenchmark("TT levelancestor ",tt_levelancestor);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


    return 0;

}