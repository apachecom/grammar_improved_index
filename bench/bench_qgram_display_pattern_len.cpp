//
// Created by root on 12-06-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "repetitive_collections.h"
#include "utils.h"
#include "../SelfGrammarIndexBSQ.h"
#include <slp/RePairSLPIndex.h>
//#include <hyb/HybridSelfIndex.h>

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

#define MAX_DATA 1e19
#define MAX_Q 10000

#define BF -1
#define H_I 0
#define R_I 1
#define SLP_I 2
#define BAL_SLP_I 3
#define LZ77 4
#define G_INDEX_BINARY_SEARCH_LEAF_TRIE 5
#define G_INDEX_BINARY_SEARCH_LEAF_NOTRIE 6
#define G_INDEX_RANK_PHRASES_TRIE 7
#define G_INDEX_RANK_PHRASES_NOTRIE 8
#define G_INDEX_RANK_PHRASES_DFS 9
#define G_INDEX_QGRAM_RANK_PHRASES_DFS 10
#define G_INDEX_QGRAM_RANK_PHRASES_REC 11


//HybridSelfIndex*            idx_hyb;
cds_static::RePairSLPIndex* idx_slp;
SelfGrammarIndexBSQ idx_g;


std::string data;
std::vector<std::pair<size_t ,size_t >>R;
int code = 1;
int len_q = 5;

void generate_random_queries(const size_t &n, const size_t &len){

    R.clear();

    std::string rg_file = files_dir+"/files/";

    std::fstream f(rg_file + "rangos/"+std::to_string(code)+"_"+std::to_string(len)+".rg", std::ios::in| std::ios::binary);
    std::cout<<rg_file + "rangos/"+std::to_string(code)+"_"+std::to_string(len)+".rg"<<std::endl;

    if(!f.is_open()){
        std::cout<<"Error the ranges file could not opened!!\n";
        return;
    }
    std::string buff; uint i = 0;

    while (!f.eof() && std::getline(f, buff) ) {
        if(buff.size() > 1){
            uint l,r;
            sscanf(buff.c_str(),"%u %u",&l,&r);
            R.emplace_back(l,r);
            i++;
        }
    }
    f.close();

//    std::cout<<"[DEBUG] RANGES SIZE: "<<R.size()<<std::endl;
    

    if(R.size() == 0) std::cout<<"Error leyendo el fichero\n";

}
auto load_data_coll=[](benchmark::State &state,  const uint& _code, const uint& len, const uint& _samp =0)
{

    len_q = len;
    code  = _code;


    load_data(MAX_DATA,data,collections_code_inv_dir[code]);

    for (auto _ : state) {
        sleep(4);
    }

    generate_random_queries(MAX_Q,len_q);

    state.counters["1 n_ptt"]     = 0;
    state.counters["2 n_occ"]     = 0;
    state.counters["3 ptt_len"]   = len_q;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = 0;
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = 0;
    state.counters["9 valid"]     = 0;


};
//static void h_index(benchmark::State &state) {
//
//    int64_t code_coll = state.range(0);
//    int64_t len = state.range(1);
//
//    std::string filename = files_dir+"/files/indices/"+std::to_string(code_coll);
//
//    char* _f = (char *)filename.c_str();
//
//    idx_hyb = new HybridSelfIndex(_f);
//
//
//
//    size_t ptt ;
//    for (auto _ : state) {
//        ptt = 0;
//        for (uint ii=  0; ii < MAX_Q && ii < R.size();++ii) {
//            uint m = R[ii].second-R[ii].first+1;
//            unsigned char *s;
//            idx_hyb->extract(R[ii].first,m,&s);
//            std::cout<<"Hyb "<<(char*)s<<std::endl;
//            delete []s;
//            ++ptt;
//        }
//    }
//
//
//    state.counters["1 n_ptt"]     = ptt;
//    state.counters["2 n_occ"]     = 0;
//    state.counters["3 ptt_len"]   = len_q;
//    state.counters["4 coll_size"] = data.size();
//    state.counters["5 coll_id"]   = code;
//    state.counters["6 size"]      = idx_hyb->sizeDS;
//    state.counters["7 samp"]      = 0;
//    state.counters["8 name"]      = H_I;
//    state.counters["9 valid"]     = 1;
//
////    delete idx_hyb;
//}
auto slp_index = [](benchmark::State &state, const uint& _code , const uint& _len, const uint& _samp = 0)
{


    uint64_t code_coll = _code;
    uint64_t len = _len;
    uint64_t samp = _samp;

    std::string filename = files_dir+"/files/indices/"+std::to_string(code_coll)+"_"+std::to_string(samp);
    char* _f = (char *)filename.c_str();
    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);

    size_t ptt ;

    for (auto _ : state) {
        ptt = 0;
        for (uint ii=  0; ii < MAX_Q &&  ii < R.size();++ii) {
            unsigned char * s = idx_slp->RePairSLPIndex::extract(R[ii].first,R[ii].second-1);
//            std::cout<<"SLP "<<(char*)s<<std::endl;
            ++ptt;
            delete s;
        }
    }


    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = 0;
    state.counters["3 ptt_len"]   = len_q;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = idx_slp->size();
    state.counters["7 samp"]      = samp;
    state.counters["8 name"]      = SLP_I;
    state.counters["9 valid"]     = 1;


    delete idx_slp;
};
//auto bal_slp_index = [](benchmark::State &state, const uint& _code , const uint& _len, const uint& _samp = 0)
//{
//
//
//    int64_t code_coll = _code;
//    int64_t len = _len;
//    int64_t samp = _samp;
//
//    std::string filename = files_dir+"/files/new_divided_indices/bal_slp"+std::to_string(code_coll)+"_"+std::to_string(samp);
//    char* _f = (char *)filename.c_str();
//    int q = cds_static::RePairSLPIndex::load(_f, &idx_slp);
//    if(!q){
//
//        std::cout<<"Error en fichero slp\n";
//        std::cout<<filename<<"\n";
//        return;
//
//    }
//
//    size_t ptt ;
//    for (auto _ : state) {
//        ptt = 0 ;
//        for (uint ii=  0; ii < MAX_Q &&  ii < R.size();++ii) {
//            unsigned char * s = idx_slp->RePairSLPIndex::extract(R[ii].first,R[ii].second-1);
//            ++ptt;
//            delete s;
//        }
//    }
//
//    state.counters["1 n_ptt"]     = ptt;
//    state.counters["2 n_occ"]     = 0;
//    state.counters["3 ptt_len"]   = len_q;
//    state.counters["4 coll_size"] = data.size();
//    state.counters["5 coll_id"]   = code;
//    state.counters["6 size"]      = idx_slp->size();
//    state.counters["7 samp"]      = samp;
//    state.counters["8 name"]      = BAL_SLP_I;
//    state.counters["9 valid"]     = 1;
//
//
//    delete idx_slp;
//};
auto g_index_binary_search_leaf_notrie = [] (benchmark::State &state, const uint& _code, const uint& _len, const uint& _samp = 0)
{


    int64_t code = _code;
    int64_t len = _len;
    int64_t samp = _samp;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);

//    idx_g = load_idx(1,0,1,1,2,code);
    idx_g.load(f);

    std::string s = "";
    s.resize(len);
    uint ptt ;
    for (auto _ : state) {
        ptt = 0;
        for (  auto &ii:R ) {
//            std::string ss = "";
//            ss.resize(len);
//            std::copy(data.begin()+ii.first,data.begin()+ii.second,ss.begin());
            idx_g.display(ii.first,ii.second-1,s);
//            ASSERT_EQ(ss,s);

            ++ptt;
            if(ptt >= MAX_Q)break;
        }

    }
    size_t size_indx = idx_g.get_grid().size_in_bytes()+(idx_g.get_grammar().size_in_bytes()- idx_g.get_grammar().right_path.size_in_bytes() - idx_g.get_grammar().left_path.size_in_bytes());

    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = 0;
    state.counters["3 ptt_len"]   = len_q;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = G_INDEX_BINARY_SEARCH_LEAF_NOTRIE;
    state.counters["9 valid"]     = 1;



};
auto g_index_binary_search_leaf_trie = [](benchmark::State &state, const uint& _code, const uint& _len , const uint& _samp = 0)
{


    int64_t code = _code;
    int64_t len = _len;
    int64_t samp = samp;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);

//    idx_g = load_idx(1,0,1,1,2,code);
    idx_g.load(f);

    std::string s = "";
    s.resize(len);
    uint ptt ;
    for (auto _ : state) {
        ptt = 0;
        for (  auto &ii:R ) {
//            std::string ss = "";
//            ss.resize(len);
//            std::copy(data.begin()+ii.first,data.begin()+ii.second,ss.begin());
            idx_g.display_trie(ii.first,ii.second-1,s);
//            ASSERT_EQ(ss,s);
            ++ptt;
            if(ptt >= MAX_Q)break;
        }

    }
    size_t size_indx = idx_g.get_grid().size_in_bytes()+idx_g.get_grammar().size_in_bytes();

    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = 0;
    state.counters["3 ptt_len"]   = len_q;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = G_INDEX_BINARY_SEARCH_LEAF_TRIE;
    state.counters["9 valid"]     = 1;



};
auto g_index_rank_phrases_notrie = [](benchmark::State &state, const uint& _code, const uint& _len , const uint& _samp = 0)
{


    int64_t code = _code;
    int64_t len = _len;
    int64_t samp = samp;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);

//    idx_g = load_idx(1,0,1,1,2,code);
    idx_g.load(f);

    std::string s = "";
    s.resize(len);
    uint ptt ;
    for (auto _ : state) {
        ptt = 0;
        for (  auto &ii:R ) {
//            std::string ss = "";
//            ss.resize(len);
//            std::copy(data.begin()+ii.first,data.begin()+ii.second,ss.begin());
            idx_g.display_L(ii.first,ii.second-1,s);
//            ASSERT_EQ(ss,s);

            ++ptt;
            if(ptt >= MAX_Q)break;
        }

    }
    size_t size_indx = idx_g.get_grid().size_in_bytes()+(idx_g.get_grammar().size_in_bytes()- idx_g.get_grammar().right_path.size_in_bytes() - idx_g.get_grammar().left_path.size_in_bytes());

    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = 0;
    state.counters["3 ptt_len"]   = len_q;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = G_INDEX_RANK_PHRASES_NOTRIE;
    state.counters["9 valid"]     = 1;



};
auto g_index_rank_phrases_trie = [](benchmark::State &state, const uint& _code , const uint& _len , const uint& _samp = 0)
{


    int64_t code_coll = _code;
    int64_t len = _len;
    int64_t samp = samp;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);

//    idx_g = load_idx(1,0,1,1,2,code);
    idx_g.load(f);

    std::string s = "";
    s.resize(len);
    uint ptt ;
    for (auto _ : state) {
        ptt = 0;
        for (  auto &ii:R ) {
//            std::string ss = "";
//            ss.resize(len);
//            std::copy(data.begin()+ii.first,data.begin()+ii.second,ss.begin());
            idx_g.display_L_trie(ii.first,ii.second-1,s);
//            ASSERT_EQ(ss,s);

            ++ptt;
            if(ptt >= MAX_Q)break;
        }

    }
    size_t size_indx = idx_g.get_grid().size_in_bytes()+(idx_g.get_grammar().size_in_bytes());

    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = 0;
    state.counters["3 ptt_len"]   = len_q;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = G_INDEX_RANK_PHRASES_TRIE;
    state.counters["9 valid"]     = 1;



};
auto g_index_rank_phrases_dfs = [](benchmark::State &state, const uint& _code, const uint&  _len, const uint& _samp = 0)
{


    int64_t code_coll = _code;
    int64_t len = _len;
    int64_t samp = samp;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);

//    idx_g = load_idx(1,0,1,1,2,code);
    idx_g.load(f);

    std::string s = "";
    s.resize(len);
    uint ptt ;
    for (auto _ : state) {
        ptt = 0;
        for (  auto &ii:R ) {
//            std::string ss = "";
//            ss.resize(len);
//            std::copy(data.begin()+ii.first,data.begin()+ii.second,ss.begin());
            idx_g.display_dfs(ii.first,ii.second-1,s);
//            ASSERT_EQ(ss,s);
            ++ptt;
            if(ptt >= MAX_Q)break;
        }

    }
    size_t size_indx = idx_g.get_grid().size_in_bytes()+(idx_g.get_grammar().size_in_bytes()- idx_g.get_grammar().right_path.size_in_bytes() - idx_g.get_grammar().left_path.size_in_bytes());

    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = 0;
    state.counters["3 ptt_len"]   = len_q;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = 0;
    state.counters["8 name"]      = G_INDEX_RANK_PHRASES_DFS;
    state.counters["9 valid"]     = 1;



};
auto g_index_rank_phrases_qgram_dfs= [](benchmark::State &state, const uint& _code , const uint& _len, const uint& _samp = 0)
{


    int64_t code_coll = _code;
    int64_t len = _len;
    int64_t smp = _samp;

//    std::cout<<"[DEBUG] REDING FILES "<<files_dir+"/files/new_divided_indices/"+std::to_string(code)+".basic_index"<<std::endl;
    std::fstream  f  (files_dir+"/files/new_divided_indices/"+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
//    std::cout<<"[DEBUG] REDING FILES "<<files_dir+"/files/new_divided_indices/"+std::to_string(code)+".basic_index"<<std::endl;
    std::ifstream fG (files_dir+"/files/new_divided_indices/"+std::to_string(code)+".qgramSampG_"+std::to_string(smp),std::ios::in|std::ios::binary);
//    std::cout<<"[DEBUG] REDING FILES "<<files_dir+"/files/new_divided_indices/"+std::to_string(code)+".basic_index"<<std::endl;
    std::ifstream fGR(files_dir+"/files/new_divided_indices/"+std::to_string(code)+".qgramSampGR_"+std::to_string(smp),std::ios::in|std::ios::binary);
//    std::cout<<"[DEBUG] REDING FILES "<<files_dir+"/files/new_divided_indices/"+std::to_string(code)+".basic_index"<<std::endl;
    std::ifstream fGS(files_dir+"/files/new_divided_indices/"+std::to_string(code)+".qgramSampGS_"+std::to_string(smp),std::ios::in|std::ios::binary);

    idx_g.load(f);
    idx_g.loadSampling(fG,fGR,fGS);

//    std::cout<<"[DEBUG] INDEX LOADED SIZE "<<idx_g.size_in_bytes()<<std::endl;


    std::string s = "";
    s.resize(len);
    uint ptt ;
    for (auto _ : state) {
        ptt = 0;
        for (  auto &ii:R ) {
//            std::string ss = "";
//            ss.resize(len);
//            std::cout<<ii.first<<" "<<ii.second<<std::endl;
//            std::copy(data.begin()+ii.first,data.begin()+ii.second,ss.begin());
            idx_g.display_qgram(ii.first,ii.second-1,s);
//            ASSERT_EQ(ss,s);
            ++ptt;
            if(ptt >= MAX_Q)break;
        }

    }

    size_t size_indx = idx_g.get_grid().size_in_bytes()+(idx_g.get_grammar().size_in_bytes()- idx_g.get_grammar().right_path.size_in_bytes() - idx_g.get_grammar().left_path.size_in_bytes())
            + idx_g.grammarSamp->getSize() + + idx_g.reverseSamp->getSize() + idx_g.sequenceSamp->getSize();

    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = 0;
    state.counters["3 ptt_len"]   = len_q;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = _samp;
    state.counters["8 name"]      = G_INDEX_QGRAM_RANK_PHRASES_DFS;
    state.counters["9 valid"]     = 1;



};
auto g_index_rank_phrases_qgram_dfs_left_rigtht = [](benchmark::State &state, const uint& _code, const uint& _len, const uint& _samp = 0)
{


    int64_t code_coll = _code;
    int64_t len = _len;
    int64_t smp = _samp;

//    std::cout<<"[DEBUG] REDING FILES "<<files_dir+"/files/new_divided_indices/"+std::to_string(code)+".basic_index"<<std::endl;
    std::fstream  f  (files_dir+"/files/new_divided_indices/"+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
//    std::cout<<"[DEBUG] REDING FILES "<<files_dir+"/files/new_divided_indices/"+std::to_string(code)+".basic_index"<<std::endl;
    std::ifstream fG (files_dir+"/files/new_divided_indices/"+std::to_string(code)+".qgramSampG_"+std::to_string(smp),std::ios::in|std::ios::binary);
//    std::cout<<"[DEBUG] REDING FILES "<<files_dir+"/files/new_divided_indices/"+std::to_string(code)+".basic_index"<<std::endl;
    std::ifstream fGR(files_dir+"/files/new_divided_indices/"+std::to_string(code)+".qgramSampGR_"+std::to_string(smp),std::ios::in|std::ios::binary);
//    std::cout<<"[DEBUG] REDING FILES "<<files_dir+"/files/new_divided_indices/"+std::to_string(code)+".basic_index"<<std::endl;
    std::ifstream fGS(files_dir+"/files/new_divided_indices/"+std::to_string(code)+".qgramSampGS_"+std::to_string(smp),std::ios::in|std::ios::binary);

    idx_g.load(f);
    idx_g.loadSampling(fG,fGR,fGS);

//    std::cout<<"[DEBUG] INDEX LOADED SIZE "<<idx_g.size_in_bytes()<<std::endl;

    std::string s = "";
    s.resize(len);
    uint ptt ;
    for (auto _ : state) {
        ptt = 0;
        for (  auto &ii:R ) {
//            std::string ss = "";
//            ss.resize(len);
//            std::cout<<ii.first<<" "<<ii.second<<std::endl;
//            ii.first = 396313818 ; ii.second = 396313828;
//            std::copy(data.begin()+ii.first,data.begin()+ii.second,ss.begin());
            idx_g.display_qgram_rec(ii.first,ii.second-1,s);
//            ASSERT_EQ(ss,s);
            ++ptt;
            if(ptt >= MAX_Q)break;
        }

    }
    size_t size_indx = idx_g.get_grid().size_in_bytes()+(idx_g.get_grammar().size_in_bytes()- idx_g.get_grammar().right_path.size_in_bytes() - idx_g.get_grammar().left_path.size_in_bytes())
            + idx_g.grammarSamp->getSize() + + idx_g.reverseSamp->getSize() + idx_g.sequenceSamp->getSize();

    state.counters["1 n_ptt"]     = ptt;
    state.counters["2 n_occ"]     = 0;
    state.counters["3 ptt_len"]   = len_q;
    state.counters["4 coll_size"] = data.size();
    state.counters["5 coll_id"]   = code;
    state.counters["6 size"]      = size_indx;
    state.counters["7 samp"]      = _samp;
    state.counters["8 name"]      = G_INDEX_QGRAM_RANK_PHRASES_REC;
    state.counters["9 valid"]     = 1;



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

    if(argc >= 3)
        files_dir = argv[2];
    else
        files_dir = "../";




    std::vector<uint> pattern_len = {10};//5,10,20,30,40,50,100,150,200};
    for (int i = 0; i < pattern_len.size() ; ++i) {

        uint len = pattern_len[i];


        benchmark::RegisterBenchmark("LOAD-RANGES" ,load_data_coll,collection,len,4)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("SLP-Index<4>" ,slp_index,collection,len,4)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("SLP-Index<8>" ,slp_index,collection,len,8)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("SLP-Index<16>" ,slp_index,collection,len,16)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("BAL_SLP-Index<4>" ,bal_slp_index,collection,len,4)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("BAL_SLP-Index<8>" ,bal_slp_index,collection,len,8)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("BAL_SLP-Index<16>" ,bal_slp_index,collection,len,16)->Unit({benchmark::kMicrosecond});
//
//        benchmark::RegisterBenchmark("G-INDEX-BINARY_SEARCH_LEAF-TRIE" ,g_index_binary_search_leaf_trie,collection,len,0)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("G_INDEX_BINARY_SEARCH_LEAF-NOTRIE" ,g_index_binary_search_leaf_notrie,collection,len,0)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("G_INDEX_RANK_PHRASES-TRIE " ,g_index_rank_phrases_trie,collection,len,0)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("G_INDEX_RANK_PHRASES-NOTRIE " ,g_index_rank_phrases_notrie,collection,len,0)->Unit({benchmark::kMicrosecond});
//
        benchmark::RegisterBenchmark("G_INDEX_QGRAM<2>_RANK_PHRASES-DFS "  ,g_index_rank_phrases_qgram_dfs,collection,len,2)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G_INDEX_QGRAM<4>_RANK_PHRASES-DFS "  ,g_index_rank_phrases_qgram_dfs,collection,len,4)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G_INDEX_QGRAM<6>_RANK_PHRASES-DFS "  ,g_index_rank_phrases_qgram_dfs,collection,len,6)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G_INDEX_QGRAM<8>_RANK_PHRASES-DFS "  ,g_index_rank_phrases_qgram_dfs,collection,len,8)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G_INDEX_QGRAM<10>_RANK_PHRASES-DFS " ,g_index_rank_phrases_qgram_dfs,collection,len,10)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("G_INDEX_QGRAM<12>_RANK_PHRASES-DFS " ,g_index_rank_phrases_qgram_dfs,collection,len,12)->Unit({benchmark::kMicrosecond});

//        benchmark::RegisterBenchmark("G_INDEX_QGRAM<2>_RANK_PHRASES_DFS_REC " ,g_index_rank_phrases_qgram_dfs_left_rigtht,collection,len,2)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("G_INDEX_QGRAM<4>_RANK_PHRASES_DFS_REC " ,g_index_rank_phrases_qgram_dfs_left_rigtht,collection,len,4)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("G_INDEX_QGRAM<6>_RANK_PHRASES_DFS_REC " ,g_index_rank_phrases_qgram_dfs_left_rigtht,collection,len,6)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("G_INDEX_QGRAM<8>_RANK_PHRASES_DFS_REC " ,g_index_rank_phrases_qgram_dfs_left_rigtht,collection,len,8)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("G_INDEX_QGRAM<10>_RANK_PHRASES_DFS_REC " ,g_index_rank_phrases_qgram_dfs_left_rigtht,collection,len,10)->Unit({benchmark::kMicrosecond});
//        benchmark::RegisterBenchmark("G_INDEX_QGRAM<12>_RANK_PHRASES_DFS_REC " ,g_index_rank_phrases_qgram_dfs_left_rigtht,collection,len,12)->Unit({benchmark::kMicrosecond});


    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();



}

