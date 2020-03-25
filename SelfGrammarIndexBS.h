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


    void build(const std::string& ) override;

    void build_basics(const std::string& t , fstream& f1,fstream& f2,fstream& f3) {
        SelfGrammarIndex::build_basics(t,f1,f2,f3);
    };
    void build(const grammar_representation&, const range_search2d& );
    void find_ranges_dfs(std::string &, std::vector<uint>& );
    void find_ranges_trie(std::string &, std::vector<uint>& );
    void find_ranges_trie(std::string &, std::vector<uint>&, std::vector<range> & );
    void find_ranges(std::string &, std::vector<uint>& );

    void locate( std::string& , sdsl::bit_vector &) ;
    void locate( std::string& , std::vector<uint> &) ;
    void locate2( std::string& , sdsl::bit_vector &) ;
    void locateNoTrie( std::string &, std::vector<uint> &);
    void display(const std::size_t& , const std::size_t&, std::string & ) override ;
    void display_trie(const std::size_t& , const std::size_t&, std::string & ) override ;
    void display_L_trie(const std::size_t& i, const std::size_t& j, std::string & s) override {
        SelfGrammarIndex::display_L_trie(i,j,s);
    };
    void set_code(const unsigned int & c){ SelfGrammarIndex::set_code(c);}
    void load_rules_pt(fstream& f){}
    void load_sfx_pt(fstream& f){}

    virtual void save(std::fstream& f)
    {
        SelfGrammarIndex::save(f);
    }
    virtual void load(std::fstream& f)
    {
        SelfGrammarIndex::load(f);
    }

    const m_patricia::compact_patricia_tree &get_pt_rules() const { }

    const m_patricia::compact_patricia_tree &get_pt_suffixes() const {  }

    size_t size_in_bytes() const override ;

     grammar_representation& get_grammar()   { return SelfGrammarIndex::get_grammar();}
     range_search2d & get_grid()   { return SelfGrammarIndex::get_grid();}
    bool bp_expand_prefix(const grammar_representation::g_long & X, std::string& s,const size_t & l,size_t & pos) const{
        SelfGrammarIndex::bp_expand_prefix(X,s,l,pos);
    }
    bool expand_prefix(const grammar_representation::g_long & X, std::string& s,const size_t & l,size_t & pos) const{
        SelfGrammarIndex::expand_prefix(X,s,l,pos);
    }

    int cmp_prefix(const grammar_representation::g_long & X,std::string::iterator&i, std::string::iterator& j)const{
        return SelfGrammarIndex::cmp_prefix(X,i,j);
    }
    int cmp_suffix(const grammar_representation::g_long & X,std::string::iterator&i, std::string::iterator& j)const{
        return SelfGrammarIndex::cmp_suffix(X,i,j);
    }


    void test_findSecondOcc(long len, uint &x1,  uint &x2, uint &y1, uint &y2,std::vector<uint> &occ){

        const auto& g_tree = _g.get_parser_tree();
        std::vector< std::pair<size_t,size_t> > pairs;
        grid.range2(x1,x2,y1,y2,pairs);

        //long len = itera-pattern.begin() +1;
        for (auto &pair : pairs){
            size_t p = grid.first_label_col(pair.second);
            size_t pos_p = _g.offsetText(g_tree[p]);
            unsigned int parent = g_tree.parent(g_tree[p]);
            long int  l = (- len + pos_p) - _g.offsetText(parent);
            unsigned int pre_parent = g_tree.pre_order(parent);
            find_second_occ_rec(l,pre_parent,occ);
        }

    }
    void test_findSecondOcc2(long len, uint &x1,  uint &x2, uint &y1, uint &y2,std::vector<uint> &occ){

        const auto& g_tree = _g.get_parser_tree();
        std::vector< std::pair<size_t,size_t> > pairs;
        grid.range2(x1,x2,y1,y2,pairs);

        //long len = itera-pattern.begin() +1;
        for (auto &pair : pairs){
            size_t p = grid.first_label_col(pair.second);
            size_t pos_p = _g.offsetText(g_tree[p]);
            unsigned int parent = g_tree.parent(g_tree[p]);
            long int  l = (- len + pos_p) - _g.offsetText(parent);
            //unsigned int pre_parent = g_tree.pre_order(parent);
            find_second_occ(l,parent,occ);
        }

    }



protected:



};


#endif //IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXBS_H
