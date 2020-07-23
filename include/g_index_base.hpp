//
// Created by alejandro on 23-07-20.
//

#ifndef IMPROVED_GRAMMAR_INDEX_G_INDEX_BASE_HPP
#define IMPROVED_GRAMMAR_INDEX_G_INDEX_BASE_HPP



#include "../compressed_grammar.h"
#include "../binary_relation.h"


namespace g_index{


    template <

            typename grammar_representation = compressed_grammar ,
            typename range_search2d = binary_relation
    >
    class g_index_base {


        public:

            g_index_base();

            g_index_base(const g_index_base&);
            g_index_base& operator=(const g_index_base&);
            virtual ~g_index_base();

            void build(
                    fstream& R,
                    fstream& C
            );
            void build(
                        const std::string & text,
                        fstream& suffixes,
                        fstream& basics,
                        fstream& repair_g
                        );


        protected:
            grammar_representation _g;
            range_search2d grid;


        protected:
            /**
             * find_second_occ
             * offset is the offset (i) corresponding to the partition pattern p[1...i....m]
             * node is the node where first occurrence appear
             * occ is the vector to return the positions
             */
            void find_second_occ(long int & offset, unsigned int & node, std::vector<uint> & occ) const {


                const auto& Tg = _g.get_parser_tree();
                std::deque<std::pair< uint, long int >> S;

                {
                    size_t pre = Tg.pre_order(node);
                    size_t Xi = _g[pre];
                    size_t n_s_occ = _g.n_occ(Xi);
                    for (size_t i = 1; i <= n_s_occ; ++i)
                    {
                        size_t pre_parent = _g.select_occ(Xi,i);
                        S.emplace_back(pre_parent,offset);
                    }
                }

                while(!S.empty())
                {
                    if(S.front().first == 1)
                        occ.push_back((uint)(S.front().second));
                    else
                    {
                        auto node = Tg[S.front().first];
                        size_t parent = Tg.parent(node);
                        size_t pre_parent = Tg.pre_order(parent);
                        size_t Xi = _g[pre_parent];
                        size_t n_s_occ = _g.n_occ(Xi);
                        long int p_offset = S.front().second + _g.offsetText(node) - _g.offsetText(parent);
                        for (size_t i = 1; i <= n_s_occ; ++i)
                        {
                            size_t pre_parent = _g.select_occ(Xi,i);
                            S.emplace_back(pre_parent,p_offset);
                        }
                    }

                    S.pop_front();
                }
            }

            /**
             * compare the prefix/suffix rule with the string
             *
             * return 1 if rule > string
             * return -1 if rule < string
             * return 0 if rule == string
             * */
            virtual uint8_t compare_prefix_rule(const uint32_t & , std::string::iterator & , std::string::iterator & ) = 0;
            virtual uint8_t compare_suffix_rule(const uint32_t & , std::string::iterator & , std::string::iterator & )= 0;

            /**
             * extract the prefix/suffix rule
             * */
            virtual void extract_prefix_rule(const uint32_t & , std::string::iterator & , std::string::iterator & )= 0;
            virtual void extract_suffix_rule(const uint32_t & , std::string::iterator & , std::string::iterator & )= 0;


            /**
             * lower/upper bound to perform binary search on virtual rules array
             * */
            template<typename K>
            bool lower_bound(bool &found , uint32_t &lr, uint32_t &hr, const K &f) const {

                found = false;

                while(lr < hr){

                    uint64_t mid = (lr+hr)/2;
                    int c = f(mid);

                    if(c < 0){
                        /*the rule is greater than the pattern */
                        hr = mid -1;
                    }else{
                        if(c > 0)
                            lr = mid+1;
                        else{
                            hr = mid;
                            found = true;
                        }

                    }
                }

                if(!found && lr == hr && f(lr) == 0 ) {
                    found = true;
                    return true;
                }

                return false;

            }
            template<typename K>
            bool upper_bound(bool &found, uint32_t &lr, uint32_t &hr, const K &f) const {


                found = false;

                while(lr < hr){

                    uint64_t mid = ceil((lr+hr)/2.0);
                    int c = f(mid);

                    if(c < 0){
                        /*the rule is greater than the pattern */
                        hr = mid -1;
                    }else{
                        if(c > 0)
                            lr = mid+1;
                        else{
                            lr = mid;
                            found = true;
                        }

                    }
                }

                if(!found && lr == hr && f(lr) == 0 ) {
                    found = true;
                    return true;
                }

                return false;


            }



    };

};


#endif //IMPROVED_GRAMMAR_INDEX_G_INDEX_BASE_HPP
