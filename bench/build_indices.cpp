//
// Created by via56 on 01-04-20.
//

#include <iostream>
#include <fstream>
#include <benchmark/benchmark.h>

//#include <slp/RePairSLPIndex.h>
#include "../SelfGrammarIndexBS.h"
//#include <ri/r_index.hpp>
//#include "../SelfGrammarIndexPTS.h"
//#include "../SelfGrammarIndexBSQ.h"

#include "sdsl/io.hpp"

#ifdef MEM_MONITOR
#include "../utils/memory/mem_monitor/mem_monitor.hpp"
#include "../utils/CLogger.h"
#endif

//using namespace cds_static;

std::string data = "";

#ifdef MEM_MONITOR
void init_fields(){

//SPACE
//    CLogger::GetLogger()->model["Text-len"]                 = 0;
//    CLogger::GetLogger()->model["Total"]                    = 0;
//    CLogger::GetLogger()->model["G"]                        = 0;
//    CLogger::GetLogger()->model["G-X"]                      = 0;
//    CLogger::GetLogger()->model["G-Pi"]                     = 0;
//    CLogger::GetLogger()->model["G-Z"]                      = 0;
//    CLogger::GetLogger()->model["G-Y"]                      = 0;
//    CLogger::GetLogger()->model["G-L"]                      = 0;
//    CLogger::GetLogger()->model["G-Tree"]                   = 0;
//    CLogger::GetLogger()->model["G-Tree+Z"]                 = 0;
//    CLogger::GetLogger()->model["G-Tries"]                  = 0;
//    CLogger::GetLogger()->model["G-Tries-Tree"]             = 0;
//    CLogger::GetLogger()->model["G-Tries-Pi"]               = 0;
//    CLogger::GetLogger()->model["Grid"]                     = 0;
//    CLogger::GetLogger()->model["Grid - L"]                 = 0;
//    CLogger::GetLogger()->model["Grid:SL"]                  = 0;
//    CLogger::GetLogger()->model["Grid:SB"]                  = 0;
//    CLogger::GetLogger()->model["Grid:XA"]                  = 0;
//    CLogger::GetLogger()->model["Grid:XB"]                  = 0;
//    CLogger::GetLogger()->model["Text-len"]                 = 0;
//    CLogger::GetLogger()->model["Patricia-Trees"]           = 0;
//    CLogger::GetLogger()->model["Patricia-Tree-Rules"]      = 0;
//    CLogger::GetLogger()->model["Patricia-Tree-Rules-Tree"] = 0;
//    CLogger::GetLogger()->model["Patricia-Tree-Rules-Seq"]  = 0;
//    CLogger::GetLogger()->model["Patricia-Tree-Rules-Jmp"]  = 0;
//    CLogger::GetLogger()->model["Patricia-Tree-Sfx"]        = 0;
//    CLogger::GetLogger()->model["Patricia-Tree-Sfx-Tree"]   = 0;
//    CLogger::GetLogger()->model["Patricia-Tree-Sfx-Seq"]    = 0;
//    CLogger::GetLogger()->model["Patricia-Tree-Sfx-Jmp"]    = 0;


//Build contruction

    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR]                                      = 0;
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_1_RE_PAIR]                            = 0;
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_1_TERMINAL_RULE]               = 0;
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_2_REMOVE_RULE_LEN_1]           = 0;
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_3_GRAMMAR_REPLACE_RULE_OCC_1]  = 0;
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_4_CALCULATE_TEXT_OFF_RULE]     = 0;
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_5_BUILD_EDA_SA_LCP_RMQ_SORT]   = 0;
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_6_SORT_REV_RULES]              = 0;
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_7_UPDATE_IDS]                  = 0;
    CLogger::GetLogger()->model[BUILD_COMPRESSED_GRAMMAR]                               = 0;
    CLogger::GetLogger()->model[BUILD_COMPRESSED_GRAMMAR_1_C]                           = 0;
    CLogger::GetLogger()->model[BUILD_COMPRESSED_GRAMMAR_2_PARSE_TREE]                  = 0;
    CLogger::GetLogger()->model[BUILD_COMPRESSED_GRAMMAR_3_TRIES]                       = 0;
    CLogger::GetLogger()->model[BUILD_COMPUTE_GRAMMAR_SFX]                              = 0;
    CLogger::GetLogger()->model[BUILD_COMPUTE_SORT_EDA_SA_LCP_RMQ]                      = 0;
    CLogger::GetLogger()->model[BUILD_SORT_GRAMMAR_SFX]                                 = 0;
    CLogger::GetLogger()->model[BUILD_GRID]                                             = 0;
    CLogger::GetLogger()->model[BUILD_G_INDEX_BS]                                       = 0;
    CLogger::GetLogger()->model[BUILD_PATRICIA_TREES_RULE]                              = 0;
    CLogger::GetLogger()->model[BUILD_PATRICIA_TREES_SFX]                               = 0;
    CLogger::GetLogger()->model[BUILD_QGRAMS_PI]                                        = 0;
    CLogger::GetLogger()->model[BUILD_QGRAMS_SAMPING_RULES_REV]                         = 0;
    CLogger::GetLogger()->model[BUILD_QGRAMS_SAMPING_RULES]                             = 0;
    CLogger::GetLogger()->model[BUILD_QGRAMS_SAMPING_SEQ]                               = 0;


}
void add_logger_elements(benchmark::State &st){
    for (auto&&it  :CLogger::GetLogger()->model ) {
        st.counters[it.first] = it.second;
    }
}

#endif


int load_data(const std::string &collection){
    std::fstream f(collection, std::ios::in| std::ios::binary);
    //std::string data;
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

    f.close();
    std::fstream f2("./temp_collection",std::ios::out );
    for(size_t i = 0 ; i < data.size(); ++i)
        f2 << data[i];

    f2 << '\0';

    f2.close();
    std::cout<<"DATA LOADED\n";
    return 1;
}
//auto ribuild = [](benchmark::State &st, const string &file_collection, const std::string& file_out
//
//#ifdef MEM_MONITOR
//, const std::string file_mem_monitor
//#endif
//        ){
//    fstream f_ridx(file_out+".ri", std::ios::out | std::ios::binary);
//
//    bool fast = false;
//    bool sais = false;
//    f_ridx.write((char*)&fast,sizeof(fast));
//
//
//    string input;
//    {
//
//        std::ifstream fs(file_collection);
////        std::ifstream fs("temp_collection");
//        std::stringstream buffer;
//        buffer << fs.rdbuf();
//        input = buffer.str();
//
//    }
//
//
//
//    ri::r_index<> idx;
//
//
//#ifdef MEM_MONITOR
////    std::cout<<file_mem_monitor<<std::endl;
//    mem_monitor mm(file_mem_monitor+"-ri.csv");
//#endif
//
//    for (auto _ : st)
//    {
//#ifdef MEM_MONITOR
//        mm.event("R-INDEX-BUILD");
//#endif
//        idx = ri::r_index<>(input,sais);
//    }
//    idx.serialize(f_ridx);
//    st.counters["size"] = idx.print_space();
//
//};
//
//auto slpbuild = [](benchmark::State &st, const string &file_collection, const std::string& file_out, const int& qgram
//
//#ifdef MEM_MONITOR
//        , const std::string file_mem_monitor
//#endif
//){
//
//
//    std::string filename = file_out+"-q"+std::to_string(qgram);
//    char* _f = (char *)filename.c_str();
//    RePairSLPIndex *indexer = new RePairSLPIndex();
//
//#ifdef MEM_MONITOR
//    mem_monitor mm(file_mem_monitor+"-slp.csv");
//#endif
//
//    ifstream in(file_collection, ifstream::in);
//    in.seekg(0,ios_base::end);
//    uint tsize = in.tellg();
//    in.seekg(0,ios_base::beg);
//    uchar *text = nullptr;
//
//    if (in.good())
//    {
//        text = loadValue<uchar>(in, tsize);
//        text[tsize] = 0;
//        std::cout<<tsize<<std::endl;
//        in.close();
//
//    } else {
//        std::cout<<"File Error\n";
//        return;
//    }
//
//    for (auto _ : st)
//    {
//#ifdef MEM_MONITOR
//        mm.event("SLP-INDEX-BAL-BUILD");
//#endif
//        indexer->build(text, tsize, qgram, _f);
//
//    }
//
//    indexer->save();
//    st.counters["size"] = indexer->size();
//
//    delete indexer;
//
//};
//
//auto slpbalbuild = [](benchmark::State &st, const string &file_collection, const std::string& file_out, const int& qgram, bool bal = false
//
//#ifdef MEM_MONITOR
//        , const std::string file_mem_monitor
//#endif
//){
//
//#ifdef MEM_MONITOR
//    init_fields();
//#endif
//
//
//    std::string filename = (bal)?file_out+"-bal-q"+std::to_string(qgram):file_out+"-q"+std::to_string(qgram);
//    char* _f = (char *)filename.c_str();
//    RePairSLPIndex *indexer = new RePairSLPIndex();
//
//#ifdef MEM_MONITOR
//    mem_monitor mm(file_mem_monitor+"-balspl.csv");
//#endif
//
//    ifstream in(file_collection, ifstream::in);
//    in.seekg(0,ios_base::end);
//    uint tsize = in.tellg();
//    in.seekg(0,ios_base::beg);
//    uchar *text = nullptr;
//
//    std::string prefix = (bal)? file_collection+"-bal":file_collection;
//    std::cout<<"prefix:"<<prefix<<std::endl;
//    std::fstream R(prefix+".R",std::ios::in|std::ios::binary);
//    if(!R.is_open()){
//        std::cout<<"[ERROR]:file " +prefix+".R"<<std::endl;
//        return;
//    }
//
//    std::fstream C(prefix+".C",std::ios::in|std::ios::binary);
//    if(!C.is_open()){
//        std::cout<<"[ERROR]:file " +prefix+".C"<<std::endl;
//        return;
//    }
//
//    std::cout<<"build-problem\n";
//    if (in.good())
//    {
//        text = loadValue<uchar>(in, tsize);
//        text[tsize] = 0;
//        in.close();
//    } else {
//        return;
//    }
//
//    for (auto _ : st)
//    {
//#ifdef MEM_MONITOR
//        mm.event("SLP-INDEX-BAL-BUILD");
//#endif
//
//        indexer->build(text, tsize, qgram, _f,R,C);
//    }
//
//    indexer->save();
//    st.counters["size"] = indexer->size();
//#ifdef MEM_MONITOR
//    add_logger_elements(st);
//#endif
//    delete indexer;
//
//};

auto g_imp_build_basics = [](benchmark::State &st, const string &file_collection, const std::string& file_out

#ifdef MEM_MONITOR
        , const std::string file_mem_monitor
#endif
){
#ifdef MEM_MONITOR
    init_fields();
#endif
    std::fstream fbasics(file_out+"-basics", std::ios::out| std::ios::binary);
    std::fstream fsuffixes(file_out+"-suffixes", std::ios::out| std::ios::binary);
    std::fstream frepair(file_out+"-grepair", std::ios::out| std::ios::binary);

    SelfGrammarIndexBS g_index;
#ifdef MEM_MONITOR
    mem_monitor mm(file_mem_monitor+"-gi.csv");
//    mm.event("G-INDEX-BS-BUILD");
#endif
    grammar not_compressed_grammar;
    std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx;

    for (auto _ : st)
    {

        g_index.build_basics(data,not_compressed_grammar,grammar_sfx
#ifdef MEM_MONITOR
        ,mm
#endif
        );

    }

    fstream f_gidx(file_out+"-bs.gi", std::ios::out | std::ios::binary);

    g_index.save(f_gidx);
    g_index.save(fbasics);
    not_compressed_grammar.save(frepair);
    unsigned long num_sfx = grammar_sfx.size();
    sdsl::serialize(num_sfx,fsuffixes);


    for (auto && e : grammar_sfx ) {

        size_t p = e.first.first;
        sdsl::serialize(p,fsuffixes);
        p = e.first.second;
        sdsl::serialize(p,fsuffixes);
        p = e.second.first;
        sdsl::serialize(p,fsuffixes);
        p = e.second.second;
        sdsl::serialize(p,fsuffixes);

    }


    st.counters["size"] = g_index.size_in_bytes();

#ifdef MEM_MONITOR
    add_logger_elements(st);
#endif
};

//
//auto g_imp_pts_build = [](
//        benchmark::State &st, const string &file_collection, const std::string& file_out, const uint8_t &sampling
//
//#ifdef MEM_MONITOR
//        , const std::string file_mem_monitor
//#endif
//        ){
//
//#ifdef MEM_MONITOR
//    init_fields();
//#endif
//
//    std::fstream fbasics(file_out+"-basics", std::ios::in| std::ios::binary);
//
//    SelfGrammarIndexPTS g_index(sampling);
//    g_index.load_basics(fbasics);
//
//#ifdef MEM_MONITOR
//    mem_monitor mm(file_mem_monitor+"-gi-pts.csv");
//#endif
//    for (auto _ : st)
//    {
//        std::fstream fsuffixes(file_out+"-suffixes", std::ios::in| std::ios::binary);
//        std::fstream frepair(file_out+"-grepair", std::ios::in| std::ios::binary);
//
//        if( frepair && fsuffixes ){
//
//            g_index.build_suffix(data,fsuffixes,frepair
//#ifdef MEM_MONITOR
//                ,mm
//#endif
//        );
//
//        }
//    }
//
//    std::fstream f_gidx(file_out+"-pts-<"+std::to_string(sampling)+">.gi", std::ios::out | std::ios::binary);
//    g_index.save(f_gidx);
//    st.counters["size"] = g_index.size_in_bytes();
//    st.counters["sampling-size"] = g_index.get_pt_suffixes().size_in_bytes() + g_index.get_pt_rules().size_in_bytes();
//#ifdef MEM_MONITOR
//    add_logger_elements(st);
//#endif
//
//};
//
//auto g_imp_qgram_build = [](
//        benchmark::State &st, const string &file_collection, const std::string& file_out
//        , const uint8_t &sampling
//
//#ifdef MEM_MONITOR
//        , const std::string file_mem_monitor
//#endif
//)
//{
//
//#ifdef MEM_MONITOR
//    init_fields();
//#endif
//    std::fstream fbasics(file_out+"-basics", std::ios::in| std::ios::binary);
//    std::ofstream foutF(file_out+"-gram-"+std::to_string(sampling)+"-smp.gi",std::ios::out|std::ios::binary);
//    std::ofstream foutG(file_out+"-gram-"+std::to_string(sampling)+"-smp-g.gi",std::ios::out|std::ios::binary);
//    std::ofstream foutGR(file_out+"-gram-"+std::to_string(sampling)+"-smp-rev.gi",std::ios::out|std::ios::binary);
//    std::ofstream foutGS(file_out+"-gram-"+std::to_string(sampling)+"-smp-seq.gi",std::ios::out|std::ios::binary);
//
//    SelfGrammarIndexBSQ idx;
//    idx.load_basics(fbasics);
//#ifdef MEM_MONITOR
//    mem_monitor mm(file_mem_monitor+"-gi-qgram.csv");
////    mm.event("G-INDEX-QGRAM<"+std::to_string(sampling)+">-BUILD-SAMPLING");
//#endif
//    for (auto _ : st) {
//        std::fstream frepair(file_out+"-grepair", std::ios::in| std::ios::binary);
//
//        if( frepair ){
//
//            idx.buildSamplings(data,frepair,sampling
//    #ifdef MEM_MONITOR
//                    ,  mm
//    #endif
//            );
//
//        }
//    }
//    idx.saveSampling(foutF,foutG,foutGR,foutGS);
//    st.counters["size"] = idx.size_in_bytes();
//    st.counters["sampling-size"] = (idx.size_in_bytes() - idx.size_basics_in_bytes()) + idx.grammarSamp->getSize()+idx.sequenceSamp->getSize()+idx.reverseSamp->getSize();
//
//#ifdef MEM_MONITOR
//    add_logger_elements(st);
//#endif
//
//};

int main (int argc, char *argv[] ){

        if(argc < 2){
        std::cout<<"bad parameters...."<<std::endl;
        return 0;
    }

    std::string collection  = argv[1];

    std::string path_out    = argv[2];

    std::cout<<"INV_PI_T: "<<INV_PI_T<<std::endl;
    std::cout<<"INV_PI_T_TRIE: "<<INV_PI_T_TRIE<<std::endl;
    std::cout<<"INV_PI_T_QGRAM: "<<INV_PI_T_QGRAM<<std::endl;

#ifdef BUILD_EXTERNAL_INDEXES
    std::cout<<"RUNING EXTERNAL INDEXES: "<<std::endl;
#endif
#ifdef MEM_MONITOR
    std::cout<<"USING MEM_MONITOR: "<<std::endl;
#endif

#ifdef PRINT_LOGS
    std::cout<<"MODE LOG ACTIVE: "<<std::endl;
#endif



#ifdef MEM_MONITOR
    std::string mem_out = argv[3];
#endif
    load_data(collection);
#ifdef BUILD_EXTERNAL_INDEXES
  //   benchmark::RegisterBenchmark("R-INDEX",  ribuild,collection,path_out
 //#ifdef MEM_MONITOR
 //            ,mem_out
 //#endif
//     );
//benchmark::RegisterBenchmark("SLP-BAL-INDEX<2>",  slpbalbuild,collection,path_out, 2,true
//#ifdef MEM_MONITOR
  //          ,mem_out
//#endif
 //   );
//    benchmark::RegisterBenchmark("SLP-BAL-INDEX<4>",  slpbalbuild,collection,path_out, 4,true
//#ifdef MEM_MONITOR
//            ,mem_out
//#endif
  //  );
  //  benchmark::RegisterBenchmark("SLP-BAL-INDEX<6>",  slpbalbuild,collection,path_out, 6,true
//#ifdef MEM_MONITOR
  //          ,mem_out
//#endif
  //  );
  //  benchmark::RegisterBenchmark("SLP-BAL-INDEX<8>",  slpbalbuild,collection,path_out, 8,true
//#ifdef MEM_MONITOR
//            ,mem_out
//#endif
//    );
//    benchmark::RegisterBenchmark("SLP-BAL-INDEX<10>",  slpbalbuild,collection,path_out, 10,true
//#ifdef MEM_MONITOR
//            ,mem_out
//#endif
//    );
 //   benchmark::RegisterBenchmark("SLP-BAL-INDEX<12>",  slpbalbuild,collection,path_out, 12,true
//#ifdef MEM_MONITOR
//            ,mem_out
//#endif
//    );


//   benchmark::RegisterBenchmark("SLP-INDEX<2>",  slpbalbuild ,collection,path_out,2,false
//#ifdef MEM_MONITOR
 //           ,mem_out
//#endif
//    );
   // benchmark::RegisterBenchmark("SLP-INDEX<4>",  slpbalbuild ,collection,path_out,4,false
//#ifdef MEM_MONITOR
//            ,mem_out
//#endif
//    );

//    benchmark::RegisterBenchmark("SLP-INDEX<6>",  slpbalbuild ,collection,path_out,6,false
//#ifdef MEM_MONITOR
//            ,mem_out
//#endif
//    );
//    benchmark::RegisterBenchmark("SLP-INDEX<8>",  slpbalbuild ,collection,path_out,8,false
//#ifdef MEM_MONITOR
//            ,mem_out
//#endif
//    );

//    benchmark::RegisterBenchmark("SLP-INDEX<10>",  slpbalbuild ,collection,path_out,10,false
//#ifdef MEM_MONITOR
//            ,mem_out
//#endif
//    );

  //  benchmark::RegisterBenchmark("SLP-INDEX<12>",  slpbalbuild ,collection,path_out,12,false
//#ifdef MEM_MONITOR
//            ,mem_out
//#endif
//    );

#endif

benchmark::RegisterBenchmark("G-INDEX",  g_imp_build_basics ,collection,path_out
     #ifdef MEM_MONITOR
                 ,mem_out
     #endif
         );
//
//    benchmark::RegisterBenchmark("G-INDEX-PTS<2>",  g_imp_pts_build ,collection,path_out,2
//#ifdef MEM_MONITOR
//            ,mem_out
//#endif
//    );

///     benchmark::RegisterBenchmark("G-INDEX-PTS<4>",  g_imp_pts_build ,collection,path_out,4
// #ifdef MEM_MONITOR
//             ,mem_out
// #endif
//     );

//     benchmark::RegisterBenchmark("G-INDEX-PTS<8>",  g_imp_pts_build ,collection,path_out,8
// #ifdef MEM_MONITOR
//             ,mem_out
// #endif
//     );
//     benchmark::RegisterBenchmark("G-INDEX-PTS<16>",  g_imp_pts_build ,collection,path_out,16
// #ifdef MEM_MONITOR
//             ,mem_out
// #endif
//     );
//     benchmark::RegisterBenchmark("G-INDEX-PTS<32>",  g_imp_pts_build ,collection,path_out,32
// #ifdef MEM_MONITOR
//             ,mem_out
// #endif
//     );
//
//     benchmark::RegisterBenchmark("G-INDEX-PTS<64>",  g_imp_pts_build ,collection,path_out,64
//#ifdef MEM_MONITOR
//            ,mem_out
// #endif
//     );
//    benchmark::RegisterBenchmark("G-INDEX-QGRAM<2>",  g_imp_qgram_build ,collection,path_out,2
//#ifdef MEM_MONITOR
//            ,mem_out
//#endif
//    );
//     benchmark::RegisterBenchmark("G-INDEX-QGRAM<4>",  g_imp_qgram_build ,collection,path_out,4
// #ifdef MEM_MONITOR
//             ,mem_out
// #endif
//     );
//    benchmark::RegisterBenchmark("G-INDEX-QGRAM<6>",  g_imp_qgram_build ,collection,path_out,6
//#ifdef MEM_MONITOR
//           ,mem_out
//#endif
///   );
//     benchmark::RegisterBenchmark("G-INDEX-QGRAM<8>",  g_imp_qgram_build ,collection,path_out,8
// #ifdef MEM_MONITOR
//             ,mem_out
// #endif
//     );
//    benchmark::RegisterBenchmark("G-INDEX-QGRAM<10>",  g_imp_qgram_build ,collection,path_out,10
//#ifdef MEM_MONITOR
//            ,mem_out
//#endif
//    );
//     benchmark::RegisterBenchmark("G-INDEX-QGRAM<12>",  g_imp_qgram_build ,collection,path_out,12
// #ifdef MEM_MONITOR
//             ,mem_out
// #endif
//     );

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;

}
