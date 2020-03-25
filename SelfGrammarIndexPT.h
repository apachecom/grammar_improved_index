//
// Created by alpachec on 20-08-18.
//

#ifndef IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXPT_H
#define IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXPT_H


#include "SelfGrammarIndex.h"


class SelfGrammarIndexPT : public SelfGrammarIndex {


protected:

///        unsigned int sampling;

    m_patricia::compact_patricia_tree sfx_p_tree;
    m_patricia::compact_patricia_tree rules_p_tree;


    //void sampling_range_suff(size_t& i, size_t& j, std::string::iterator& , std::string::iterator& )const;

    //void sampling_range_rules(size_t &i, size_t &j, std::string::iterator& , std::string::iterator& )const;


    /*size_t _st(const size_t & i)const {
        return  (i-1)*sampling + 1;
    } //sampling transform;*/




public:
    SelfGrammarIndexPT() = default;

    virtual ~SelfGrammarIndexPT();

    const m_patricia::compact_patricia_tree &get_pt_rules() const { return rules_p_tree; }

    const m_patricia::compact_patricia_tree &get_pt_suffixes() const { return sfx_p_tree; }

    void build(const std::string &) override;

    void set_code(const unsigned int &c) { SelfGrammarIndex::set_code(c); }


    void build(const grammar_representation &, const range_search2d &, const m_patricia::compact_patricia_tree &,
               const m_patricia::compact_patricia_tree &);

    compressed_grammar &get_grammar() { return _g; }

    size_t size_in_bytes() const override;

    void locate2(std::string &, sdsl::bit_vector &);

    virtual void locate(std::string &, sdsl::bit_vector &) override;

    virtual void locate(std::string &, std::vector<uint> &) override;

    virtual void locateNoTrie(std::string &, std::vector<uint> &);

    virtual void find_ranges_trie(std::string & s, std::vector<uint> & X){};
    virtual void find_ranges_trie(std::string & s, std::vector<uint> & X, std::vector<range> & f){};
    virtual void find_ranges(std::string & s, std::vector<uint> & X){};
    void find_ranges_dfs(std::string &, std::vector<uint>& ){}
    void display(const std::size_t &, const std::size_t &, std::string &) override;

    void save(std::fstream &) override;

    void load(std::fstream &) override;

    void load_basics(std::fstream &f) { SelfGrammarIndex::load_basics(f); }

    void build_suffix(const string &, fstream &, fstream &);


    void load_rules_pt(fstream &f) { rules_p_tree.load(f); }

    void load_sfx_pt(fstream &f) { sfx_p_tree.load(f); }


};


#endif //IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXPT_H
