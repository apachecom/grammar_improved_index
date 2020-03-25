//
// Created by root on 12-06-19.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "../bench/repetitive_collections.h"
#include "../bench/utils.h"
#include "../SelfGrammarIndexBSQ.h"

#define MAX_DATA 1e19

using namespace std;

SelfGrammarIndexBSQ idx_g;
int code = 1;

string cdata = "";

auto expand_rules = [](benchmark::State &state, const uint& _code)
{

    int64_t code = _code;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    fstream fout ("../files/2Dranges/"+std::to_string(code)+".rules",std::ios::out|std::ios::binary);
    
    if(!f.is_open() || !fout.is_open())
    {
        std::cout<<"fichero no encontrado\n";
        std::cout<<read_path+std::to_string(code)+".basic_index\n";
        return;
    }else{
        idx_g.load(f);
    }

    for (auto _ : state) {
        
        auto& G = idx_g.get_grammar();
        auto& T = G.get_parser_tree();
        
        uint n_rules = G.n_rules();
        std::cout<<"n_rules:"<<n_rules<<std::endl;
        uint bytes = 0;
        for(size_t i = 1; i < n_rules; i++)        
        {
            
            auto l = G.limits_rule(T[G.select_occ(i,1)]);
            
            //std::cout<<"rule:"<<i+1<<std::endl;
            //std::cout<<"node:"<<T[G.select_occ(i+1,1)]<<std::endl;
            //std::cout<<"len rule:"<<l.second-l.first<<std::endl;
            //std::string s;
            //s.resize(l.second-l.first);
            //idx_g.display(l.first,l.second,s);
            uint len = l.second-l.first;
            uint left = l.first;
            //std::cout<<i+1<<":("<<l.first<<","<<len<<")"<<std::endl;

            fout.write((char*)&left,4);
            fout.write((char*)&len,4);
            bytes+=len+4;

            //std::cout<<"exp rule "<<i+1<<"->"<<s<<std::endl;
        
        }
        std::cout<<bytes<<std::endl;
        sleep(4);
        
    }
    state.counters["g"] = idx_g.n_rules();

};


auto expand_sfx = [](benchmark::State &state, const uint& _code)
{

    int64_t code = _code;

    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);
    fstream fout ("../files/2Dranges/"+std::to_string(code)+".sfx",std::ios::out|std::ios::binary);
    fstream fgout ("../files/2Dranges/"+std::to_string(code)+".grid",std::ios::out|std::ios::binary);
    if(!f.is_open())
    {
        std::cout<<"fichero no encontrado\n";
        std::cout<<read_path+std::to_string(code)+".basic_index\n";
        return;
    }else{
        idx_g.load(f);
    }

    for (auto _ : state) {
        
        auto& G = idx_g.get_grammar();
        auto& T = G.get_parser_tree();
        

        uint n_sfx = idx_g.get_grid().n_columns();
        std::cout<<"numero de sufijos:"<<n_sfx<<std::endl;
        size_t bytes = 0;
        
        for(size_t i = 0; i < n_sfx; i++)
        {
            auto node_sfx_pre = idx_g.get_grid().first_label_col(i+1);
            auto node = T[node_sfx_pre];
            auto parent = T.parent(node);
            
            auto rule_parent  = G[T.pre_order(parent)];
            
            auto ir = T.childrank(node);
            if(ir == 1)
                {
                    std::cout<<"error childrank 1\n";
                    sleep(3);
                    return;
                }
            auto rule_prev =  G[T.pre_order(T.child(parent,ir-1))];

            auto l = G.limits_rule(node);
            auto lp = G.limits_rule(parent);
            
            bytes += lp.second-l.first + 4;
            /**regla*/
            fout.write((char*)&rule_parent,4);
            /**offset*/
            uint off = l.first - lp.first;
            fout.write((char*)&off,4);
            uint sfx = i+1;


            
            
            fgout.write((char*)&sfx,4);
            fgout.write((char*)&rule_prev,4);
            //std::cout<<"X:"<<rule_prev<<","<<off<<std::endl;

            //if(lp.second-lp.first > 5 ) continue;
            /* std::string s;

            s = "";
            s.resize(lp.second - lp.first);
            idx_g.display(lp.first,l.second,s);
            std::cout<<" rule "<<s<<" ";

            s = "";
            s.resize(l.first-1 - lp.first);
            idx_g.display(lp.first,l.first-1,s);
            std::cout<<s<<" ";
            
            s = "";
            s.resize(lp.second-l.first);
            idx_g.display(l.first,lp.second,s);
            std::cout<<s<<"\n"; */
            
        }
        std::cout<<"bytes:"<<bytes<<std::endl;
        sleep(4);
        
    }
    state.counters["g"] = idx_g.n_rules();

};



auto check_data = [](benchmark::State &state, const uint& _code)
{

    int64_t code = _code;


    std::fstream  f  (read_path+std::to_string(code)+".basic_index",std::ios::in|std::ios::binary);


    fstream frules("../files/2Dranges/"+std::to_string(code)+".rules",std::ios::in|std::ios::binary);
    fstream fsfx ("../files/2Dranges/"+std::to_string(code)+".sfx",std::ios::in|std::ios::binary);
    fstream fgin ("../files/2Dranges/"+std::to_string(code)+".grid",std::ios::in|std::ios::binary);



    if(!f.is_open() || !frules.is_open() || !fsfx.is_open() || !fgin.is_open()){
        std::cout<<"fichero no encontrado\n";
        std::cout<<read_path+std::to_string(code)+".basic_index\n";
        std::cout<<"../files/2Dranges/"+std::to_string(code)+".rules";
        std::cout<<"../files/2Dranges/"+std::to_string(code)+".sfx";
        std::cout<<"../files/2Dranges/"+std::to_string(code)+".grid";
        return;
    }else{
        idx_g.load(f);
    }


    string data = "";

    load_data(1e19,data,collections_code_inv_dir[code]);

    std::map<uint,pair<uint,uint>> R;
    uint ii = 0, off = 0 ,len = 0;
    while(!frules.eof() && frules.read((char*)&off,4) && frules.read((char*)&len,4) ){
        R[++ii] = make_pair(off,len);
    }
    
    std::map<uint,pair<uint,uint>> SFX;
    ii = 0; 
    off = 0;
    uint rule = 0 ;
    
    while(!fsfx.eof() && fsfx.read((char*)&rule,4) && fsfx.read((char*)&off,4) ){
        SFX[++ii] = make_pair(rule,off);
    }
    

    vector<pair<uint, uint>> PP;
    uint X = 0, Y = 0 ;
    while(!fgin.eof() && fgin.read((char*)&X,4) && fgin.read((char*)&Y,4) )
        PP.push_back(make_pair(X,Y));

    auto &G = idx_g.get_grammar();
    auto& T = G.get_parser_tree();
    
    std::cout<<"numero de puntos"<<PP.size()<<std::endl;
    std::cout<<"numero de sufijos"<<SFX.size()<<std::endl;    
    std::cout<<"numero de reglas"<<R.size()<<std::endl;    
    for (auto _ : state) {

        for(size_t i = 0; i < PP.size(); i++)
        {
            uint rule = PP[i].second;   
            uint sfx = PP[i].first;
            uint prule = SFX[sfx].first;
            //std::cout<<rule<<" "<<sfx<<std::endl;
            //std::cout<<"rule parent:"<<std::endl; 

            //std::cout<<"sfx:"<< SFX[sfx].first << " " << SFX[sfx].second <<std::endl; 
             
            uint total_off = R[prule].first + SFX[sfx].second;
            string ss; 
            
            
            if(R[prule].second - SFX[sfx].second + 1 <  10000 )
            {
                ss.resize(R[prule].second - SFX[sfx].second + 1);
                //std::cout<<"ss.size():"<<R[prule].second - SFX[sfx].second + 1<<std::endl;
                
                ////std::cout<<total_off<<" "<<R[prule].first+SFX[sfx].second + 1<<std::endl;
                //std::cout<<"sfx "<<R[prule].first+SFX[sfx].second<<" "<<R[prule].first+R[prule].second+1<<std::endl;
                
                if(R[prule].first+R[prule].second+1 <= data.size())
                    copy(data.begin()+total_off,data.begin()+R[prule].first+R[prule].second+1,ss.begin());
                else{
                    std::cout<<"Error out of range "<<R[prule].first+R[prule].second+1<<" "<<data.size()<<std::endl;
                }
                //std::cout<<"ss:"<<ss<<std::endl;
                
                string ext;ext.resize(ss.size());
                    
                    idx_g.display(total_off,R[prule].first+R[prule].second,ext);
                //sleep(1);
            
                    ASSERT_EQ(ext,ss);

                
            
            
                auto pre = G.select_occ(rule,1);
                auto node  = T[pre];
                auto l = G.limits_rule(node);

                ///std::cout<<"rule:"<<l.first<<" "<<l.second<<std::endl;
                std::string sr; sr.resize(l.second - l.first + 1);


                if(l.second + 1 <= data.size())
                    std::copy(data.begin() + l.first, data.begin() + l.second + 1, sr.begin() );
                else{
                    std::cout<<"Error II out of range "<<R[prule].first+R[prule].second+1<<" "<<data.size()<<std::endl;
                }

                uint ll = sr.size();
                int pf = total_off - ll;
                if(pf < 0){
                    std::cout<<" Error II < 0  range "<<std::endl;
                    return;
                }
                std::string srII; srII.resize(ll);
                std::copy(data.begin() + pf, data.begin() + pf + ll , srII.begin() );
                ///std::cout<<sr<<" "<<srII<<std::endl;
                ASSERT_EQ( sr ,srII);
            
            }
        }
    }
  
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

    for (int i = 1; i < 9 ; ++i) {

        //benchmark::RegisterBenchmark("2D gen" ,expand_rules,i+1)->Unit({benchmark::kMicrosecond});
        //benchmark::RegisterBenchmark("2D gen" ,expand_sfx,i+1)->Unit({benchmark::kMicrosecond});
        benchmark::RegisterBenchmark("2D gen" ,check_data,i+1)->Unit({benchmark::kMicrosecond});

    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();



}


