//
// Created by alpachec on 25-09-18.
//


#include <iostream>
#include <fstream>


#include <gflags/gflags.h>
#include <benchmark/benchmark.h>
#include "repetitive_collections.h"

std::string data;



int main (int argc, char *argv[] ){

    if(argc < 1){
        std::cout<<"bad parameters....";
        return 0;
    }

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


    return 0;

}