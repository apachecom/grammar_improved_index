//
// Created by root on 12-06-19.
//


#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "../SelfGrammarIndexBSQ.h"

#include "../bench/utils.h"
#include "../bench/repetitive_collections.h"

#define MAX_DATA 1e19
#define  MAX_Q 1000
#define MAX_OCC 1e7


#define G_INDEX_BINARY_SEARCH_TRIE 5
#define G_INDEX_BINARY_SEARCH_NOTRIE 6
#define G_INDEX_PATRICIA_TRIE 7
#define G_INDEX_PATRICIA_NOTRIE 8
#define G_INDEX_QGRAM_CMP_SMP 9
#define G_INDEX_QGRAM_CMP_TRIE 10
#define G_INDEX_QGRAM_CMP_DFS 11

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

int len_q = 0;
int _code = 0;

SelfGrammarIndexBSQ idx;

SelfGrammarIndex* pidx;
std::string data = "";

std::map< int,std::vector<std::string> > R;

void generate_random_queries(const size_t &max_q, const int& l, const int& c) {

    std::srand(std::time(nullptr));

    ////////////////reading patterns/////////////
    R[len_q].clear();

    std::string pattern_file = "../files/patrones/" + std::to_string(c)+"_len_200.ptt";
//    std::cout<<"patterns: "<<pattern_file<<std::endl;
//    std::cout<<"collection"<<c<<std::endl;
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);
    if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
    }
    std::string buff; uint i = 0;
    std::string N = "";
    for (int j = 0; j < len_q; ++j) {
        N+='N';
    }
    while (!f.eof() && std::getline(f, buff)) {
        std::string sss;
        sss.resize(len_q);
        std::copy(buff.begin(),buff.begin()+len_q,sss.begin());
//        std::cout<<i<<"\t"<<sss<<std::endl;

        if(sss == N)continue;

        for (int k = 0; k < sss.size(); ++k){
            if(int(sss[k]) == 0 || int(sss[k]) == 1 || int(sss[k]) == 2 ){
                sss[k] = 65;
                //return 0;
            }
        }



        R[len_q].push_back( sss );
        i++;
        if(i > max_q) break;
    }

    std::cout<<R[len_q][i-1]<<std::endl;

    f.close();



}
auto build_idx = [](benchmark::State &state,const uint& coll, const uint & len, const uint& smp) {
////////////0123456789012345

    len_q = len;
    _code  = coll;


    load_data(MAX_DATA,data,collections_code_inv_dir[_code]);

    for (auto _ : state) {
        sleep(4);
    }

    generate_random_queries(MAX_Q,len_q,_code);

    state.counters["1 n_ptt"]     = 0;
    state.counters["2 n_occ"]     = 0;
    state.counters["3 ptt_len"]   = len_q;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = _code;
    state.counters["6 size"]      = 0;
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = 0;
    state.counters["9 valid"]     = 0;

};

auto bs_no_trie = [](benchmark::State &state,const uint& coll, const uint & len, const uint& smp) {

    int code = coll;
    int s    = smp;

    pidx=load_idx(1,0,1,1,2,code);
//    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
//    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);
//
//    idx.load(f);
//    idx.loadSampling(fG,fGR, fGS);

    size_t occ;
    uint ptt;
    for (auto _ : state) {

        occ = 0, ptt = 0;

        for (  auto &ii:R[len] ) {
            std::vector<uint> X;
            pidx->locateNoTrie(ii,X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            ptt++;
            if(occ > MAX_OCC)break;
        }
    }


    size_t size_indx = pidx->get_grid().size_in_bytes()+(pidx->get_grammar().size_in_bytes()- pidx->get_grammar().right_path.size_in_bytes() - pidx->get_grammar().left_path.size_in_bytes());

    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = occ;
    state.counters["3 ptt_len"]   = len;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = G_INDEX_BINARY_SEARCH_NOTRIE;
    state.counters["9 valid"]     = 1;
    delete pidx;
};
auto bs_trie = [](benchmark::State &state,const uint& coll, const uint & len, const uint& smp) {

    int code = coll;
    int s    = smp;

    pidx=load_idx(1,0,1,1,1,code);
//    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
//    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);
//
//    idx.load(f);
//    idx.loadSampling(fG,fGR, fGS);

    size_t occ,ptt;

    for (auto _ : state) {

        occ = 0, ptt = 0;

        for (  auto &ii:R[len] ) {
            std::vector<uint> X;
            pidx->locate(ii,X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            ++ptt;
            if(occ > MAX_OCC)break;
        }
    }


    size_t size_indx = pidx->get_grid().size_in_bytes()+pidx->get_grammar().size_in_bytes();

    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = occ;
    state.counters["3 ptt_len"]   = len;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = G_INDEX_BINARY_SEARCH_TRIE;
    state.counters["9 valid"]     = 1;

    delete pidx;
};
auto pts_no_trie = [](benchmark::State &state, const uint& coll, const uint & len, const uint& smp) {

    int code = coll;
    int s    = smp;

    pidx=load_idx(3,s,1,1,2,code);
//    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
//    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);
//
//    idx.load(f);
//    idx.loadSampling(fG,fGR, fGS);

    size_t occ,ptt;

    for (auto _ : state) {

        occ = 0,ptt =0 ;

        for (  auto &ii:R[len] ) {
            std::vector<uint> X;
            pidx->locateNoTrie(ii,X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            ++ptt;
            if(occ > MAX_OCC)break;
        }
    }


    size_t size_indx = pidx->get_grid().size_in_bytes()+(pidx->get_grammar().size_in_bytes()- pidx->get_grammar().right_path.size_in_bytes() - pidx->get_grammar().left_path.size_in_bytes())
                       + pidx->get_pt_rules().size_in_bytes()+pidx->get_pt_suffixes().size_in_bytes();

    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = occ;
    state.counters["3 ptt_len"]   = len;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = s;
    state.counters["8 name"]      = G_INDEX_PATRICIA_NOTRIE;
    state.counters["9 valid"]     = 1;
    delete pidx;

};
auto pts_trie = [](benchmark::State &state,const uint& coll, const uint & len, const uint& smp) {

    int code = coll;
    int s    = smp;

    pidx=load_idx(3,s,1,1,1,code);
//    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
//    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
//    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);
//
//    idx.load(f);
//    idx.loadSampling(fG,fGR, fGS);

    size_t occ,ptt;

    for (auto _ : state) {

        occ = 0, ptt = 0;

        for (  auto &ii:R[len] ) {
            std::vector<uint> X;
            pidx->locate(ii,X);
            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());
            occ += X.size();
            ++ptt;
            if(occ > MAX_OCC)break;
        }
    }


    size_t size_indx = pidx->get_grid().size_in_bytes()+pidx->get_grammar().size_in_bytes()
                       + pidx->get_pt_rules().size_in_bytes()+pidx->get_pt_suffixes().size_in_bytes();

    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = occ;
    state.counters["3 ptt_len"]   = len;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = s;
    state.counters["8 name"]      = G_INDEX_PATRICIA_TRIE;
    state.counters["9 valid"]     = 1;

    delete pidx;


};
auto qgram_nosmp_no_trie = [](benchmark::State &state,const uint& coll, const uint & len, const uint& smp) {

    int code = coll;
    int s    = smp;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR,fGS);

    size_t occ,ptt;

    for (auto _ : state) {

        occ = 0,ptt = 0;

        for (  auto &ii:R[len] ) {
            std::vector<uint> X;

            idx.locateNoTrie(ii,X);

            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());

            occ += X.size();
            ++ptt;

            if(occ > MAX_OCC)break;
        }
    }

    size_t size_indx = idx.get_grid().size_in_bytes()+(idx.get_grammar().size_in_bytes()- idx.get_grammar().right_path.size_in_bytes() - idx.get_grammar().left_path.size_in_bytes());
};
auto qgram_nosmp_trie = [](benchmark::State &state, const uint& coll, const uint & len, const uint& smp){

    int code = coll;
    int s    = smp;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR,fGS);

    size_t occ,ptt;

    for (auto _ : state) {

        occ = 0,ptt = 0;

        for (  auto &ii:R[len] ) {
            std::vector<uint> X;

            idx.locate(ii,X);

            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());

            occ += X.size();
            ++ptt;
            if(occ > MAX_OCC)break;
        }
    }

    size_t size_indx = idx.get_grid().size_in_bytes() + idx.get_grammar().size_in_bytes();

    state.counters["occ"] = occ;
    state.counters["space"] = size_indx;
    state.counters["data_len"] = idx.get_grammar().L.size();
    state.counters["bps"] = size_indx*8.0/idx.get_grammar().L.size();
    state.counters["ptt"] = ptt;

};
auto qgram_smp = [](benchmark::State &state, const uint& coll, const uint & len, const uint& smp){

    int code = coll;
    int s    = smp;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR,fGS);

    size_t occ,ptt;

    for (auto _ : state) {

        occ = 0, ptt = 0;

        for (  auto &ii:R[len] ) {
            std::vector<uint> X;

            idx.qgram_locate(ii,X);

            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());

            occ += X.size();
            ++ptt;
            if(occ > MAX_OCC)break;
        }
    }


    size_t size_indx = idx.get_grid().size_in_bytes()+(idx.get_grammar().size_in_bytes()- idx.get_grammar().right_path.size_in_bytes() - idx.get_grammar().left_path.size_in_bytes())
                       + idx.grammarSamp->getSize() + + idx.reverseSamp->getSize();


    state.counters["occ"] = occ;
    state.counters["space"] = size_indx;
    state.counters["data_len"] = idx.get_grammar().L.size();
    state.counters["bps"] = size_indx*8.0/idx.get_grammar().L.size();
    state.counters["ptt"] = ptt;

    state.counters["ptt"] = R[len].size();


};
auto qgram_smp_rules_search = [](benchmark::State &state, const uint& coll, const uint & len, const uint& smp){

    int code = coll;
    int s    = smp;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR,fGS);

    size_t occ,ptt;

    for (auto _ : state) {

        occ = 0, ptt = 0;

        for (  auto &ii:R[len] ) {
            std::vector<uint> X;

            idx.qgram_locate_prefix_search(ii,X);

            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());

            occ += X.size();
            ++ptt;
            if(occ > MAX_OCC)break;
        }
    }

    size_t size_indx = idx.get_grid().size_in_bytes()+(idx.get_grammar().size_in_bytes()- idx.get_grammar().right_path.size_in_bytes() - idx.get_grammar().left_path.size_in_bytes())
                       + idx.grammarSamp->getSize() + + idx.reverseSamp->getSize() + idx.sequenceSamp->getSize();


    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = occ;
    state.counters["3 ptt_len"]   = len;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = s;
    state.counters["8 name"]      = G_INDEX_QGRAM_CMP_SMP;
    state.counters["9 valid"]     = 1;

};
auto qgram_smp_rules_sfx_search_trie_locate = [](benchmark::State &state,const uint& coll, const uint & len, const uint& smp){

    int code = coll;
    int s    = smp;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR,fGS);

    size_t occ, ptt;

    for (auto _ : state) {

        occ = 0, ptt = 0;

        for (  auto &ii:R[len] ) {
            std::vector<uint> X;

            idx.qgram_trie_locate(ii,X);

            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());

            occ += X.size();
            ++ptt;
            if(occ > MAX_OCC)break;
        }
    }

    size_t size_indx = idx.get_grid().size_in_bytes()+ idx.get_grammar().size_in_bytes()
                       + idx.grammarSamp->getSize() + + idx.reverseSamp->getSize() + idx.sequenceSamp->getSize();


    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = occ;
    state.counters["3 ptt_len"]   = len;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = s;
    state.counters["8 name"]      = G_INDEX_QGRAM_CMP_TRIE;
    state.counters["9 valid"]     = 1;

};
auto qgram_smp_rules_sfx_search_dfs_locate = [](benchmark::State &state,const uint& coll, const uint & len, const uint& smp)    {

    int code = coll;
    int s    = smp;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    std::ifstream fG (read_path+std::to_string(code)+".qgramSampG_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGR(read_path+std::to_string(code)+".qgramSampGR_"+std::to_string(s),std::ios::in|std::ios::binary);
    std::ifstream fGS(read_path+std::to_string(code)+".qgramSampGS_"+std::to_string(s),std::ios::in|std::ios::binary);

    idx.load(f);
    idx.loadSampling(fG,fGR,fGS);

    size_t occ, ptt;

    for (auto _ : state) {

        occ = 0, ptt = 0;

        for (  auto &ii:R[len] ) {
            std::vector<uint> X;

            idx.qgram_dfs_locate(ii,X);

            std::sort(X.begin(),X.end());
            auto last = std::unique(X.begin(),X.end());
            X.erase(last,X.end());

            occ += X.size();
            ++ptt;
            if(occ > MAX_OCC)break;
        }
    }

    size_t size_indx = idx.get_grid().size_in_bytes()+(idx.get_grammar().size_in_bytes()- idx.get_grammar().right_path.size_in_bytes() - idx.get_grammar().left_path.size_in_bytes())
                       + idx.grammarSamp->getSize() + + idx.reverseSamp->getSize() + idx.sequenceSamp->getSize();


    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = occ;
    state.counters["3 ptt_len"]   = len;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = s;
    state.counters["8 name"]      = G_INDEX_QGRAM_CMP_DFS;
    state.counters["9 valid"]     = 1;
};
auto bt_locate = [](benchmark::State &state, const uint& coll, const uint & len, const uint& smp){

    size_t nocc = 0;
    double mean =0;

    for (auto _ : state)
    {
        nocc = 0;
        for (auto &&  i : R[len] )
        {
            uint occs = 0;
//            auto start = timer::now();

            size_t pos = data.find(i, 0);
            while(pos != string::npos)
            {
                ++nocc;
                ++occs;
                pos = data.find(i,pos+1);
            }
            if(nocc > MAX_OCC)break;
        }

    }


    //b_f.close();
    state.counters["occ"] = nocc;
    state.counters["space"] = 8*(data.size());
    state.counters["data_len"] = data.length();
    state.counters["bps"] = 8;
    state.counters["tries"] = 0;
    state.counters["tries bytes"] = 0;
    state.counters["ptt"] = R[len].size();


};


int main (int argc, char *argv[] ){


    if(argc < 2){
        std::cout<<"bad parameters....";
        return 0;
    }


    uint collection = atoi(argv[1]);
    if(collections_code_inv.find(collection) == collections_code_inv.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
        return 0;
    }
    uint pattern_len[] = {5,10,20,30,40,50};

    for (int i = 0; i < 6 ; ++i) {

        uint len = pattern_len[i];


        benchmark::RegisterBenchmark("LOAD-RANGES"                        ,build_idx,                                     collection,len,4)->Unit({benchmark::kMicrosecond});

        benchmark::RegisterBenchmark("G-INDEX-BINARY_SEARCH-NOTRIE"       ,bs_no_trie,                                    collection,len,0)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-BINARY_SEARCH-TRIE"         ,bs_trie,                                       collection,len,0)->Unit({benchmark::kMicrosecond});

        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<4>-NOTRIE"    ,pts_no_trie,                                   collection,len,4)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<8>-NOTRIE"    ,pts_no_trie,                                   collection,len,8)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<16>-NOTRIE"   ,pts_no_trie,                                   collection,len,16)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<32>-NOTRIE"   ,pts_no_trie,                                   collection,len,32)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<64>-NOTRIE"   ,pts_no_trie,                                   collection,len,64)->Unit({benchmark::kMicrosecond});

        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<4>-TRIE"      ,pts_trie,                                      collection,len,4)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<8>-TRIE"      ,pts_trie,                                      collection,len,8)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<16>-TRIE"     ,pts_trie,                                      collection,len,16)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<32>-TRIE"     ,pts_trie,                                      collection,len,32)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-PATRICIA_TREE<64>-TRIE"     ,pts_trie,                                      collection,len,64)->Unit({benchmark::kMicrosecond});

        benchmark::RegisterBenchmark("G-INDEX-QGRAM<2>-CMP_SMP"           ,qgram_smp_rules_search,                        collection,len,2)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<4>-CMP_SMP"           ,qgram_smp_rules_search,                        collection,len,4)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<6>-CMP_SMP"           ,qgram_smp_rules_search,                        collection,len,6)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<8>-CMP_SMP"           ,qgram_smp_rules_search,                        collection,len,8)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<10>-CMP_SMP"          ,qgram_smp_rules_search,                        collection,len,10)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<12>-CMP_SMP"          ,qgram_smp_rules_search,                        collection,len,12)->Unit({benchmark::kMicrosecond});

        benchmark::RegisterBenchmark("G-INDEX-QGRAM<2>-CMP_TRIE"          ,qgram_smp_rules_sfx_search_trie_locate,        collection,len,2)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<4>-CMP_TRIE"          ,qgram_smp_rules_sfx_search_trie_locate,        collection,len,4)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<6>-CMP_TRIE"          ,qgram_smp_rules_sfx_search_trie_locate,        collection,len,6)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<8>-CMP_TRIE"          ,qgram_smp_rules_sfx_search_trie_locate,        collection,len,8)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<10>-CMP_TRIE"         ,qgram_smp_rules_sfx_search_trie_locate,        collection,len,10)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<12>-CMP_TRIE"         ,qgram_smp_rules_sfx_search_trie_locate,        collection,len,12)->Unit({benchmark::kMicrosecond});

        benchmark::RegisterBenchmark("G-INDEX-QGRAM<2>-CMP_DFS"          ,qgram_smp_rules_sfx_search_dfs_locate,        collection,len,2)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<4>-CMP_DFS"          ,qgram_smp_rules_sfx_search_dfs_locate,        collection,len,4)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<6>-CMP_DFS"          ,qgram_smp_rules_sfx_search_dfs_locate,        collection,len,6)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<8>-CMP_DFS"          ,qgram_smp_rules_sfx_search_dfs_locate,        collection,len,8)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<10>-CMP_DFS"         ,qgram_smp_rules_sfx_search_dfs_locate,        collection,len,10)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G-INDEX-QGRAM<12>-CMP_DFS"         ,qgram_smp_rules_sfx_search_dfs_locate,        collection,len,12)->Unit({benchmark::kMicrosecond});


    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

}

