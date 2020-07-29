//
// Created by inspironXV on 8/16/2018.
//

#ifndef IMPROVED_GRAMMAR_INDEX_BINARY_RELATION_H
#define IMPROVED_GRAMMAR_INDEX_BINARY_RELATION_H


#include <utility>
#include <vector>
#include <sdsl/wavelet_trees.hpp>
#include <fstream>

class binary_relation {

    public:
        unsigned int code{};
        typedef unsigned int bin_long;
        typedef sdsl::wt_int<> wavelet_tree;
        //typedef sdsl::wm_int<> wavelet_tree;
        typedef sdsl::rrr_vector<> bin_bit_vector_xb;
        typedef sdsl::rrr_vector<> bin_bit_vector_xa;
//        typedef sdsl::vlc_vector<> compressed_seq;
        typedef sdsl::int_vector<> compressed_seq;
        typedef std::pair< std::pair< unsigned int,  unsigned int> , unsigned int> point;

    ///protected:
        wavelet_tree SB;
        compressed_seq SL;

        bin_bit_vector_xb XB;
        bin_bit_vector_xa XA;

        bin_bit_vector_xb::rank_1_type xb_rank1;
        bin_bit_vector_xb::select_1_type xb_sel1;
        bin_bit_vector_xb::select_0_type xb_sel0;
        ///sdsl::rrr_vector<>::select_1_type xa_sel1;
        bin_bit_vector_xa::rank_1_type xa_rank1;

   /// public:
        binary_relation() = default;
        ~binary_relation() = default;

        binary_relation(const binary_relation& );
        void build(std::vector<point>::iterator , std::vector<point>::iterator, const bin_long &, const bin_long&);

        void range(binary_relation::bin_long& , binary_relation::bin_long& , binary_relation::bin_long& , binary_relation::bin_long& , std::vector< std::pair<size_t,size_t>>& );
        void range2(binary_relation::bin_long& , binary_relation::bin_long& , binary_relation::bin_long& , binary_relation::bin_long& , std::vector< std::pair<size_t,size_t>>& );
        bin_long labels(const size_t& , const size_t &) const;
        bin_long first_label_col(const size_t& ) const;

        void load(std::fstream&);
        void save(std::fstream&) const;

        bin_long n_columns() const ;
#ifdef PRINT_LOGS
        void print_size();

#endif
        void print()
        {
                std::cout<<std::endl;
                auto cols = xa_rank1(XA.size());
                auto rows = xb_rank1(XB.size());
                for (size_t j = 0; j < rows ; ++j) {
                        for (size_t i = 0; i < cols; ++i) {
                                std::cout<<labels(j+1,i+1)<<" ";
                        }
                        std::cout<<std::endl;
                }
                std::cout<<std::endl;

                std::cout<<"XA: ";
                for (auto &&i : XA) {
                        std::cout<< i <<" ";
                }
                std::cout<<"\n";
                std::cout<<"XB: ";
                for (auto &&i : XB) {
                        std::cout<< i <<" ";
                }
                std::cout<<"\n";

                std::cout<<"SB: ";
                for (unsigned long i : SB) {
                        std::cout<<(int) i <<" ";
                }
                std::cout<<"\n";
                std::cout<<"SL: ";
                for (int i = 0; i < SL.size() ; ++i) {
                        std::cout<<SL[i]<<" ";
                }
                std::cout<<"\n";
        }
        unsigned long long size_in_bytes() const ;

        binary_relation& operator=(const binary_relation& );

        auto get_SB_size() const{ return sdsl::size_in_bytes(SB);}
        auto get_SL_size() const{ return sdsl::size_in_bytes(SL);}
        auto get_XA_size() const{ return sdsl::size_in_bytes(XA)+sdsl::size_in_bytes(xa_rank1);}
        auto get_XB_size() const{ return sdsl::size_in_bytes(XB)+
                                         sdsl::size_in_bytes(xb_rank1)+
                                         sdsl::size_in_bytes(xb_sel1)+
                                         sdsl::size_in_bytes(xb_sel0);}

        void load_SB(std::fstream&f){
                sdsl::load(SB,f);
        }
        void load_SL(std::fstream&f){
                sdsl::load(SL,f);
        }
        void load_XA(std::fstream&f){
                sdsl::load(XA,f);
                xa_rank1  = bin_bit_vector_xa::rank_1_type(&XA);
        }
        void load_XB(std::fstream&f){
                sdsl::load(XB,f);
                xb_sel1 = bin_bit_vector_xb::select_1_type(&XB);
                xb_sel0 = bin_bit_vector_xb::select_0_type(&XB);
                xb_rank1  = bin_bit_vector_xb::rank_1_type(&XB);
        };

    protected:
        bin_long map(const bin_long&) const;
        bin_long unmap(const bin_long &) const;
};


#endif //IMPROVED_GRAMMAR_INDEX_BINARY_RELATION_H

