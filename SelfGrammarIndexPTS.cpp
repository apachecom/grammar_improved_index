//
// Created by alpachec on 20-09-18.
//


#include <sdsl/lcp_bitcompressed.hpp>
#include <sdsl/rmq_succinct_sada.hpp>

#include "SelfGrammarIndexPTS.h"


void SelfGrammarIndexPTS::build(const std::string &text)
{

        /*
     * Building grammar by repair algorithm
     *
     * */
    std::cout<<"Building grammar by repair algorithm"<<std::endl;


    grammar not_compressed_grammar;
    not_compressed_grammar.buildRepair(text);
    //not_compressed_grammar.print(text);

    std::cout<<"\t number of rules "<<not_compressed_grammar.n_rules()<<std::endl;
    std::cout<<"\t total size of rules "<<not_compressed_grammar.get_size()<<std::endl;
    std::cout<<"\t size of the representation "<<not_compressed_grammar.size_in_bytes()*1/(1024*1024)<<"(mb)"<<std::endl;


    /*
     * Building compressed grammar
     *
    * */
    std::cout<<"Building compressed grammar"<<std::endl;
    grid.code = code;
    _g.code = code;
    _g.build(not_compressed_grammar);


    ////_g.print_size_in_bytes();
    ////std::cout<<"total size of compressed grammar*******************"<<_g.size_in_bytes()<<std::endl;
    /*
     * Build sufix of grammar
     *
     * */

    std::cout<<"Compute suffix grammar"<<std::endl;

    std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx;

    const auto& gtree = _g.get_parser_tree();

    unsigned long num_sfx = 0;

    for (auto r_begin = not_compressed_grammar.begin(); r_begin != not_compressed_grammar.end(); ++r_begin) {

        rule r = r_begin->second;
        rule::r_long r_id = r_begin->first;
        size_t node = gtree[ _g.select_occ(r_id,1)];

        rule::r_long off = 0;

        for (auto j = r._rule.size()-1; j >= 1 ; --j) {
            off += not_compressed_grammar[r._rule[j]].len();

            size_t  left =  r.r - off + 1;
            size_t  right = r.r;

            size_t tag = gtree.pre_order(gtree.child(node,j+1));
            grammar_sfx.emplace_back(std::make_pair(std::make_pair(left,right),std::make_pair(r._rule[j-1],tag)));
            num_sfx++;
        }
    }

    std::cout<<"\t number of suffixes "<<num_sfx<<std::endl;

    /*
     * Sorting suffixes
     *
     * */
    sdsl::int_vector<> SA(text.size(),0);
    sdsl::algorithm::calculate_sa( (unsigned char*)text.c_str(),text.size(),SA);
    sdsl::inv_perm_support<> SA_1(&SA);
    sdsl::lcp_bitcompressed<> LCP;
    sdsl::construct_im(LCP,text.c_str(),sizeof(unsigned char));
    sdsl::rmq_succinct_sada<> rmq(&LCP);


    std::sort(grammar_sfx.begin(),grammar_sfx.end(),
              [this,&text,&SA_1,&LCP,&rmq](const std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > & a,
                                           const std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > &b )->bool{
                  /*
                   * offset of suffix in the text
                   * */
                  ulong a_pos = a.first.first;
                  ulong b_pos = b.first.first;

                  ulong size_a = a.first.second - a.first.first +1;
                  ulong size_b = b.first.second - b.first.first +1;

                  /*
                   * is start at the same position return the shortest
                   * */
                  if(a_pos == b_pos)
                      return size_a < size_b;

                  auto sa_1_a = SA_1[a_pos];
                  auto sa_1_b = SA_1[b_pos];

                  int min= LCP[rmq(std::min(sa_1_a,sa_1_b)+2,std::max(sa_1_a,sa_1_b)+1)];

                  /*
                   * Check if one is prefix of the other
                   * return the shortest
                   * */

                  if(std::min(size_a,size_b) <= min){
                      return size_a < size_b;
                  }

                  /*
                   * then return the lowest lexicographical
                   * */

                  return sa_1_a < sa_1_b;

              });

 /*   uint ooo = 0;
    for (auto && sf  : grammar_sfx) {

        std::string sfs;

        auto n = sf.first.second - sf.first.first +1;
        *//*sfs.resize(n);
        std::copy(text.begin()+sf.first.first,text.begin()+sf.first.second+1,sfs.begin());
        std::cout<<++ooo<<"->  "<<sfs+"|";
*//*

        if(n < 5){
            sfs.resize(sf.first.second - sf.first.first + 1);
            std::copy(text.begin()+sf.first.first,text.begin()+sf.first.second+1,sfs.begin());
            std::cout<<++ooo<<"->  "<<sfs+"|";
        }else{
            sfs.resize(5);
            std::copy(text.begin()+sf.first.first,text.begin()+sf.first.first+5,sfs.begin());
            std::cout<<++ooo<<"->  "<<sfs+"|";
        }
        for (int i = 0; i < sfs.size() ; ++i) {
            std::cout<<(uint)sfs[i]<<" ";
        }
        std::cout<<std::endl;
    }*/


    /*
     *
     * Building grid for 2d range search first occ
     *
     * */

    std::cout<<"Building grid for 2d range search first occ"<<std::endl;

    std::vector<std::pair<std::pair<range_search2d::bin_long ,range_search2d::bin_long >,range_search2d::bin_long>> grid_points(num_sfx);
    size_t bpair = 0;
    for (auto && s  :grammar_sfx ) {
        grid_points[bpair] = make_pair(make_pair(s.second.first,bpair+1),s.second.second);
        ++bpair;
    }

    grid.build(grid_points.begin(),grid_points.end(),not_compressed_grammar.n_rules(),num_sfx);
    std::cout<<"grid size "<<grid.size_in_bytes()<<std::endl;
    ///grid.print_size();

    /*
     * Building Sampled Patricia Trees for suffixes.
     *
     * Sampling by log(u)log(log(n))/log(n)
     * where n is the number of symbols in the repair grammar and u is the length of the original text
     * */

    std::cout<<"Building Sampled Patricia Trees for suffixes."<<std::endl;



    //sampling = (unsigned long)(log2(text.size()) * log2(log2(not_compressed_grammar.n_rules()))/log2(not_compressed_grammar.n_rules()));

    std::cout<<"\t sampled: "<<sampling<<std::endl;
    std::cout<<"\t numero de sufijos count: "<<num_sfx<<std::endl;
    std::cout<<"\t grammar_sfx.size() "<<grammar_sfx.size()<<std::endl;
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
        //T.print();
        sfx_p_tree.build(T);
        /*for (auto &&  a: sfx_p_tree.get_seq()) {
            std::cout<<(char)a<<" ";
        }
        std::cout<<"\n";*/
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

       // T.print();
        rules_p_tree.build(T);
        /*for (auto &&  a: rules_p_tree.get_seq()) {
            std::cout<<(char)a<<" ";
        }
        std::cout<<"\n";*/
        ///std::cout<<"rules_p_tree size "<<rules_p_tree.size_in_bytes()<<std::endl;
        ////rules_p_tree.print_size_in_bytes();
    }




}

//void SelfGrammarIndexPTS::locateNoTrie( std::string & pattern, std::vector<uint> &occ)
//{
//
//    if(pattern.size() == 1)
//    {
//        locate_ch(pattern[0],occ);
//        return;
//    }
//
//    size_t p_n = pattern.size();
//    const auto& g_tree = _g.get_parser_tree();
//    auto nrules = _g.n_rules()-1;
//    auto nsfx = grid.n_columns();
//
//
//    for (size_t i = 1; i <= p_n ; ++i) {
//
//        /* std::string p1,p2;
//         p1.resize(i);
//         p2.resize(p_n-i);
//
//         std::copy(pattern.begin(),pattern.begin()+i,p1.begin());
//         std::copy(pattern.begin()+i,pattern.end(),p2.begin());
//         std::reverse(p1.begin(),p1.end());
//        */
//
//        auto itera = pattern.begin() + i - 1;
//
//        /*
//         *
//         * Extracting range for rev(p[1...k]) in the rule patricia tree
//         *
//         * */
//
//        m_patricia::rev_string_pairs sp1(pattern, 1);
//        sp1.set_left(0);
//        sp1.set_right(i - 1);
//
//        const auto &rules_t = rules_p_tree.get_tree();
//        auto node_match_rules = rules_p_tree.node_match(sp1);
//        //const auto& rules_leaf = rules_t.leafrank(node_match_rules);
//
//        size_t p_r2 = nrules, p_r1 = rules_t.leafrank(node_match_rules);
//
//        auto begin_r_string = pattern.begin();
//        auto end_r_string = itera;
//
//        auto rcmp_rules = cmp_suffix(_st(p_r1), end_r_string, begin_r_string);
//        auto match_rules = itera - end_r_string;
//
//
//        begin_r_string = pattern.begin();
//        end_r_string = itera;
//
//        if (match_rules == sp1.size()) // match_node  == locus_node && all the symbols of pattern are consumed
//        {
//
//            p_r2 = p_r1 + rules_t.leafnum(node_match_rules) - 1;
//            // sampled rules corresponding to leaves.
//            size_t ii = _st(p_r1), jj = _st(p_r2);
//            // new range of search
//            size_t ii_low = (ii == 1) ? 1 : ii - sampling;
//            size_t jj_hight = (jj + sampling <= nrules) ? jj + sampling : nrules;
//
//            grammar_representation::g_long lb = ii_low, ub = ii;
//            if(!lower_bound(lb, ub, [&begin_r_string,&end_r_string, this](const grammar_representation::g_long &a) -> int {
//                auto begin = begin_r_string;
//                auto end = end_r_string;
//                auto r = cmp_suffix(a,end,begin);
//                if (r == 0 && end != begin - 1) return 1;
//                return r;
//            }))
//                continue;
//
//            p_r1 = lb;
//
//            lb = jj, ub = jj_hight;
//            //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND
//
//
//            if(!upper_bound(lb,ub,[&begin_r_string,&end_r_string,this](const grammar_representation::g_long & a)->int
//            {
//                auto begin = begin_r_string;
//                auto end = end_r_string;
//                auto r =  cmp_suffix(a,end,begin);
//                if (r == 0 && end != begin - 1) return 1;
//                return r;
//            }))
//                continue;
//
//            p_r2 = ub;
//
//            // sampling_range_rules(p_r1,p_r2,begin_r_string,end_r_string);
//
//        } else  // if all the symbols of the patterns are not consumed
//        {
//            unsigned int pos_locus = 0;
//            auto locus_node_rules = rules_p_tree.node_locus(sp1, match_rules, pos_locus);
//            // find the left most leaf in the real range
//            size_t ii, jj;
//            if (rules_t.isleaf(locus_node_rules)) {
//                p_r1 = rules_t.leafrank(node_match_rules);
//                ii = (p_r1 == 1) ? 1 : p_r1 - 1;
//                ii = _st(ii), jj = ii + ((p_r1 == 1) ? sampling : 2 * sampling);
//            } else {
//                p_r1 = rules_p_tree.find_child_range(locus_node_rules, sp1, pos_locus, rcmp_rules);
//                // sampled rules corresponding to leaves.
//                ii = _st(p_r1), jj = ii + sampling;
//
//            }
//            jj = (jj < nrules) ? jj : nrules;
//
//
//
//
//            ///////////////////////////////////////////////////
//            grammar_representation::g_long lb = ii, ub = jj;
//
//            if(!lower_bound(lb, ub, [&begin_r_string,&end_r_string, this](const grammar_representation::g_long &a) -> int {
//                auto begin = begin_r_string;
//                auto end = end_r_string;
//                auto r = cmp_suffix(a,end,begin);
//                if (r == 0 && end != begin - 1) return 1;
//                return r;
//            }))
//                continue;
//
//            p_r1 = lb;
//            ub = jj;
//
//            if(!upper_bound(lb,ub,[&begin_r_string,&end_r_string,this](const grammar_representation::g_long & a)->int
//            {
//                auto begin = begin_r_string;
//                auto end = end_r_string;
//                auto r =  cmp_suffix(a,end,begin);
//                if (r == 0 && end != begin - 1) return 1;
//                return r;
//            }))
//                continue;
//
//            p_r2 = ub;
//
//            ///////////////////////////////////////////////////
//
//        }
//
//        /*
//         *
//         * Extracting range for (p[k...m]) in the suffix patricia tree
//         *
//         * */
//        //start = timer::now();
//        m_patricia::string_pairs sp2(pattern, 2);
//        sp2.set_left(i);
//        sp2.set_right(pattern.size() - 1);
//
//        const auto &suff_t = sfx_p_tree.get_tree();
//        auto node_match_suff = sfx_p_tree.node_match(sp2);
//        //const auto& suff_leaf = suff_t.leafrank(node_match_suff);
//
//        size_t p_c2 = nsfx, p_c1 = suff_t.leafrank(node_match_suff);
//
//        auto begin_sfx_string = itera + 1;
//        auto end_sfx_string = pattern.end();
//
//        auto rcmp_sfx = cmp_suffix_grammar(_st(p_c1), begin_sfx_string, end_sfx_string);
//        auto match = begin_sfx_string - itera - 1;
//
//
//        begin_sfx_string = itera + 1;
//        end_sfx_string = pattern.end();
//
//        if (match == sp2.size())// match_node  == locus_node && all the symbols of pattern are consumed
//        {
//            p_c2 = p_c1 + suff_t.leafnum(node_match_suff) - 1;
//            // sampled suffix corresponding to leaves.
//            size_t ii = _st(p_c1), jj = _st(p_c2);
//            // new range of search
//            size_t ii_low = (ii == 1) ? 1 : ii - sampling;
//            size_t jj_hight = (jj + sampling <= nsfx) ? jj + sampling : nsfx;
//
//            //////////////////////////////////////////////
//
//            /*size_t lb = ii_low, ub = ii;
//            int r = -1;
//            while (lb <= ub) {
//                auto begin = begin_sfx_string;
//                auto end = end_sfx_string;
//                r = bp_cmp_suffix_grammar(lb, begin, end);
//
//                if (r == 0)
//                    break;
//
//                if (r < 0 || lb == ub)
//                    break;
//                ++lb;
//            }
//            if (r != 0) continue;
//            p_c1 = lb;
//
//
//            lb = jj, ub = jj_hight;
//
//            while (ub >= lb) {
//                auto begin = begin_sfx_string;
//                auto end = end_sfx_string;
//                auto r = bp_cmp_suffix_grammar(ub, begin, end);
//
//                if (r == 0)
//                    break;
//
//                if (r > 0 || lb == ub)
//                    break;
//                --ub;
//            }
//            if (r != 0) continue;
//
//            p_c2 = ub;*/
//
//
//            /////////////////////////////////////////////////////////////
//
//            grammar_representation::g_long lb = ii_low, ub = ii;
//
//            if(!lower_bound(lb, ub, [&begin_sfx_string,&end_sfx_string, this](const grammar_representation::g_long &a) -> int {
//                auto begin = begin_sfx_string;
//                auto end = end_sfx_string;
//                auto r = cmp_suffix_grammar(a, begin, end);
//                return r;
//            }))
//                continue;
//
//            p_c1 = lb;
//
//            lb = jj, ub = jj_hight;
//            //BINARY SEARCH ON THE INTERVAL FOR UPPER BOUND
//
//
//            if(!upper_bound(lb,ub,[&begin_sfx_string,&end_sfx_string,this](const grammar_representation::g_long & a)->int
//            {
//                auto begin = begin_sfx_string;
//                auto end = end_sfx_string;
//
//                auto r =  cmp_suffix_grammar(a,begin,end);
//                return r;
//            }))
//                continue;
//
//            p_c2 = ub;
//
//        } else {// if all the symbols of the patterns are not consumed
//
//
//            unsigned int pos_locus = 0;
//            auto locus_node_suff = sfx_p_tree.node_locus(sp2, match, pos_locus);
//
//            size_t ii, jj;
//
//            if (suff_t.isleaf(locus_node_suff)) {
//                p_c1 = suff_t.leafrank(locus_node_suff);
//                ii = (p_c1 == 1) ? 1 : p_c1 - 1;
//                ii = _st(ii), jj = ii + ((p_c1 == 1) ? sampling : 2 * sampling);
//
//
//            } else {
//                // find the left most leaf in the real range
//                p_c1 = sfx_p_tree.find_child_range(locus_node_suff, sp2, pos_locus, rcmp_sfx);
//                ii = _st(p_c1), jj = ii + sampling;
//
//            }
//            jj = (jj < nsfx) ? jj : nsfx;
//
//
//
//
//            ///////////////////////////////////////////////////
//
//            /*size_t lb = ii, ub = jj;
//
//            int r = -1;
//            while (lb <= ub) {
//                auto begin = begin_sfx_string;
//                auto end = end_sfx_string;
//                r = bp_cmp_suffix_grammar(lb, begin, end);
//
//                if (r == 0)
//                    break;
//
//                if (r < 0 || lb == ub)
//                    break;
//                ++lb;
//            }
//            if (r != 0) continue;
//
//            p_c1 = lb;
//            ub = jj;
//
//
//            while (ub >= lb) {
//                auto begin = begin_sfx_string;
//                auto end = end_sfx_string;
//                r = bp_cmp_suffix_grammar(ub, begin, end);
//
//                if (r == 0)
//                    break;
//
//                if (r > 0 || lb == ub)
//                    break;
//                --ub;
//            }
//            if (r != 0) continue;
//            p_c2 = ub;
//             */
//
//            ///////////////////////////////////////////////////
//
//
//            grammar_representation::g_long lb = ii, ub = jj;
//
//            if(!lower_bound(lb, ub, [&begin_sfx_string,&end_sfx_string, this](const grammar_representation::g_long &a) -> int {
//                auto begin = begin_sfx_string;
//                auto end = end_sfx_string;
//
//                auto r =  cmp_suffix_grammar(a,begin,end);
//                return r;
//            }))
//                continue;
//
//            p_c1 = lb;
//            ub = jj;
//
//            if(!upper_bound(lb,ub,[&begin_sfx_string,&end_sfx_string,this](const grammar_representation::g_long & a)->int
//            {
//                auto begin = begin_sfx_string;
//                auto end = end_sfx_string;
//                auto r = cmp_suffix_grammar(a, begin, end);
//                return r;
//            })) continue;
//
//            p_c2 = ub;
//
//        }
//
//
//
//
//        /* binary_relation::bin_long x1 = (uint) p_r1, x2 = (uint) p_r2, y1 = (uint) p_c1, y2 = (uint) p_c2;
//         long len = itera - pattern.begin() + 1;
//         //std::vector<std::pair<size_t, size_t> > pairs;
//         //grid.range2(x1, x2, y1, y2, pairs);
//         fstream f(std::to_string(sampling)+"bf_ptos",std::ios::out|std::ios::binary|std::ios::app);
//
//         sdsl::serialize((uint)x1,f);
//         sdsl::serialize((uint)x2,f);
//         sdsl::serialize((uint)y1,f);
//         sdsl::serialize((uint)y2,f);
//         sdsl::serialize(len,f);*/
//
//
//        /*
//        size_t oo = pairs.size();
//        sdsl::serialize(oo,f);
//        for (auto &&  ll: pairs) {
//            sdsl::serialize(ll.second,f);
//        }
//        sdsl::serialize(len,f);*/
//
//
////        std::vector<std::pair<size_t, size_t> > pairs;
//        binary_relation::bin_long x1 = (uint) p_r1, x2 = (uint) p_r2, y1 = (uint) p_c1, y2 = (uint) p_c2;
//
//        long len = itera-pattern.begin() +1;
//
//
//        find_second_occ(x1,x2,x1,x2,len,occ);
//
////        long len = itera - pattern.begin() + 1;
////        grid.range2(x1, x2, y1, y2, pairs);
////
////
////        for (auto &pair : pairs) {
////
////            size_t p = grid.first_label_col(pair.second);
////            size_t pos_p = _g.offsetText(g_tree[p]);
////            unsigned int parent = g_tree.parent(g_tree[p]);
////            long int l = (-len + pos_p) - _g.offsetText(parent);
////            unsigned int preP = g_tree.pre_order(parent);
////            //find_second_occ_rec(l, preP, occ);
////            find_second_occ(l, parent, occ);
////        }
//
//
//    }
//}

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

//        std::vector<std::pair<size_t, size_t> > pairs;
        binary_relation::bin_long x1 = (uint) p_r1, x2 = (uint) p_r2, y1 = (uint) p_c1, y2 = (uint) p_c2;


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
        binary_relation::bin_long x1 = (uint) p_r1, x2 = (uint) p_r2, y1 = (uint) p_c1, y2 = (uint) p_c2;


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
    ///size_t n_sj = grid.n_columns();

    /*vector<size_t > S (n_sj,1);
    for (size_t j = 0; j < n_sj; ++j)
        S[j] = j+1;*/

    /////std::cout<<"////////////////////////cadena "<<pattern<<" /////////////////////////////////////////////////////////////////"<<std::endl;
    for (size_t i = 1; i <= p_n ; ++i)
    {
        // CREATE PARTITIONS
        /////std::cout<<"////////////////////////PARTITION "<<i<<" /////////////////////////////////////////////////////////////////"<<std::endl;
        /* std::string p1,p2;
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

        //auto start = timer::now();

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

        //auto stop = timer::now();
        /// std::cout<<"\t\trules PT search "<<duration_cast<nanoseconds>(stop - start).count()<<"(ns)"<<std::endl;

        /////////////////

        /*m_patricia::rev_string_pairs sp1(pattern,1);
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
        */
        ///////////////////


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

        //stop = timer::now();
        //std::cout<<"\t\tsufx PT search "<<duration_cast<nanoseconds>(stop - start).count()<<"(ns)"<<std::endl;




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

        //start = timer::now();
        binary_relation::bin_long  x1 = (uint)p_r1,x2 = (uint)p_r2,y1 = (uint)p_c1,y2 = (uint)p_c2;

        long len = itera-pattern.begin() +1;


        find_second_occ(x1,x2,y1,y2,len,occ);

//        grid.range2(x1,x2,y1,y2,pairs);
//
//
//        long len = itera-pattern.begin() +1;
//        const auto& g_tree = _g.get_parser_tree();
//
//        //start = timer::now();
//        for (auto &pair : pairs) {
//
//
//            //size_t p = grid.labels(pair.first, pair.second);
//            size_t p = grid.first_label_col(pair.second);
//            size_t pos_p = _g.offsetText(g_tree[p]);
//            unsigned int parent = g_tree.parent(g_tree[p]);
//            long int  l = (- len + pos_p) - _g.offsetText(parent);
//
//
//            find_second_occ(l,parent,occ);
//
//        }

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
{

}

void
SelfGrammarIndexPTS::build(const SelfGrammarIndex::grammar_representation &G, const SelfGrammarIndex::range_search2d &R,
                           const m_patricia::compact_patricia_tree &sfx, const m_patricia::compact_patricia_tree &rules,
                           const unsigned int & s) {
    SelfGrammarIndexPT::build(G, R, sfx, rules);
    sampling = s;
}

void SelfGrammarIndexPTS::build_suffix(const string & text, fstream &suffixes, fstream &repair_g) {

    grammar not_compressed_grammar;
    not_compressed_grammar.load(repair_g);

    std::cout<<"Size of grammar "<<not_compressed_grammar.get_size()<<"\n";
    std::cout<<"Print repair grammar "<<sampling<<"\n";

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

    std::cout<<"\t sampled: "<<sampling<<std::endl;
    std::cout<<"\t numero de sufijos count: "<<num_sfx<<std::endl;
    std::cout<<"\t grammar_sfx.size() "<<grammar_sfx.size()<<std::endl;

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
        //T.print();
        sfx_p_tree.build(T);
        /*for (auto &&  a: sfx_p_tree.get_seq()) {
            std::cout<<(char)a<<" ";
        }
        std::cout<<"\n";*/
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

        // T.print();
        rules_p_tree.build(T);
        /*for (auto &&  a: rules_p_tree.get_seq()) {
            std::cout<<(char)a<<" ";
        }
        std::cout<<"\n";*/
        ///std::cout<<"rules_p_tree size "<<rules_p_tree.size_in_bytes()<<std::endl;
        ////rules_p_tree.print_size_in_bytes();
    }

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
