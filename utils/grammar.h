//
// Created by inspironXV on 8/17/2018.
//

#ifndef IMPROVED_GRAMMAR_INDEX_GRAMMAR_H
#define IMPROVED_GRAMMAR_INDEX_GRAMMAR_H


#include <vector>
#include <map>
#include <cassert>
#include "repair/RePair.h"
#include <sdsl/io.hpp>
#include <set>
#include "../macros.h"
#include "sdsl/int_vector.hpp"

#ifdef MEM_MONITOR
#include <ctime>
#include "memory/mem_monitor/mem_monitor.hpp"
#include "CLogger.h"
using timer = std::chrono::high_resolution_clock;
using namespace std::chrono;
#endif

struct rule{

    typedef unsigned int r_long;
    r_long id;
    bool terminal;
    uint node;
    /*
     * Rule production
     * */
    std::vector<rule::r_long > _rule;
    /*
     * offset in the text
     * */
    rule::r_long l,r;

    rule():id(0),terminal(false),l(0),r(0),node(0){

    }
    rule(const r_long & i, const bool& t):id(i),terminal(t),l(0),r(0),node(0) {}
    rule& operator=(const rule& R) = default;
    rule::r_long size_in_bytes()const
    {
        return sizeof(rule::r_long)*3 + 1 + _rule.size()*sizeof(rule::r_long);
    }

    rule::r_long len() const{ return r - l + 1; }

    void load(std::fstream& f)
    {
        sdsl::load(id,f);
        sdsl::load(terminal,f);
        sdsl::load(r,f);
        sdsl::load(l,f);
        rule::r_long n;
        sdsl::load(n,f);
        _rule.clear();
        _rule.resize(n);
        for (int i = 0; i < n ; ++i) {
            rule::r_long item;
            sdsl::load(item,f);
            _rule[i] = item;
        }
    }
    void save (std::fstream& f)
    {
        sdsl::serialize(id,f);
        sdsl::serialize(terminal,f);
        sdsl::serialize(r,f);
        sdsl::serialize(l,f);
        rule::r_long ns = _rule.size();
        sdsl::serialize(ns,f);

        for (auto &&  item: _rule) {
            sdsl::serialize(item,f);
        }
    }


};

struct rule_trav{

    rule* rules;
    uint* rule_position;
    uint* last_processed;
    uint* label_last_processed;
    uint* len_rule;

    int level;
    uint read;
//    uint discard;
//    unsigned char* extr_seq;


//    rule_trav(int max){
//
//        rules = new rule[max];
//        rule_position = new uint[max];
//        last_processed = new uint[max];
//        label_last_processed = new uint[max];
//        len_rule = new uint[max];
//        read = 0;
//        level =0;
//
//        for (int i = 0; i < max; ++i) {
//            rule_position[i] = 0;
//            last_processed[i] = 0;
//            len_rule[i] = 0;
//            label_last_processed[i] = 0;
//        }
//    };

    rule_trav(uint _id, uint len,uint node, int max){


        rules = new rule[max];
        rule_position = new uint[max];
        last_processed = new uint[max];
        label_last_processed = new uint[max];
        len_rule = new uint[max];

        for (int i = 0; i < max; ++i) {
            rule_position[i] = 0;
            last_processed[i] = 0;
            len_rule[i] = 0;
            label_last_processed[i] = 0;
        }

        level = 0;
        rules[0].id = _id;
        rules[0].node = node;

        len_rule[0] = len;
        last_processed[0] = 0;
        read = 0;
    }

};

class grammar {

    public:

        typedef std::map<rule::r_long ,rule>::iterator grammar_iterator;

        void preprocess(const std::string &

#ifdef MEM_MONITOR
                ,mem_monitor&
#endif
        );
    protected:
        std::map<rule::r_long ,rule> _grammar;

        rule::r_long initial_rule;

        rule::r_long _size;

        size_t repair_grammar_size;

        std::map<unsigned int,unsigned char> alp;



        void replace(const rule::r_long& , std::map<rule::r_long ,rule::r_long > &, std::set<rule::r_long> &);

        void compute_offset_text(const rule::r_long& ,  rule::r_long&, std::set<rule::r_long>&);
//        void reduce(const std::string &);
    public:


        grammar();

        ~grammar();

        void buildRepair(const std::string&
#ifdef MEM_MONITOR
                ,mem_monitor& mm
#endif
        );
        void buildBalRepair( const std::string &,std::fstream&,std::fstream &
#ifdef MEM_MONITOR
                ,mem_monitor& mm
#endif

        );

        grammar_iterator begin();

        grammar_iterator end();

        const rule& operator[](const rule::r_long&);

        const std::map<unsigned int,unsigned char>& get_map() const;

        void permutationSortRules(const std::string& text,sdsl::int_vector<> &Pi){

//            uint n = n_rules();
            Pi.resize(n_rules()+1);

            for(uint i = 0; i < n_rules()+1; i++)
                Pi[i] = i;

            sort(Pi.begin()+1, Pi.end(),[this,&text](const uint & a, const uint &b )->bool{
                std::string sa,sb;

                sa.resize(_grammar[a].len());
                sb.resize(_grammar[b].len());

                std::copy(text.begin()+_grammar[a].l, text.begin()+_grammar[a].r+1,sa.begin());
                std::copy(text.begin()+_grammar[b].l, text.begin()+_grammar[b].r+1,sb.begin());
                sa = sa + "$";
                sb = sb + "$";

                return sa < sb;
            });
//            std::cout<<"pi: ";
//            for (int j = 0; j < n_rules()+1; ++j) {
//                std::cout<<Pi[j]<<" ";
//            }
//            std::cout<<"\n";

        }


        template< typename F >
        void dfs(const rule::r_long& X, const F &f ){

                assert(_grammar.find(X) != _grammar.end() );
                rule r = _grammar[X];

                if(!f(r)) return;

                for (int i = 0; i < r._rule.size() ; ++i) {
                    dfs(r._rule[i],f);
                }
            }

        const rule::r_long& get_size();

        rule::r_long n_rules();

        rule::r_long size_in_bytes();

        const rule::r_long& get_initial_rule();

        rule::r_long text_size();

        void print(const std::string& );

        rule::r_long num_terminals()const ;

        void save(std::fstream& f);

        void load(std::fstream& f);

        void write_text( fstream & out);
};


#endif //IMPROVED_GRAMMAR_INDEX_GRAMMAR_H
