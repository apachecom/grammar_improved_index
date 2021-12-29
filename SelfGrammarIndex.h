//
// Created by inspironXV on 8/16/2018.
//

#ifndef IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEX_H
#define IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEX_H


#include <string>
#include <stack>
#include <sdsl/bit_vectors.hpp>
#include "compressed_grammar.h"
#include "binary_relation.h"
#include "trees/patricia_tree/compact_patricia_tree.h"


#ifdef MEM_MONITOR
#include <ctime>
#include "utils/memory/mem_monitor/mem_monitor.hpp"
#include "utils/CLogger.h"
using timer = std::chrono::high_resolution_clock;
using namespace std::chrono;
#endif

struct range{
    uint x1,x2,y1,y2,len;
    range(){};
};

class SelfGrammarIndex {

public:
    typedef compressed_grammar grammar_representation;
    typedef binary_relation range_search2d;



    unsigned int code;
protected:
    grammar_representation _g;
    range_search2d grid;


public:


    SelfGrammarIndex() {};

    virtual ~SelfGrammarIndex() {};

    virtual void locateNoTrie(std::string &, std::vector<uint> &) = 0;

    virtual void set_code(const unsigned int &c) { code = c; }
    virtual void build(const std::string &
#ifdef MEM_MONITOR
            , mem_monitor& mm
#endif
    );

    virtual void build_basics(const std::string &, fstream &, fstream &, fstream &
#ifdef MEM_MONITOR
    ,mem_monitor &mm
#endif
    );

    virtual void build_basics(const std::string &,grammar &not_compressed_grammar,
                              std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > >& grammar_sfx
#ifdef MEM_MONITOR
            , mem_monitor&
#endif
    );

    virtual void load_basics(fstream &);
    virtual void save(std::fstream &);
    virtual void load(std::fstream &);

    virtual grammar_representation &get_grammar() { return _g; }
    virtual range_search2d &get_grid() { return grid; }
    virtual void display(const std::size_t &, const std::size_t &, std::string &);

    virtual void display_L(const std::size_t &i, const std::size_t &j, std::string &str) {
        str.resize(j - i + 1);
        size_t p = 0;
        expand_interval(_g.m_tree.root(), make_pair(i, j), str, p);
    }

    virtual size_t size_in_bytes() const;

    virtual bool expand_prefix(const grammar_representation::g_long &, std::string &, const size_t &, size_t &pos) const;
    bool expand_suffix(const grammar_representation::g_long &, std::string &, const size_t &, size_t &) const;

    void find_second_occ(long int &, unsigned int &, std::vector<uint> &) const;


    template<typename K>
    bool lower_bound(bool &found , grammar_representation::g_long &lr, grammar_representation::g_long &hr, const K &f) const {

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
    bool upper_bound(bool &found, grammar_representation::g_long &lr, grammar_representation::g_long &hr, const K &f) const {


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


    template<typename K>
    bool lower_bound(grammar_representation::g_long &lr, grammar_representation::g_long &hr, const K &f) const {
        if (lr >= hr || lr + 1 == hr) {
            if (f(lr) == 0) return true;

            if (f(hr) == 0) {
                lr = hr;
                return true;
            }
            return false;
        }

        grammar_representation::g_long m = (lr + hr) / 2;

        auto res = f(m);
        if (res < 0) {
            /*
             * If value < value(node) f must return
             * */
            hr = m - 1;
        } else {
            if (res > 0) {
                /*
                 * If value > value(node) f must return true
                 * */
                (lr = m);
            } else {
                /*
                 * If value == value(node) f must return true
                 * */
                hr = m;
            }
        }


        return lower_bound(lr, hr, f);
    }

    template<typename K>
    bool upper_bound(grammar_representation::g_long &lr, grammar_representation::g_long &hr, const K &f) const {

        if (lr >= hr || lr + 1 == hr) {
            if (f(hr) == 0) return true;

            if (f(lr) == 0) {
                hr = lr;
                return true;
            }
            return false;
        }

        grammar_representation::g_long m = (lr + hr) / 2;

        auto res = f(m);

        if (res < 0) {
            /*
             * If value < value(node) f must return true
             * */
            hr = m;
        } else {

            if (res > 0) {
                /*
                 * If value > value(node) f must return true
                 * */
                (lr = m + 1);
            } else {
                /*
                * If value == value(node) f must return true
                * */
                lr = m;
            }


        }

        return upper_bound(lr, hr, f);
    }

    int cmp_suffix_grammar(const size_t &, std::string::iterator &, std::string::iterator &);

    void expand_interval(const size_t &node, const std::pair<size_t, size_t> &range, std::string &s, std::size_t &pos)
    {
        /*precondition range always belongs to [node.leftmost-leaf, node.rigthmost-leaf]*/

        /*base case*/

        /*recursion case*/


        auto llb = _g.rank_L(range.first) + _g.L[range.first]; //left leaf begin
        auto pllb = _g.select_L(llb); // position of left leaf begin


        auto lle = _g.rank_L(range.second) + _g.L[range.second]; //left leaf end
        auto plle = _g.select_L(lle); // position of left leaf end


        /* If the range only expands a unique leaf */
        uint64_t added_ch = 0;

        if (llb == lle) {
            /* Need to jump to llb definition */

            /* compute label X of llb */
            auto X = _g[_g.m_tree.pre_order(_g.m_tree.leafselect(llb))];
            /* if X is a terminal symbol append to the string*/
            if (_g.isTerminal(X)) {
                unsigned char a_th = _g.terminal_simbol(X);
                s[pos] = a_th;
                ++pos;
                ++added_ch;

                if (added_ch >= range.second - range.first + 1)
                    return;

            }
            /* if X is not a terminal symbol */

            /* compute first occurrence of X in the parser tree*/

            auto node_def = _g.m_tree[_g.select_occ(X, 1)];

            /* compute new range */
            auto p = _g.select_L(_g.m_tree.leafrank(node_def)); // pos of leftmost leaf of node_def
            auto l_range = std::make_pair(p + range.first - pllb, p + range.second - plle);

            expand_interval(node_def, l_range, s, pos);
            return;

        }

        if (added_ch > range.second - range.first + 1)
            return;

        /* If the range expands more than one leaf */

        /* Divide the expansion in three range left, middle and right*/

        /* left range*/
        {
            /* Need to jump to llb definition */

            /* compute label X of llb */

            auto X = _g[_g.m_tree.pre_order(_g.m_tree.leafselect(llb))];
            /* if X is a terminal symbol append to the string*/
            if (_g.isTerminal(X)) {
                unsigned char a_th = _g.terminal_simbol(X);
                s[pos] = a_th;
                ++pos;
                ++added_ch;
                if (added_ch >= range.second - range.first + 1)
                    return;
            } else {
                /* if X is not a terminal symbol */
                /* compute first occurrence of X in the parser tree*/

                ///auto node_def = _g.m_tree[_g.select_occ(X, 1)];

                /* compute new range */

                ///auto p = _g.select_L(_g.m_tree.leafrank(node_def)); // pos of leftmost leaf of node_def

                size_t pnllb = _g.select_L(llb + 1); // position of left + 1 leaf begin

                ////auto l_range = std::make_pair(p + range.first - pllb, p + (pnllb - pllb) - 1);

                ///expand_interval(node_def, l_range, s, pos);

                size_t off = pnllb - range.first;
                size_t p_f = pos;

                expand_suffix(X,s,off,pos);

                std::reverse(s.begin()+p_f,s.begin()+p_f+off);

            }


        }

        if (added_ch > range.second - range.first + 1)
            return;

        /* middle range*/
        if (llb != lle - 1) {
            /*expand all the phrases in the middle*/
            auto t = llb + 1;

            while (t < lle) {
                auto off = _g.select_L(t + 1) - _g.select_L(t) + pos;
                //expand_interval(_g.m_tree.leafselect(t), make_pair(_g.select_L(t),_g.select_L(t + 1)-1),s,pos);
                expand_prefix(_g[_g.m_tree.pre_order(_g.m_tree.leafselect(t))], s, off, pos);
                ++t;
            }

        }

        if (added_ch > range.second - range.first + 1)
            return;

        /* right range*/
        {
            /* Need to jump to llb definition */
            /* compute label X of lle */
            auto X = _g[_g.m_tree.pre_order(_g.m_tree.leafselect(lle))];
            /* if X is a terminal symbol append to the string*/
            if (_g.isTerminal(X)) {
                unsigned char a_th = _g.terminal_simbol(X);
                s[pos] = a_th;
                ++pos;
                ++added_ch;
                if (added_ch >= range.second - range.first + 1)
                    return;
            } else
            {
                /* if X is not a terminal symbol */
                /* compute first occurrence of X in the parser tree*/

                ///auto node_def = _g.m_tree[_g.select_occ(X, 1)];

                /* compute new range */

                auto off = range.second - plle + 1;
                ///             auto p = _g.select_L(_g.m_tree.leafrank(node_def)); // pos of leftmost leaf of node_def

///                auto l_range = std::make_pair(p, p + range.second - plle);

                expand_prefix(X,s,pos+off,pos);

                ///expand_interval(node_def, l_range, s, pos);
            }
        }

    }

    virtual int  dfs_cmp_suffix    (const grammar_representation::g_long & X, std::string::iterator & itera, std::string::iterator & end) const{

        if(_g.isTerminal(X) ){

            unsigned char a_th = _g.terminal_simbol(X);

            if(a_th < (unsigned char)(*itera)) return 1;
            if(a_th > (unsigned char)(*itera)) return  -1;
            --itera;
            if(itera == end-1) return 0;
            return 0;
        }



        uint rule_node = _g.m_tree[_g.select_occ(X,1)];

        uint nch  = _g.m_tree.children(rule_node);

        for (int j = nch; j > 0 ; --j)
        {   uint child = _g.m_tree.child(rule_node,j);
            uint V = _g[_g.m_tree.pre_order(child)];
            int r = dfs_cmp_suffix(V,itera,end);
            if(r != 0) return r;
            if(itera == end - 1)
                return 0;
        }

        return 0;
    }

    virtual int  dfs_cmp_prefix    (const grammar_representation::g_long & X, std::string::iterator & itera, std::string::iterator & end) const{

        if(_g.isTerminal(X) ){

            unsigned char a_th = _g.terminal_simbol(X);

            if(a_th < (unsigned char)(*itera)) return 1;
            if(a_th > (unsigned char)(*itera)) return  -1;
            ++itera;
            if(itera == end) return 0;
            return 0;
        }

        uint rule_node = _g.m_tree[_g.select_occ(X,1)];

        uint nch  = _g.m_tree.children(rule_node);
        for (uint j = 1; j <= nch ; ++j)
        {   uint child = _g.m_tree.child(rule_node,j);
            uint V = _g[_g.m_tree.pre_order(child)];
            int r = dfs_cmp_prefix(V,itera,end);
            if(r != 0) return r;
            if(r == 0 && itera == end)
                return 0;
        }

        return 0;
    }


};


#endif //IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEX_H
