//
// Created by via56 on 01-04-20.
//

#include <iostream>
#include <fstream>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include <cstring>
#include "unistd.h"


int process_data(const std::string &collection,const std::string &file_out, uint &sigma){


    std::set<char> sigma_set;
    std::fstream f(collection, std::ios::in| std::ios::binary);
    //std::string data;
    if(!f.is_open()){
        std::cout<<"Error the file could not opened!!\n";
        return 0;
    }

    std::fstream f2(file_out,std::ios::out );

    if(!f2.is_open()){
        std::cout<<"Error the file could not opened!!\n";
        return 0;
    }

    std::string buff;

    char c = 0; long size = 0;

    while(!f.eof() &&  f.read(&c,1)){

        if(c == 0 || c == 1 || c == 2)
            c = 3;

        f2.write(&c,1) ;

        size++;
        sigma_set.insert(c);

    }
//    c = '\0';
//    f2.write(&c,1) ;

    f.close();
    f2.close();

    sigma = sigma_set.size();
    return size;
}


auto first_test = [](benchmark::State &st){

    for (auto _ : st)
    {
        sleep(3);
    }

    st.counters["Size"] = 0;

};


auto process_coll = [](benchmark::State &st,const std::string &collection,const std::string &file_out){

    long size = 0;
    uint sigma = 0;

    for (auto _ : st)
    {
        size = process_data(collection,file_out,sigma);
        sleep(3);
    }

    st.counters["size"] = size;
    st.counters["sigma"] = sigma;

};

int main (int argc, char *argv[] ){

    std::cout<<argc<<std::endl;

    if(argc < 2){
        std::cout<<"bad parameters...."<<std::endl;
        return 0;
    }

    std::string collection  = argv[1];

    //"../tests/collections/repetitive/reals/para";
    std::string path_out    = argv[2];

    benchmark::RegisterBenchmark("First Test",  first_test);
    benchmark::RegisterBenchmark("PROCESSING COLLECTION",  process_coll ,collection,path_out);



    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;

}