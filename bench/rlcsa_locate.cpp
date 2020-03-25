//
// Created by alpachec on 19-12-18.
//




#include <gflags/gflags.h>
#include <benchmark/benchmark.h>


std::string path = "../files/indices/";
CSA::RLCSA* idx_rlca;








int main (int argc, char *argv[] ){

/*
    if(argc < 2){
        std::cout<<"bad parameters....";
        return 0;
    }
*/

    std::string collection = (argv[1]);

    if(collections_code.find(collection) == collections_code.end()){
        std::cout<<"bad parameters (not collection code found)....\n";
        return 0;
    }


    std::fstream f_c(collection, std::ios::in| std::ios::binary);

/*  std::string buff;
    while (!f_c.eof() && std::getline(f_c, buff)) {
        lenght += buff.size();
        data_bs+=buff;
    }
*/
    unsigned char buffer[1000];
    while(!f_c.eof()){
        f_c.read((char*)buffer,1000);
        data_bs.append((char*) buffer,f_c.gcount());
    }

    for (int i = 0; i < data_bs.size(); ++i) {
        if(data_bs[i] == 0 || data_bs[i] == 1)
            data_bs[i] = 2;
    }
    /*  std::string S;
      S.reserve(10);
      for (int j = 0; j < 10; ++j) {
          std::cout<<data_bs[92911778+j];
      }
      std::cout<<std::endl;
  */
    f_c.close();
    code = collections_code[collection];

    unsigned int num_patterns = atoi(argv[2]);
    unsigned int len_patterns = atoi(argv[4]);
    //patt.reserve(num_patterns);

    std::string pattern_file = (argv[3])+ std::to_string(code)+"_len_200.ptt";
    std::fstream f(pattern_file, std::ios::in| std::ios::binary);
    std::cout<<pattern_file<<std::endl;

    if(!f.is_open()){
        std::cout<<"Error the pattern file could not opened!!\n";
        //  return 0;
    }
    std::string buff; uint i = 0;

    while (!f.eof() && std::getline(f, buff) && i < num_patterns) {
        std::string sss;
        sss.resize(len_patterns);
        std::copy(buff.begin(),buff.begin()+len_patterns,sss.begin());
        ///std::cout<<i<<"\t"<<sss<<std::endl;

        for (int k = 0; k < sss.size(); ++k){
            if(int(sss[k]) == 0 || int(sss[k]) == 1 )
                sss[k] = 65;
        }
        patt.push_back(sss);
        i++;
    }
    f.close();


    //benchmark::RegisterBenchmark("Brute Force-Index"    , bt_locate);

    benchmark::Initialize(&argc, argv);
    benchmark::RunSpecifiedBenchmarks();


}
