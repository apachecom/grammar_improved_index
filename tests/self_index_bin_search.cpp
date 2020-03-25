//
// Created by alpachec on 20-08-18.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "collections.cpp"
#include "../SelfGrammarIndexBS.h"

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;


static void partial_build_locate(benchmark::State& state)
{
    size_t points = state.range(0);
    for (auto _ : state)
    {
        uint folder = state.range(0);
        uint coll   = state.range(1);
        std::string collection = "../" + dirFolder[folder]+dircollection[coll];
        std::fstream f(collection, std::ios::in| std::ios::binary);
        std::string data;
        std::cout << "collection: "<< collection << std::endl;
        if(!f.is_open()){
            std::cout<<"Error the file could not opened!!\n";
            return;
        }
        std::string buff;
        while (!f.eof() && std::getline(f, buff)) {
            data += buff;
        }
        std::cout << "size of the string: " << data.length() << std::endl;
        f.close();
        for (int i = 0; i < data.size() ; ++i) {
            if(data[i]==0){
                data[i] = 1;
               //////// std::cout<<"contiene zero"<<std::endl;
            }
        }

        std::fstream fg("grp_"+dircollection[coll],std::ios::in|std::ios::binary);
        ///data = "anaplanchaconcuatroplanchaconcuantasplanchasanaplancha";
        grammar G;
        ////G.buildRepair(data);
        G.load(fg);
        std::cout<<"Building compressed grammar"<<std::endl;

        compressed_grammar _g;

        _g.build(G);
        _g.print_size_in_bytes();
        std::cout<<"total size of compressed grammar*******************"<<_g.size_in_bytes()*1.0/1024/1024<<std::endl;


        fstream sfx_f("sfx_"+dircollection[coll],std::ios::in|std::ios::binary);

        size_t n_sufx;

        sdsl::load(n_sufx,sfx_f);
        std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx(n_sufx);
        for (int j = 0; j < n_sufx; ++j)
        {
            std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > item;

            sdsl::load(item.first.first,sfx_f);
            sdsl::load(item.first.second,sfx_f);
            sdsl::load(item.second.first,sfx_f);
            sdsl::load(item.second.second,sfx_f);
            grammar_sfx[j] = item;
        }

        sfx_f.close();


        std::cout<<"Building grid for 2d range search first occ"<<std::endl;

        std::vector<std::pair<std::pair<SelfGrammarIndex::range_search2d::bin_long ,SelfGrammarIndex::range_search2d::bin_long >,SelfGrammarIndex::range_search2d::bin_long>> grid_points(n_sufx);
        size_t bpair = 0;
        for (auto && s  :grammar_sfx ) {
            grid_points[bpair] = make_pair(make_pair(s.second.first,bpair+1),s.second.second);
            ++bpair;
        }

        SelfGrammarIndex::range_search2d grid;

        grid.build(grid_points.begin(),grid_points.end(),G.n_rules(),n_sufx);
        std::cout<<"***************************grid size "<<grid.size_in_bytes()*1.0/1024/1024<<std::endl;
        grid.print_size();


        SelfGrammarIndexBS self_index;
        self_index.build(_g,grid);

        uint R = 10000;
        std::srand(std::time(nullptr));
        uint t = 0;
        double mean = 0;
        unsigned long occ_n= 0;


     /*   std::fstream fpp("prob_pattern_pt",std::ios::in| std::ios::binary);
        std::string buff_p;

        while (!fpp.eof() && std::getline(fpp, buff_p)) {

            sdsl::bit_vector _occ(data.size(),0);
            sdsl::bit_vector _X(data.size(),0);


            auto start = timer::now();
            size_t pos = data.find(buff_p, 0);
            uint tooc = 0;
            while(pos != string::npos)
            {
                _X[pos] = true;
                tooc++;;
                pos = data.find(buff_p,pos+1);
            }
            auto stop = timer::now();


            auto start_ = timer::now();
            self_index.locate(buff_p, _occ);
            auto stop_ = timer::now();

            EXPECT_EQ(_occ,_X);
            cout << "query[" << ++t << "] cadena: " <<buff_p<<"\t my time\t"<< duration_cast<microseconds>(stop_ - start_).count()<<"\t num de occ:"<<tooc<<std::endl;
            cout << "query[" << t << "] cadena: " <<buff_p<<"\t BF time\t"<< duration_cast<microseconds>(stop - start).count()<<std::endl;
            mean  += duration_cast<microseconds>(stop_ - start_).count()*1.0/100;
            occ_n += tooc;


        }
*/
        std::fstream fpp("prob_pattern_bin",std::ios::out| std::ios::binary);
        std::string buff_p;

        while (--R) {


            sdsl::bit_vector _occ(data.size(),0);
            sdsl::bit_vector _X(data.size(),0);

            auto r1 = std::rand()%data.size();
            auto r2 = std::rand()%data.size();
            std::string patt;

            if (r1 > data.size() / 2) {
                r2 = r1 - 5;
            } else {
                r2 = r1 + 5;
            }

            if(r1 > r2) std::swap(r1,r2);
            patt.resize(r2-r1);


            std::copy(data.begin() + r1, data.begin() + r2 +1, patt.begin());

            ///if(patt.empty())continue;
            ////patt = "racad";

            auto start = timer::now();
            size_t pos = data.find(patt, 0);
            uint tooc = 0;
            while(pos != string::npos)
            {
                _X[pos] = true;
                tooc++;;
                pos = data.find(patt,pos+1);
            }
            auto stop = timer::now();


            auto start_ = timer::now();
            self_index.locate(patt, _occ);
            auto stop_ = timer::now();

            EXPECT_EQ(_occ,_X);
            if(_occ!=_X){
                fpp << patt <<'\n';
                return;
            }



            cout << "query[" << ++t << "] cadena: " <<patt<<"\t my time\t"<< duration_cast<microseconds>(stop_ - start_).count()<<"\t num de occ:"<<tooc<<std::endl;
            cout << "query[" << t << "] cadena: " <<patt<<"\t BF time\t"<< duration_cast<microseconds>(stop - start).count()<<std::endl;
            mean  += duration_cast<microseconds>(stop_ - start_).count()*1.0/100;
            occ_n += tooc;
        }

        cout << "query[ mean ] time: " <<mean<<" occurrencias totales: "<<occ_n<<std::endl;

        std::fstream fi("idx_" + dircollection[coll],std::ios::out|std::ios::binary);
        self_index.save(fi);

    }

}

static void partial_build_extract(benchmark::State& state)
{
    size_t points = state.range(0);
    for (auto _ : state)
    {
        uint folder = state.range(0);
        uint coll   = state.range(1);
        std::string collection = "../" + dirFolder[folder]+dircollection[coll];
        std::fstream f(collection, std::ios::in| std::ios::binary);
        std::string data;
        std::cout << "collection: "<< collection << std::endl;
        if(!f.is_open()){
            std::cout<<"Error the file could not opened!!\n";
            return;
        }
        std::string buff;
        while (!f.eof() && std::getline(f, buff)) {
            data += buff;
        }
        std::cout << "size of the string: " << data.length() << std::endl;
        f.close();
        for (int i = 0; i < data.size() ; ++i) {
            if(data[i]==0){
                data[i] = 1;
                /////std::cout<<"contiene zero"<<std::endl;
            }
        }

        std::fstream fg("grp_"+dircollection[coll],std::ios::in|std::ios::binary);
        ///data = "anaplanchaconcuatroplanchaconcuantasplanchasanaplancha";
        grammar G;
        ////G.buildRepair(data);
        G.load(fg);
        std::cout<<"Building compressed grammar"<<std::endl;

        compressed_grammar _g;

        _g.build(G);
        _g.print_size_in_bytes();
        std::cout<<"total size of compressed grammar*******************"<<_g.size_in_bytes()*1.0/1024/1024<<std::endl;


        fstream sfx_f("sfx_"+dircollection[coll],std::ios::in|std::ios::binary);

        size_t n_sufx;

        sdsl::load(n_sufx,sfx_f);
        std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx(n_sufx);
        for (int j = 0; j < n_sufx; ++j)
        {
            std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > item;

            sdsl::load(item.first.first,sfx_f);
            sdsl::load(item.first.second,sfx_f);
            sdsl::load(item.second.first,sfx_f);
            sdsl::load(item.second.second,sfx_f);
            grammar_sfx[j] = item;
        }

        sfx_f.close();


        std::cout<<"Building grid for 2d range search first occ"<<std::endl;

        std::vector<std::pair<std::pair<SelfGrammarIndex::range_search2d::bin_long ,SelfGrammarIndex::range_search2d::bin_long >,SelfGrammarIndex::range_search2d::bin_long>> grid_points(n_sufx);
        size_t bpair = 0;
        for (auto && s  :grammar_sfx ) {
            grid_points[bpair] = make_pair(make_pair(s.second.first,bpair+1),s.second.second);
            ++bpair;
        }

        SelfGrammarIndex::range_search2d grid;

        grid.build(grid_points.begin(),grid_points.end(),G.n_rules(),n_sufx);
        std::cout<<"***************************grid size "<<grid.size_in_bytes()*1.0/1024/1024<<std::endl;
        grid.print_size();


        SelfGrammarIndexBS self_index;
        self_index.build(_g,grid);

        uint R = 1000 ;
        std::srand(std::time(nullptr));
        double mean;

        while(--R){

            auto r1 = std::rand()%data.size();
            auto r2 = std::rand()%data.size();

            std::string patt,s;
            if (r1 > data.size() / 2)
            {
                r2 = r1 - 5;
            }else {
                r2 = r1 + 5;
            }

            if(r1 > r2) std::swap(r1,r2);
            patt.resize(r2-r1+1);
            std::copy(data.begin()+r1,data.begin()+r2+1,patt.begin());

            auto start = timer::now();
            self_index.display(r1,r2,s);
            auto stop = timer::now();
            unsigned long tttt = duration_cast<microseconds>(stop-start).count();
            cout <<"query["<<1000-R<<"] "<<" # extractSubstring_time = " <<tttt<< "\t\t %/c "<<tttt/11<<endl;
            mean+= tttt*1.0/11.0;
            EXPECT_STREQ(s.c_str(),patt.c_str());



        }

        cout <<"mean of time for a character extraction"<<mean/1000<<endl;
        std::fstream fi("idx_" + dircollection[coll],std::ios::out|std::ios::binary);
        self_index.save(fi);


    }

}
static void sibs_build(benchmark::State& state)
{
    size_t points = state.range(0);
    for (auto _ : state)
    {
        uint folder = state.range(0);
        uint coll   = state.range(1);
        std::string collection = "../" + dirFolder[folder]+dircollection[coll];
        std::fstream f(collection, std::ios::in| std::ios::binary);
        std::string data;
        std::cout << "collection: "<< collection << std::endl;
        if(!f.is_open()){
            std::cout<<"Error the file could not opened!!\n";
            return;
        }
        std::string buff;
        while (!f.eof() && std::getline(f, buff)) {
            data += buff;
        }
        for (int i = 0; i < data.size(); ++i) {
            if(data[i] == '$' || data[i] == '#')
                data[i] = 'Z';
        }
        std::cout << "size of the string: " << data.length() << std::endl;
        f.close();

        std::fstream f_idx("bs_index_" + dircollection[coll], std::ios::out | std::ios::binary);


        SelfGrammarIndexBS self_index;
        self_index.build(data);
        self_index.save(f_idx);
        f_idx.close();



    }
}
static void sibs_extract_subtrings(benchmark::State& state)
{

    size_t points = state.range(0);
    for (auto _ : state)
    {

        uint folder = state.range(0);
        uint coll   = state.range(1);
        std::string collection = "../" + dirFolder[folder]+dircollection[coll];
        std::fstream f(collection, std::ios::in| std::ios::binary);
        std::string data;
        std::cout << "collection: "<< collection << std::endl;
        if(!f.is_open()){
            std::cout<<"Error the file could not opened!!\n";
            return;
        }
        std::string buff;
        while (!f.eof() && std::getline(f, buff)) {
            data += buff;

        }


        std::cout << "size of the string: " << data.length() << std::endl;
        f.close();
        /*std::fstream f_idx("pt_index_" + dircollection[coll], std::ios::in | std::ios::binary);
        streampos begin, end;
        begin = f_idx.tellg();
        f_idx.seekg (0, ios::end);
        end = f_idx.tellg();
        std::cout<<"size of load file "<<end-begin<<std::endl;
*/

        SelfGrammarIndexBS self_index;
        data = "panapanamabananamapanpanapanamabananamabananaamapanamabananabanpanamabananaanabanana";
        self_index.build(data);
        ///self_index.load(f_idx);


        uint R = 1000 ;
        std::srand(std::time(nullptr));
        double mean;

        while(--R){

            auto r1 = std::rand()%data.size();
            auto r2 = std::rand()%data.size();

            std::string patt,s;
            if (r1 > data.size() / 2)
            {
                r2 = r1 - 10;
            }else {
                r2 = r1 + 10;
            }


            if(r1 > r2) std::swap(r1,r2);
            patt.resize(r2-r1+1);
            std::copy(data.begin()+r1,data.begin()+r2+1,patt.begin());

            ////patt = "banana";
            auto start = timer::now();
            self_index.display(r1,r2,s);
            auto stop = timer::now();
            unsigned long tttt = duration_cast<microseconds>(stop-start).count();
            cout <<"query["<<1000-R<<"] "<<" # extractSubstring_time = " <<tttt<< "\t\t %/c "<<tttt/11<<endl;
            mean+= tttt*1.0/11.0;
            EXPECT_STREQ(s.c_str(),patt.c_str());



        }

        cout <<"mean of time for a character extraction"<<mean/1000<<endl;
    }


}
static void sibs_locate(benchmark::State& state)
{

    size_t points = state.range(0);
    for (auto _ : state)
    {
        uint folder = state.range(0);
        uint coll   = state.range(1);
        //std::string collection = "prob_pattern_bin";
        std::string collection = "../tests/collections/repetitive/reals/einstein.de.txt";//"../" + dirFolder[folder]+dircollection[coll];
        std::fstream f(collection, std::ios::in| std::ios::binary);
        std::string data;
        std::cout << "collection: "<< collection << std::endl;
        if(!f.is_open()){
            std::cout<<"Error the file could not opened!!\n";
            return;
        }
        std::string buff;
        while (!f.eof() && std::getline(f, buff)) {
            data += buff;

        }
        for (int i = 0; i < data.size(); ++i) {
            if(data[i] == '$' || data[i] == '#')
                data[i] = 'Z';
        }
        std::cout << "size of the string: " << data.length() << std::endl;
        f.close();
        /*std::fstream f_idx("2222_" + dircollection[coll], std::ios::out | std::ios::binary);
        for (int j = 0; j < 2000000; ++j) {
            f_idx << data[j];
        }*/

        SelfGrammarIndexBS self_index;

        ///data = "abraabracadapanamabananabracadaabrpanamabananaaabracadabracadabraapanamabananapanamabananabraabraapanamabananabracadabracadabraabracabracadabraaabracadabradabracabracadabraaabracpanamabananaadabradabrababrpanamabananaaabracadabracadabraabracabracadpanamabananaabraaabracadabrpanamabananaadabraraabracabrpanamabananaacadabraapanamabananaabracadabradabra";
        ///data = "panamabanana";
        self_index.build(data);
        std::fstream sf("einstein-bin.idx2", std::ios::in| std::ios::binary);
        self_index.save(sf);
        sf.close();
/*
       {
           std::cout<<"Hello"<<std::endl;
            std::fstream ptf("einstein.words_f1_100", std::ios::in| std::ios::binary);
            sdsl::bit_vector _occ(data.size(),0);
            sdsl::bit_vector _X(data.size(),0);
            uint t = 0;

            double mean = 0, total_time = 0, n_occ =0;

            std::getline(ptf, buff);

            while (!ptf.eof() && std::getline(ptf, buff)) {
                int i,l;
                char _patt [100];
                sscanf(buff.c_str(),"%d %d %s",&i,&l,_patt);
                std::string patt(_patt);



                size_t pos = data.find(patt, 0);
                uint tooc = 0;
                while(pos != string::npos)
                {
                    _X[pos] = true;
                    tooc++;;
                    pos = data.find(patt,pos+1);
                }

                auto start_ = timer::now();
                self_index.locate(patt, _occ);
                auto stop_ = timer::now();

                EXPECT_EQ(_occ,_X);

                sdsl::bit_vector::rank_1_type _r(&_occ);
               /// cout << "query[" << ++t << "] cadena: " <<patt<<"\t my time\t"<< duration_cast<microseconds>(stop_ - start_).count()<<"\t num de occ:"<<_r.rank(_occ.size())<<std::endl;
                n_occ += _r.rank(_occ.size());
                mean += duration_cast<microseconds>(stop_ - start_).count()*1.0/100;
                total_time += duration_cast<microseconds>(stop_ - start_).count();
            }

            cout << "query[ mean ] time: " <<mean<<std::endl;
            cout << "total time: " <<total_time<<std::endl;
            cout << "occurrencias: " <<n_occ<<std::endl;

        }
*/




        uint R = 100;
        //std::srand(std::time(nullptr));
        uint t = 0;
        double mean = 0, total_time = 0;

        while (--R)
        {


            sdsl::bit_vector _occ(data.size(),0);
            sdsl::bit_vector _X(data.size(),0);

            auto r1 = std::rand()%data.size();
            auto r2 = std::rand()%data.size();
            std::string patt;

            if (r1 > data.size() / 2) {
                r2 = r1 - 10;
            } else {
                r2 = r1 + 10;
            }

            if(r1 > r2) std::swap(r1,r2);
            patt.resize(r2-r1);


            std::copy(data.begin() + r1, data.begin() + r2 +1, patt.begin());

            if(patt.empty())continue;

            auto start = timer::now();
            size_t pos = data.find(patt, 0);
            uint tooc = 0;
            while(pos != string::npos)
            {
                _X[pos] = true;
                tooc++;;
                pos = data.find(patt,pos+1);
            }
            auto stop = timer::now();


            auto start_ = timer::now();
            self_index.locate(patt, _occ);
            auto stop_ = timer::now();

            EXPECT_EQ(_occ,_X);

            sdsl::bit_vector::rank_1_type _r(&_occ);

            cout << "query[" << ++t << "] cadena: " <<patt<<"\t my time\t"<< duration_cast<microseconds>(stop_ - start_).count()<<"\t num de occ:"<<_r.rank(_occ.size())<<std::endl;
            cout << "query[" << t << "] cadena: " <<patt<<"\t BF time\t"<< duration_cast<microseconds>(stop - start).count()<<"\t num de occ:"<<tooc<<std::endl;
            mean += duration_cast<microseconds>(stop_ - start_).count()*1.0/100;
            total_time += duration_cast<microseconds>(stop_ - start_).count();
        }

        cout << "query[ mean ] time: " <<mean<<std::endl;
        cout << "total time: " <<total_time<<std::endl;


    }
}

////BENCHMARK(sibs_build)->Args({dataDir::dir_DNA, dataCollection::DNA50})->Unit(benchmark::kMillisecond);


BENCHMARK(sibs_locate)->Args({dataDir::dir_DNA, dataCollection::DNA50})->Unit(benchmark::kMillisecond);
//BENCHMARK(sibs_extract_subtrings)->Args({dataDir::dir_DNA, dataCollection::DNA50})->Unit(benchmark::kMillisecond);
/*

BENCHMARK(partial_build_extract)->Args({dataDir::dir_DNA, dataCollection::DNA50})->Unit(benchmark::kMillisecond);
BENCHMARK(partial_build_extract)->Args({dataDir::dir_sources, dataCollection::sources50})->Unit(benchmark::kMillisecond);
BENCHMARK(partial_build_extract)->Args({dataDir::dir_pitches, dataCollection::pitches50})->Unit(benchmark::kMillisecond);

*/

//BENCHMARK(partial_build_locate)->Args({dataDir::dir_DNA, dataCollection::DNA50})->Unit(benchmark::kMillisecond);
//BENCHMARK(partial_build_locate)->Args({dataDir::dir_sources, dataCollection::sources50})->Unit(benchmark::kMillisecond);
///BENCHMARK(partial_build_locate)->Args({dataDir::dir_pitches, dataCollection::pitches50})->Unit(benchmark::kMillisecond);

/*
BENCHMARK(sibs_locate)->Args({dataDir::dir_DNA, dataCollection::DNA50})->Unit(benchmark::kMillisecond);
BENCHMARK(sibs_locate)->Args({dataDir::dir_sources, dataCollection::sources50})->Unit(benchmark::kMillisecond);
BENCHMARK(sibs_locate)->Args({dataDir::dir_pitches, dataCollection::pitches50})->Unit(benchmark::kMillisecond);
*/

///BENCHMARK(sibs_locate)->Args({dataDir::dir_english, dataCollection::english50})->Unit(benchmark::kMillisecond);

/*BENCHMARK(sibs_build)->Args({dataDir::dir_DNA, dataCollection::DNA100})->Unit(benchmark::kMillisecond);
BENCHMARK(sibs_build)->Args({dataDir::dir_DNA, dataCollection::DNA200})->Unit(benchmark::kMillisecond);
BENCHMARK(sibs_build)->Args({dataDir::dir_DNA, dataCollection::DNA})->Unit(benchmark::kMillisecond);
*/
BENCHMARK_MAIN();
