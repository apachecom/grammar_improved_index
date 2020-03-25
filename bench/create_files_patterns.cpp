//
// Created by alpachec on 24-09-18.
//



#include <iostream>
#include <fstream>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include <chrono>
#include "repetitive_collections.h"

std::string path = "../files/";
uint8_t code;


using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;


auto create_patterns= [](benchmark::State &st, const std::string& data, const uint& len, const uint & n)
{

    for (auto _ : st)
    {
        std::fstream f(path + std::to_string(code)+"_len_"+std::to_string(len)+".ptt", std::ios::out|std::ios::binary);
        std::srand(std::time(nullptr));

        for (int i = 0; i < n ; ++i) {


            auto r1 = std::rand()%data.size();
            auto r2 = std::rand()%data.size();

            std::string patt;

            if (r1 > data.size() / 2) {
                r2 = r1 - len;
            } else {
                r2 = r1 + len;
            }

            if(r1 > r2) std::swap(r1,r2);
            patt.resize(r2-r1);


            std::copy(data.begin() + r1, data.begin() + r2 +1, patt.begin());

            f << patt << "\n";

        }

        f.close();
    }

};



auto create_ranges= [](benchmark::State &st,const std::string& data, const uint& len, const uint & n)
{
    for (auto _ : st)
    {
        std::fstream f(path+"/rangos/"+std::to_string(code)+"_"+std::to_string(len)+".rg", std::ios::out|std::ios::binary);
        std::cout<<path+"/rangos/"+std::to_string(code)+"_"+std::to_string(len)+".rg"<<std::endl;

        std::srand(std::time(nullptr));

        for (int i = 0; i < n ; ++i) {


            auto r1 = std::rand()%data.size();
            auto r2 = std::rand()%data.size();

            if (r1 > data.size() / 2) {
                r2 = r1 - len;
            } else {
                r2 = r1 + len;
            }

            if(r1 > r2) std::swap(r1,r2);

            f << r1 <<" "<< r2 << "\n";

        }
    }

};



int main (int argc, char *argv[] ){

    if(argc < 1){
        std::cout<<"bad parameters....";
        return 0;
    }
    ///slp_build2();
    //// build improve grammar index patrcia tree /////
    std::string collection(argv[1]);

    if(collections_code.find(collection) == collections_code.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
        return 0;
    }

    code = collections_code[collection];

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
        if(int(data[i]) == 0 || int(data[i]) == 1)
            data[i] = 2;
    }

    uint len = atoi(argv[2]);
    uint n = atoi(argv[3]);
    uint size_ranges = atoi(argv[4]);

    if(len >= data.size()/2)
    {
        std::cout<<"error len > data.size()/2\n";
    }

    std::string name_test_patt = "patterns "+ collection;
    std::string name_test_rg = "ranges "+ collection;

    //benchmark::RegisterBenchmark(name_test_patt.c_str(), create_patterns, data, len , n);
    benchmark::RegisterBenchmark(name_test_rg.c_str()  , create_ranges, data, size_ranges , n);


    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


    return 0;

}
