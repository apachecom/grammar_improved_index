//
// Created by via56 on 01-04-20.
//

#include <iostream>
#include <fstream>
#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include <cstring>
#include <ctime>
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


int load_data(const std::string &collection, std::string&  data){

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



void create_file_patterns(){}


/*
 * max_len maxima longitud del patron a buscar/extraer
 * n numero de samplings
 *
 */
auto create_files= [](benchmark::State &st, const std::string& file_coll, const std::string& path_out, const uint& max_len, const uint & n)
{
    std::string data = "";
    load_data(file_coll,data);

    for (auto _ : st)
    {
        std::fstream fpoints(path_out+"-"+std::to_string(max_len)+".pos", std::ios::out|std::ios::binary);
        std::fstream fpatterns(path_out+"-"+std::to_string(max_len)+".ptt", std::ios::out|std::ios::binary);

        std::srand(std::time(nullptr));

        for (int i = 0; i < n ; ++i) {

            size_t r1 = std::rand()%data.size();
            size_t  r2 = 0;

            if (r1 > data.size() / 2) {
                r2 = r1 - max_len;
            } else {
                r2 = r1 + max_len;
            }
            if(r1 > r2) std::swap(r1,r2);


            std::string patt;
            patt.resize(max_len);
            std::copy(data.begin() + r1, data.begin() + r2 +1, patt.begin());


            fpoints<< r1 << "\n";
            fpatterns.write(patt.c_str(),max_len);
        }
    }

};






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

//    std::cout<<argc<<std::endl;

    if(argc < 2){
        std::cout<<"bad parameters...."<<std::endl;
        return 0;
    }

    std::string collection  = argv[1];

    //"../tests/collections/repetitive/reals/para";
    std::string path_out    = argv[2];


//    benchmark::RegisterBenchmark("First Test",  first_test);
//    benchmark::RegisterBenchmark("PROCESSING COLLECTION",  process_coll ,collection,path_out);
    benchmark::RegisterBenchmark("CREATING FILES",  create_files ,collection,path_out,100,1000);



    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;

}