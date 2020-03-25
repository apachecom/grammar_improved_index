//
// Created by alejandro on 06-06-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include <slp/Samplings/sampling.h>
#include "../SelfGrammarIndexBSQ.h"
#include "../bench/utils.h"
#include "../bench/repetitive_collections.h"

#define MAX_DATA 100000
#define CODE_DATA 5
#define MAX_Q 1000
#define len 10

std::string data;
std::map<size_t, std::string> expansions;
std::vector<size_t> R;
std::vector<size_t> L;
std::vector<std::string> Pt;
SelfGrammarIndexBSQ *idx = new SelfGrammarIndexBSQ();

void generate_random_queries   (const size_t &n, const size_t &max_q) {

    std::srand(std::time(nullptr));

    R.clear();

    L.clear();

    for (int i = 0; i < max_q; ++i) {

        R.push_back(rand() % n + 1);

        L.push_back(rand() % n + 1);

        uint p = rand() % data.size();

        std::string s1; s1.resize(len);

        if( p < data.size()/2){
            std::copy(data.begin()+p,data.begin()+p+len,s1.begin());
        }else
            {
                std::copy(data.begin()+p-len,data.begin()+p,s1.begin());
            }

        Pt.push_back(s1);

    }

}
void get_expansions_rule       () {

    /*LOAD DATA*/
    load_data(MAX_DATA,data,collections_code_inv_dir[CODE_DATA]);
    data.erase(data.begin()+MAX_DATA,data.end());
//    std::cout<<data<<std::endl;

//    data = "panamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabananapanamabanana";

//    /*build temporal grammar*/
//    data += char(0);
    grammar G;
    G.buildRepair(data);
  //  fstream f("../files/new_divided_indices/bal/"+std::to_string(CODE_DATA)+"bal_repair_g",std::ios::in|std::ios::binary);
//
//    G.load(f);
//
//    G.print(data);
    expansions.clear();
    for (size_t i = 1; i <= G.n_rules(); ++i) {
        std::string tt;
        tt.resize(G[i].r - G[i].l + 1);
        std::copy(data.begin() + G[i].l, data.begin() + G[i].r + 1, tt.begin());
        expansions[i] = tt;
        if(i < 10)
            std::cout<<tt<<std::endl;
    }

    generate_random_queries(1, MAX_Q);
//
    std::cout << "TEST DATA GENERATED\n";
}
static void t_load_data          (benchmark::State &state){


    for (auto _ : state) {
        get_expansions_rule();
        sleep(3);
        //idx->build(data);
    }
}
static void t_load_idx           (benchmark::State &state) {

    for (auto _ : state) {
        {

            std::fstream f("testIdx2",std::ios::out|std::ios::binary);
            std::ofstream fG("testIdxFG2");
            std::ofstream fGR("testIdxFGR2");
            std::ofstream fGS("testIdxFGS2");

//        idx.load(f);
            idx->build(data,16);
            idx->buildSamplings(16);

            idx->save(f);
            idx->saveSampling(fG,fGR,fGS);



        }
        {

            std::fstream f("testIdx2",std::ios::in|std::ios::binary);
            std::ifstream fG("testIdxFG2");
            std::ifstream fGR("testIdxFGR2");
            std::ifstream fGS("testIdxFGS2");

            idx->load(f);
            idx->loadSampling(fG,fGR,fGS);
        }
//        idx->printSampling();
        sleep(3);

    }

}
static void print_samplings(benchmark::State &state){

    for (auto _ : state) {


//        std::cout<<"Suffixes: "<<std::endl;
//        for (int i = 1; i <= n_sfx ; ++i) {
//            uchar *s = new uchar[100];
//            idx->sequenceSamp->extract(i,s);
//            std::cout<<i<<" "<<s<<std::endl;
//        }
//
//        std::cout<<"Grammar: "<<std::endl;
//        for (int i = 1; i < n_g ; ++i) {
//            uchar *s = new uchar[100];
//            idx->reverseSamp->extract(idx->pi[i],s);
//            std::cout<<i<<" "<<s<<std::endl;
//        }
//
//
//        std::cout<<"Grammar rev: "<<std::endl;
//        for (int i = 1; i < n_g ; ++i) {
//            uchar *s = new uchar[100];
//            idx->grammarSamp->extract(i,s);
//            std::cout<<i<<" "<<s<<std::endl;
//        }

        std::cout<<"total size "                 <<idx->size_in_bytes()<<std::endl;
        std::cout<<"size basics with tries "     <<idx->get_grammar().size_in_bytes()+idx->get_grid().size_in_bytes()<<std::endl;
        std::cout<<"size basics without tries "  <<idx->size_in_bytes() - (idx->get_grammar().left_path.size_in_bytes()+idx->get_grammar().right_path.size_in_bytes())<<std::endl;


        std::cout<<"size grid "                  <<idx->get_grid().size_in_bytes()<<std::endl;
        std::cout<<"size compressed grammar "    <<idx->get_grammar().size_in_bytes()<<std::endl;
        std::cout<<"size tries "                 <<idx->get_grammar().left_path.size_in_bytes()+idx->get_grammar().right_path.size_in_bytes()<<std::endl;

        std::cout<<"size rev grammar sampling "  <<idx->grammarSamp->getSize()<<std::endl;
        std::cout<<"size grammar  sampling "     <<idx->reverseSamp->getSize()<<std::endl;
        std::cout<<"size sfx sampling "          <<idx->sequenceSamp->getSize()<<std::endl;

        sleep(3);

    }

    state.counters["size"] = idx->size_in_bytes();
    state.counters["size basics with tries"] = idx->get_grammar().size_in_bytes()+idx->get_grid().size_in_bytes();
    state.counters["size basics without tries"] = idx->size_in_bytes() - (idx->get_grammar().left_path.size_in_bytes()+idx->get_grammar().right_path.size_in_bytes());

    state.counters["size grid"] = idx->get_grid().size_in_bytes();

    state.counters["size compressed grammar"] = idx->get_grammar().size_in_bytes();

    state.counters["size tries"] = idx->get_grammar().left_path.size_in_bytes()+idx->get_grammar().right_path.size_in_bytes();

    state.counters["size rev grammar sampling"] = idx->grammarSamp->getSize();
    state.counters["size grammar  sampling"] = idx->reverseSamp->getSize();
    state.counters["size sfx sampling"] = idx->sequenceSamp->getSize();
}
static void compare_rule_suffix(benchmark::State &state) {
    std::srand(std::time(nullptr));
    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s1 = expansions[R[ii]];
            std::string s2 = expansions[R[ii]];
            std::reverse(s2.begin(),s2.end());
            auto iter  = s2.begin();
            auto iend  = s2.end();
            int r1 = idx->dfs_cmp_suffix(R[ii],iter,iend);
            int r2 = std::strcmp(s2.c_str(),s1.c_str()) ;
            if(r2 > 0) r2 = 1;
            if(r2 < 0 ) r2 = -1;

            ASSERT_EQ(r1, r2);
        }
    }
}

static void expand_rule_suffix (benchmark::State &state) {
    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s2 = expansions[R[ii]];
            uint r = s2.size();
            std::reverse(s2.begin(),s2.end());
            std::string sfx;
            std::string sfx_r;
            sfx_r.resize(r);
            std::copy(s2.begin(),s2.begin()+r,sfx_r.begin());
            sfx.resize(r);
            size_t pos = 0;
            idx->dfs_expand_suffix2(R[ii],sfx,r,pos);
            if(sfx != sfx_r){
                pos = 0;
                sfx.clear();
                sfx.resize(r);
                idx->dfs_expand_suffix2(R[ii],sfx,r,pos);
            }
            ASSERT_EQ(sfx, sfx_r);
        }
    }

}
static void expand_rule_prefix (benchmark::State &state) {

    uint64_t r = state.range(0);
    uint maxpos = 0;

    for (size_t ii = 0; ii < R.size(); ++ii) {
        if (expansions[R[ii]].size() > expansions[R[maxpos]].size())
            maxpos = ii;
    }

    std::cout<<maxpos<<std::endl;
    std::cout<<R[maxpos]<<std::endl;
    std::cout<<expansions[R[maxpos]]<<std::endl;

    std::string s2 = expansions[R[maxpos]];

    std::string sfx;
    std::string sfx_r;
    sfx_r.resize(r);
    std::copy(s2.begin(),s2.begin()+r,sfx_r.begin());
    sfx.resize(r);
    size_t pos = 0;

    for (auto _ : state) {

            idx->dfs_expand_prefix2(R[maxpos],sfx,r,pos);

//            if(sfx != sfx_r){
//                    pos = 0;
//                    sfx.clear();
//                    sfx.resize(r);
//                    idx->dfs_expand_prefix2(R[maxpos],sfx,r,pos);
//                }
//                ASSERT_EQ(sfx, sfx_r);
    }

}
static void prefix_search      (benchmark::State &state) {

    for (auto _ : state) {
        for (size_t ii = 0; ii < R.size(); ++ii) {

            std::string s1 = expansions[R[ii]];
            std::reverse(s1.begin(),s1.end());
            char* ss = new char[4];
            std::copy(s1.begin(),s1.begin()+4,ss);
            ss[4] = '\0';

            uint left = 1;
            uint right = idx->n_rules();
            size_t first = idx->c_rank1(uchar(ss[0]))+1;

            uint lb = idx->grammarSamp->leftBlockForTerminal(first);
            uint rb = idx->grammarSamp->rightBlockForTerminal(first);

            idx->grammarSamp->locatePrefix((uchar*)ss,strlen(ss),&left,&right,&lb,&rb);
//            std::cout<<"string: "+s1<<" ["<<left<<","<<right<<","<<lb<<","<<rb<<"]"<<std::endl;

        }
    }
}
static void cmp_rule_suffix    (benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s2 = expansions[R[ii]];
            std::string s1 = expansions[L[ii]];

            auto itera = s1.end()-1;
            auto end = s1.begin()-1;
            int r = idx->dfs_cmp_suffix_q(R[ii],itera,end);

            itera = s1.end()-1;
            end = s1.begin()-1;

            int r2 = idx->bp_cmp_suffix(R[ii],itera,end);
            if(r2 == 0 && itera != end) r2 = 1;

            if(r != r2){
                itera = s1.end()-1;
                end = s1.begin()-1;

                r = idx->dfs_cmp_suffix_q(R[ii],itera,end);
                itera = s1.end()-1;
                end = s1.begin()-1;

                r2 = idx->bp_cmp_suffix(R[ii],itera,end);
            }


            ASSERT_EQ(r, r2);
        }
    }
}
static void cmp_rule_prefix    (benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s2 = expansions[R[ii]];
            std::string s1 = expansions[L[ii]];

            auto itera = s1.begin();
            auto end = s1.end();

            int r = idx->dfs_cmp_prefix_q(R[ii],itera,end);

            uint ml = s1.size() > s2.size() ? s2.size():s1.size();

            std::string sc1,sc2;
            sc1.resize(ml);sc2.resize(ml);

            std::copy(s1.begin(),s1.begin()+ml,sc1.begin());
            std::copy(s2.begin(),s2.begin()+ml,sc2.begin());

//            std::cout<<"s1: "+sc1<<std::endl;
//            std::cout<<"s2: "+sc2<<std::endl;

            int r_r = strcmp(sc1.c_str(),sc2.c_str());

            if(r_r == 0 && s1.size() > s2.size())
                r_r = 1;
            else
                if(r_r == 0 && s1.size() < s2.size())
                    r_r = -1;
                else
                    r_r = r_r > 0?1:(r_r<0?-1:0);



            if(r_r != r){

                std::cout<<"s1: "+sc1<<std::endl;
                std::cout<<"s2: "+sc2<<std::endl;

                std::string s12 = expansions[L[ii]];

                auto itera2 = s12.begin();
                auto end2   = s12.end()  ;

                r = idx->dfs_cmp_prefix_q(R[ii],itera2,end2);

            }
            ASSERT_EQ(r, r_r);
        }
    }
}
static void match_prefix(benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s2 = expansions[R[ii]];
            std::string s1 = expansions[L[ii]];

            auto itera = s1.begin();
            auto end = s1.end();

            int r_r = 0, r = idx->match_prefix(R[ii],itera,end);

            uint p = s2.find(s1.c_str());

            if(p != 0){
                r_r = (s1 > s2)?1:-1;
            }else{
                r_r = s1.size() > s2.size()?1:0;
            }

            ASSERT_EQ(r, r_r);
        }
    }
}
static void match_suffix(benchmark::State &state){

    for (auto _ : state) {

        for (size_t ii = 0; ii < R.size(); ++ii)
        {
            std::string s2 = expansions[R[ii]];
            std::string s1 = expansions[L[ii]];

            auto itera = s1.end()-1;
            auto end = s1.begin()-1;
            int r_r = 0, r = idx->match_suffix(R[ii],itera,end);

            std::reverse(s1.begin(),s1.end());
            std::reverse(s2.begin(),s2.end());

            uint p = s2.find(s1.c_str());

            if(p != 0){
                r_r = (s1 > s2)?1:-1;
            }else{
                r_r = s1.size() > s2.size()?1:0;
            }


            ASSERT_EQ(r, r_r);
        }
    }

}
static void bs_locate             (benchmark::State &state){

    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint j = 0; j < Pt.size() ; j++)
        {
            std::vector<uint> X;
//            std::string ss = "dabra";
            idx->locate(Pt[j], X);

            std::sort(X.begin(), X.end());
            auto last = std::unique(X.begin(), X.end());
            X.erase(last, X.end());

            occ += X.size();
        }

    }
    state.counters["occ"] = occ;
    state.counters["space"] = idx->size_in_bytes();
    state.counters["data_len"] = idx->get_grammar().L.size();
    state.counters["bps"] = idx->size_in_bytes()*8.0/idx->get_grammar().L.size();
    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    state.counters["tries"] = lt+rt;
    //std::cout<<lt+rt<<std::endl;
    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();

}
static void bs_qgram_locate             (benchmark::State &state){

    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint j = 0; j < Pt.size() ; j++)
        {
            std::vector<uint> X;
//            std::string ss = "dabra";
            idx->qgram_locate(Pt[j], X);

            if(X.size() == 0) {
//                std::cout<<Pt[j]<<std::endl;
                idx->qgram_locate(Pt[j], X);
            }

            std::sort(X.begin(), X.end());
            auto last = std::unique(X.begin(), X.end());
            X.erase(last, X.end());

            occ += X.size();



        }

    }
    state.counters["occ"] = occ;
    state.counters["space"] = idx->size_in_bytes();
    state.counters["data_len"] = idx->get_grammar().L.size();
    state.counters["bps"] = idx->size_in_bytes()*8.0/idx->get_grammar().L.size();
    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    state.counters["tries"] = lt+rt;
    //std::cout<<lt+rt<<std::endl;
    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();

}
static void bs_qgram_prefix_search_locate             (benchmark::State &state){
    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint j = 0; j < Pt.size() ; j++)
        {
            std::vector<uint> X;
//
//            std::string sss = "dabra";
//            std::cout<<Pt[j]<<","<<X.size()<<std::endl;
            idx->qgram_locate_prefix_search(Pt[j], X);


            std::sort(X.begin(), X.end());
            auto last = std::unique(X.begin(), X.end());
            X.erase(last, X.end());
            occ += X.size();
        }
        sleep(3);
    }
    state.counters["occ"] = occ;
    state.counters["space"] = idx->size_in_bytes();
    state.counters["data_len"] = idx->get_grammar().L.size();
    state.counters["bps"] = idx->size_in_bytes()*8.0/idx->get_grammar().L.size();
    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    state.counters["tries"] = lt+rt;
    //std::cout<<lt+rt<<std::endl;
    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();

}
static void bs_qgram_prefix_search_trie_locate(benchmark::State &state){
    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint j = 0; j < Pt.size() ; j++)
        {
            std::vector<uint> X;
//            std::string ss = "dabra";
            idx->qgram_trie_locate(Pt[j], X);

            std::sort(X.begin(), X.end());
            auto last = std::unique(X.begin(), X.end());
            X.erase(last, X.end());

            occ += X.size();
        }

    }
    state.counters["occ"] = occ;
    state.counters["space"] = idx->size_in_bytes();
    state.counters["data_len"] = idx->get_grammar().L.size();
    state.counters["bps"] = idx->size_in_bytes()*8.0/idx->get_grammar().L.size();
    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    state.counters["tries"] = lt+rt;
    //std::cout<<lt+rt<<std::endl;
    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();

}
static void bs_qgram_prefix_search_dfs_locate(benchmark::State &state){
    size_t occ;

    for (auto _ : state) {

        occ = 0;

        for (uint j = 0; j < Pt.size() ; j++)
        {
            std::vector<uint> X;
//            std::string ss = "dabra";
            idx->qgram_dfs_locate(Pt[j], X);

            std::sort(X.begin(), X.end());
            auto last = std::unique(X.begin(), X.end());
            X.erase(last, X.end());


            occ += X.size();
        }

    }
    state.counters["occ"] = occ;
    state.counters["space"] = idx->size_in_bytes();
    state.counters["data_len"] = idx->get_grammar().L.size();
    state.counters["bps"] = idx->size_in_bytes()*8.0/idx->get_grammar().L.size();
    float lt = idx->get_grammar().left_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    float rt = idx->get_grammar().right_path.size_in_bytes()*8.0/idx->get_grammar().L.size();
    state.counters["tries"] = lt+rt;
    //std::cout<<lt+rt<<std::endl;
    state.counters["tries bytes"] = idx->get_grammar().left_path.size_in_bytes() + idx->get_grammar().right_path.size_in_bytes();

}
static void bt_locate          (benchmark::State &state){
    size_t nocc = 0;
    double mean =0;

    for (auto _ : state)
    {
        nocc = 0;
        for (auto &&  i : Pt )
        {
            uint occs = 0;
            size_t pos = data.find(i, 0);
            while(pos != string::npos)
            {
                ++nocc;
                ++occs;
                pos = data.find(i,pos+1);
            }

//            std::cout<<i<<","<<occs<<std::endl;
        }

        sleep(3);

    }


    //b_f.close();
    state.counters["occ"] = nocc;
    state.counters["space"] = 8*(data.size());
    state.counters["data_len"] = data.length();
    state.counters["bps"] = 8;
    state.counters["tries"] = 0;
    state.counters["tries bytes"] = 0;




}


static void g_index_rank_phrases_qgram_dfs(benchmark::State &state){

    std::srand(std::time(nullptr));

    for (auto _ : state) {

        std::string ss = "",s="";
        uint i = rand()%data.size();
        uint j = rand()%data.size();

        if(i > j) swap(i,j);

        if(i == j)continue;

        uint ll = j-i;
        s.resize(ll);
        ss.resize(ll);

        std::copy(data.begin()+i,data.begin()+j,ss.begin());
        idx->display_qgram_rec(i,j-1,s);
        if(ss!=s){
            s= "";s.resize(ll);
            idx->display_qgram_rec(i,j-1,s);
        }
        ASSERT_EQ(ss,s);
    }


}

static void g_index_rank_phrases_qgram_dfs_len(benchmark::State &state){

    uint64_t _m = state.range(0);

    std::srand(std::time(nullptr));

    for (auto _ : state) {

        std::string ss = "",s="";
        uint i = rand()%data.size();
        uint j =  i+_m;

        if(j > data.size()){
            j  = i;
            i  = i - _m;
        }

        if(i == j)continue;

        uint ll = j-i;
        s.resize(ll);
        ss.resize(ll);

        std::copy(data.begin()+i,data.begin()+j,ss.begin());
        idx->display_qgram_rec(i,j-1,s);
        if(ss!=s){
            s= "";s.resize(ll);
            idx->display_qgram_rec(i,j-1,s);
        }
        ASSERT_EQ(ss,s);
    }


}






BENCHMARK(t_load_data)->Unit(benchmark::kMicrosecond);
BENCHMARK(t_load_idx)->Unit(benchmark::kMicrosecond);
//BENCHMARK(print_samplings)->Unit(benchmark::kMicrosecond);

//BENCHMARK(prefix_search)->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_rule_prefix)->Args({5})->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_rule_prefix)->Args({15})->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_rule_prefix)->Args({25})->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_rule_prefix)->Args({35})->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_rule_prefix)->Args({45})->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_rule_prefix)->Args({55})->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_rule_prefix)->Args({65})->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_rule_prefix)->Args({75})->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_rule_prefix)->Args({85})->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_rule_prefix)->Args({95})->Unit(benchmark::kMicrosecond);
BENCHMARK(expand_rule_prefix)->Args({105})->Unit(benchmark::kMicrosecond);
//BENCHMARK(expand_rule_suffix)->Unit(benchmark::kMicrosecond);
//BENCHMARK(cmp_rule_prefix)->Unit(benchmark::kMicrosecond);
//BENCHMARK(cmp_rule_suffix)->Unit(benchmark::kMicrosecond);
//BENCHMARK(match_prefix)->Unit(benchmark::kMicrosecond);
//BENCHMARK(match_suffix)->Unit(benchmark::kMicrosecond);
//BENCHMARK(bs_locate)->Unit(benchmark::kMicrosecond);
//BENCHMARK(bs_qgram_locate)->Unit(benchmark::kMicrosecond);
//BENCHMARK(bs_qgram_prefix_search_locate)->Unit(benchmark::kMicrosecond);
//BENCHMARK(bs_qgram_prefix_search_trie_locate)->Unit(benchmark::kMicrosecond);
//BENCHMARK(bs_qgram_prefix_search_dfs_locate)->Unit(benchmark::kMicrosecond);
//BENCHMARK(bt_locate)->Unit(benchmark::kMicrosecond);
//BENCHMARK(g_index_rank_phrases_qgram_dfs)->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({5})->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({10})->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({20})->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({30})->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({40})->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({50})->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({60})->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({70})->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({80})->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({90})->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({100})->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({110})->Unit(benchmark::kMicrosecond);
BENCHMARK(g_index_rank_phrases_qgram_dfs_len)->Args({120})->Unit(benchmark::kMicrosecond);


BENCHMARK_MAIN();
