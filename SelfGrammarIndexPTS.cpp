//
// Created by alpachec on 20-09-18.
//


#include <sdsl/lcp_bitcompressed.hpp>
#include <sdsl/rmq_succinct_sada.hpp>

#include "SelfGrammarIndexPTS.h"


void SelfGrammarIndexPTS::build(const std::string &text
#ifdef MEM_MONITOR
        , mem_monitor& mm
#endif
)
{



    grammar not_compressed_grammar;
    std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx;
    SelfGrammarIndex::build_basics(text,not_compressed_grammar,grammar_sfx
#ifdef MEM_MONITOR
       ,mm
#endif
    );

    /*
     * Building Sampled Patricia Trees for suffixes.
     *
     * Sampling by log(u)log(log(n))/log(n)
     * where n is the number of symbols in the repair grammar and u is the length of the original text
     * */

#ifdef MEM_MONITOR
    std::cout<<"Building Sampled Patricia Trees."<<std::endl;
    auto start = timer::now();
#endif
    {
        m_patricia::patricia_tree<m_patricia::string_pairs> T;
        unsigned long id = 0;
        for (int i = 0; i < grammar_sfx.size(); i += sampling)
        {
            m_patricia::string_pairs s(text,++id);
            s.set_left(grammar_sfx[i].first.first);
            s.set_right(grammar_sfx[i].first.second);
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
            if( id % sampling == 0)
            {
                m_patricia::rev_string_pairs s(text,++id);
                s.set_left(r.second.l);
                s.set_right(r.second.r);
                T.insert(s);
            }else{
                ++id;
            }
        }
        rules_p_tree.build(T);
    }
#ifdef MEM_MONITOR
    auto stop = timer::now();
    CLogger::GetLogger()->model["build-patricia-trees"] = duration_cast<microseconds>(stop-start).count();;
#endif

}


void SelfGrammarIndexPTS::locate( std::string & pattern, std::vector<uint> &occ)
{

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }

    size_t p_n = pattern.size();
    const auto& g_tree = _g.get_parser_tree();
    auto nrules = _g.n_rules()-1;
    auto nsfx = grid.n_columns();


    for (size_t i = 1; i <= p_n ; ++i) {

        auto itera = pattern.begin() + i - 1;
        /*
         *
         * Extracting range for rev(p[1...k]) in the rule patricia tree
         *
         * */

        m_patricia::rev_string_pairs sp1(pattern, 1);
        sp1.set_left(0);
        sp1.set_right(i - 1);

        const auto &rules_t = rules_p_tree.get_tree();
        auto node_match_rules = rules_p_tree.node_match(sp1);
        //const auto& rules_leaf = rules_t.leafrank(node_match_rules);

        size_t p_r2 = nrules, p_r1 = rules_t.leafrank(node_match_rules);

        auto begin_r_string = pattern.begin();
        auto end_r_string = itera;

        auto rcmp_rules = bp_cmp_suffix(_st(p_r1), end_r_string, begin_r_string);
        auto match_rules = itera - end_r_string;


        begin_r_string = pattern.begin();
        end_r_string = itera;

        if (match_rules == sp1.size()) // match_node  == locus_node && all the symbols of pattern are consumed
        {

            p_r2 = p_r1 + rules_t.leafnum(node_match_rules) - 1;
            // sampled rules corresponding to leaves.
            size_t ii = _st(p_r1), jj = _st(p_r2);
            // new range of search
            size_t ii_low = (ii == 1) ? 1 : ii - sampling;
            size_t jj_hight = (jj + sampling <= nrules) ? jj + sampling : nrules;

            grammar_representation::g_long lb = ii_low, ub = ii;
            bool found = false;
            lower_bound(found,lb, ub, [&begin_r_string,&end_r_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r = bp_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });
            if(!found)
                continue;

            p_r1 = lb;

            lb = jj, ub = jj_hight;
            //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND

            found = false;
            upper_bound(found,lb,ub,[&begin_r_string,&end_r_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r =  bp_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });
            if(!found)
                continue;

            p_r2 = ub;

            // sampling_range_rules(p_r1,p_r2,begin_r_string,end_r_string);

        } else  // if all the symbols of the patterns were not consumed
        {
            unsigned int pos_locus = 0;
            auto locus_node_rules = rules_p_tree.node_locus(sp1, match_rules, pos_locus);
            // find the left most leaf in the real range
            size_t ii, jj;
            if (rules_t.isleaf(locus_node_rules)) {
                p_r1 = rules_t.leafrank(node_match_rules);
                ii = (p_r1 == 1) ? 1 : p_r1 - 1;
                ii = _st(ii), jj = ii + ((p_r1 == 1) ? sampling : 2 * sampling);
            } else {
                p_r1 = rules_p_tree.find_child_range(locus_node_rules, sp1, pos_locus, rcmp_rules);
                // sampled rules corresponding to leaves.
                ii = _st(p_r1), jj = ii + sampling;

            }

            jj = (jj < nrules) ? jj : nrules;




            ///////////////////////////////////////////////////
            grammar_representation::g_long lb = ii, ub = jj;
            bool found = false;
            lower_bound(found,lb, ub, [&begin_r_string,&end_r_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r = bp_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });
            if(!found)
                continue;

            p_r1 = lb;
            ub = jj;

            found = false;
            upper_bound(found,lb,ub,[&begin_r_string,&end_r_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r =  bp_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });
            if(!found)
                continue;

            p_r2 = ub;


        }

        /*
         *
         * Extracting range for (p[k...m]) in the suffix patricia tree
         *
         * */
        //start = timer::now();
        m_patricia::string_pairs sp2(pattern, 2);
        sp2.set_left(i);
        sp2.set_right(pattern.size() - 1);

        const auto &suff_t = sfx_p_tree.get_tree();
        auto node_match_suff = sfx_p_tree.node_match(sp2);
        //const auto& suff_leaf = suff_t.leafrank(node_match_suff);

        size_t p_c2 = nsfx, p_c1 = suff_t.leafrank(node_match_suff);

        auto begin_sfx_string = itera + 1;
        auto end_sfx_string = pattern.end();

        auto rcmp_sfx = bp_cmp_suffix_grammar(_st(p_c1), begin_sfx_string, end_sfx_string);
        auto match = begin_sfx_string - itera - 1;


        begin_sfx_string = itera + 1;
        end_sfx_string = pattern.end();

        if (match == sp2.size())// match_node  == locus_node && all the symbols of pattern are consumed
        {
            p_c2 = p_c1 + suff_t.leafnum(node_match_suff) - 1;
            // sampled suffix corresponding to leaves.
            size_t ii = _st(p_c1), jj = _st(p_c2);
            // new range of search
            size_t ii_low = (ii == 1) ? 1 : ii - sampling;
            size_t jj_hight = (jj + sampling <= nsfx) ? jj + sampling : nsfx;

            //////////////////////////////////////////////

             grammar_representation::g_long lb = ii_low, ub = ii;
             bool found = false;
            lower_bound(found,lb, ub, [&begin_sfx_string,&end_sfx_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;
                auto r = bp_cmp_suffix_grammar(a, begin, end);
                return r;
            });
             if(!found)
                continue;

             p_c1 = lb;

             lb = jj, ub = jj_hight;
             //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND

            found = false;
            upper_bound(found,lb,ub,[&begin_sfx_string,&end_sfx_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;

                auto r =  bp_cmp_suffix_grammar(a,begin,end);
                return r;
            });
            if(!found)
                continue;

             p_c2 = ub;

        } else {// if all the symbols of the patterns are not consumed


            unsigned int pos_locus;
            auto locus_node_suff = sfx_p_tree.node_locus(sp2, match,pos_locus);

            size_t ii, jj;

            if (suff_t.isleaf(locus_node_suff)) {

                p_c1 = suff_t.leafrank(locus_node_suff);
                ii = (p_c1 == 1) ? 1 : p_c1 - 1;
                ii = _st(ii), jj = ii + ((p_c1 == 1) ? sampling : 2 * sampling);


            } else {
                // find the left most leaf in the real range
                p_c1 = sfx_p_tree.find_child_range(locus_node_suff, sp2, pos_locus, rcmp_sfx);
                ii = _st(p_c1), jj = ii + sampling;

            }
            jj = (jj < nsfx) ? jj : nsfx;


             ///////////////////////////////////////////////////


            grammar_representation::g_long lb = ii, ub = jj;
            bool found = false;
            lower_bound(found,lb, ub, [&begin_sfx_string,&end_sfx_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;

                auto r =  bp_cmp_suffix_grammar(a,begin,end);
                return r;
            });
            if(!found)
                continue;

            p_c1 = lb;
            ub = jj;
            found = false;
            upper_bound(found,lb,ub,[&begin_sfx_string,&end_sfx_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;
                auto r = bp_cmp_suffix_grammar(a, begin, end);
                return r;
            });
            if(!found) continue;

            p_c2 = ub;

        }

        auto x1 = (uint) p_r1, x2 = (uint) p_r2, y1 = (uint) p_c1, y2 = (uint) p_c2;


        long len = itera-pattern.begin() +1;


        find_second_occ(x1,x2,y1,y2,len,occ);

    }
}


void SelfGrammarIndexPTS::locateNoTrie( std::string & pattern, std::vector<uint> &occ)
{

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }

    size_t p_n = pattern.size();
    const auto& g_tree = _g.get_parser_tree();
    auto nrules = _g.n_rules()-1;
    auto nsfx = grid.n_columns();


    for (size_t i = 1; i <= p_n ; ++i) {


        auto itera = pattern.begin() + i - 1;
      /*
         *
         * Extracting range for rev(p[1...k]) in the rule patricia tree
         *
         * */

        m_patricia::rev_string_pairs sp1(pattern, 1);
        sp1.set_left(0);
        sp1.set_right(i - 1);

        const auto &rules_t = rules_p_tree.get_tree();
        auto node_match_rules = rules_p_tree.node_match(sp1);
        //const auto& rules_leaf = rules_t.leafrank(node_match_rules);

        size_t p_r2 = nrules, p_r1 = rules_t.leafrank(node_match_rules);

        auto begin_r_string = pattern.begin();
        auto end_r_string = itera;

        auto rcmp_rules = dfs_cmp_suffix(_st(p_r1), end_r_string, begin_r_string);
        auto match_rules = itera - end_r_string;


        begin_r_string = pattern.begin();
        end_r_string = itera;

        if (match_rules == sp1.size()) // match_node  == locus_node && all the symbols of pattern are consumed
        {

            p_r2 = p_r1 + rules_t.leafnum(node_match_rules) - 1;
            // sampled rules corresponding to leaves.
            size_t ii = _st(p_r1), jj = _st(p_r2);
            // new range of search
            size_t ii_low = (ii == 1) ? 1 : ii - sampling;
            size_t jj_hight = (jj + sampling <= nrules) ? jj + sampling : nrules;

            grammar_representation::g_long lb = ii_low, ub = ii;
            bool found = false;
            lower_bound(found,lb, ub, [&begin_r_string,&end_r_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r = dfs_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });
            if(!found)
                continue;

            p_r1 = lb;

            lb = jj, ub = jj_hight;
            //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND

            found = false;
            upper_bound(found,lb,ub,[&begin_r_string,&end_r_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r =  dfs_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });
            if(!found)
                continue;

            p_r2 = ub;

            // sampling_range_rules(p_r1,p_r2,begin_r_string,end_r_string);

        } else  // if all the symbols of the patterns were not consumed
        {
            unsigned int pos_locus = 0;
            auto locus_node_rules = rules_p_tree.node_locus(sp1, match_rules, pos_locus);
            // find the left most leaf in the real range
            size_t ii, jj;
            if (rules_t.isleaf(locus_node_rules)) {
                p_r1 = rules_t.leafrank(node_match_rules);
                ii = (p_r1 == 1) ? 1 : p_r1 - 1;
                ii = _st(ii), jj = ii + ((p_r1 == 1) ? sampling : 2 * sampling);
            } else {
                p_r1 = rules_p_tree.find_child_range(locus_node_rules, sp1, pos_locus, rcmp_rules);
                // sampled rules corresponding to leaves.
                ii = _st(p_r1), jj = ii + sampling;

            }

            jj = (jj < nrules) ? jj : nrules;




            ///////////////////////////////////////////////////
            grammar_representation::g_long lb = ii, ub = jj;
            bool found = false;
            lower_bound(found,lb, ub, [&begin_r_string,&end_r_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r = dfs_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });
            if(!found)
                continue;

            p_r1 = lb;
            ub = jj;

            found = false;
            upper_bound(found,lb,ub,[&begin_r_string,&end_r_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r =  dfs_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });
            if(!found)
                continue;

            p_r2 = ub;


        }

        /*
         *
         * Extracting range for (p[k...m]) in the suffix patricia tree
         *
         * */
        //start = timer::now();
        m_patricia::string_pairs sp2(pattern, 2);
        sp2.set_left(i);
        sp2.set_right(pattern.size() - 1);

        const auto &suff_t = sfx_p_tree.get_tree();
        auto node_match_suff = sfx_p_tree.node_match(sp2);
        //const auto& suff_leaf = suff_t.leafrank(node_match_suff);

        size_t p_c2 = nsfx, p_c1 = suff_t.leafrank(node_match_suff);

        auto begin_sfx_string = itera + 1;
        auto end_sfx_string = pattern.end();

        auto rcmp_sfx = cmp_suffix_grammar(_st(p_c1), begin_sfx_string, end_sfx_string);
        auto match = begin_sfx_string - itera - 1;


        begin_sfx_string = itera + 1;
        end_sfx_string = pattern.end();

        if (match == sp2.size())// match_node  == locus_node && all the symbols of pattern are consumed
        {
            p_c2 = p_c1 + suff_t.leafnum(node_match_suff) - 1;
            // sampled suffix corresponding to leaves.
            size_t ii = _st(p_c1), jj = _st(p_c2);
            // new range of search
            size_t ii_low = (ii == 1) ? 1 : ii - sampling;
            size_t jj_hight = (jj + sampling <= nsfx) ? jj + sampling : nsfx;

            //////////////////////////////////////////////

            grammar_representation::g_long lb = ii_low, ub = ii;
            bool found = false;
            lower_bound(found,lb, ub, [&begin_sfx_string,&end_sfx_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;
                auto r = cmp_suffix_grammar(a, begin, end);
                return r;
            });
            if(!found)
                continue;

            p_c1 = lb;

            lb = jj, ub = jj_hight;
            //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND

            found = false;
            upper_bound(found,lb,ub,[&begin_sfx_string,&end_sfx_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;

                auto r =  cmp_suffix_grammar(a,begin,end);
                return r;
            });
            if(!found)
                continue;

            p_c2 = ub;

        } else {// if all the symbols of the patterns are not consumed


            unsigned int pos_locus;
            auto locus_node_suff = sfx_p_tree.node_locus(sp2, match,pos_locus);

            size_t ii, jj;

            if (suff_t.isleaf(locus_node_suff)) {

                p_c1 = suff_t.leafrank(locus_node_suff);
                ii = (p_c1 == 1) ? 1 : p_c1 - 1;
                ii = _st(ii), jj = ii + ((p_c1 == 1) ? sampling : 2 * sampling);


            } else {
                // find the left most leaf in the real range
                p_c1 = sfx_p_tree.find_child_range(locus_node_suff, sp2, pos_locus, rcmp_sfx);
                ii = _st(p_c1), jj = ii + sampling;

            }
            jj = (jj < nsfx) ? jj : nsfx;


            ///////////////////////////////////////////////////


            grammar_representation::g_long lb = ii, ub = jj;
            bool found = false;
            lower_bound(found,lb, ub, [&begin_sfx_string,&end_sfx_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;

                auto r =  cmp_suffix_grammar(a,begin,end);
                return r;
            });
            if(!found)
                continue;

            p_c1 = lb;
            ub = jj;
            found = false;
            upper_bound(found,lb,ub,[&begin_sfx_string,&end_sfx_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;
                auto r = cmp_suffix_grammar(a, begin, end);
                return r;
            });
            if(!found) continue;

            p_c2 = ub;

        }

//        std::vector<std::pair<size_t, size_t> > pairs;
        auto x1 = (uint) p_r1, x2 = (uint) p_r2, y1 = (uint) p_c1, y2 = (uint) p_c2;


        long len = itera-pattern.begin() +1;


        find_second_occ(x1,x2,y1,y2,len,occ);

        //        long len = itera - pattern.begin() + 1;

//        grid.range2(x1, x2, y1, y2, pairs);
//
//
//        for (auto &pair : pairs) {
//
//            size_t p = grid.first_label_col(pair.second);
//            size_t pos_p = _g.offsetText(g_tree[p]);
//            unsigned int parent = g_tree.parent(g_tree[p]);
//            long int l = (-len + pos_p) - _g.offsetText(parent);
//            unsigned int preP = g_tree.pre_order(parent);
//            //find_second_occ_rec(l, preP, occ);
//            find_second_occ(l, parent, occ);
//        }


    }
}


void SelfGrammarIndexPTS::locate2( std::string & pattern, std::vector<uint> &occ)
{

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }

    size_t p_n = pattern.size();
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

        size_t p_r1 = rules_t.leafrank(node_match_rules);

        auto begin_r_string = pattern.begin();
        auto end_r_string = itera;

        bp_cmp_suffix(_st(p_r1),end_r_string,begin_r_string);

        auto match_rules = itera-end_r_string;
        unsigned int pos_locus = 0;
        auto locus_node_rules = rules_p_tree.node_locus(sp1,match_rules,pos_locus);

        p_r1 = rules_t.leafrank(locus_node_rules);
        size_t p_r2 = p_r1 + rules_t.leafnum(locus_node_rules) - 1;

        begin_r_string = pattern.begin();
        end_r_string = itera;

        sampling_range_rules(p_r1,p_r2,begin_r_string,end_r_string);


        begin_r_string = pattern.begin();
        end_r_string = itera;
        auto r = bp_cmp_suffix(p_r1,end_r_string,begin_r_string);

        if(r != 0 || end_r_string != begin_r_string-1)
            continue;

        begin_r_string = pattern.begin();
        end_r_string = itera;
        r = bp_cmp_suffix(p_r2,end_r_string,begin_r_string);

        if(r != 0 || end_r_string != begin_r_string-1 )
            continue;

        /*
        *
        * Extracting range for (p[k...m]) in the suffix patricia tree
        *
        * */
        //start = timer::now();
        m_patricia::string_pairs sp2(pattern,2);
        sp2.set_left(i);
        sp2.set_right(pattern.size()-1);

        const auto& suff_t = sfx_p_tree.get_tree();
        auto node_match_suff = sfx_p_tree.node_match(sp2);
        const auto& suff_leaf = suff_t.leafrank(node_match_suff);

        size_t p_c1 = suff_t.leafrank(node_match_suff);

        auto begin_sfx_string = itera+1;
        auto end_sfx_string = pattern.end() ;

        bp_cmp_suffix_grammar(_st(p_c1),begin_sfx_string,end_sfx_string);

        auto match = begin_sfx_string -itera-1;
        pos_locus = 0;
        auto locus_node_suff = sfx_p_tree.node_locus(sp2,match, pos_locus);

        p_c1 = suff_t.leafrank(locus_node_suff);
        size_t p_c2 = p_c1 + suff_t.leafnum(locus_node_suff) - 1;

        begin_sfx_string = itera+1;
        end_sfx_string = pattern.end() ;

        sampling_range_suff(p_c1,p_c2,begin_sfx_string,end_sfx_string);


        begin_sfx_string = itera+1;
        end_sfx_string = pattern.end() ;
        r = bp_cmp_suffix_grammar(p_c1,begin_sfx_string,end_sfx_string);
        if(r != 0 )
            continue;

        begin_sfx_string = itera+1;
        end_sfx_string = pattern.end() ;
        r = bp_cmp_suffix_grammar(p_c2,begin_sfx_string,end_sfx_string);
        if(r != 0 )
            continue;

        std::vector< std::pair<size_t,size_t> > pairs;
        auto  x1 = (uint)p_r1,x2 = (uint)p_r2,y1 = (uint)p_c1,y2 = (uint)p_c2;
        long len = itera-pattern.begin() +1;
        find_second_occ(x1,x2,y1,y2,len,occ);
    }

}
void SelfGrammarIndexPTS::save(std::fstream & f_out)
{
    SelfGrammarIndexPT::save(f_out);
    f_out << sampling;
}

void SelfGrammarIndexPTS::load(std::fstream & f_in)
{
    SelfGrammarIndexPT::load(f_in);
    f_in >> sampling;
}

void SelfGrammarIndexPTS::sampling_range_rules(size_t &i, size_t &j,std::string::iterator& iterator1, std::string::iterator& iterator2) const
{

    auto nrules = _g.n_rules()-1;

    size_t ii  = _st(i),jj = _st(j);

    ii  = (ii == 1) ? 1 : ii - sampling;
    jj =  (jj+sampling <= nrules) ? jj + sampling: nrules;



    //BINARY SEARCH ON THE INTERVAL FOR LOWER BOUND

    grammar_representation::g_long lb = ii, ub = jj;

    lower_bound(lb, ub, [&iterator1,&iterator2, &i, this](const grammar_representation::g_long &a) -> int {
        auto begin = iterator1;
        auto end = iterator2;
        auto r = bp_cmp_suffix(a,end,begin);
        if (r == 0 && end != begin - 1) return 1;
        return r;
    });

    i = lb;

    //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND
    ub = jj;

    upper_bound(lb,ub,[&iterator1,&iterator2,&j,this](const grammar_representation::g_long & a)->int
    {
        auto begin = iterator1;
        auto end = iterator2;
        auto r =  bp_cmp_suffix(a,end,begin);
        if (r == 0 && end != begin - 1) return 1;
        return r;
    });

    j = ub;
}

void SelfGrammarIndexPTS::sampling_range_suff(size_t &i, size_t &j,std::string::iterator& iterator1, std::string::iterator& iterator2) const
{

    //definig ranges for binary search;

    auto nsfx = grid.n_columns();

    size_t ii  = _st(i),jj = _st(j);

    ii  = (ii == 1) ? 1 : ii - sampling;
    jj =  (jj+sampling <= nsfx) ? jj + sampling: nsfx;


    //BINARY SEARCH ON THE INTERVAL FOR LOWER BOUND

    grammar_representation::g_long lb = ii, ub = jj;

    lower_bound(lb, ub, [&iterator1,&iterator2, &i, this](const grammar_representation::g_long &a) -> int {
        auto begin = iterator1;
        auto end = iterator2;
        auto r = bp_cmp_suffix_grammar(a, begin, end);
        //if (r == 0 && end != begin - 1) return 1;
        return r;
    });

    i = lb;

    //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND
    ub = jj;

    upper_bound(lb,ub,[&iterator1,&iterator2,&j,this](const grammar_representation::g_long & a)->int
    {
        auto begin = iterator1;
        auto end = iterator2;
        auto r =  bp_cmp_suffix_grammar(a,begin,end);
        return r;
    });

    j = ub;


}

size_t SelfGrammarIndexPTS::size_in_bytes() const
{
//    std::cout<<"SelfGrammarIndexPTS::size_in_bytes()\n";

    return SelfGrammarIndexPT::size_in_bytes() + sizeof(sampling);
}

void SelfGrammarIndexPTS::display(const std::size_t &i, const std::size_t &j, std::string &s)
{
    SelfGrammarIndexPT::display( i,j,s);
}

size_t SelfGrammarIndexPTS::_st(const size_t & i)const
{
    return  (i-1)*sampling + 1;
}

SelfGrammarIndexPTS::SelfGrammarIndexPTS(const int & s):SelfGrammarIndexPT()
{
    sampling = s;
}

SelfGrammarIndexPTS::~SelfGrammarIndexPTS()
= default;

void
SelfGrammarIndexPTS::build(const SelfGrammarIndex::grammar_representation &G, const SelfGrammarIndex::range_search2d &R,
                           const m_patricia::compact_patricia_tree &sfx, const m_patricia::compact_patricia_tree &rules,
                           const unsigned int & s) {
    SelfGrammarIndexPT::build(G, R, sfx, rules);
    sampling = (int)s;
}

void SelfGrammarIndexPTS::build_suffix(const string & text, fstream &suffixes, fstream &repair_g

#ifdef MEM_MONITOR
        ,mem_monitor &mm
#endif
) {


    grammar not_compressed_grammar;
    not_compressed_grammar.load(repair_g);


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


#ifdef PRINT_LOGS
    std::cout<<BUILD_PATRICIA_TREES_RULE<<std::endl;
#endif
#ifdef MEM_MONITOR
    mm.event(BUILD_PATRICIA_TREES_RULE);
    auto start = timer::now();
#endif
    {
        m_patricia::patricia_tree<m_patricia::string_pairs> T;
        unsigned long id = 0;
        for (int i = 0; i < grammar_sfx.size(); i += sampling)
        {
            m_patricia::string_pairs s(text,++id);
            s.set_left(grammar_sfx[i].first.first);
            s.set_right(grammar_sfx[i].first.second);
            T.insert(s);
        }
        sfx_p_tree.build(T);
    }


#ifdef PRINT_LOGS
    std::cout<<BUILD_PATRICIA_TREES_SFX<<std::endl;
#endif
#ifdef MEM_MONITOR
    mm.event(BUILD_PATRICIA_TREES_SFX);
#endif
    /*
     * Building Patricia Trees for rules.
     *
     * */
    {
        m_patricia::patricia_tree<m_patricia::rev_string_pairs> T;
        unsigned long id = 0;
        for (auto &&  r: not_compressed_grammar) {
            if( id % sampling == 0)
            {
                m_patricia::rev_string_pairs s(text,++id);
                s.set_left(r.second.l);
                s.set_right(r.second.r);
                T.insert(s);
            }else{
                ++id;
            }
        }

        rules_p_tree.build(T);
    }

#ifdef MEM_MONITOR
    auto stop = timer::now();
    CLogger::GetLogger()->model[BUILD_PATRICIA_TREES_SFX] = duration_cast<microseconds>(stop-start).count();;
#endif


}
//sampling transform;



void SelfGrammarIndexPTS::find_ranges(std::string & pattern, std::vector<uint> & X) {
    size_t p_n = pattern.size();
    const auto &g_tree = _g.get_parser_tree();
    auto nrules = _g.n_rules() - 1;
    auto nsfx = grid.n_columns();


    for (size_t i = 1; i <= p_n; ++i) {

        /* std::string p1,p2;
         p1.resize(i);
         p2.resize(p_n-i);

         std::copy(pattern.begin(),pattern.begin()+i,p1.begin());
         std::copy(pattern.begin()+i,pattern.end(),p2.begin());
         std::reverse(p1.begin(),p1.end());
        */

        auto itera = pattern.begin() + i - 1;

        /*
         *
         * Extracting range for rev(p[1...k]) in the rule patricia tree
         *
         * */

        m_patricia::rev_string_pairs sp1(pattern, 1);
        sp1.set_left(0);
        sp1.set_right(i - 1);

        const auto &rules_t = rules_p_tree.get_tree();
        auto node_match_rules = rules_p_tree.node_match(sp1);
        //const auto& rules_leaf = rules_t.leafrank(node_match_rules);

        size_t p_r2 = nrules, p_r1 = rules_t.leafrank(node_match_rules);

        auto begin_r_string = pattern.begin();
        auto end_r_string = itera;

        auto rcmp_rules = bp_cmp_suffix(_st(p_r1), end_r_string, begin_r_string);
        auto match_rules = itera - end_r_string;


        begin_r_string = pattern.begin();
        end_r_string = itera;

        if (match_rules == sp1.size()) // match_node  == locus_node && all the symbols of pattern are consumed
        {

            p_r2 = p_r1 + rules_t.leafnum(node_match_rules) - 1;
            // sampled rules corresponding to leaves.
            size_t ii = _st(p_r1), jj = _st(p_r2);
            // new range of search
            size_t ii_low = (ii == 1) ? 1 : ii - sampling;
            size_t jj_hight = (jj + sampling <= nrules) ? jj + sampling : nrules;

            // find real range of search

            /*size_t lb = ii_low, ub = ii;
            int r = -1;
            auto begin = begin_r_string;
            auto end = end_r_string;
            while (lb <= ub) {
                begin = begin_r_string;
                end = end_r_string;
                r = bp_cmp_suffix(lb, end, begin);

                if (r == 0 && end == begin - 1)
                    break;

                if (r < 0 || lb == ub)
                    break;
                ++lb;
            }
            if (r != 0 || end != begin - 1) continue;

            p_r1 = lb;


            lb = jj, ub = jj_hight;

            while (ub >= lb) {
                begin = begin_r_string;
                end = end_r_string;
                r = bp_cmp_suffix(ub, end, begin);

                if (r == 0 && end == begin - 1)
                    break;

                if (r > 0 || lb == ub)
                    break;
                --ub;
            }
            if (r != 0 || end != begin - 1) continue;
            p_r2 = ub;*/

            grammar_representation::g_long lb = ii_low, ub = ii;
            if (!lower_bound(lb, ub,
                             [&begin_r_string, &end_r_string, this](const grammar_representation::g_long &a) -> int {
                                 auto begin = begin_r_string;
                                 auto end = end_r_string;
                                 auto r = bp_cmp_suffix(a, end, begin);
                                 if (r == 0 && end != begin - 1) return 1;
                                 return r;
                             }))
                continue;

            p_r1 = lb;

            lb = jj, ub = jj_hight;
            //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND


            if (!upper_bound(lb, ub,
                             [&begin_r_string, &end_r_string, this](const grammar_representation::g_long &a) -> int {
                                 auto begin = begin_r_string;
                                 auto end = end_r_string;
                                 auto r = bp_cmp_suffix(a, end, begin);
                                 if (r == 0 && end != begin - 1) return 1;
                                 return r;
                             }))
                continue;

            p_r2 = ub;

            // sampling_range_rules(p_r1,p_r2,begin_r_string,end_r_string);

        } else  // if all the symbols of the patterns are not consumed
        {
            unsigned int pos_locus = 0;
            auto locus_node_rules = rules_p_tree.node_locus(sp1, match_rules, pos_locus);
            // find the left most leaf in the real range
            size_t ii, jj;
            if (rules_t.isleaf(locus_node_rules)) {
                p_r1 = rules_t.leafrank(node_match_rules);
                ii = (p_r1 == 1) ? 1 : p_r1 - 1;
                ii = _st(ii), jj = ii + ((p_r1 == 1) ? sampling : 2 * sampling);
            } else {
                p_r1 = rules_p_tree.find_child_range(locus_node_rules, sp1, pos_locus, rcmp_rules);
                // sampled rules corresponding to leaves.
                ii = _st(p_r1), jj = ii + sampling;

            }
            jj = (jj < nrules) ? jj : nrules;




            ///////////////////////////////////////////////////
            grammar_representation::g_long lb = ii, ub = jj;

            if (!lower_bound(lb, ub,
                             [&begin_r_string, &end_r_string, this](const grammar_representation::g_long &a) -> int {
                                 auto begin = begin_r_string;
                                 auto end = end_r_string;
                                 auto r = bp_cmp_suffix(a, end, begin);
                                 if (r == 0 && end != begin - 1) return 1;
                                 return r;
                             }))
                continue;

            p_r1 = lb;
            ub = jj;

            if (!upper_bound(lb, ub,
                             [&begin_r_string, &end_r_string, this](const grammar_representation::g_long &a) -> int {
                                 auto begin = begin_r_string;
                                 auto end = end_r_string;
                                 auto r = bp_cmp_suffix(a, end, begin);
                                 if (r == 0 && end != begin - 1) return 1;
                                 return r;
                             }))
                continue;

            p_r2 = ub;

         }

        /*
         *
         * Extracting range for (p[k...m]) in the suffix patricia tree
         *
         * */
        //start = timer::now();
        m_patricia::string_pairs sp2(pattern, 2);
        sp2.set_left(i);
        sp2.set_right(pattern.size() - 1);

        const auto &suff_t = sfx_p_tree.get_tree();
        auto node_match_suff = sfx_p_tree.node_match(sp2);
        //const auto& suff_leaf = suff_t.leafrank(node_match_suff);

        size_t p_c2 = nsfx, p_c1 = suff_t.leafrank(node_match_suff);

        auto begin_sfx_string = itera + 1;
        auto end_sfx_string = pattern.end();

        auto rcmp_sfx = bp_cmp_suffix_grammar(_st(p_c1), begin_sfx_string, end_sfx_string);
        auto match = begin_sfx_string - itera - 1;


        begin_sfx_string = itera + 1;
        end_sfx_string = pattern.end();

        if (match == sp2.size())// match_node  == locus_node && all the symbols of pattern are consumed
        {
            p_c2 = p_c1 + suff_t.leafnum(node_match_suff) - 1;
            // sampled suffix corresponding to leaves.
            size_t ii = _st(p_c1), jj = _st(p_c2);
            // new range of search
            size_t ii_low = (ii == 1) ? 1 : ii - sampling;
            size_t jj_hight = (jj + sampling <= nsfx) ? jj + sampling : nsfx;

            //////////////////////////////////////////////


            grammar_representation::g_long lb = ii_low, ub = ii;

            if (!lower_bound(lb, ub, [&begin_sfx_string, &end_sfx_string, this](
                    const grammar_representation::g_long &a) -> int {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;
                auto r = bp_cmp_suffix_grammar(a, begin, end);
                return r;
            }))
                continue;

            p_c1 = lb;

            lb = jj, ub = jj_hight;
            //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND


            if (!upper_bound(lb, ub, [&begin_sfx_string, &end_sfx_string, this](
                    const grammar_representation::g_long &a) -> int {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;

                auto r = bp_cmp_suffix_grammar(a, begin, end);
                return r;
            }))
                continue;

            p_c2 = ub;

        } else {// if all the symbols of the patterns are not consumed


            unsigned int pos_locus = 0;
            auto locus_node_suff = sfx_p_tree.node_locus(sp2, match, pos_locus);

            size_t ii, jj;

            if (suff_t.isleaf(locus_node_suff)) {
                p_c1 = suff_t.leafrank(locus_node_suff);
                ii = (p_c1 == 1) ? 1 : p_c1 - 1;
                ii = _st(ii), jj = ii + ((p_c1 == 1) ? sampling : 2 * sampling);


            } else {
                // find the left most leaf in the real range
                p_c1 = sfx_p_tree.find_child_range(locus_node_suff, sp2, pos_locus, rcmp_sfx);
                ii = _st(p_c1), jj = ii + sampling;

            }
            jj = (jj < nsfx) ? jj : nsfx;




            grammar_representation::g_long lb = ii, ub = jj;

            if (!lower_bound(lb, ub, [&begin_sfx_string, &end_sfx_string, this](
                    const grammar_representation::g_long &a) -> int {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;

                auto r = bp_cmp_suffix_grammar(a, begin, end);
                return r;
            }))
                continue;

            p_c1 = lb;
            ub = jj;

            if (!upper_bound(lb, ub, [&begin_sfx_string, &end_sfx_string, this](
                    const grammar_representation::g_long &a) -> int {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;
                auto r = bp_cmp_suffix_grammar(a, begin, end);
                return r;
            }))
                continue;

            p_c2 = ub;

        }



    }
}

void SelfGrammarIndexPTS::find_ranges_trie(std::string & pattern, std::vector<uint> & X) {


    size_t p_n = pattern.size();
    const auto& g_tree = _g.get_parser_tree();
    auto nrules = _g.n_rules()-1;
    auto nsfx = grid.n_columns();


    for (size_t i = 1; i <= p_n ; ++i) {

        /* std::string p1,p2;
         p1.resize(i);
         p2.resize(p_n-i);

         std::copy(pattern.begin(),pattern.begin()+i,p1.begin());
         std::copy(pattern.begin()+i,pattern.end(),p2.begin());
         std::reverse(p1.begin(),p1.end());
        */

        auto itera = pattern.begin() + i - 1;

        /*
         *
         * Extracting range for rev(p[1...k]) in the rule patricia tree
         *
         * */

        m_patricia::rev_string_pairs sp1(pattern, 1);
        sp1.set_left(0);
        sp1.set_right(i - 1);

        const auto &rules_t = rules_p_tree.get_tree();
        auto node_match_rules = rules_p_tree.node_match(sp1);
        //const auto& rules_leaf = rules_t.leafrank(node_match_rules);

        size_t p_r2 = nrules, p_r1 = rules_t.leafrank(node_match_rules);

        auto begin_r_string = pattern.begin();
        auto end_r_string = itera;

        auto rcmp_rules = bp_cmp_suffix(_st(p_r1), end_r_string, begin_r_string);
        auto match_rules = itera - end_r_string;


        begin_r_string = pattern.begin();
        end_r_string = itera;

        if (match_rules == sp1.size()) // match_node  == locus_node && all the symbols of pattern are consumed
        {

            p_r2 = p_r1 + rules_t.leafnum(node_match_rules) - 1;
            // sampled rules corresponding to leaves.
            size_t ii = _st(p_r1), jj = _st(p_r2);
            // new range of search
            size_t ii_low = (ii == 1) ? 1 : ii - sampling;
            size_t jj_hight = (jj + sampling <= nrules) ? jj + sampling : nrules;


            grammar_representation::g_long lb = ii_low, ub = ii;
            bool found = false;

            lower_bound(found,lb, ub, [&begin_r_string,&end_r_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r = bp_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });

            if(!found)
                continue;

            p_r1 = lb;

            lb = jj, ub = jj_hight;
            //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND

            found = false;

            upper_bound(found,lb,ub,[&begin_r_string,&end_r_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r =  bp_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });

            if(!found)
                continue;

            p_r2 = ub;

            // sampling_range_rules(p_r1,p_r2,begin_r_string,end_r_string);

        } else  // if all the symbols of the patterns are not consumed
        {
            unsigned int pos_locus = 0;
            auto locus_node_rules = rules_p_tree.node_locus(sp1, match_rules, pos_locus);
            // find the left most leaf in the real range
            size_t ii, jj;
            if (rules_t.isleaf(locus_node_rules)) {
                p_r1 = rules_t.leafrank(node_match_rules);
                ii = (p_r1 == 1) ? 1 : p_r1 - 1;
                ii = _st(ii), jj = ii + ((p_r1 == 1) ? sampling : 2 * sampling);
            } else {
                p_r1 = rules_p_tree.find_child_range(locus_node_rules, sp1, pos_locus, rcmp_rules);
                // sampled rules corresponding to leaves.
                ii = _st(p_r1), jj = ii + sampling;

            }
            jj = (jj < nrules) ? jj : nrules;




            ///////////////////////////////////////////////////
            grammar_representation::g_long lb = ii, ub = jj;
            bool found = false;

            lower_bound(found,lb, ub, [&begin_r_string,&end_r_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r = bp_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });

            if(!found)
                continue;

            p_r1 = lb;
            ub = jj;

            found = false;

            upper_bound(found, lb,ub,[&begin_r_string,&end_r_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r =  bp_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });

            if(!found)
                continue;

            p_r2 = ub;

            ///////////////////////////////////////////////////


        }

        /*
         *
         * Extracting range for (p[k...m]) in the suffix patricia tree
         *
         * */
        //start = timer::now();
        m_patricia::string_pairs sp2(pattern, 2);
        sp2.set_left(i);
        sp2.set_right(pattern.size() - 1);

        const auto &suff_t = sfx_p_tree.get_tree();
        auto node_match_suff = sfx_p_tree.node_match(sp2);
        //const auto& suff_leaf = suff_t.leafrank(node_match_suff);

        size_t p_c2 = nsfx, p_c1 = suff_t.leafrank(node_match_suff);

        auto begin_sfx_string = itera + 1;
        auto end_sfx_string = pattern.end();

        auto rcmp_sfx = bp_cmp_suffix_grammar(_st(p_c1), begin_sfx_string, end_sfx_string);
        auto match = begin_sfx_string - itera - 1;


        begin_sfx_string = itera + 1;
        end_sfx_string = pattern.end();

        if (match == sp2.size())// match_node  == locus_node && all the symbols of pattern are consumed
        {
            p_c2 = p_c1 + suff_t.leafnum(node_match_suff) - 1;
            // sampled suffix corresponding to leaves.
            size_t ii = _st(p_c1), jj = _st(p_c2);
            // new range of search
            size_t ii_low = (ii == 1) ? 1 : ii - sampling;
            size_t jj_hight = (jj + sampling <= nsfx) ? jj + sampling : nsfx;

            //////////////////////////////////////////////

            grammar_representation::g_long lb = ii_low, ub = ii;

            bool found = false;

            lower_bound(found,lb, ub, [&begin_sfx_string,&end_sfx_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;
                auto r = bp_cmp_suffix_grammar(a, begin, end);
                return r;
            });

            if(!found )
                continue;

            p_c1 = lb;

            lb = jj, ub = jj_hight;
            //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND

            found = false;

            upper_bound(found,lb,ub,[&begin_sfx_string,&end_sfx_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;

                auto r =  bp_cmp_suffix_grammar(a,begin,end);
                return r;
            });

            if(!found )
                continue;

            p_c2 = ub;

        } else {// if all the symbols of the patterns are not consumed


            unsigned int pos_locus = 0;
            auto locus_node_suff = sfx_p_tree.node_locus(sp2, match, pos_locus);

            size_t ii, jj;

            if (suff_t.isleaf(locus_node_suff)) {
                p_c1 = suff_t.leafrank(locus_node_suff);
                ii = (p_c1 == 1) ? 1 : p_c1 - 1;
                ii = _st(ii), jj = ii + ((p_c1 == 1) ? sampling : 2 * sampling);


            } else {
                // find the left most leaf in the real range
                p_c1 = sfx_p_tree.find_child_range(locus_node_suff, sp2, pos_locus, rcmp_sfx);
                ii = _st(p_c1), jj = ii + sampling;

            }
            jj = (jj < nsfx) ? jj : nsfx;





            grammar_representation::g_long lb = ii, ub = jj;

            bool found = false;

            lower_bound(found,lb, ub, [&begin_sfx_string,&end_sfx_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;

                auto r =  bp_cmp_suffix_grammar(a,begin,end);
                return r;
            });

            if(!found)
                continue;

            p_c1 = lb;
            ub = jj;

            found = false;

            upper_bound(found,lb,ub,[&begin_sfx_string,&end_sfx_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;
                auto r = bp_cmp_suffix_grammar(a, begin, end);
                return r;
            });

            if(!found ) continue;

            p_c2 = ub;

        }


    }
}



void SelfGrammarIndexPTS::find_ranges_trie(std::string & pattern, std::vector<uint> & X, std::vector<range> & fr) {


    size_t p_n = pattern.size();
    const auto& g_tree = _g.get_parser_tree();
    auto nrules = _g.n_rules()-1;
    auto nsfx = grid.n_columns();


    for (size_t i = 1; i <= p_n ; ++i) {

        /* std::string p1,p2;
         p1.resize(i);
         p2.resize(p_n-i);

         std::copy(pattern.begin(),pattern.begin()+i,p1.begin());
         std::copy(pattern.begin()+i,pattern.end(),p2.begin());
         std::reverse(p1.begin(),p1.end());
        */

        auto itera = pattern.begin() + i - 1;

        /*
         *
         * Extracting range for rev(p[1...k]) in the rule patricia tree
         *
         * */

        m_patricia::rev_string_pairs sp1(pattern, 1);
        sp1.set_left(0);
        sp1.set_right(i - 1);

        const auto &rules_t = rules_p_tree.get_tree();
        auto node_match_rules = rules_p_tree.node_match(sp1);
        //const auto& rules_leaf = rules_t.leafrank(node_match_rules);

        size_t p_r2 = nrules, p_r1 = rules_t.leafrank(node_match_rules);

        auto begin_r_string = pattern.begin();
        auto end_r_string = itera;

        auto rcmp_rules = bp_cmp_suffix(_st(p_r1), end_r_string, begin_r_string);
        auto match_rules = itera - end_r_string;


        begin_r_string = pattern.begin();
        end_r_string = itera;

        if (match_rules == sp1.size()) // match_node  == locus_node && all the symbols of pattern are consumed
        {

            p_r2 = p_r1 + rules_t.leafnum(node_match_rules) - 1;
            // sampled rules corresponding to leaves.
            size_t ii = _st(p_r1), jj = _st(p_r2);
            // new range of search
            size_t ii_low = (ii == 1) ? 1 : ii - sampling;
            size_t jj_hight = (jj + sampling <= nrules) ? jj + sampling : nrules;


            grammar_representation::g_long lb = ii_low, ub = ii;
            bool found = false;

            lower_bound(found,lb, ub, [&begin_r_string,&end_r_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r = bp_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });

            if(!found)
                continue;

            p_r1 = lb;

            lb = jj, ub = jj_hight;
            //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND

            found = false;

            upper_bound(found,lb,ub,[&begin_r_string,&end_r_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r =  bp_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });

            if(!found)
                continue;

            p_r2 = ub;

            // sampling_range_rules(p_r1,p_r2,begin_r_string,end_r_string);

        } else  // if all the symbols of the patterns are not consumed
        {
            unsigned int pos_locus = 0;
            auto locus_node_rules = rules_p_tree.node_locus(sp1, match_rules, pos_locus);
            // find the left most leaf in the real range
            size_t ii, jj;
            if (rules_t.isleaf(locus_node_rules)) {
                p_r1 = rules_t.leafrank(node_match_rules);
                ii = (p_r1 == 1) ? 1 : p_r1 - 1;
                ii = _st(ii), jj = ii + ((p_r1 == 1) ? sampling : 2 * sampling);
            } else {
                p_r1 = rules_p_tree.find_child_range(locus_node_rules, sp1, pos_locus, rcmp_rules);
                // sampled rules corresponding to leaves.
                ii = _st(p_r1), jj = ii + sampling;

            }
            jj = (jj < nrules) ? jj : nrules;




            ///////////////////////////////////////////////////
            grammar_representation::g_long lb = ii, ub = jj;
            bool found = false;

            lower_bound(found,lb, ub, [&begin_r_string,&end_r_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r = bp_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });

            if(!found)
                continue;

            p_r1 = lb;
            ub = jj;

            found = false;

            upper_bound(found, lb,ub,[&begin_r_string,&end_r_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_r_string;
                auto end = end_r_string;
                auto r =  bp_cmp_suffix(a,end,begin);
                if (r == 0 && end != begin - 1) return 1;
                return r;
            });

            if(!found)
                continue;

            p_r2 = ub;

            ///////////////////////////////////////////////////


        }

        /*
         *
         * Extracting range for (p[k...m]) in the suffix patricia tree
         *
         * */
        //start = timer::now();
        m_patricia::string_pairs sp2(pattern, 2);
        sp2.set_left(i);
        sp2.set_right(pattern.size() - 1);

        const auto &suff_t = sfx_p_tree.get_tree();
        auto node_match_suff = sfx_p_tree.node_match(sp2);
        //const auto& suff_leaf = suff_t.leafrank(node_match_suff);

        size_t p_c2 = nsfx, p_c1 = suff_t.leafrank(node_match_suff);

        auto begin_sfx_string = itera + 1;
        auto end_sfx_string = pattern.end();

        auto rcmp_sfx = bp_cmp_suffix_grammar(_st(p_c1), begin_sfx_string, end_sfx_string);
        auto match = begin_sfx_string - itera - 1;


        begin_sfx_string = itera + 1;
        end_sfx_string = pattern.end();

        if (match == sp2.size())// match_node  == locus_node && all the symbols of pattern are consumed
        {
            p_c2 = p_c1 + suff_t.leafnum(node_match_suff) - 1;
            // sampled suffix corresponding to leaves.
            size_t ii = _st(p_c1), jj = _st(p_c2);
            // new range of search
            size_t ii_low = (ii == 1) ? 1 : ii - sampling;
            size_t jj_hight = (jj + sampling <= nsfx) ? jj + sampling : nsfx;

            //////////////////////////////////////////////

            grammar_representation::g_long lb = ii_low, ub = ii;

            bool found = false;

            lower_bound(found,lb, ub, [&begin_sfx_string,&end_sfx_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;
                auto r = bp_cmp_suffix_grammar(a, begin, end);
                return r;
            });

            if(!found )
                continue;

            p_c1 = lb;

            lb = jj, ub = jj_hight;
            //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND

            found = false;

            upper_bound(found,lb,ub,[&begin_sfx_string,&end_sfx_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;

                auto r =  bp_cmp_suffix_grammar(a,begin,end);
                return r;
            });

            if(!found )
                continue;

            p_c2 = ub;

        } else {// if all the symbols of the patterns are not consumed


            unsigned int pos_locus = 0;
            auto locus_node_suff = sfx_p_tree.node_locus(sp2, match, pos_locus);

            size_t ii, jj;

            if (suff_t.isleaf(locus_node_suff)) {
                p_c1 = suff_t.leafrank(locus_node_suff);
                ii = (p_c1 == 1) ? 1 : p_c1 - 1;
                ii = _st(ii), jj = ii + ((p_c1 == 1) ? sampling : 2 * sampling);


            } else {
                // find the left most leaf in the real range
                p_c1 = sfx_p_tree.find_child_range(locus_node_suff, sp2, pos_locus, rcmp_sfx);
                ii = _st(p_c1), jj = ii + sampling;

            }
            jj = (jj < nsfx) ? jj : nsfx;





            grammar_representation::g_long lb = ii, ub = jj;

            bool found = false;

            lower_bound(found,lb, ub, [&begin_sfx_string,&end_sfx_string, this](const grammar_representation::g_long &a) -> int {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;

                auto r =  bp_cmp_suffix_grammar(a,begin,end);
                return r;
            });

            if(!found)
                continue;

            p_c1 = lb;
            ub = jj;

            found = false;

            upper_bound(found,lb,ub,[&begin_sfx_string,&end_sfx_string,this](const grammar_representation::g_long & a)->int
            {
                auto begin = begin_sfx_string;
                auto end = end_sfx_string;
                auto r = bp_cmp_suffix_grammar(a, begin, end);
                return r;
            });

            if(!found ) continue;

            p_c2 = ub;

        }

        long len = itera-pattern.begin() +1;


//        range r;
//        r.x1 = p_r1;
//        r.x2 = p_r2;
//        r.y1 = p_c1;
//        r.y2 = p_c2;
//        r.len = len;
//        fr.emplace_back(r);


    }
}
