//
// Created by alpachec on 26-08-18.
//

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include <sdsl/lcp.hpp>
#include <sdsl/rmq_succinct_sada.hpp>
#include "collections.cpp"
#include "../utils/grammar.h"
#include "../compressed_grammar.h"


static void suffixes_grammmar_size(benchmark::State& state)
{


    size_t points = state.range(0);
    for (auto _ : state)
    {

       /* {

            std::string rev_text = "abracadabra";
            sdsl::int_vector<> SA(rev_text.size(),0);
            sdsl::algorithm::calculate_sa( (unsigned char*)rev_text.c_str(),rev_text.size(),SA);
            sdsl::inv_perm_support<> SA_1(&SA);
            sdsl::lcp_wt<> LCP;
            sdsl::construct_im(LCP,rev_text.c_str(),sizeof(char));
            sdsl::rmq_succinct_sada<true,sdsl::bp_support_sada<>> rmq(&LCP);

            std::cout<<"SA\t";
            for (auto &&  i: SA)
            {
                std::cout<<i<<" ";
            }
            std::cout<<"\n";
            std::cout<<"SA_1\t";
            for (auto &&  i: SA_1)
            {
                std::cout<<i<<" ";
            }
            std::cout<<"\n";
            std::cout<<"LCP\t";
            for (auto &&  i: LCP)
            {
                std::cout<<i<<" ";
            }
            std::cout<<"\n";

            return;

        }*/










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
                std::cout<<"contiene zero"<<std::endl;
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

        /*
         * Build sufix of grammar
         *
         * */
        std::cout<<"Compute suffix grammar"<<std::endl;

        std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx;
        const auto& gtree = _g.get_parser_tree();
        unsigned long num_sfx = 0;

        for (auto r_begin = G.begin(); r_begin != G.end(); ++r_begin) {

            rule r = r_begin->second;
            rule::r_long r_id = r_begin->first;
            size_t node = gtree[ _g.select_occ(r_id,1)];

            rule::r_long off = 0;

            for (auto j = r._rule.size()-1; j >= 1 ; --j) {
                off += G[r._rule[j]].len();

                size_t  left =  r.r - off + 1;
                size_t  right = r.r;

                size_t tag = gtree.pre_order(gtree.child(node,j+1));
                grammar_sfx.emplace_back(std::make_pair(std::make_pair(left,right),std::make_pair(r._rule[j-1],tag)));
                num_sfx++;
            }
        }
        std::cout<<"\t number of suffixes "<<num_sfx<<std::endl;

        /*
         * Sorting suffixes
         *
         * */

        std::cout<<"Building structures for sorting"<<std::endl;
        sdsl::int_vector<> SA(data.size(),0);
        sdsl::algorithm::calculate_sa( (unsigned char*)data.c_str(),data.size(),SA);
        sdsl::inv_perm_support<> SA_1(&SA);
        sdsl::lcp_bitcompressed<> LCP;
        sdsl::construct_im(LCP,data,sizeof(char));
        sdsl::rmq_succinct_sada<true,sdsl::bp_support_sada<>> rmq(&LCP);
        std::cout<<"LCP size "<< LCP.size()<<std::endl;
        std::cout<<"SA_1 size "<< SA_1.size()<<std::endl;



        std::cout<<"sorting suffixes ........."<<std::endl;
        std::sort(grammar_sfx.begin(),grammar_sfx.end(),
                  [&data,&SA_1,&LCP,&rmq](const std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > & a,
                                               const std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > &b )->bool
                  {

                      ulong a_pos = a.first.first;
                      ulong b_pos = b.first.first;

                      ulong size_a = a.first.second - a.first.first +1;
                      ulong size_b = b.first.second - b.first.first +1;

                     /* std::cout<<"apos "<<a_pos<<std::endl;
                      std::cout<<"bpos "<<b_pos<<std::endl;
                      std::cout<<"size_a "<<size_a<<std::endl;
                      std::cout<<"size_b "<<size_b<<std::endl;
*/
                      /*
                       * offset of suffix in the text
                       * */
                     /*


                      std::string sa,sb;
                      sa.resize(size_a);
                      sb.resize(size_b);

                      std::copy(data.begin()+a_pos,data.begin()+a_pos+size_a,sa.begin());
                      std::copy(data.begin()+b_pos,data.begin()+b_pos+size_b,sb.begin());

                      return sa < sb;*/

                      /*
                       * is start at the same position return the shortest
                       * */
                      if(a_pos == b_pos)
                          return size_a < size_b;

                      auto sa_1_a = SA_1[a_pos];
                      auto sa_1_b = SA_1[b_pos];

                    /*  std::cout<<"sa_1_a "<<sa_1_a<<std::endl;
                      std::cout<<"sa_1_b "<<sa_1_b<<std::endl;
                      std::cout<<"rmq "<<rmq(std::min(sa_1_a,sa_1_b)+1,std::max(sa_1_a,sa_1_b)+1)<<std::endl;
                      std::cout<<"lcp "<<LCP[rmq(std::min(sa_1_a,sa_1_b)+1,std::max(sa_1_a,sa_1_b)+1)]<<std::endl;
                      std::cout<<"**********"<<std::endl;
                      std::cout<<std::endl;
*/

                      int min= LCP[rmq(std::min(sa_1_a,sa_1_b)+1,std::max(sa_1_a,sa_1_b)+1)];

                      /*
                       * Check if one is prefix of the other
                       * return the shortest
                       * */

                      if(std::min(size_a,size_b) <= min){
                          return size_a < size_b;
                      }

                      /*
                       * then return the lowest lexicographical
                       * */

                      return sa_1_a < sa_1_b;

                  });

        ////G.print(data);
        std::cout<<"end sorting suffixes "<<std::endl;


        fstream sfx_f("sfx_"+dircollection[coll],std::ios::out|std::ios::binary);

        sdsl::serialize(grammar_sfx.size(),sfx_f);

        for (auto && item: grammar_sfx)
        {
            sdsl::serialize(item.first.first,sfx_f);
            sdsl::serialize(item.first.second,sfx_f);
            sdsl::serialize(item.second.first,sfx_f);
            sdsl::serialize(item.second.second,sfx_f);
        }

        sfx_f.close();

    }
}

BENCHMARK(suffixes_grammmar_size)->Args({dataDir::dir_pitches, dataCollection::pitches50})->Unit(benchmark::kMillisecond);
BENCHMARK(suffixes_grammmar_size)->Args({dataDir::dir_DNA, dataCollection::DNA50})->Unit(benchmark::kMillisecond);
BENCHMARK(suffixes_grammmar_size)->Args({dataDir::dir_sources, dataCollection::sources50})->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
