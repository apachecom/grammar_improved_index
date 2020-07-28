//
// Created by alpachec on 19-08-18.
//

#include <sdsl/lcp_bitcompressed.hpp>
#include <sdsl/rmq_succinct_sada.hpp>
#include "SelfGrammarIndexBS.h"



using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

void SelfGrammarIndexBS::build(const std::string & text
#ifdef MEM_MONITOR
        , mem_monitor& mm
#endif
) {
    SelfGrammarIndex::build(text
#ifdef MEM_MONITOR
            ,mm
#endif
    );

}
void SelfGrammarIndexBS::locate2( std::string & pattern, sdsl::bit_vector & occ){

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }


    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();

    const auto& g_tree = _g.get_parser_tree();
    size_t nnodes = g_tree.subtree(g_tree.root()) + 1;
    std::vector<bool> mark(nnodes,false);

    /*
     *
     * partitioning the pattern
     *
     * */
    for (size_t  i = 1; i <= p_n ; ++i)
    {
        auto itera = pattern.begin() + i-1;
        grammar_representation::g_long lr = 1,hr = n_xj;

        if(
                !lower_bound(lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {
                    auto begin = pattern.begin();
                    auto end = itera;

                    //auto r =  bp_cmp_suffix(a,end,begin);
                    auto r =  cmp_suffix(a,end,begin);
                    if(r == 0 && end != begin-1) return 1;
                    return r;

                })	
        )continue;
        grammar_representation::g_long r1 = lr;
	
        lr = 1,hr = n_xj;

        if(
                !upper_bound(lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {
                    auto begin = pattern.begin();
                    auto end = itera;

                    auto r = cmp_suffix(a,end,begin);
                    if(r == 0 && end != begin-1) return 1;

                    return r;
                })
         ) continue;
        grammar_representation::g_long r2 = hr;

        grammar_representation::g_long ls = 1,hs = n_sj;
        if(
                !lower_bound(ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {
                    auto end = pattern.end();
                    auto it2 = itera+1;
                    auto r = cmp_suffix_grammar(a,it2,end);
                    return r;
                })
            )
        continue;

    grammar_representation::g_long c1 = ls;
        ls = 1,hs = n_sj;
        if(
                !upper_bound(ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {

                    auto end = pattern.end();
                    auto it2 = itera+1;
                    auto r =  cmp_suffix_grammar(a,it2,end);
                    return r;

                })
            )
            continue;
        grammar_representation::g_long c2 = hs;
        std::vector< std::pair<size_t,size_t> > pairs;
        grid.range2(r1,r2,c1,c2,pairs);
        long len = itera-pattern.begin() +1;
        for (auto &pair : pairs) {
            size_t p = grid.first_label_col(pair.second);
            size_t pos_p = _g.offsetText(g_tree[p]);
            unsigned int parent = g_tree.parent(g_tree[p]);
            long   l = long (- len + pos_p) - _g.offsetText(parent);
            if(mark[p] == false)
            {
                mark[p] = true;
                find_second_occ(l,parent,occ);
            }
        }
    }


}
void SelfGrammarIndexBS::locate( std::string & pattern, sdsl::bit_vector & occ){

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }


    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();
    /*
     *
     * partitioning the pattern
     *
     * */
    for (size_t  i = 1; i <= p_n ; ++i)
    {

        auto itera = pattern.begin() + i-1;

        grammar_representation::g_long lr = 1,hr = n_xj;

        if(
                !lower_bound(lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {

                    auto begin = pattern.begin();
                    auto end = itera;

                    auto r =  bp_cmp_suffix(a,end,begin);
                    if(r == 0 && end != begin-1) return 1;
                    return r;

                })
                )continue;
        grammar_representation::g_long r1 = lr;
        lr = 1,hr = n_xj;

        if(
                !upper_bound(lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {
                    auto begin = pattern.begin();
                    auto end = itera;

                    auto r =  bp_cmp_suffix(a,end,begin);
                    if(r == 0 && end != begin-1) return 1;

                    return r;
                })
                ) continue;
        grammar_representation::g_long r2 = hr;
        grammar_representation::g_long ls = 1,hs = n_sj;
        if(
                !lower_bound(ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {
                    auto end = pattern.end();
                    auto it2 = itera+1;

                    auto r = bp_cmp_suffix_grammar(a,it2,end);

                    return r;

                })
                )
            continue;
        grammar_representation::g_long c1 = ls;
        ls = 1,hs = n_sj;

        if(
                !upper_bound(ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {

                    auto end = pattern.end();
                    auto it2 = itera+1;
                    auto r =  bp_cmp_suffix_grammar(a,it2,end);
                    return r;

                })
                )
            continue;
        grammar_representation::g_long c2 = hs;
        std::vector< std::pair<size_t,size_t> > pairs;
        grid.range2(r1,r2,c1,c2,pairs);

        const auto& g_tree = _g.get_parser_tree();


        long len = itera-pattern.begin() +1;

        for (auto &pair : pairs) {
            size_t p = grid.first_label_col(pair.second);
            size_t pos_p = _g.offsetText(g_tree[p]);
            unsigned int parent = g_tree.parent(g_tree[p]);
            long   l = long (- len + pos_p) - _g.offsetText(parent);
            find_second_occ(l,parent,occ);
        }

    }


}
void SelfGrammarIndexBS::locate( std::string & pattern, std::vector<uint> & occ){

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }


    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();
    /*
     *
     * partitioning the pattern
     *
     * */
    for (size_t  i = 1; i <= p_n ; ++i)
    {


        auto itera = pattern.begin() + i-1;
        grammar_representation::g_long lr = 1,hr = n_xj;

        bool found = false;

        lower_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
        {
            auto begin = pattern.begin();
            auto end = itera;
            auto r =  bp_cmp_suffix(a,end,begin);
            if(r == 0 && end != begin-1) return 1;
            return r;
        });


        if(!found)
            continue;

        binary_relation::bin_long r1 = lr;
        hr = n_xj;

        found = false;
        upper_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
        {
            auto begin = pattern.begin();
            auto end = itera;
            auto r =  bp_cmp_suffix(a,end,begin);
            if(r == 0 && end != begin-1) return 1;

            return r;
        });
        if(!found)
            continue;

        binary_relation::bin_long r2 = hr;


        grammar_representation::g_long ls = 1,hs = n_sj;

        found = false;
        lower_bound(found, ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
        {
            auto end = pattern.end();
            auto it2 = itera+1;
            auto r = bp_cmp_suffix_grammar(a,it2,end);

            return r;
        });

        if(!found)
            continue;
        binary_relation::bin_long c1 = ls;
        hs = n_sj;

        found = false;

        upper_bound(found,ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
        {
            auto end = pattern.end();
            auto it2 = itera+1;
            auto r =  bp_cmp_suffix_grammar(a,it2,end);
            return r;
        });

        if(!found)
            continue;


        binary_relation::bin_long c2 = hs;

        long len = itera-pattern.begin() +1;


        find_second_occ(r1,r2,c1,c2,len,occ);



    }


}
void SelfGrammarIndexBS::locateNoTrie( std::string & pattern, std::vector<uint> & occ){


    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();

    for (size_t  i = 1; i <= p_n ; ++i)
    {

        std::string::iterator itera = pattern.begin() + i-1;
        grammar_representation::g_long lr = 1,hr = n_xj;

        bool found = false;

        lower_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
        {
            std::string::iterator begin = pattern.begin();
            std::string::iterator end = itera;
            auto r =  dfs_cmp_suffix(a,end,begin);
            if(r == 0 && end != begin-1) return 1;
            return r;
        });


        if(!found)
            continue;

        binary_relation::bin_long r1 = lr;
        hr = n_xj;

        found = false;
        upper_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
        {
            auto begin = pattern.begin();
            auto end = itera;
            auto r =  dfs_cmp_suffix(a,end,begin);
            if(r == 0 && end != begin-1) return 1;

            return r;
        });
        if(!found)
            continue;

        binary_relation::bin_long r2 = hr;


        grammar_representation::g_long ls = 1,hs = n_sj;

        found = false;
        lower_bound(found, ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
        {
            auto end = pattern.end();
            auto it2 = itera+1;
            auto r = cmp_suffix_grammar(a,it2,end);

            return r;
        });

        if(!found)
            continue;
        binary_relation::bin_long c1 = ls;
        hs = n_sj;

        found = false;

        upper_bound(found,ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
        {
            auto end = pattern.end();
            auto it2 = itera+1;
            auto r =  cmp_suffix_grammar(a,it2,end);
            return r;
        });

        if(!found)
            continue;



        binary_relation::bin_long c2 = hs;


        long len = itera-pattern.begin() +1;


        find_second_occ(r1,r2,c1,c2,len,occ);

    }


}
void SelfGrammarIndexBS::display(const std::size_t &i, const std::size_t &j, std::string &s) {
    SelfGrammarIndex::display( i,j,s);
}
void SelfGrammarIndexBS::display_trie(const std::size_t &i, const std::size_t &j, std::string &s) {
    SelfGrammarIndex::display_trie( i,j,s);
}
void SelfGrammarIndexBS::build(const SelfGrammarIndex::grammar_representation &G, const SelfGrammarIndex::range_search2d &R) {
    _g = G;
    grid = R;
}
size_t SelfGrammarIndexBS::size_in_bytes() const {
//    std::cout<<"SelfGrammarIndexBS::size_in_bytes()\n";
    return SelfGrammarIndex::size_in_bytes();
}
void SelfGrammarIndexBS::find_ranges_trie(std::string & pattern, std::vector<uint>&X) {

    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    for (size_t  i = 1; i <= p_n ; ++i) {

        auto itera = pattern.begin() + i - 1;
        grammar_representation::g_long lr = 1, hr = n_xj;

        bool found = false;
        lower_bound(found,lr, hr, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {

            auto begin = pattern.begin();
            auto end = itera;
            auto r = bp_cmp_suffix(a, end, begin);
            if (r == 0 && end != begin - 1) return 1;
            return r;
        });

        if (!found)
            continue;
   }
}
void SelfGrammarIndexBS::find_ranges_trie(std::string & pattern, std::vector<uint>&X, std::vector<range>& fr) {

    size_t p_n = pattern.size();
    size_t n_sj = grid.n_columns();

    for (size_t  i = 1; i <= p_n ; ++i) {

        auto itera = pattern.begin() + i - 1;
        bool found = false;
        grammar_representation::g_long ls = 1, hs = n_sj;

        found  = false;
        lower_bound(found,ls, hs, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
            auto end = pattern.end();
            auto it2 = itera + 1;
            auto r = bp_cmp_suffix_grammar(a, it2, end);
            return r;
        });

        if (!found)
            continue;

        hs = n_sj;

        found = false;
        upper_bound(found, ls, hs, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
            auto end = pattern.end();
            auto it2 = itera + 1;
            auto r = bp_cmp_suffix_grammar(a, it2, end);
            return r;
        });


        if (!found )
            continue;
    }
}
void SelfGrammarIndexBS::find_ranges(std::string &pattern, std::vector<uint> &X) {

    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();

    for (size_t  i = 1; i <= p_n ; ++i) {

        auto itera = pattern.begin() + i - 1;
        grammar_representation::g_long ls = 1, hs = n_sj;
        if (
                !lower_bound(ls, hs, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
                    auto end = pattern.end();
                    auto it2 = itera + 1;
                    auto r = bp_cmp_suffix_grammar(a, it2, end);
                    return r;
                })
                )
            continue;
        ls = 1, hs = n_sj;

        if (
                !upper_bound(ls, hs, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
                    auto end = pattern.end();
                    auto it2 = itera + 1;
                    auto r = bp_cmp_suffix_grammar(a, it2, end);
                    return r;
                })
                )
            continue;

    }

}
void SelfGrammarIndexBS::find_ranges_dfs(std::string & pattern, std::vector<uint>&X) {

    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;

    for (size_t  i = 1; i <= p_n ; ++i) {

        auto itera = pattern.begin() + i - 1;
        grammar_representation::g_long lr = 1, hr = n_xj;

        bool found = false;
        lower_bound(found,lr, hr, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {

            auto begin = pattern.begin();
            auto end = itera;
            auto r = dfs_cmp_suffix(a, end, begin);
            if (r == 0 && end != begin - 1) return 1;
            return r;
        });

        if (!found)
            continue;
  }
}
