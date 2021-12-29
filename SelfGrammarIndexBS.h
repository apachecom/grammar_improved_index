//
// Created by alpachec on 19-08-18.
//

#ifndef IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXBS_H
#define IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXBS_H


#include "SelfGrammarIndex.h"




class SelfGrammarIndexBS: public SelfGrammarIndex {


public:

    SelfGrammarIndexBS()= default;
    ~SelfGrammarIndexBS() override = default;


    void build(const std::string&
#ifdef MEM_MONITOR
            , mem_monitor& mm
#endif
    ) override;

    void build_basics(const std::string& t , fstream& f1,fstream& f2,fstream& f3
#ifdef MEM_MONITOR
                  ,mem_monitor &mm
#endif
    ) override {
        SelfGrammarIndex::build_basics(t,f1,f2,f3

#ifdef MEM_MONITOR
                ,mm
#endif
        );
    }

    void build_basics(
            const std::string & text,
            grammar &not_compressed_grammar,
            std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > >& grammar_sfx
#ifdef MEM_MONITOR
            , mem_monitor& mm
#endif
    ) {
        SelfGrammarIndex::build_basics(text,not_compressed_grammar,grammar_sfx
#ifdef MEM_MONITOR
        ,mm
#endif
        );
    }
    void build(const grammar_representation&, const range_search2d& );

    void locateNoTrie( std::string &, std::vector<uint> &) override;

    void display(const std::size_t& , const std::size_t&, std::string & ) override ;

    void set_code(const unsigned int & c) override { SelfGrammarIndex::set_code(c);}


    void save(std::fstream& f)override
    {
        SelfGrammarIndex::save(f);
    }
    void load(std::fstream& f)override
    {
        SelfGrammarIndex::load(f);
    }

    size_t size_in_bytes() const override ;

     grammar_representation& get_grammar() override  { return SelfGrammarIndex::get_grammar();}
     range_search2d & get_grid()  override { return SelfGrammarIndex::get_grid();}



};


#endif //IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXBS_H
