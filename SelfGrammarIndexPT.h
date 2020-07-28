//
// Created by alpachec on 20-08-18.
//

#ifndef IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXPT_H
#define IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXPT_H


#include "SelfGrammarIndex.h"


class SelfGrammarIndexPT : public SelfGrammarIndex {


protected:
    m_patricia::compact_patricia_tree sfx_p_tree;
    m_patricia::compact_patricia_tree rules_p_tree;




public:
    SelfGrammarIndexPT() = default;

    ~SelfGrammarIndexPT() override;

    virtual const m_patricia::compact_patricia_tree &get_pt_rules() const  { return rules_p_tree; }

    virtual const m_patricia::compact_patricia_tree &get_pt_suffixes() const { return sfx_p_tree; }

    void build(const std::string &
#ifdef MEM_MONITOR
            , mem_monitor&
#endif
    ) override;

    void set_code(const unsigned int &c) override{ SelfGrammarIndex::set_code(c); }


    void build(const grammar_representation &, const range_search2d &, const m_patricia::compact_patricia_tree &,
               const m_patricia::compact_patricia_tree &);

    compressed_grammar &get_grammar() override{ return _g; }

    size_t size_in_bytes() const override;

    void locate2(std::string &, sdsl::bit_vector &);

    void locate(std::string &, sdsl::bit_vector &) override;

    void locate(std::string &, std::vector<uint> &) override;

    void locateNoTrie(std::string &, std::vector<uint> &) override;

    void find_ranges_trie(std::string & s, std::vector<uint> & X) override{};
    void find_ranges_trie(std::string & s, std::vector<uint> & X, std::vector<range> & f) override{};
    void find_ranges(std::string & s, std::vector<uint> & X) override{};
    void find_ranges_dfs(std::string &, std::vector<uint>& ) override{}
    void display(const std::size_t &, const std::size_t &, std::string &) override;

    void save(std::fstream &) override;

    void load(std::fstream &) override;

    void load_basics(std::fstream &f) override { SelfGrammarIndex::load_basics(f); }

    virtual void build_suffix(const string &, fstream &, fstream &
#ifdef MEM_MONITOR
            ,mem_monitor &mm
#endif
    );


    void load_rules_pt(fstream &f) override { rules_p_tree.load(f); }

    void load_sfx_pt(fstream &f) override { sfx_p_tree.load(f); }


};


#endif //IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXPT_H
