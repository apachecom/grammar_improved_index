//
// Created by via56 on 01-04-20.
//

#include <iostream>
#include <fstream>
//#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include <cstring>
#include <ctime>
#include "unistd.h"

bool valid(const std::string &patt,int& c) {
    c = 0;
    for (const auto &item : patt) {
        c += (item == 'N')?1:0;
    }

    return (float(c)/float(patt.size())) > 0.80;
}

int process_data(const std::string &collection,const std::string &file_out, uint &sigma){


    std::set<char> sigma_set;
    std::fstream f(collection, std::ios::in| std::ios::binary);
    //std::string data;
    if(!f.is_open()){
        std::cout<<collection<<std::endl;
        std::cout<<"Error the file could not opened!!\n";
        return 0;
    }

    std::fstream f2(file_out,std::ios::out );

    if(!f2.is_open()){

        std::cout<<file_out<<std::endl;
        std::cout<<"Error the file could not opened!!\n";
        return 0;
    }

    std::string buff;

    char c = 0; long size = 0;

    while(!f.eof() &&  f.read(&c,1)){

        if(c == 0 || c == 1 || c == 2)
            c = 65;

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
    // std::fstream f2("./temp_collection",std::ios::out );
    // for(size_t i = 0 ; i < data.size(); ++i)
    //     f2 << data[i];

    // f2 << '\0';
    // f2.close();
    std::cout<<"DATA LOADED\n";
    return 1;
}



void create_file_patterns(){}


/*
 * max_len maxima longitud del patron a buscar/extraer
 * n numero de samplings
 *
 */
auto create_files= [](benchmark::State &st, const std::string& file_coll, const std::string& path_out, const uint32_t& max_len, const uint32_t & n)
{
    std::string data = "";
    std::cout<<"collection:"<<file_coll<<std::endl;
    load_data(file_coll,data);

    uint invalid_patt = 0;
    for (auto _ : st)
    {
        std::fstream fpoints(path_out+"-"+std::to_string(max_len)+".pos", std::ios::out|std::ios::binary);
        std::fstream fpatterns(path_out+"-"+std::to_string(max_len)+".ptt", std::ios::out|std::ios::binary);
        fpatterns.write((const char *)&max_len,sizeof (uint32_t ));
        fpatterns.write((const char *)&n,sizeof (uint32_t ));
        std::cout<<"positions file:"<<path_out+"-"+std::to_string(max_len)+".pos"<<std::endl;
        std::cout<<"patterns file:"<<path_out+"-"+std::to_string(max_len)+".ptt"<<std::endl;

        std::srand(std::time(nullptr));

        for (uint i = 0; i < n ; ++i) {
            std::string patt;
            size_t r1 = 0;
            size_t  r2 = 0;
            int c = 0;
            do{

                r1 = std::rand()%data.size();
                r2 = 0;

                if (r1 > data.size() / 2) {
                    r2 = r1 - max_len;
                } else {
                    r2 = r1 + max_len;
                }
                if(r1 > r2) std::swap(r1,r2);
                patt.resize(max_len);
                std::copy(data.begin() + r1, data.begin() + r2 +1, patt.begin());

                if(valid(patt,c)){
                    invalid_patt++;
//                    std::cout<<c<<":->"<<patt<<std::endl;
                }

            }
            while(valid(patt,c));

            fpoints<< r1 << "\n";
            fpatterns.write(patt.c_str(),max_len);
        }

        std::cout<<"End files created\n";
        sleep(5);
    }

    st.counters["invalid"] = invalid_patt;

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
    std::string path_out    = argv[2];

    int op    = std::atoi(argv[3]);
    int max_len = std::atoi(argv[4]);
    int max_queries = std::atoi(argv[5]);

    if(op == 1)
     benchmark::RegisterBenchmark("PROCESSING COLLECTION",  process_coll ,collection,path_out);
    else
        benchmark::RegisterBenchmark("CREATING FILES",  create_files ,collection,path_out,max_len,max_queries);



    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();

    return 0;

}