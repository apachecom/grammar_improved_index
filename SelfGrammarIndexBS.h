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
    void find_ranges_dfs(std::string &, std::vector<uint>& ) override;
    void find_ranges_trie(std::string &, std::vector<uint>& ) override;
    void find_ranges_trie(std::string &, std::vector<uint>&, std::vector<range> & ) override;
    void find_ranges(std::string &, std::vector<uint>& ) override;

    void locate( std::string& , sdsl::bit_vector &) override;
    void locate( std::string& , std::vector<uint> &) override;
    void locate2( std::string& , sdsl::bit_vector &) ;
    void locateNoTrie( std::string &, std::vector<uint> &) override;
    void display(const std::size_t& , const std::size_t&, std::string & ) override ;
    void display_trie(const std::size_t& , const std::size_t&, std::string & ) override ;
    void display_L_trie(const std::size_t& i, const std::size_t& j, std::string & s) override {
        SelfGrammarIndex::display_L_trie(i,j,s);
    };
    void set_code(const unsigned int & c) override { SelfGrammarIndex::set_code(c);}
    void load_rules_pt(fstream& f)override{}
    void load_sfx_pt(fstream& f)override{}

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
    bool bp_expand_prefix(const grammar_representation::g_long & X, std::string& s,const size_t & l,size_t & pos) const override {
        return SelfGrammarIndex::bp_expand_prefix(X,s,l,pos);
    }
    bool expand_prefix(const grammar_representation::g_long & X, std::string& s,const size_t & l,size_t & pos) const override{
        return SelfGrammarIndex::expand_prefix(X,s,l,pos);
    }

    int cmp_prefix(const grammar_representation::g_long & X,std::string::iterator&i, std::string::iterator& j)const override{
        return SelfGrammarIndex::cmp_prefix(X,i,j);
    }
    int cmp_suffix(const grammar_representation::g_long & X,std::string::iterator&i, std::string::iterator& j)const override{
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
            long int  l = long (- len + pos_p) - _g.offsetText(parent);
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
            long int  l = long (- len + pos_p) - _g.offsetText(parent);
            //unsigned int pre_parent = g_tree.pre_order(parent);
            find_second_occ(l,parent,occ);
        }

    }



protected:



};


#endif //IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXBS_H
