//
// Created by alpachec on 20-08-18.
//

#include <sdsl/lcp_bitcompressed.hpp>
#include <sdsl/rmq_succinct_sada.hpp>
#include "SelfGrammarIndexPT.h"




void SelfGrammarIndexPT::build(const std::string &text
#ifdef MEM_MONITOR
        , mem_monitor& mm
#endif
) {

    grammar not_compressed_grammar;
    std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx;
    SelfGrammarIndex::build_basics(text,not_compressed_grammar,grammar_sfx
#ifdef MEM_MONITOR
            ,mm
#endif
    );

    {
        m_patricia::patricia_tree<m_patricia::string_pairs> T;
        unsigned long id = 0;
        for (auto & i : grammar_sfx)
        {
            m_patricia::string_pairs s(text,++id);
            s.set_left(i.first.first);
            s.set_right(i.first.second);
            T.insert(s);
        }
        sfx_p_tree.build(T);
    }

    /*
     * Building Patricia Trees for rules.
     *
     * */
    {
        m_patricia::patricia_tree<m_patricia::rev_string_pairs> T;
        unsigned long id = 0;
        for (auto &&  r: not_compressed_grammar) {
                m_patricia::rev_string_pairs s(text,++id);
                s.set_left(r.second.l);
                s.set_right(r.second.r);
                T.insert(s);
        }

        rules_p_tree.build(T);
    }

}
void SelfGrammarIndexPT::locate2( std::string & pattern, sdsl::bit_vector & occ) {


    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }

    size_t p_n = pattern.size();
    const auto& g_tree = _g.get_parser_tree();
    size_t nnodes = g_tree.subtree(g_tree.root());

    std::vector<bool> mark(nnodes,false);

    for (size_t i = 1; i <= p_n ; ++i)
    {
        // CREATE PARTITIONS
        /////std::cout<<"////////////////////////PARTITION "<<i<<" /////////////////////////////////////////////////////////////////"<<std::endl;
        /*std::string p1,p2;
        p1.resize(i);
        p2.resize(p_n-i);

        std::copy(pattern.begin(),pattern.begin()+i,p1.begin());
        std::copy(pattern.begin()+i,pattern.end(),p2.begin());
        std::reverse(p1.begin(),p1.end());*/
        //BINARY SEARCH OVER THE Frev SORT RULES
        /////// std::cout<<"prefijo rev: "<<p1<<"\t sufijo: "<<p2;
        /////auto start = timer::now();
        /////////////////////////////////////////////////////////////////////////////////////////
        auto itera = pattern.begin() + i-1;


        //auto rows = rules_trie.matches(p1.begin(),p1.end());

        ////auto start = timer::now();

        /*
         *
         * Extracting range for rev(p[1...k]) in the rule patricia tree
         *
         * */

        m_patricia::rev_string_pairs sp1(pattern,1);
        sp1.set_left(0);
        sp1.set_right(i-1);

        const auto& rules_t = rules_p_tree.get_tree();
        auto node_match_rules = rules_p_tree.node_match(sp1);
        const auto& rules_leaf = rules_t.leafrank(node_match_rules);

        auto begin_rule_string = pattern.begin();
        auto end_rule_string = itera;
        auto r = bp_cmp_suffix(rules_leaf,end_rule_string,begin_rule_string);
        if(r != 0 || end_rule_string+1 != begin_rule_string)
            continue;

        size_t p_r1 = rules_t.leafrank(node_match_rules);
        size_t p_r2 = p_r1 + rules_t.leafnum(node_match_rules) - 1;



        /* std::string s_rule;
         s_rule.resize(p1.size());
         size_t pos_sr = 0;
         expand_suffix(rules_leaf,s_rule,sp1.size(),pos_sr);
         std::reverse(s_rule.begin(),s_rule.end());

         unsigned long p = 0;
         while(p < sp1.size() && s_rule[p] == sp1[p]) ++p;

         if(p == 0 && !sp1.empty()) continue;

         auto node_locus_rules = rules_p_tree.node_locus(sp1,p);
         std::pair<size_t , size_t > rows(0,0);
         rows.first = rules_t.leafrank(node_locus_rules);
         rows.first = rows.first + rules_t.leafnum(node_locus_rules) - 1;
         ////auto stop = timer::now();
         ////std::cout<<"\t\trules PT search "<<duration_cast<nanoseconds>(stop - start).count()<<"(ns)"<<std::endl;
         std::cout<<" rule PT search"<<std::endl;
         /////auto start = timer::now();

         //////////////////////////////////////////////////////////////
         size_t p_r1 = rows.first,p_r2 = rows.second;
         if(p_r1 == 0 && p_r2 == 0)continue;
         ////std::cout<<"rows_1("<<p_r1<<") \t rows_2)("<<p_r2<<")\n";
 */

        //// start = timer::now();
        /////////////////////////////////////////////////////////////////////////////////////////

        /*
        *
        * Extracting range for (p[k...m]) in the suffix patricia tree
        *
        * */

        m_patricia::string_pairs sp2(pattern,2);
        sp2.set_left(i);
        sp2.set_right(pattern.size()-1);

        const auto& suff_t = sfx_p_tree.get_tree();
        auto node_match_suff = sfx_p_tree.node_match(sp2);
        const auto& suff_leaf = suff_t.leafrank(node_match_suff);

        auto begin_sfx_string = itera+1;
        auto end_sfx_string = pattern.end() ;
        r = bp_cmp_suffix_grammar(suff_leaf,begin_sfx_string,end_sfx_string);
        if(r != 0 )
            continue;

        size_t p_c1 = suff_t.leafrank(node_match_suff);
        size_t p_c2 = p_c1 + suff_t.leafnum(node_match_suff) - 1;

        /*std::string s_sufx;
        expand_grammar_sfx((rules_leaf-1)*sampling + 1,s_sufx,p2.size());

        p = 0;
        while(p < sp2.size() && s_sufx[p] == sp2[p]) ++p;
        if(p == 0 && !sp1.empty()) continue;
        */
        /*      auto node_locus_suff = sfx_p_tree.node_locus(sp2,p);
              std::pair<size_t , size_t > cols(0,0);
              cols.first = rules_t.leafrank(node_locus_suff);
              cols.first = rows.first + rules_t.leafnum(node_locus_rules) - 1;


              size_t p_c1 = cols.first,p_c2 = cols.second;
              if(p_c1 == 0 && p_c2 == 0)continue;
      */
        ////  sampling_range_suff(p_c1,p_c2,S,p2);

        ////stop = timer::now();
        ////std::cout<<"\t\tbinary search logn sampling "<<duration_cast<nanoseconds>(stop - start).count()<<"(ns)"<<std::endl;

        //std::cout<<" sfx PT search"<<std::endl;


        // CHECK THE RANGE

        ////start = timer::now();

        /////////////////////////////////////////////////////////////////////////////////////////
        /*      std::string p1s , p2s;
              expand_grammar_sfx(p_r1,p1s,p1.length());
              ///std::cout<<"rows_1: "<<p1s<<std::endl;
              if(p1 != p1s) continue;
              expand_grammar_sfx(p_r2,p2s,p1.length());
              ///std::cout<<"rows_2: "<<p2s<<std::endl;
              if(p1 != p2s) continue;
              // CHECK THE RANGE

              // CHECK THE RANGE
              p1s =""; p2s="";
              expand_grammar_sfx(p_c1,p1s,p2.length());
              ///std::cout<<"cols_1: "<<p1s<<std::endl;
              if(p2 != p1s) continue;
              expand_grammar_sfx(p_c2,p2s,p2.length());
              ///std::cout<<"cols_2: "<<p2s<<std::endl;
              if(p2 != p2s) continue;
          */    /////////////////////////////////////////////////////////////////////////////////////////

        ////stop = timer::now();
        ////std::cout<<"\t\tcheck ranges time:" <<duration_cast<nanoseconds>(stop-start).count()<<"(ns)"<<std::endl;
        // CHECK THE RANGE

        std::vector< std::pair<size_t,size_t> > pairs;
        ////start = timer::now();
        /////////////////////////////////////////////////////////////////////////////////////////

        auto  x1 = (uint)p_r1,x2 = (uint)p_r2,y1 = (uint)p_c1,y2 = (uint)p_c2;
        grid.range2(x1,x2,y1,y2,pairs);



        ///std::cout<<"pairs size "<<pairs.size()<<std::endl;

        /////////////////////////////////////////////////////////////////////////////////////////
        ////stop = timer::now();
        ////std::cout<<"\t\t2d search binary relation time:" <<duration_cast<nanoseconds>(stop-start).count()<<"(ns)"<<std::endl;


        long len = itera-pattern.begin() +1;

        ////start = timer::now();
        for (auto &pair : pairs) {


            size_t p = grid.first_label_col(pair.second);
            size_t pos_p = _g.offsetText(g_tree[p]);
            unsigned int parent = g_tree.parent(g_tree[p]);
            long  l = long (- len + pos_p) - _g.offsetText(parent);

            if(mark[p] == false)
            {
                mark[p] = true;
                find_second_occ(l,parent,occ);
            }
            ///start = timer::now();

            //// stop = timer::now();
            ///std::cout<<"\t\tsearch_grammar_tree_second_occ time:" <<duration_cast<nanoseconds>(stop-start).count()<<"(ns)"<<std::endl;

            /*std::cout<<"occ"<<std::endl;
            for (auto &&i  :occ ) {
                std::cout<<i<<" ";
            }
            std::cout<<std::endl;*/

            /////////////////////////////////////////////////////////////////////////////////////////


        }

    }



}
void SelfGrammarIndexPT::locate( std::string & pattern, sdsl::bit_vector & occ) {

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }

    size_t p_n = pattern.size();
    const auto& g_tree = _g.get_parser_tree();

    ///size_t n_sj = grid.n_columns();

    /*vector<size_t > S (n_sj,1);
    for (size_t j = 0; j < n_sj; ++j)
        S[j] = j+1;*/



    /////std::cout<<"////////////////////////cadena "<<pattern<<" /////////////////////////////////////////////////////////////////"<<std::endl;
    for (size_t i = 1; i <= p_n ; ++i)
    {
        // CREATE PARTITIONS
        /////std::cout<<"////////////////////////PARTITION "<<i<<" /////////////////////////////////////////////////////////////////"<<std::endl;
        /*std::string p1,p2;
        p1.resize(i);
        p2.resize(p_n-i);

        std::copy(pattern.begin(),pattern.begin()+i,p1.begin());
        std::copy(pattern.begin()+i,pattern.end(),p2.begin());
        std::reverse(p1.begin(),p1.end());*/
        //BINARY SEARCH OVER THE Frev SORT RULES
        /////// std::cout<<"prefijo rev: "<<p1<<"\t sufijo: "<<p2;
        /////auto start = timer::now();
        /////////////////////////////////////////////////////////////////////////////////////////
        auto itera = pattern.begin() + i-1;


        //auto rows = rules_trie.matches(p1.begin(),p1.end());

        ////auto start = timer::now();

        /*
         *
         * Extracting range for rev(p[1...k]) in the rule patricia tree
         *
         * */

        m_patricia::rev_string_pairs sp1(pattern,1);
        sp1.set_left(0);
        sp1.set_right(i-1);

        const auto& rules_t = rules_p_tree.get_tree();
        auto node_match_rules = rules_p_tree.node_match(sp1);
        const auto& rules_leaf = rules_t.leafrank(node_match_rules);


        auto begin_rule_string = pattern.begin();
        auto end_rule_string = itera;
        auto r = bp_cmp_suffix(rules_leaf,end_rule_string,begin_rule_string);
        if(r != 0 || end_rule_string+1 != begin_rule_string)
            continue;

        size_t p_r1 = rules_t.leafrank(node_match_rules);
        size_t p_r2 = p_r1 + rules_t.leafnum(node_match_rules) - 1;



        /* std::string s_rule;
         s_rule.resize(p1.size());
         size_t pos_sr = 0;
         expand_suffix(rules_leaf,s_rule,sp1.size(),pos_sr);
         std::reverse(s_rule.begin(),s_rule.end());

         unsigned long p = 0;
         while(p < sp1.size() && s_rule[p] == sp1[p]) ++p;

         if(p == 0 && !sp1.empty()) continue;

         auto node_locus_rules = rules_p_tree.node_locus(sp1,p);
         std::pair<size_t , size_t > rows(0,0);
         rows.first = rules_t.leafrank(node_locus_rules);
         rows.first = rows.first + rules_t.leafnum(node_locus_rules) - 1;
         ////auto stop = timer::now();
         ////std::cout<<"\t\trules PT search "<<duration_cast<nanoseconds>(stop - start).count()<<"(ns)"<<std::endl;
         std::cout<<" rule PT search"<<std::endl;
         /////auto start = timer::now();

         //////////////////////////////////////////////////////////////
         size_t p_r1 = rows.first,p_r2 = rows.second;
         if(p_r1 == 0 && p_r2 == 0)continue;
         ////std::cout<<"rows_1("<<p_r1<<") \t rows_2)("<<p_r2<<")\n";
 */

        //// start = timer::now();
        /////////////////////////////////////////////////////////////////////////////////////////

        /*
        *
        * Extracting range for (p[k...m]) in the suffix patricia tree
        *
        * */

        m_patricia::string_pairs sp2(pattern,2);
        sp2.set_left(i);
        sp2.set_right(pattern.size()-1);

        const auto& suff_t = sfx_p_tree.get_tree();
        auto node_match_suff = sfx_p_tree.node_match(sp2);
        const auto& suff_leaf = suff_t.leafrank(node_match_suff);

        auto begin_sfx_string = itera+1;
        auto end_sfx_string = pattern.end() ;
        r = bp_cmp_suffix_grammar(suff_leaf,begin_sfx_string,end_sfx_string);
        if(r != 0 )
            continue;

        size_t p_c1 = suff_t.leafrank(node_match_suff);
        size_t p_c2 = p_c1 + suff_t.leafnum(node_match_suff) - 1;

        /*std::string s_sufx;
        expand_grammar_sfx((rules_leaf-1)*sampling + 1,s_sufx,p2.size());

        p = 0;
        while(p < sp2.size() && s_sufx[p] == sp2[p]) ++p;
        if(p == 0 && !sp1.empty()) continue;
        */
        /*      auto node_locus_suff = sfx_p_tree.node_locus(sp2,p);
              std::pair<size_t , size_t > cols(0,0);
              cols.first = rules_t.leafrank(node_locus_suff);
              cols.first = rows.first + rules_t.leafnum(node_locus_rules) - 1;


              size_t p_c1 = cols.first,p_c2 = cols.second;
              if(p_c1 == 0 && p_c2 == 0)continue;
      */
        ////  sampling_range_suff(p_c1,p_c2,S,p2);

        ////stop = timer::now();
        ////std::cout<<"\t\tbinary search logn sampling "<<duration_cast<nanoseconds>(stop - start).count()<<"(ns)"<<std::endl;

        //std::cout<<" sfx PT search"<<std::endl;


        // CHECK THE RANGE

        ////start = timer::now();

        /////////////////////////////////////////////////////////////////////////////////////////
        /*      std::string p1s , p2s;
              expand_grammar_sfx(p_r1,p1s,p1.length());
              ///std::cout<<"rows_1: "<<p1s<<std::endl;
              if(p1 != p1s) continue;
              expand_grammar_sfx(p_r2,p2s,p1.length());
              ///std::cout<<"rows_2: "<<p2s<<std::endl;
              if(p1 != p2s) continue;
              // CHECK THE RANGE

              // CHECK THE RANGE
              p1s =""; p2s="";
              expand_grammar_sfx(p_c1,p1s,p2.length());
              ///std::cout<<"cols_1: "<<p1s<<std::endl;
              if(p2 != p1s) continue;
              expand_grammar_sfx(p_c2,p2s,p2.length());
              ///std::cout<<"cols_2: "<<p2s<<std::endl;
              if(p2 != p2s) continue;
          */    /////////////////////////////////////////////////////////////////////////////////////////

        ////stop = timer::now();
        ////std::cout<<"\t\tcheck ranges time:" <<duration_cast<nanoseconds>(stop-start).count()<<"(ns)"<<std::endl;
        // CHECK THE RANGE

        /////std::cout<<" END THE PATRICIA TREE"<<std::endl;

        std::vector< std::pair<size_t,size_t> > pairs;

        auto  x1 = (uint)p_r1,x2 = (uint)p_r2,y1 = (uint)p_c1,y2 = (uint)p_c2;
        grid.range2(x1,x2,y1,y2,pairs);


        long len = itera-pattern.begin() +1;

        for (auto &pair : pairs) {


            size_t p = grid.first_label_col(pair.second);
            size_t pos_p = _g.offsetText(g_tree[p]);
            unsigned int parent = g_tree.parent(g_tree[p]);
            long   l = long(- len + pos_p) - _g.offsetText(parent);


            find_second_occ(l,parent,occ);


        }

    }



}

void SelfGrammarIndexPT::locate( std::string & pattern, std::vector<uint> & occ) {

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }

    size_t p_n = pattern.size();
    const auto& g_tree = _g.get_parser_tree();

    ///size_t n_sj = grid.n_columns();

    /*vector<size_t > S (n_sj,1);
    for (size_t j = 0; j < n_sj; ++j)
        S[j] = j+1;*/



    /////std::cout<<"////////////////////////cadena "<<pattern<<" /////////////////////////////////////////////////////////////////"<<std::endl;
    for (size_t i = 1; i <= p_n ; ++i)
    {
        // CREATE PARTITIONS
        /////std::cout<<"////////////////////////PARTITION "<<i<<" /////////////////////////////////////////////////////////////////"<<std::endl;
        /*std::string p1,p2;
        p1.resize(i);
        p2.resize(p_n-i);

        std::copy(pattern.begin(),pattern.begin()+i,p1.begin());
        std::copy(pattern.begin()+i,pattern.end(),p2.begin());
        std::reverse(p1.begin(),p1.end());*/
        //BINARY SEARCH OVER THE Frev SORT RULES
        /////// std::cout<<"prefijo rev: "<<p1<<"\t sufijo: "<<p2;
        /////auto start = timer::now();
        /////////////////////////////////////////////////////////////////////////////////////////
        auto itera = pattern.begin() + i-1;


        //auto rows = rules_trie.matches(p1.begin(),p1.end());

        ////auto start = timer::now();

        /*
         *
         * Extracting range for rev(p[1...k]) in the rule patricia tree
         *
         * */

        m_patricia::rev_string_pairs sp1(pattern,1);
        sp1.set_left(0);
        sp1.set_right(i-1);

        const auto& rules_t = rules_p_tree.get_tree();
        auto node_match_rules = rules_p_tree.node_match(sp1);
        const auto& rules_leaf = rules_t.leafrank(node_match_rules);

        auto begin_rule_string = pattern.begin();
        auto end_rule_string = itera;
        auto r = bp_cmp_suffix(rules_leaf,end_rule_string,begin_rule_string);
        if(r != 0 || end_rule_string + 1 != begin_rule_string)
            continue;

        size_t p_r1 = rules_t.leafrank(node_match_rules);
        size_t p_r2 = p_r1 + rules_t.leafnum(node_match_rules) - 1;



        /* std::string s_rule;
         s_rule.resize(p1.size());
         size_t pos_sr = 0;
         expand_suffix(rules_leaf,s_rule,sp1.size(),pos_sr);
         std::reverse(s_rule.begin(),s_rule.end());

         unsigned long p = 0;
         while(p < sp1.size() && s_rule[p] == sp1[p]) ++p;

         if(p == 0 && !sp1.empty()) continue;

         auto node_locus_rules = rules_p_tree.node_locus(sp1,p);
         std::pair<size_t , size_t > rows(0,0);
         rows.first = rules_t.leafrank(node_locus_rules);
         rows.first = rows.first + rules_t.leafnum(node_locus_rules) - 1;
         ////auto stop = timer::now();
         ////std::cout<<"\t\trules PT search "<<duration_cast<nanoseconds>(stop - start).count()<<"(ns)"<<std::endl;
         std::cout<<" rule PT search"<<std::endl;
         /////auto start = timer::now();

         //////////////////////////////////////////////////////////////
         size_t p_r1 = rows.first,p_r2 = rows.second;
         if(p_r1 == 0 && p_r2 == 0)continue;
         ////std::cout<<"rows_1("<<p_r1<<") \t rows_2)("<<p_r2<<")\n";
 */

        //// start = timer::now();
        /////////////////////////////////////////////////////////////////////////////////////////

        /*
        *
        * Extracting range for (p[k...m]) in the suffix patricia tree
        *
        * */

        m_patricia::string_pairs sp2(pattern,2);
        sp2.set_left(i);
        sp2.set_right(pattern.size()-1);

        const auto& suff_t = sfx_p_tree.get_tree();
        auto node_match_suff = sfx_p_tree.node_match(sp2);
        const auto& suff_leaf = suff_t.leafrank(node_match_suff);

        auto begin_sfx_string = itera+1;
        auto end_sfx_string = pattern.end() ;
        r = bp_cmp_suffix_grammar(suff_leaf,begin_sfx_string,end_sfx_string);
        if(r != 0 )
            continue;

        size_t p_c1 = suff_t.leafrank(node_match_suff);
        size_t p_c2 = p_c1 + suff_t.leafnum(node_match_suff) - 1;



        /*std::string s_sufx;
        expand_grammar_sfx((rules_leaf-1)*sampling + 1,s_sufx,p2.size());

        p = 0;
        while(p < sp2.size() && s_sufx[p] == sp2[p]) ++p;
        if(p == 0 && !sp1.empty()) continue;
        */
        /*      auto node_locus_suff = sfx_p_tree.node_locus(sp2,p);
              std::pair<size_t , size_t > cols(0,0);
              cols.first = rules_t.leafrank(node_locus_suff);
              cols.first = rows.first + rules_t.leafnum(node_locus_rules) - 1;


              size_t p_c1 = cols.first,p_c2 = cols.second;
              if(p_c1 == 0 && p_c2 == 0)continue;
      */
        ////  sampling_range_suff(p_c1,p_c2,S,p2);

        ////stop = timer::now();
        ////std::cout<<"\t\tbinary search logn sampling "<<duration_cast<nanoseconds>(stop - start).count()<<"(ns)"<<std::endl;

        //std::cout<<" sfx PT search"<<std::endl;


        // CHECK THE RANGE

        ////start = timer::now();

        /////////////////////////////////////////////////////////////////////////////////////////
        /*      std::string p1s , p2s;
              expand_grammar_sfx(p_r1,p1s,p1.length());
              ///std::cout<<"rows_1: "<<p1s<<std::endl;
              if(p1 != p1s) continue;
              expand_grammar_sfx(p_r2,p2s,p1.length());
              ///std::cout<<"rows_2: "<<p2s<<std::endl;
              if(p1 != p2s) continue;
              // CHECK THE RANGE

              // CHECK THE RANGE
              p1s =""; p2s="";
              expand_grammar_sfx(p_c1,p1s,p2.length());
              ///std::cout<<"cols_1: "<<p1s<<std::endl;
              if(p2 != p1s) continue;
              expand_grammar_sfx(p_c2,p2s,p2.length());
              ///std::cout<<"cols_2: "<<p2s<<std::endl;
              if(p2 != p2s) continue;
          */    /////////////////////////////////////////////////////////////////////////////////////////

        ////stop = timer::now();
        ////std::cout<<"\t\tcheck ranges time:" <<duration_cast<nanoseconds>(stop-start).count()<<"(ns)"<<std::endl;
        // CHECK THE RANGE

        /////std::cout<<" END THE PATRICIA TREE"<<std::endl;

/*

        long len = itera-pattern.begin() +1;
        binary_relation::bin_long  x1 = (uint)p_r1,x2 = (uint)p_r2,y1 = (uint)p_c1,y2 = (uint)p_c2;
        //std::vector< std::pair<size_t,size_t> > pairs;
        //grid.range2(x1,x2,y1,y2,pairs);

        fstream f("pt_ptos",std::ios::out|std::ios::binary|std::ios::app);


       */
/* size_t oo = pairs.size();
        sdsl::serialize(oo,f);
        for (auto &&  ll: pairs) {
            sdsl::serialize(ll.second,f);
        }
        sdsl::serialize(len,f);*//*



        sdsl::serialize((uint)x1,f);
        sdsl::serialize((uint)x2,f);
        sdsl::serialize((uint)y1,f);
        sdsl::serialize((uint)y2,f);
        sdsl::serialize(len,f);

*/






        std::vector< std::pair<size_t,size_t> > pairs;
        auto  x1 = (uint)p_r1,x2 = (uint)p_r2,y1 = (uint)p_c1,y2 = (uint)p_c2;
        grid.range2(x1,x2,y1,y2,pairs);

        long len = itera-pattern.begin() +1;

        for (auto &pair : pairs) {


            size_t p = grid.first_label_col(pair.second);
            size_t pos_p = _g.offsetText(g_tree[p]);
            unsigned int parent = g_tree.parent(g_tree[p]);
            long  l = long(- len + pos_p) - _g.offsetText(parent);
            find_second_occ(l,parent,occ);
            //unsigned int preP = g_tree.pre_order(parent);
            //find_second_occ_rec(l, preP, occ);

        }


    }



}

void SelfGrammarIndexPT::locateNoTrie( std::string & pattern, std::vector<uint> & occ) {

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }

    size_t p_n = pattern.size();
    const auto& g_tree = _g.get_parser_tree();

    for (size_t i = 1; i <= p_n ; ++i)
    {
       auto itera = pattern.begin() + i-1;


        /*
         *
         * Extracting range for rev(p[1...k]) in the rule patricia tree
         *
         * */

        m_patricia::rev_string_pairs sp1(pattern,1);
        sp1.set_left(0);
        sp1.set_right(i-1);

        const auto& rules_t = rules_p_tree.get_tree();
        auto node_match_rules = rules_p_tree.node_match(sp1);
        const auto& rules_leaf = rules_t.leafrank(node_match_rules);

        auto begin_rule_string = pattern.begin();
        auto end_rule_string = itera;
        auto r = dfs_cmp_suffix(rules_leaf,end_rule_string,begin_rule_string);
        if(r != 0 || end_rule_string + 1 != begin_rule_string)
            continue;

        size_t p_r1 = rules_t.leafrank(node_match_rules);
        size_t p_r2 = p_r1 + rules_t.leafnum(node_match_rules) - 1;


        /*
        *
        * Extracting range for (p[k...m]) in the suffix patricia tree
        *
        * */

        m_patricia::string_pairs sp2(pattern,2);
        sp2.set_left(i);
        sp2.set_right(pattern.size()-1);

        const auto& suff_t = sfx_p_tree.get_tree();
        auto node_match_suff = sfx_p_tree.node_match(sp2);
        const auto& suff_leaf = suff_t.leafrank(node_match_suff);

        auto begin_sfx_string = itera+1;
        auto end_sfx_string = pattern.end() ;
        r = cmp_suffix_grammar(suff_leaf,begin_sfx_string,end_sfx_string);
        if(r != 0 )
            continue;

        size_t p_c1 = suff_t.leafrank(node_match_suff);
        size_t p_c2 = p_c1 + suff_t.leafnum(node_match_suff) - 1;


        std::vector< std::pair<size_t,size_t> > pairs;
        auto  x1 = (uint)p_r1,x2 = (uint)p_r2,y1 = (uint)p_c1,y2 = (uint)p_c2;
        grid.range2(x1,x2,y1,y2,pairs);

        long len = itera-pattern.begin() +1;

        for (auto &pair : pairs) {


            size_t p = grid.first_label_col(pair.second);
            size_t pos_p = _g.offsetText(g_tree[p]);
            unsigned int parent = g_tree.parent(g_tree[p]);
            long l = long (- len + pos_p) - _g.offsetText(parent);
            find_second_occ(l,parent,occ);


        }


    }



}

void SelfGrammarIndexPT::save(std::fstream & f_out) {
    SelfGrammarIndex::save(f_out);
    sfx_p_tree.save(f_out);
    rules_p_tree.save(f_out);
}

void SelfGrammarIndexPT::load(std::fstream & f_in) {
    SelfGrammarIndex::load(f_in);
    sfx_p_tree.load(f_in);
    rules_p_tree.load(f_in);


}

size_t SelfGrammarIndexPT::size_in_bytes() const {
//    std::cout<<"SelfGrammarIndexPT::size_in_bytes()\n";
    return SelfGrammarIndex::size_in_bytes() + sfx_p_tree.size_in_bytes() + rules_p_tree.size_in_bytes();
}

void SelfGrammarIndexPT::display(const std::size_t &i, const std::size_t &j, std::string &s) {
    SelfGrammarIndex::display( i,j,s);
}

void
SelfGrammarIndexPT::build(const SelfGrammarIndex::grammar_representation & G, const SelfGrammarIndex::range_search2d & R,
        const m_patricia::compact_patricia_tree &sfx, const m_patricia::compact_patricia_tree &rules) {
    _g = G;
    grid = R;
    sfx_p_tree = sfx;
    rules_p_tree = rules;
}

SelfGrammarIndexPT::~SelfGrammarIndexPT() {

}

void SelfGrammarIndexPT::build_suffix(const string & text, fstream & suffixes, fstream& repair_g

#ifdef MEM_MONITOR
        ,mem_monitor &mm
#endif
) {

    grammar not_compressed_grammar;


    not_compressed_grammar.load(repair_g);

    std::cout<<"Size of grammar "<<not_compressed_grammar.get_size()<<"\n";

    std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx;

    size_t num_sfx = 0;

    sdsl::load(num_sfx,suffixes);

    grammar_sfx.resize(num_sfx);

    for (auto && e : grammar_sfx ) {

        sdsl::load(e.first.first,suffixes);
        sdsl::load(e.first.second,suffixes);
        sdsl::load(e.second.first,suffixes);
        sdsl::load(e.second.second,suffixes);
    }

    std::cout<<"Building Sampled Patricia Trees for suffixes."<<std::endl;

    //sampling = (unsigned long)(log2(text.size()) * log2(log2(not_compressed_grammar.n_rules()))/log2(not_compressed_grammar.n_rules()));
    //sampling = 16;
    //sampling = ( sampling != 0 )?sampling:1;
    //std::cout<<"\t sampled: "<<sampling<<std::endl;

    std::cout<<"\t numero de sufijos count: "<<num_sfx<<std::endl;
    std::cout<<"\t grammar_sfx.size() "<<grammar_sfx.size()<<std::endl;
    {
        m_patricia::patricia_tree<m_patricia::string_pairs> T;
        unsigned long id = 0;
        for (auto & i : grammar_sfx)
        {
            m_patricia::string_pairs s(text,++id);
            s.set_left(i.first.first);
            s.set_right(i.first.second);
            T.insert(s);
        }
        //T.print();
        sfx_p_tree.build(T);
        ///std::cout<<"sfx_p_tree size "<<sfx_p_tree.size_in_bytes()<<std::endl;
        ////sfx_p_tree.print_size_in_bytes();
    }

    /*
     * Building Patricia Trees for rules.
     *
     * */
    std::cout<<"Building Patricia Trees for rules."<<std::endl;
    {
        m_patricia::patricia_tree<m_patricia::rev_string_pairs> T;
        unsigned long id = 0;
        for (auto &&  r: not_compressed_grammar) {
            //  if( id % sampling == 0)
            //  {
            m_patricia::rev_string_pairs s(text,++id);
            s.set_left(r.second.l);
            s.set_right(r.second.r);
            T.insert(s);
            // }else{
            //     ++id;
            // }
        }

        //T.print();
        rules_p_tree.build(T);
        ///std::cout<<"rules_p_tree size "<<rules_p_tree.size_in_bytes()<<std::endl;
        ////rules_p_tree.print_size_in_bytes();
    }
}

