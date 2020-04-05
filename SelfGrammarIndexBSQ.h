//
// Created by alejandro on 06-06-19.
//

#ifndef IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXBSQ_H
#define IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXBSQ_H


#include "SelfGrammarIndexBS.h"
#include "utils/Samplings/sampling.h"
#include <sdsl/lcp_bitcompressed.hpp>
#include <sdsl/rmq_succinct_sada.hpp>
//#include "/home/alejandro/libCSD/StringDictionaryHTFC.h"



class SelfGrammarIndexBSQ : public SelfGrammarIndexBS {


//    static const uint BLOCK_SAMPLING = 16;
    static const uint BLOCK_SAMPLING = 256;


public:


    sdsl::int_vector<> pi;
    sdsl::inv_perm_support<> inv_pi;

    sdsl::bit_vector C;
    sdsl::bit_vector::rank_1_type c_rank1;
    sdsl::bit_vector::select_1_type c_sel1;

    Sampling *grammarSamp;
    Sampling *reverseSamp;
    Sampling* sequenceSamp;


    SelfGrammarIndexBSQ() {}

    SelfGrammarIndexBSQ(SelfGrammarIndex *base_indx) {
        this->grid = base_indx->get_grid();
        this->_g = base_indx->get_grammar();
    }

    ~SelfGrammarIndexBSQ() {

        delete grammarSamp;
        delete reverseSamp;
        delete sequenceSamp;

    }

    size_t n_rules() { return _g.n_rules(); }

    void build_basics(const std::string &text) {
        std::cout<<"SelfGrammarIndexBSQ()"<<std::endl;
        /*
     * Building grammar by repair algorithm
     *
     * */
        std::cout << "Building grammar by repair algorithm" << std::endl;


        grammar not_compressed_grammar;
        not_compressed_grammar.buildRepair(text);


        /*std::cout<<"\t number of rules "<<not_compressed_grammar.n_rules()<<std::endl;
        std::cout<<"\t total size of rules "<<not_compressed_grammar.get_size()<<std::endl;
        std::cout<<"\t size of the representation "<<not_compressed_grammar.size_in_bytes()*1/(1024*1024)<<"(mb)"<<std::endl;
    */
        //not_compressed_grammar.print(text);


        /*
         * Building compressed grammar
         *
        * */
        //std::cout<<"Building compressed grammar"<<std::endl;

//    not_compressed_grammar.print(text);
        _g.code = code;
        _g.build(not_compressed_grammar);

        //  _g.print_size_in_bytes();
        //std::cout<<"total size of compressed grammar*******************"<<_g.size_in_bytes()*1.0/1024/1024<<std::endl;
        /*
         * Build sufix of grammar
         *
         * */


        //std::cout<<"Compute suffix grammar"<<std::endl;

        std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t> > > grammar_sfx;
        const auto &gtree = _g.get_parser_tree();
        unsigned long num_sfx = 0;

        for (auto r_begin = not_compressed_grammar.begin(); r_begin != not_compressed_grammar.end(); ++r_begin) {

            rule r = r_begin->second;
            rule::r_long r_id = r_begin->first;
            size_t node = gtree[_g.select_occ(r_id, 1)];

            rule::r_long off = 0;

            for (auto j = r._rule.size() - 1; j >= 1; --j) {
                off += not_compressed_grammar[r._rule[j]].len();

                size_t left = r.r - off + 1;
                size_t right = r.r;

                size_t tag = gtree.pre_order(gtree.child(node, j + 1));
                grammar_sfx.emplace_back(
                        std::make_pair(std::make_pair(left, right), std::make_pair(r._rule[j - 1], tag)));
                num_sfx++;
            }
        }
        size_t cc = 0;
//    sdsl::sd_vector<>::rank_1_type rrr(&(_g.L));

        //std::cout<<"L size: "<<_g.L.size()<<std::endl;
        //std::cout<<"Number of boundaries: "<<rrr.rank(_g.L.size())-1<<std::endl;


        //std::cout<<"\t number of suffixes "<<num_sfx<<std::endl;

        /*
         * Sorting suffixes
         *
         * */

        //std::cout<<"Building structures for sorting"<<std::endl;
        sdsl::int_vector<> SA(text.size(), 0);
        sdsl::algorithm::calculate_sa((unsigned char *) text.c_str(), text.size(), SA);
        sdsl::inv_perm_support<> SA_1(&SA);
        sdsl::lcp_wt<> LCP;
        sdsl::construct_im(LCP, text.c_str(), sizeof(char));
        sdsl::rmq_succinct_sada<true, sdsl::bp_support_sada<>> rmq(&LCP);

//    std::cout<<"sorting suffixes ........."<<std::endl;
        std::sort(grammar_sfx.begin(), grammar_sfx.end(),
                  [this, &text, &SA_1, &LCP, &rmq](
                          const std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t> > &a,
                          const std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t> > &b) -> bool {
                      /*
                       * offset of suffix in the text
                       * */
                      ulong a_pos = a.first.first;
                      ulong b_pos = b.first.first;

                      ulong size_a = a.first.second - a.first.first + 1;
                      ulong size_b = b.first.second - b.first.first + 1;

                      /*
                       * is start at the same position return the shortest
                       * */
                      if (a_pos == b_pos)
                          return size_a < size_b;

                      auto sa_1_a = SA_1[a_pos];
                      auto sa_1_b = SA_1[b_pos];

                      int min = LCP[rmq(std::min(sa_1_a, sa_1_b) + 2, std::max(sa_1_a, sa_1_b) + 1)];

                      /*
                       * Check if one is prefix of the other
                       * return the shortest
                       * */

                      if (std::min(size_a, size_b) <= min) {
                          return size_a < size_b;
                      }

                      /*
                       * then return the lowest lexicographical
                       * */

                      return sa_1_a < sa_1_b;

                  });


//    std::cout<<"end sorting suffixes "<<std::endl;

/*
    uint i = 0;
    for (auto && sf  : grammar_sfx) {

        std::string sfs;
        sfs.resize(sf.first.second - sf.first.first + 1);
        std::copy(text.begin()+sf.first.first,text.begin()+sf.first.second+1,sfs.begin());
        std::cout<<++i<<"-|"<<sfs<<std::endl;

    }
*/


        /*
         *
         * Building grid for 2d range search first occ
         *
         * */

//    std::cout<<"Building grid for 2d range search first occ"<<std::endl;

        std::vector<std::pair<std::pair<range_search2d::bin_long, range_search2d::bin_long>, range_search2d::bin_long>> grid_points(
                num_sfx);
        size_t bpair = 0;
        for (auto &&s  :grammar_sfx) {
            grid_points[bpair] = make_pair(make_pair(s.second.first, bpair + 1), s.second.second);
            ++bpair;
        }


        grid.code = code;
        grid.build(grid_points.begin(), grid_points.end(), not_compressed_grammar.n_rules(), num_sfx);
//    std::cout<<"Number of columns in the grid: "<<grid.n_columns()<<std::endl;
        //std::cout<<"***************************grid size "<<grid.size_in_bytes()*1.0/1024/1024<<std::endl;
        ///grid.print_size();
        ////grid.print();

        /*
         * Building Sampled Patricia Trees for suffixes.
         *
         * Sampling by log(u)log(log(n))/log(n)
         * where n is the number of symbols in the repair grammar and u is the length of the original text
         * */



        uint n_rules;
        not_compressed_grammar.permutationSortRules(text, pi, n_rules);
        inv_pi = sdsl::inv_perm_support<>(&pi);
        std::cout << "inv_pi: ";
//        for (int k = 0; k < pi.size(); ++k) {
//            std::cout << inv_pi[k] << " ";
//        }
//        std::cout << "\n";

        C = sdsl::bit_vector(256, 0);
        for (int i = 0; i < _g.alp.size(); ++i) {
            uint c = (uint) _g.alp[i];
            C[c] = 1;
        }

        c_rank1 = sdsl::bit_vector::rank_1_type(&C);
        c_sel1 = sdsl::bit_vector::select_1_type(&C);

    }

    void build(const std::string &text, const uint qsampling) {


        /*
     * Building grammar by repair algorithm
     *
     * */
        std::cout << "Building grammar by repair algorithm" << std::endl;


        grammar not_compressed_grammar;
        not_compressed_grammar.buildRepair(text);


        /*std::cout<<"\t number of rules "<<not_compressed_grammar.n_rules()<<std::endl;
        std::cout<<"\t total size of rules "<<not_compressed_grammar.get_size()<<std::endl;
        std::cout<<"\t size of the representation "<<not_compressed_grammar.size_in_bytes()*1/(1024*1024)<<"(mb)"<<std::endl;
    */
        //not_compressed_grammar.print(text);


        /*
         * Building compressed grammar
         *
        * */
        //std::cout<<"Building compressed grammar"<<std::endl;

//    not_compressed_grammar.print(text);
        _g.code = code;
        _g.build(not_compressed_grammar);

        //  _g.print_size_in_bytes();
        //std::cout<<"total size of compressed grammar*******************"<<_g.size_in_bytes()*1.0/1024/1024<<std::endl;
        /*
         * Build sufix of grammar
         *
         * */


        //std::cout<<"Compute suffix grammar"<<std::endl;

        std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t> > > grammar_sfx;
        const auto &gtree = _g.get_parser_tree();
        unsigned long num_sfx = 0;

        for (auto r_begin = not_compressed_grammar.begin(); r_begin != not_compressed_grammar.end(); ++r_begin) {

            rule r = r_begin->second;
            rule::r_long r_id = r_begin->first;
            size_t node = gtree[_g.select_occ(r_id, 1)];

            rule::r_long off = 0;

            for (auto j = r._rule.size() - 1; j >= 1; --j) {
                off += not_compressed_grammar[r._rule[j]].len();

                size_t left = r.r - off + 1;
                size_t right = r.r;

                size_t tag = gtree.pre_order(gtree.child(node, j + 1));
                grammar_sfx.emplace_back(
                        std::make_pair(std::make_pair(left, right), std::make_pair(r._rule[j - 1], tag)));
                num_sfx++;
            }
        }
        size_t cc = 0;
//    sdsl::sd_vector<>::rank_1_type rrr(&(_g.L));

        //std::cout<<"L size: "<<_g.L.size()<<std::endl;
        //std::cout<<"Number of boundaries: "<<rrr.rank(_g.L.size())-1<<std::endl;


        //std::cout<<"\t number of suffixes "<<num_sfx<<std::endl;

        /*
         * Sorting suffixes
         *
         * */

        //std::cout<<"Building structures for sorting"<<std::endl;
        sdsl::int_vector<> SA(text.size(), 0);
        sdsl::algorithm::calculate_sa((unsigned char *) text.c_str(), text.size(), SA);
        sdsl::inv_perm_support<> SA_1(&SA);
        sdsl::lcp_wt<> LCP;
        sdsl::construct_im(LCP, text.c_str(), sizeof(char));
        sdsl::rmq_succinct_sada<true, sdsl::bp_support_sada<>> rmq(&LCP);

//    std::cout<<"sorting suffixes ........."<<std::endl;
        std::sort(grammar_sfx.begin(), grammar_sfx.end(),
                  [this, &text, &SA_1, &LCP, &rmq](
                          const std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t> > &a,
                          const std::pair<std::pair<size_t, size_t>, std::pair<size_t, size_t> > &b) -> bool {
                      /*
                       * offset of suffix in the text
                       * */
                      ulong a_pos = a.first.first;
                      ulong b_pos = b.first.first;

                      ulong size_a = a.first.second - a.first.first + 1;
                      ulong size_b = b.first.second - b.first.first + 1;

                      /*
                       * is start at the same position return the shortest
                       * */
                      if (a_pos == b_pos)
                          return size_a < size_b;

                      auto sa_1_a = SA_1[a_pos];
                      auto sa_1_b = SA_1[b_pos];

                      int min = LCP[rmq(std::min(sa_1_a, sa_1_b) + 2, std::max(sa_1_a, sa_1_b) + 1)];

                      /*
                       * Check if one is prefix of the other
                       * return the shortest
                       * */

                      if (std::min(size_a, size_b) <= min) {
                          return size_a < size_b;
                      }

                      /*
                       * then return the lowest lexicographical
                       * */

                      return sa_1_a < sa_1_b;

                  });


//    std::cout<<"end sorting suffixes "<<std::endl;

//
//    uint i = 0;
//    for (auto && sf  : grammar_sfx) {
//
//        std::string sfs;
//        sfs.resize(sf.first.second - sf.first.first + 1);
//        std::copy(text.begin()+sf.first.first,text.begin()+sf.first.second+1,sfs.begin());
//        std::cout<<++i<<"-|"<<sfs<<std::endl;
//
//    }



        /*
         *
         * Building grid for 2d range search first occ
         *
         * */

//    std::cout<<"Building grid for 2d range search first occ"<<std::endl;

        std::vector<std::pair<std::pair<range_search2d::bin_long, range_search2d::bin_long>, range_search2d::bin_long>> grid_points(
                num_sfx);
        size_t bpair = 0;
        for (auto &&s  :grammar_sfx) {
            grid_points[bpair] = make_pair(make_pair(s.second.first, bpair + 1), s.second.second);
            ++bpair;
        }
        grid.code = code;
        grid.build(grid_points.begin(), grid_points.end(), not_compressed_grammar.n_rules(), num_sfx);
//    std::cout<<"Number of columns in the grid: "<<grid.n_columns()<<std::endl;
        //std::cout<<"***************************grid size "<<grid.size_in_bytes()*1.0/1024/1024<<std::endl;
        ///grid.print_size();
//        grid.print();
        /*
         * Building Sampled Patricia Trees for suffixes.
         *
         * Sampling by log(u)log(log(n))/log(n)
         * where n is the number of symbols in the repair grammar and u is the length of the original text
         * */



        uint n_rules;
        not_compressed_grammar.permutationSortRules(text, pi, n_rules);
        inv_pi = sdsl::inv_perm_support<>(&pi);
//        std::cout << "inv_pi: ";
//        for (int k = 0; k < pi.size(); ++k) {
//            std::cout << inv_pi[k] << " ";
//        }
//        std::cout << "\n";

        C = sdsl::bit_vector(256, 0);
        for (int i = 0; i < _g.alp.size(); ++i) {
            uint c = (uint) _g.alp[i];
            C[c] = 1;
        }

        c_rank1 = sdsl::bit_vector::rank_1_type(&C);
        c_sel1 = sdsl::bit_vector::select_1_type(&C);

        std::cout<<"SelfGrammarIndexBS: "<<SelfGrammarIndexBS::size_in_bytes()<<std::endl;


//        buildSamplings(qsampling);
//
//        {
//            std::ofstream fout("qgram");
//            if (!fout.is_open())
//                std::cout << "Error con fout\n";
//            grammarSamp->save(fout);
//
//        }
//        {
//            std::ifstream fin("qgram");
//            if (!fin.is_open())
//                std::cout << "Error con fin\n";
//            grammarSamp = Sampling::load(fin);
//
//        }
//
//        {
//            std::ofstream fout("qgram");
//            if (!fout.is_open())
//                std::cout << "Error con fout\n";
//            reverseSamp->save(fout);
//
//        }
//        {
//            std::ifstream fin("qgram");
//            if (!fin.is_open())
//                std::cout << "Error con fin\n";
//            reverseSamp = Sampling::load(fin);
//
//        }

    }

    void buildSamplings(uint qsampling) {

        std::cout << "building GRAMMAR REVERSE SAMPLING\n";
        {
            // *** GRAMMAR REVERSE SAMPLING
            // *******************
            uint DELTA_SAMP = 4;
            uint rules = _g.n_rules();
            uchar *prefix = new uchar[qsampling + 1];
            uchar *current = new uchar[qsampling + 1];
            current[0] = '\0';

            uint csamp = 0;
            uint *pointers = new uint[rules + 1];
            uchar *samples = new uchar[rules * (qsampling + 1)];
            pointers[csamp] = 0;
            csamp++;
            uchar *ptr = samples;

            uint terminals = c_rank1(256);
            uint *posterms = new uint[terminals + 1];
            terminals = 0;

            for (uint i = 1; i < rules; i++) {
                uint plen = _g.len_rule(_g.m_tree[_g.select_occ(i, 1)]);


                std::string temp = "";
                size_t pos = 0;
                temp.resize(qsampling);
                this->bp_expand_suffix(i, temp, qsampling, pos);
                temp[pos] = '\0';
                strcpy((char *) prefix, temp.c_str());
                prefix[pos] = '\0';


                if (strcmp((char *) current, (char *) prefix) != 0) {
                    strcpy((char *) ptr, (char *) prefix);

                    ptr += strlen((char *) prefix);

                    *ptr = '\0';
                    ptr++;

                    if (current[0] != prefix[0]) {
                        posterms[terminals] = ((csamp - 1) / BLOCK_SAMPLING);

                        terminals++;
                    }

                    pointers[csamp] = i;

                    csamp++;

                    strcpy((char *) current, (char *) prefix);
                }
            }
//            std::cout << "end extracting rules\n";
            pointers[csamp] = rules + 1;
            pointers[csamp + 1] = rules + 2;

            posterms[terminals] = ((csamp - 2) / BLOCK_SAMPLING);
            terminals++;
            grammarSamp = new Sampling(samples, ptr - samples, pointers, csamp, BLOCK_SAMPLING, terminals,
                                       posterms, DELTA_SAMP);

            delete[] pointers;
            delete[] samples;
            delete[] prefix;
            delete[] current;
        }

        std::cout << "building GRAMMAR SAMPLING\n";
        {
            // *** GRAMMAR SAMPLING
            // ********************
            uint DELTA_SAMP = 4;
//            uint rules = slp->getNColumns();
            uint rules = _g.n_rules();
            uchar *prefix = new uchar[qsampling + 1];
            uchar *suffix = new uchar[qsampling + 1];
            uchar *current = new uchar[qsampling + 1];
            current[0] = '\0';

            uint csamp = 0;
            uint *pointers = new uint[rules + 1];
            uchar *samples = new uchar[rules * (qsampling + 1)];
            pointers[csamp] = 0;
            csamp++;
            uchar *ptr = samples;

//            uint terminals = C->rank1(256);
            uint terminals = _g.rank_Y(rules);
            uint *posterms = new uint[terminals + 1];
            terminals = 0;
            std::cout << "rules " << rules << std::endl;
            std::cout << "pi " << pi.size() << std::endl;
            std::cout << "start extracting rules\n";
            for (uint i = 1; i < rules; i++) {
//                uint crule = permutation->pi(i);
//                uint plen = lenrules->access(crule);
                uint crule = pi[i];
//                std::cout << "rule " << crule << std::endl;
                uint plen = _g.len_rule(_g.m_tree[_g.select_occ(i, 1)]);
                std::string temp = "";
                size_t pos = 0;
                temp.resize(qsampling);
                this->bp_expand_prefix(crule, temp, qsampling, pos);
                temp[pos] = '\0';
                std::copy(temp.begin(), temp.end(), prefix);
                prefix[qsampling] = '\0';

//                if (plen > qsampling)
//                {
//                    extractCharsReverseNoSamples(crule, plen, qsampling, suffix);
//                    suffix[qsampling] = '\0';
//                    for (int k=qsampling-1; k>=0; k--) { prefix[qsampling-k-1] = suffix[k]; }
//                    prefix[qsampling] = '\0';
//                }
//                else
//                {
//                    extractCharsReverseNoSamples(crule, plen, plen, suffix);
//                    suffix[plen] = '\0';
//                    for (int k=plen-1; k>=0; k--) { prefix[plen-k-1] = suffix[k]; }
//                    prefix[plen] = '\0';
//                }
//                std::cout << "extracted rule" + temp + "\n";
                if (strcmp((char *) current, (char *) prefix) != 0) {
                    strcpy((char *) ptr, (char *) prefix);
                    ptr += strlen((char *) prefix);
                    *ptr = '\0';
                    ptr++;

                    if (current[0] != prefix[0]) {
                        posterms[terminals] = ((csamp - 1) / BLOCK_SAMPLING);
                        terminals++;
                    }

                    pointers[csamp] = i;
                    csamp++;

                    strcpy((char *) current, (char *) prefix);
//                    std::cout << "constructed structure " + temp + "\n";
                }
            }
//            std::cout << "end extracting rules\n";
            pointers[csamp] = rules + 1;
            pointers[csamp + 1] = rules + 2;

            posterms[terminals] = ((csamp - 2) / BLOCK_SAMPLING);
            terminals++;

            reverseSamp = new Sampling(samples, ptr - samples, pointers, csamp, BLOCK_SAMPLING, terminals, posterms,
                                       DELTA_SAMP);

            delete[] pointers;
            delete[] samples;
            delete[] prefix;
            delete[] suffix;
            delete[] current;
        }
        std::cout << "building GRID SFX SAMPLING\n";
        {


            // *** SEQUENCE SAMPLING
            // ********************
            uint DELTA_SAMP = 4;
//            uint lenseq = seqR->getNRows();
            uint lenseq = get_grid().n_columns();

            uchar *prefix = new uchar[qsampling+1];
            uchar *current = new uchar[qsampling+1];
            current[0] = '\0';

            uint csamp = 0;
            uint *pointers = new uint[lenseq+1];
            uchar *samples = new uchar[lenseq*(qsampling+1)];
            pointers[csamp] = 0; csamp++;
            uchar *ptr = samples;

//            uint terminals = C->rank1(256);
            uint terminals = _g.rank_Y(_g.n_rules());
            uint *posterms = new uint[terminals+1];
            terminals = 0;

            for (uint i=1; i<=lenseq; i++)
            {
                std::string ss;
                ss.resize(qsampling);
                this->expand_grammar_sfx(i,ss,qsampling);
//                ss[pos] = '\0';
                std::copy(ss.begin(), ss.end(), prefix);
                prefix[ss.size()] = '\0';

                if (strcmp((char*)current, (char*)prefix) != 0)
                {
                    strcpy((char*)ptr, (char*)prefix);
                    ptr += strlen((char*)prefix);
                    *ptr = '\0'; ptr++;

                    if (current[0] != prefix[0])
                    {
                        if (c_sel1(terminals+1) != (size_t)prefix[0])
                        {
                            size_t t;

                            do
                            {
                                posterms[terminals] = ((csamp-1)/BLOCK_SAMPLING);
                                terminals++;
                                t = c_sel1(terminals+1);
                            }
                            while (t != (size_t)prefix[0]);
                        }

                        posterms[terminals] = ((csamp-1)/BLOCK_SAMPLING);
                        terminals++;
                    }

                    pointers[csamp] = i;
                    csamp++;

                    strcpy((char*)current, (char*)prefix);
                }
            }

            pointers[csamp] = lenseq;
            pointers[csamp+1] = lenseq+1;

            posterms[terminals] = ((csamp-2)/BLOCK_SAMPLING);
            terminals++;

            sequenceSamp = new Sampling(samples, ptr-samples, pointers, csamp, BLOCK_SAMPLING, terminals, posterms, DELTA_SAMP);

            delete [] pointers;
            delete [] samples;
            delete [] prefix; delete [] current;
        }


        std::cout<<"SelfGrammarIndexBSQ: "<<size_in_bytes()<<std::endl;


    }

    void buildSamplingsFromGrammar(std::fstream &repair_g, const string &text,const uint &samplings) {

        grammar g;
        g.load(repair_g);

        uint n_rules;

        g.preprocess(text);

        std::cout << "sorting rules (" << g.n_rules() << ")\n";


        g.permutationSortRules(text, pi, n_rules);
        inv_pi = sdsl::inv_perm_support<>(&pi);

        C = sdsl::bit_vector(256, 0);
        for (int i = 0; i < _g.alp.size(); ++i) {
            uint c = (uint) _g.alp[i];
            C[c] = 1;
        }

        c_rank1 = sdsl::bit_vector::rank_1_type(&C);
        c_sel1 = sdsl::bit_vector::select_1_type(&C);

        buildSamplings(samplings);


    }
    void buildSamplingsFromGrammar(grammar &g, const string &text) {


        uint n_rules;

        g.preprocess(text);

        std::cout << "sorting rules (" << g.n_rules() << ")\n";


        g.permutationSortRules(text, pi, n_rules);
        inv_pi = sdsl::inv_perm_support<>(&pi);

        C = sdsl::bit_vector(256, 0);
        for (int i = 0; i < _g.alp.size(); ++i) {
            uint c = (uint) _g.alp[i];
            C[c] = 1;
        }

        c_rank1 = sdsl::bit_vector::rank_1_type(&C);
        c_sel1 = sdsl::bit_vector::select_1_type(&C);


    }

    void dfs_expand_suffix2(const grammar_representation::g_long &X, std::string &s, const size_t &l, size_t &pos) const {

        uint node = _g.m_tree[_g.select_occ(X, 1)];
        uint off = 0;
        dfs_expand_suffix_aux2(node,X, s, off, l, pos);
    }

    void dfs_expand_prefix2(const grammar_representation::g_long &X, std::string &s, const size_t &l, size_t &pos) const {

        uint node = _g.m_tree[_g.select_occ(X, 1)];
        uint off = 0;
        dfs_expand_prefix_aux2(node,X, s, off, l, pos);
    }


    /*
     *
     * node in dfuds representation to expand
     * X the label corresponding to the rule to expand
     * off number of symbols already expanded
     * l
     *
     * */
    void dfs_expand_suffix_aux2( uint &node,const uint& X, std::string &s,uint &off,const size_t &l,size_t &pos) const {

        /*
         *
         * Si la regla es terminal extraemos el simbolo
         *
         * */

        if (_g.isTerminal(X))
        {
            unsigned char a_th = _g.terminal_simbol(X); // a_th symbol in the sorted alphabet
            s[pos]= a_th;
            ++pos;
            return;
        }

        /*
         *
         * Si el nodo es hoja pero la regla no es terminal saltamos a su definicion
         *
         * */

        if(_g.m_tree.isleaf(node))
        {
            node = _g.m_tree[_g.select_occ(X,1)];
        }

        /*
         *
         * calculamos las posiciones de la expansion del nodo
         * y la longitud de la regla
         *
         * */
        auto limit = _g.limits_rule(node);

        uint len_rule = limit.second - limit.first + 1;


        uint qs = grammarSamp->getMaxLength();

        if (off < qs && len_rule > off) { ////////////REVISAR LA PRIMERA PREGUNTA OFF < QS ???
            /*
             * if we get a node where there are new sampled symbols that we need to expand
             * */

            /*
             * extract prefix rule
             *
             * */


            uchar *ss = new uchar[qs];

            grammarSamp->extract(X, ss);

            uint min_len = (qs < len_rule) ? qs : len_rule;

            ss[min_len] = '\0';

            uint diff = 0;

            if( (pos-off) + min_len < l )
            {
                std::copy(ss+off,ss+min_len,s.begin()+pos);
                diff = min_len - off;
            }
            else{
                std::copy(ss+off,ss+off+(l-pos),s.begin()+pos);
                diff = l - pos;
            }

            off += diff;
            pos += diff;

            delete ss;

            if(pos == l || min_len == len_rule) return ;

        }
        /*
         *
         * if we still in a node that is inside the sampling
         *
         * */
        uint children = _g.m_tree.children(node);

        uint rlch = children;


        for (;rlch > 0;--rlch)
        {
            uint child = this->_g.m_tree.child(node, rlch);

            auto child_limits = _g.limits_rule(child);

            uint lleft = limit.second - child_limits.first;

            uint lright = limit.second - child_limits.second;

            if (off >= lright && off <= lleft)
            {
                uint new_off = off  - (limit.second - child_limits.second);

                size_t old_pos = pos;

                dfs_expand_suffix_aux2(child,_g[_g.m_tree.pre_order(child)],s,new_off,l,pos);

                off = off + (pos - old_pos);

                if(l == pos) return;

                break;
            }


        }

        --rlch;
        for (;rlch > 0;--rlch){
            uint child = this->_g.m_tree.child(node, rlch);
            uint n_off = 0;
            dfs_expand_suffix_aux2(child,_g[_g.m_tree.pre_order(child)],s,n_off,l,pos);
            if(l == pos) return;
        }
    }

    void dfs_expand_prefix_aux2(uint &node,const uint& X, std::string &s,uint &off,const size_t &l,size_t &pos) const {

        if (_g.isTerminal(X)) {

            unsigned char a_th = _g.terminal_simbol(X); // a_th symbol in the sorted alphabet
            s[pos]= a_th;
            ++pos;
            return;
        }

        if(_g.m_tree.isleaf(node))
        {
            node = _g.m_tree[_g.select_occ(X,1)];
        }


        auto limit = _g.limits_rule(node);

        uint len_rule = limit.second - limit.first + 1;

        uint qs = grammarSamp->getMaxLength();

        if (off < qs && len_rule > off) {
            /*
             * if we get a node where there are new sampled symbols that we need to expand
             * */
            uchar *ss = new uchar[qs];
            uint rev_rules = inv_pi[X];
            reverseSamp->extract(rev_rules, ss);
            uint min_len = (qs < len_rule) ? qs : len_rule;
            ss[min_len] = '\0';
            uint diff = 0;

            if( (pos-off) +  min_len < l ){
                std::copy(ss+off,ss+min_len,s.begin()+pos);
                diff = min_len - off;
            }
            else{
                std::copy(ss+off,ss+off+(l-pos),s.begin()+pos);
                diff = l - pos;
            }

            off += diff;
            pos += diff;


            delete ss;

            if(pos == l || min_len == len_rule ) return ;

        }
        /*
         * if we still in a node that is inside the sampling
         *
         * */
        uint children = _g.m_tree.children(node);

        uint rfch = 1;


        for (;rfch <= children; ++rfch)
        {
            uint child = this->_g.m_tree.child(node, rfch);

            auto child_limits = _g.limits_rule(child);

            uint lleft = child_limits.first - limit.first+1;

            uint lright = child_limits.second - limit.first+1;

            if (off+1 >= lleft && off+1 <= lright){

                uint new_off = off  - (child_limits.first - limit.first);

                size_t old_pos = pos;

                dfs_expand_prefix_aux2(child,_g[_g.m_tree.pre_order(child)],s,new_off,l,pos);

                off = off + (pos - old_pos);

                if(l == pos) return;

                break;
            }

        }

        ++rfch;
        for (;rfch <= children; ++rfch){
            uint child = this->_g.m_tree.child(node, rfch);
            uint n_off = 0;
            dfs_expand_prefix_aux2(child,_g[_g.m_tree.pre_order(child)],s,n_off,l,pos);
            if(l == pos) return;
        }

    }

    virtual int dfs_cmp_suffix_q(const uint &X, std::string::iterator &itera,std::string::iterator &end){

        uint off = 0, pos = 0;

        uint node = _g.m_tree[_g.select_occ(X, 1)];

        auto limit = _g.limits_rule(node);

        int r = dfs_cmp_suffix_aux(node,X,limit,itera,end,off,pos);

        if(r == 0 && itera != end) return 1;

        if(r == 0 && itera == end && pos < limit.second-limit.first+1) return -1;

        return r;

    }

    virtual int dfs_cmp_suffix_aux(uint &node, const uint &X, std::pair<uint,uint>& limit, std::string::iterator &itera,std::string::iterator &end, uint &off, uint& pos) const {

        if (_g.isTerminal(X)) {

            unsigned char a_th = _g.terminal_simbol(X);

            if (a_th < (unsigned char) (*itera)) return 1;

            if (a_th > (unsigned char) (*itera)) return -1;

            --itera;++pos;++off;
            return 0;
        }

        if(_g.m_tree.isleaf(node))
        {
            node = _g.m_tree[_g.select_occ(X,1)];
            limit = _g.limits_rule(node);
        }

        uint len_rule = limit.second - limit.first + 1;

        uint qs = grammarSamp->getMaxLength();

        if (off < qs && len_rule > off) {
            /*
             * if we get a node where there are new sampled symbols that we need to expand
             * */
            uchar *ss = new uchar[qs];
            grammarSamp->extract(X, ss);
            uint min_len = (qs < len_rule) ? qs : len_rule;
            ss[min_len] = '\0';

            for (int i = off; i < min_len; ++i)
            {
                if(itera == end){
                    delete ss;
                    return 0;
                }

                if(ss[i] < uchar(*itera)) return 1;
                if(ss[i] > uchar(*itera)) return -1;
                --itera;
                ++pos;++off;
            }

            delete ss;

            if(itera == end||min_len == len_rule)
            {
                return 0;
            }

        }



        /*
         * if we still in a node that is inside the sampling
         *
         * */
        uint children = _g.m_tree.children(node);

        uint rfch = 1, rlch = children;


        for (;rlch > 0;--rlch)
        {
            uint child = this->_g.m_tree.child(node, rlch);

            auto child_limits = _g.limits_rule(child);

            uint lleft = limit.second - child_limits.first;

            uint lright = limit.second - child_limits.second;

            if (off >= lright && off <= lleft)
            {
                uint new_off = off  - (limit.second - child_limits.second);

                size_t old_pos = pos;

                int rr = dfs_cmp_suffix_aux(child,_g[_g.m_tree.pre_order(child)],child_limits,itera,end, new_off,pos);

                if ( rr != 0) return rr;
                if(itera == end) return 0;
                off = off + (pos - old_pos);

            }


        }



//        uint n_leaves = _g.m_tree.rank_00(_g.m_tree.bit_vector.size());
//
//        uint rch = _g.m_tree.find_child_dbs_mirror(node, rfch, rlch,
//                       [this, &node, &limit, &off, &n_leaves](const uint &rank_ch) {
//
//                           uint child = this->_g.m_tree.child(node, rank_ch);
//
//                           uint next_node = _g.m_tree.nextTreeNode(child);
//                           uint child_right_pos;
//
//                           if (next_node == _g.m_tree.bit_vector.size())
//                               child_right_pos = _g.L.size() - 1;
//                           else {
//                               child_right_pos =
//                                       _g.select_L(_g.m_tree.leafrank(next_node)) - 1;
//                           }
//
//                           uint ch_n_sym = limit.second - child_right_pos;
//
//                           if (off + 1 <= ch_n_sym)
//                               return false;
//                           return true;
//                       });

//        for (int i = rlch; i > 0; --i) {
//
//            uint child = this->_g.m_tree.child(node, i);
//
//            uint next_node = _g.m_tree.nextTreeNode(child);
//            uint child_right_pos;
//
//            if (next_node == _g.m_tree.bit_vector.size())
//                child_right_pos = _g.L.size() - 1;
//            else {
//                child_right_pos = _g.select_L(_g.m_tree.leafrank(next_node)) - 1;
//            }
//            uint new_off = off - (limit.second - child_right_pos);
//
//            size_t old_pos = pos;
//
//            int rr = dfs_cmp_suffix_aux(_g[_g.m_tree.pre_order(child)],itera,end, new_off,pos);
//
//            if ( rr != 0)
//                return rr;
//            if(itera == end-1)
//                return 0;
//
//            off = off + (pos - old_pos);
//
//        }

        return 0;

    }

    virtual int dfs_cmp_prefix_q(const grammar_representation::g_long &X, std::string::iterator &itera,std::string::iterator &end) const {
        uint off = 0, pos = 0;

        uint node = _g.m_tree[_g.select_occ(X, 1)];

        auto limit = _g.limits_rule(node);

        int r = dfs_cmp_prefix_aux(node,X,limit,itera,end,off,pos);

        if(r == 0 && itera != end) return 1;

        if(r == 0 && itera == end && pos < limit.second-limit.first+1) return -1;

        return r;
    }

    virtual int dfs_cmp_prefix_aux(uint& node,const uint &X, std::pair<uint,uint> limit, std::string::iterator &itera,std::string::iterator &end, uint &off, uint& pos) const{



        if (_g.isTerminal(X)) {

            unsigned char a_th = _g.terminal_simbol(X);

            if (a_th < (unsigned char) (*itera)) return 1;

            if (a_th > (unsigned char) (*itera)) return -1;

            ++itera;++pos;++off;
            return 0;
        }


        if(_g.m_tree.isleaf(node))
        {
            node = _g.m_tree[_g.select_occ(X,1)];
            limit = _g.limits_rule(node);
        }

        uint len_rule = limit.second - limit.first + 1;

        uint qs = reverseSamp->getMaxLength();

        if (off < qs && len_rule > off) {
            /*
             * if we get a node where there are new sampled symbols that we need to expand
             * */
            uchar *ss = new uchar[qs];
            reverseSamp->extract(inv_pi[X], ss);
            uint min_len = (qs < len_rule) ? qs : len_rule;
            ss[min_len] = '\0';

            for (int i = off; i < min_len; ++i)
            {
                if(itera == end){
                    delete ss;
                    return 0;
                }

                if(ss[i] < uchar(*itera)) return 1;
                if(ss[i] > uchar(*itera)) return -1;

                ++itera;
                ++pos;++off;
            }

            delete ss;

            if(itera == end||min_len == len_rule)
            {
                return 0;
            }

        }

        uint children = _g.m_tree.children(node);

        uint rfch = 1;


        for (;rfch <= children; ++rfch)
        {
            uint child = this->_g.m_tree.child(node, rfch);

            auto child_limits = _g.limits_rule(child);

            uint lleft = child_limits.first - limit.first+1;

            uint lright = child_limits.second - limit.first+1;

            if (off+1 >= lleft && off+1 <= lright)
            {
                uint new_off = off  - (child_limits.first - limit.first);

                size_t old_pos = pos;

                int rr = dfs_cmp_prefix_aux(child,_g[_g.m_tree.pre_order(child)],child_limits,itera,end, new_off,pos);

                if ( rr != 0) return rr;
                if(itera == end) return 0;
                off = off + (pos - old_pos);

            }


        }

        return 0;




//        uint children = _g.m_tree.children(node);
//
//        uint rfch = 1, rlch = children;
//
//
//        for( rfch = 1 ; rfch <= rlch; ++rfch)
//        {
//            uint child = this->_g.m_tree.child(node, rfch);
//
//            size_t child_left = this->_g.select_L(_g.m_tree.leafrank(child));
//
//            size_t ch_n_sym = child_left - limit.first;
//
//            if (off + 1 >= ch_n_sym)
//                break;
//        }
//
////        uint n_leaves = _g.m_tree.rank_00(_g.m_tree.bit_vector.size());
////
////        uint rch = _g.m_tree.find_child_dbs(node, rfch, rlch,
////                                            [this, &node, &limit, &off, &n_leaves](const uint &rank_ch) {
////
////                                                uint child = this->_g.m_tree.child(node, rank_ch);
////
////                                                size_t child_left = this->_g.select_L(_g.m_tree.leafrank(child));
////
////                                                size_t ch_n_sym = child_left - limit.first;
////
////                                                if (off + 1 <= ch_n_sym)
////                                                    return false;
////                                                return true;
////                                            });
//
//
//        for (int i = rfch; i <= children; ++i) {
//
//            uint child = this->_g.m_tree.child(node, i);
//
//            uint child_left = this->_g.select_L(_g.m_tree.leafrank(child));
//
//            uint new_off = limit.first + off - child_left;
//
//            uint old_pos = pos;
//
//            int r = dfs_cmp_prefix_aux(_g[_g.m_tree.pre_order(child)], itera, end, new_off, pos);
//            if (r != 0) return r;
//            if (itera == end)return 0;
//
//            off = off + (pos - old_pos);
//        }
//
//        return 0;


}

    void printSampling() {


        std::cout << "grammarSamp->getMaxLength() " << grammarSamp->getMaxLength() << std::endl;
        std::cout << "grammarSamp->getSize() " << grammarSamp->getSize() << std::endl;
        std::cout << "grammarSamp->getTerminals() " << grammarSamp->getTerminals() << std::endl;
        std::cout << "grammarSamp->getBlocks() " << grammarSamp->getBlocks() << std::endl;


        for (uint i = 1; i < _g.n_rules(); ++i) {
            uchar *s = new uchar[100];

            grammarSamp->extract(i, s);

            std::cout << i << "-> " << s << std::endl;

        }


        std::cout << "reverseSamp->getMaxLength() " << reverseSamp->getMaxLength() << std::endl;
        std::cout << "reverseSamp->getSize() " << reverseSamp->getSize() << std::endl;
        std::cout << "reverseSamp->getTerminals() " << reverseSamp->getTerminals() << std::endl;
        std::cout << "reverseSamp->getBlocks() " << reverseSamp->getBlocks() << std::endl;


        for (uint i = 1; i < _g.n_rules(); ++i) {
            uchar *s = new uchar[100];

            reverseSamp->extract(i, s);

            std::cout << i << "-> " << s << std::endl;

        }

    }

    void load(fstream &f) {

        SelfGrammarIndexBS::load(f);
        sdsl::load(pi, f);
        sdsl::load(C, f);
        c_rank1 = sdsl::bit_vector::rank_1_type(&C);
        c_sel1 = sdsl::bit_vector::select_1_type(&C);
        inv_pi = sdsl::inv_perm_support<>(&pi);

    }

    void load_basics(fstream &f) {
//        std::cout<<"loading.."<<std::endl;
        SelfGrammarIndexBS::load_basics(f);

//        std::cout<<"loaded.."<<std::endl;
    }

    void save(fstream &f) {
        SelfGrammarIndexBS::save(f);
        sdsl::serialize(pi, f);
        sdsl::serialize(C, f);

    }

    void saveSampling(std::ofstream &G, std::ofstream &GR,std::ofstream &GS) {
        grammarSamp->save(G);
        reverseSamp->save(GR);
        sequenceSamp->save(GS);
    }

    void saveSampling(std::ofstream &f, std::ofstream &G, std::ofstream &GR,std::ofstream &GS) {

        sdsl::serialize(pi, f);
        sdsl::serialize(C, f);
        grammarSamp->save(G);
        reverseSamp->save(GR);
        sequenceSamp->save(GS);
    }

    void loadSampling(std::ifstream &f, std::ifstream &G, std::ifstream &GR, std::ifstream &GS) {


        sdsl::load(pi, f);
        sdsl::load(C, f);
        c_rank1 = sdsl::bit_vector::rank_1_type(&C);
        c_sel1 = sdsl::bit_vector::select_1_type(&C);
        inv_pi = sdsl::inv_perm_support<>(&pi);


        grammarSamp = Sampling::load(G);
        reverseSamp = Sampling::load(GR);
        sequenceSamp= Sampling::load(GS);

    }
    void loadSampling(std::ifstream &G, std::ifstream &GR, std::ifstream &GS) {

        grammarSamp = Sampling::load(G);
        reverseSamp = Sampling::load(GR);
        sequenceSamp= Sampling::load(GS);

    }

    size_t size_in_bytes() {
        return SelfGrammarIndexBS::size_in_bytes() + grammarSamp->getSize() + reverseSamp->getSize() + sequenceSamp->getSize() +
               sdsl::size_in_bytes(pi) + sdsl::size_in_bytes(inv_pi) + sdsl::size_in_bytes(C) +
               sdsl::size_in_bytes(c_sel1) + sdsl::size_in_bytes(c_rank1);
    }

    virtual void qgram_locate( std::string & pattern, std::vector<uint> & occ){

        if(pattern.size() == 1)
        {
            locate_ch(pattern[0],occ);
            return;
        }


        size_t p_n = pattern.size();
        size_t n_xj = _g.n_rules()-1;
        size_t n_sj = grid.n_columns();
        uint smp = grammarSamp->getMaxLength();
        /*
         *
         * partitioning the pattern
         *
         * */
        for (size_t  i = 1; i <= p_n ; ++i)
        {

            auto itera = pattern.begin() + i-1;


            grammar_representation::g_long lr = 1, hr = n_xj;

            bool found = false;

            lower_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto begin = pattern.begin()-1;
                auto end = itera;

                auto r =  match_suffix(a,end,begin);

                if(r == 0 && begin != end) return 1;

                return r;
            });

            if(!found)
                continue;

            binary_relation::bin_long r1 = lr;
            hr = n_xj;//right > n_xj?n_xj:right;

//            std::cout<<ss<<"("<<lr<<","<<hr<<")"<<std::endl;
//            sleep(2);

            found = false;
            upper_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto begin = pattern.begin()-1;
                auto end = itera;
                auto r =  match_suffix(a,end,begin);
                if(r == 0 && begin != end) return 1;
//                if(r == 0 && end != begin) return 1;

                return r;
            });
            if(!found)
                continue;
//            std::cout<<ss<<"("<<lr<<","<<hr<<")"<<std::endl;
//            sleep(2);
            binary_relation::bin_long r2 = hr;


            grammar_representation::g_long ls = 1,hs = n_sj;

            found = false;
            lower_bound(found, ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto end = pattern.end();
                auto begin = itera+1;
                auto r = dfs_cmp_suffix_grammar(a,begin,end);

                return r;
            });

            if(!found)
                continue;
            binary_relation::bin_long c1 = ls;
            hs = n_sj;

            found = false;

            upper_bound(found,ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto end = pattern.end();
                auto begin = itera+1;
                auto r =  dfs_cmp_suffix_grammar(a,begin,end);
                return r;
            });

            if(!found)
                continue;
//
//

            binary_relation::bin_long c2 = hs;
////            std::cout<<r1<<" "<<r2<<" "<<c1<<" "<<c2<<std::endl;
//


            long len = itera-pattern.begin() +1;


            find_second_occ(r1,r2,c1,c2,len,occ);

//            const auto& g_tree = _g.get_parser_tree();
//            std::vector< std::pair<size_t,size_t> > pairs;
//            grid.range2(r1,r2,c1,c2,pairs);
//
//            for (auto &pair : pairs) {
//                size_t p = grid.first_label_col(pair.second);
//                size_t pos_p = _g.offsetText(g_tree[p]);
//                unsigned int parent = g_tree.parent(g_tree[p]);
//                long int  l = (- len + pos_p) - _g.offsetText(parent);
//                find_second_occ(l,parent,occ);
//            }
//

        }


    }
    virtual void qgram_locate_prefix_search( std::string & pattern, std::vector<uint> & occ){

        if(pattern.size() == 1)
        {
            locate_ch(pattern[0],occ);
            return;
        }


        size_t p_n = pattern.size();
        size_t n_xj = _g.n_rules()-1;
        size_t n_sj = grid.n_columns();
        uint smp = grammarSamp->getMaxLength();


        /*
         *
         * partitioning the pattern
         *
         * */
        for (size_t  i = 1; i <= p_n ; ++i)
        {

            auto itera = pattern.begin() + i-1;

            uint _len_p = itera-pattern.begin()+1;
            char* ss;
            if(i > smp){
                ss  = new char[smp];
                std::copy(itera-smp+1,itera+1,ss);
                std::reverse(ss,ss+smp);
                ss[smp] = '\0';
            }else{
                ss = new char[_len_p];
                std::copy(pattern.begin(),itera+1,ss);
                ss[_len_p] = '\0';
                std::reverse(ss,ss+_len_p);
            }

            uint left = 1;
            uint right = n_xj;

            size_t first = c_rank1(uchar(*itera))+1;

            uint lb = grammarSamp->leftBlockForTerminal(first);
            uint rb = grammarSamp->rightBlockForTerminal(first);

            grammarSamp->locatePrefix((uchar*)ss,strlen(ss),&left,&right,&lb,&rb);
            if(left > right) std::swap(left,right);
//
//            std::cout<<ss<<"("<<left<<","<<right<<")"<<std::endl;
            delete ss;
//            sleep(2);
//
            if(!left || !right)
                continue;

            grammar_representation::g_long lr = left,hr = right > n_xj?n_xj:right;

            bool found = false;

            lower_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto begin = pattern.begin()-1;
                auto end = itera;
                auto r =  match_suffix(a,end,begin);
                if(r == 0 && begin != end) return 1;
//                if(r == 0 && end != begin) return 1;
                return r;
            });

            if(!found)
                continue;

            binary_relation::bin_long r1 = lr;
            hr = right > n_xj?n_xj:right;

//            std::cout<<ss<<"("<<lr<<","<<hr<<")"<<std::endl;
//            sleep(2);

            found = false;
            upper_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto begin = pattern.begin()-1;
                auto end = itera;
                auto r =  match_suffix(a,end,begin);
                if(r == 0 && begin != end) return 1;
//                if(r == 0 && end != begin) return 1;

                return r;
            });
            if(!found)
                continue;

            binary_relation::bin_long r2 = hr;

            ////////////////////////binary search on suffix/////////
            _len_p = pattern.end()-itera-1;

            if(smp > _len_p){
                ss = new char[_len_p];
                std::copy(itera+1,pattern.end(),ss);
                ss[_len_p] = '\0';
            }else{
                ss = new char[smp];
                std::copy(itera+1,itera+1+smp,ss);
                ss[smp] = '\0';
            }

            left = 1;
            right = n_sj;

            first = c_rank1(uchar(*(itera+1)))+1;

            lb = sequenceSamp->leftBlockForTerminal(first);
            rb = sequenceSamp->rightBlockForTerminal(first);

//            std::cout<<"ss:"<<ss<<std::endl;
//            std::cout<<"left:"<<left<<std::endl;
//            std::cout<<"right:"<<right<<std::endl;
//            std::cout<<"lb:"<<lb<<std::endl;
//            std::cout<<"rb:"<<rb<<std::endl;
//            std::cout<<"first:"<<first<<std::endl;
//            std::cout<<"////////////////////\n";
            if(_len_p!=0)
                sequenceSamp->locatePrefix((uchar*)ss,strlen(ss),&left,&right,&lb,&rb);

            if(left > right) std::swap(left,right);

            delete ss;

            if(!left || !right)
                continue;

            grammar_representation::g_long ls = left,hs = right > n_sj?n_sj:right;

            found = false;
            lower_bound(found, ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto end = pattern.end();
                auto it2 = itera+1;
                auto r = dfs_cmp_suffix_grammar(a,it2,end);

                return r;
            });

            if(!found)
                continue;
            binary_relation::bin_long c1 = ls;
            hs = right > n_sj?n_sj:right;;

            found = false;

            upper_bound(found,ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto end = pattern.end();
                auto it2 = itera+1;
                auto r =  dfs_cmp_suffix_grammar(a,it2,end);
                return r;
            });

            if(!found)
                continue;

            binary_relation::bin_long c2 = hs;
//            std::cout<<r1<<" "<<r2<<" "<<c1<<" "<<c2<<std::endl;


            long len = itera-pattern.begin() +1;


            find_second_occ(r1,r2,c1,c2,len,occ);

//            const auto& g_tree = _g.get_parser_tree();
//            std::vector< std::pair<size_t,size_t> > pairs;
//            grid.range2(r1,r2,c1,c2,pairs);
//
//            long len = itera-pattern.begin() +1;
//
//            for (auto &pair : pairs) {
//                size_t p = grid.first_label_col(pair.second);
//                size_t pos_p = _g.offsetText(g_tree[p]);
//                unsigned int parent = g_tree.parent(g_tree[p]);
//                long int  l = (- len + pos_p) - _g.offsetText(parent);
//                find_second_occ(l,parent,occ);
//            }
//

        }


    }
    virtual void qgram_trie_locate(std::string & pattern, std::vector<uint> & occ){


        if(pattern.size() == 1)
        {
            locate_ch(pattern[0],occ);
            return;
        }


        size_t p_n = pattern.size();
        size_t n_xj = _g.n_rules()-1;
        size_t n_sj = grid.n_columns();
        uint smp = grammarSamp->getMaxLength();


        /*
         *
         * partitioning the pattern
         *
         * */
        for (size_t  i = 1; i <= p_n ; ++i)
        {

            auto itera = pattern.begin() + i-1;

            uint _len_p = itera-pattern.begin()+1;
            char* ss;
            if(i > smp){
                ss  = new char[smp];
                std::copy(itera-smp+1,itera+1,ss);
                std::reverse(ss,ss+smp);
                ss[smp] = '\0';
            }else{
                ss = new char[_len_p];
                std::copy(pattern.begin(),itera+1,ss);
                ss[_len_p] = '\0';
                std::reverse(ss,ss+_len_p);
            }

            uint left = 1;
            uint right = n_xj;

            size_t first = c_rank1(uchar(*itera))+1;

            uint lb = grammarSamp->leftBlockForTerminal(first);
            uint rb = grammarSamp->rightBlockForTerminal(first);

            grammarSamp->locatePrefix((uchar*)ss,strlen(ss),&left,&right,&lb,&rb);
            if(left > right) std::swap(left,right);
//
//            std::cout<<ss<<"("<<left<<","<<right<<")"<<std::endl;
            delete ss;
//            sleep(2);
//
            if(!left || !right)
                continue;

            grammar_representation::g_long lr = left,hr = right > n_xj?n_xj:right;

            bool found = false;

            lower_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto begin = pattern.begin();
                auto end = itera;
                auto r =  bp_cmp_suffix(a,end,begin);
                if(r == 0 && end != begin-1) return 1;
                return r;
            });

            if(!found)
                continue;

            binary_relation::bin_long r1 = lr;
            hr = right > n_xj?n_xj:right;

//            std::cout<<ss<<"("<<lr<<","<<hr<<")"<<std::endl;
//            sleep(2);

            found = false;
            upper_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto begin = pattern.begin();
                auto end = itera;
                auto r =  bp_cmp_suffix(a,end,begin);
                if(r == 0 && end != begin-1) return 1;
                return r;
            });
            if(!found)
                continue;

            binary_relation::bin_long r2 = hr;

            ////////////////////////binary search on suffix/////////
            _len_p = pattern.end()-itera-1;

            if(smp > _len_p){
                ss = new char[_len_p];
                std::copy(itera+1,pattern.end(),ss);
                ss[_len_p] = '\0';
            }else{
                ss = new char[smp];
                std::copy(itera+1,itera+1+smp,ss);
                ss[smp] = '\0';
            }

            left = 1;
            right = n_sj;

            first = c_rank1(uchar(*(itera+1)))+1;

            lb = sequenceSamp->leftBlockForTerminal(first);
            rb = sequenceSamp->rightBlockForTerminal(first);

//            std::cout<<"ss:"<<ss<<std::endl;
//            std::cout<<"left:"<<left<<std::endl;
//            std::cout<<"right:"<<right<<std::endl;
//            std::cout<<"lb:"<<lb<<std::endl;
//            std::cout<<"rb:"<<rb<<std::endl;
//            std::cout<<"first:"<<first<<std::endl;
//            std::cout<<"////////////////////\n";
            if(_len_p!=0)
                sequenceSamp->locatePrefix((uchar*)ss,strlen(ss),&left,&right,&lb,&rb);

            if(left > right) std::swap(left,right);

            delete ss;

            if(!left || !right)
                continue;

            grammar_representation::g_long ls = left,hs = right > n_sj?n_sj:right;

            found = false;
            lower_bound(found, ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto end = pattern.end();
                auto it2 = itera+1;
                auto r = bp_cmp_suffix_grammar(a,it2,end);
                return r;
            });

            if(!found)
                continue;
            binary_relation::bin_long c1 = ls;
            hs = right > n_sj?n_sj:right;;

            found = false;

            upper_bound(found,ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto end = pattern.end();
                auto it2 = itera+1;
                auto r = bp_cmp_suffix_grammar(a,it2,end);
                return r;
            });

            if(!found)
                continue;

            binary_relation::bin_long c2 = hs;
//            std::cout<<r1<<" "<<r2<<" "<<c1<<" "<<c2<<std::endl;

            long len = itera-pattern.begin() +1;


            find_second_occ(r1,r2,c1,c2,len,occ);
//            const auto& g_tree = _g.get_parser_tree();
//            std::vector< std::pair<size_t,size_t> > pairs;
//            grid.range2(r1,r2,c1,c2,pairs);
//
//            long len = itera-pattern.begin() +1;
//
//            for (auto &pair : pairs) {
//                size_t p = grid.first_label_col(pair.second);
//                size_t pos_p = _g.offsetText(g_tree[p]);
//                unsigned int parent = g_tree.parent(g_tree[p]);
//                long int  l = (- len + pos_p) - _g.offsetText(parent);
//                find_second_occ(l,parent,occ);
//            }
////

        }

    }
    virtual void qgram_dfs_locate(std::string & pattern, std::vector<uint> & occ){

        if(pattern.size() == 1)
        {
            locate_ch(pattern[0],occ);
            return;
        }


        size_t p_n = pattern.size();
        size_t n_xj = _g.n_rules()-1;
        size_t n_sj = grid.n_columns();
        uint smp = grammarSamp->getMaxLength();


        /*
         *
         * partitioning the pattern
         *
         * */
        for (size_t  i = 1; i <= p_n ; ++i)
        {

            auto itera = pattern.begin() + i-1;

            uint _len_p = itera-pattern.begin()+1;
            char* ss;
            if(i > smp){
                ss  = new char[smp];
                std::copy(itera-smp+1,itera+1,ss);
                std::reverse(ss,ss+smp);
                ss[smp] = '\0';
            }else{
                ss = new char[_len_p];
                std::copy(pattern.begin(),itera+1,ss);
                ss[_len_p] = '\0';
                std::reverse(ss,ss+_len_p);
            }

            uint left = 1;
            uint right = n_xj;

            size_t first = c_rank1(uchar(*itera))+1;

            uint lb = grammarSamp->leftBlockForTerminal(first);
            uint rb = grammarSamp->rightBlockForTerminal(first);

            grammarSamp->locatePrefix((uchar*)ss,strlen(ss),&left,&right,&lb,&rb);
            if(left > right) std::swap(left,right);
//
//            std::cout<<ss<<"("<<left<<","<<right<<")"<<std::endl;
            delete ss;
//            sleep(2);
//
            if(!left || !right)
                continue;

            grammar_representation::g_long lr = left,hr = right > n_xj?n_xj:right;

            bool found = false;

            lower_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto begin = pattern.begin();
                auto end = itera;
                auto r =  dfs_cmp_suffix(a,end,begin);
                if(r == 0 && end != begin-1) return 1;
                return r;
            });

            if(!found)
                continue;

            binary_relation::bin_long r1 = lr;
            hr = right > n_xj?n_xj:right;

//            std::cout<<ss<<"("<<lr<<","<<hr<<")"<<std::endl;
//            sleep(2);

            found = false;
            upper_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto begin = pattern.begin();
                auto end = itera;
                auto r =  dfs_cmp_suffix(a,end,begin);
                if(r == 0 && end != begin-1) return 1;
                return r;
            });
            if(!found)
                continue;

            binary_relation::bin_long r2 = hr;

            ////////////////////////binary search on suffix/////////
            _len_p = pattern.end()-itera-1;

            if(smp > _len_p){
                ss = new char[_len_p];
                std::copy(itera+1,pattern.end(),ss);
                ss[_len_p] = '\0';
            }else{
                ss = new char[smp];
                std::copy(itera+1,itera+1+smp,ss);
                ss[smp] = '\0';
            }

            left = 1;
            right = n_sj;

            first = c_rank1(uchar(*(itera+1)))+1;

            lb = sequenceSamp->leftBlockForTerminal(first);
            rb = sequenceSamp->rightBlockForTerminal(first);

//            std::cout<<"ss:"<<ss<<std::endl;
//            std::cout<<"left:"<<left<<std::endl;
//            std::cout<<"right:"<<right<<std::endl;
//            std::cout<<"lb:"<<lb<<std::endl;
//            std::cout<<"rb:"<<rb<<std::endl;
//            std::cout<<"first:"<<first<<std::endl;
//            std::cout<<"////////////////////\n";
            if(_len_p!=0)
                sequenceSamp->locatePrefix((uchar*)ss,strlen(ss),&left,&right,&lb,&rb);

            if(left > right) std::swap(left,right);

            delete ss;

            if(!left || !right)
                continue;

            grammar_representation::g_long ls = left,hs = right > n_sj?n_sj:right;

            found = false;
            lower_bound(found, ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto end = pattern.end();
                auto it2 = itera+1;
                auto r = cmp_suffix_grammar(a,it2,end);
                return r;
            });

            if(!found)
                continue;
            binary_relation::bin_long c1 = ls;
            hs = right > n_sj?n_sj:right;;

            found = false;

            upper_bound(found,ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
            {
                auto end = pattern.end();
                auto it2 = itera+1;
                auto r = cmp_suffix_grammar(a,it2,end);
                return r;
            });

            if(!found)
                continue;

            binary_relation::bin_long c2 = hs;
//            std::cout<<r1<<" "<<r2<<" "<<c1<<" "<<c2<<std::endl;

            long len = itera-pattern.begin() +1;


            find_second_occ(r1,r2,c1,c2,len,occ);
//            const auto& g_tree = _g.get_parser_tree();
//            std::vector< std::pair<size_t,size_t> > pairs;
//            grid.range2(r1,r2,c1,c2,pairs);
//
//            long len = itera-pattern.begin() +1;
//
//            for (auto &pair : pairs) {
//                size_t p = grid.first_label_col(pair.second);
//                size_t pos_p = _g.offsetText(g_tree[p]);
//                unsigned int parent = g_tree.parent(g_tree[p]);
//                long int  l = (- len + pos_p) - _g.offsetText(parent);
//                find_second_occ(l,parent,occ);
//            }
//

        }

    }

    int dfs_cmp_suffix_grammar(const size_t & sfx, std::string::iterator & iterator1, std::string::iterator & iterator2){

        if(iterator1 == iterator2)
        {
            return 1;
        }


        const auto& parser_tree = _g.get_parser_tree();

        size_t sfx_preorder = grid.first_label_col(sfx);
        size_t X = _g[sfx_preorder];

        auto r = match_prefix(X,iterator1,iterator2);

        if(r == 0)
        {
            auto node = parser_tree[sfx_preorder];
            auto parent = parser_tree.parent(node);
            auto child_r = parser_tree.childrank(node);
            auto n  = parser_tree.children(parent);
            child_r ++;

            while(r == 0 && iterator1!=iterator2 && child_r <= n ){
                size_t rnode = parser_tree.child(parent,child_r);
                rnode = parser_tree.pre_order(rnode);
                auto Y = _g[rnode];
                r = match_prefix(Y,iterator1,iterator2);
                child_r ++;
            }

            if(r == 0 && child_r > n && iterator1 != iterator2) return  1; // the sufix is prefix of the string
        }

        return r;
    }
    virtual void display(const std::size_t& i, const std::size_t& j, std::string & s){
//        s.resize(j - i + 1);
//        size_t p = 0;
        SelfGrammarIndex::display(i,j,s);
    }
    virtual void display_qgram(const std::size_t& i, const std::size_t& j, std::string & s){
        s.resize(j - i + 1);
        size_t p = 0;
        expand_interval_qgram(_g.m_tree.root(), make_pair(i, j), s, p);
    }
    virtual void display_qgram_rec(const std::size_t& i, const std::size_t& j, std::string & s){
        s.resize(j - i + 1);
        size_t p = 0;s.resize(j - i + 1);
        expand_interval_qgram_rec(_g.m_tree.root(), make_pair(i, j), s, p);
    }
    void expand_interval_qgram_rec(const size_t &node, const std::pair<size_t, size_t> &range, std::string &s, std::size_t &pos){


        auto llb = _g.rank_L(range.first) + _g.L[range.first]; //left leaf begin
        auto pllb = _g.select_L(llb); // position of left leaf begin


        auto lle = _g.rank_L(range.second) + _g.L[range.second]; //left leaf end
        auto plle = _g.select_L(lle); // position of left leaf end


        /* If the range only expands a unique leaf */
        int added_ch = 0;

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

            expand_interval_qgram_rec(node_def, l_range, s, pos);
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

                /* compute new range */
                size_t pnllb = _g.select_L(llb + 1); // position of left + 1 leaf begin

                size_t off = pnllb - range.first;
                size_t p_f = pos;

                dfs_expand_suffix2(X,s,pos+off,pos);

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
//                auto off = _g.select_L(t + 1) - _g.select_L(t) + pos;
                expand_interval_qgram_rec(_g.m_tree.leafselect(t), make_pair(_g.select_L(t),_g.select_L(t + 1)-1),s,pos);
//                dfs_expand_prefix2(_g[_g.m_tree.pre_order(_g.m_tree.leafselect(t))], s, off, pos);
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

                auto off = range.second - plle + 1;
                dfs_expand_prefix2(X,s,pos+off,pos);
            }
        }

    }
    void expand_interval_qgram(const size_t &node, const std::pair<size_t, size_t> &range, std::string &s, std::size_t &pos){

        auto llb = _g.rank_L(range.first) + _g.L[range.first]; //left leaf begin
        auto pllb = _g.select_L(llb); // position of left leaf begin


        auto lle = _g.rank_L(range.second) + _g.L[range.second]; //left leaf end
        auto plle = _g.select_L(lle); // position of left leaf end


        /* If the range only expands a unique leaf */
        int added_ch = 0;

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

            expand_interval_qgram(node_def, l_range, s, pos);
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

                dfs_expand_suffix2(X,s,off,pos);

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
                dfs_expand_prefix2(_g[_g.m_tree.pre_order(_g.m_tree.leafselect(t))], s, off, pos);
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

                dfs_expand_prefix2(X,s,pos+off,pos);

                ///expand_interval(node_def, l_range, s, pos);
            }
        }

    }
    virtual int match_suffix(const uint &X, std::string::iterator &itera,std::string::iterator &end){
        uint off = 0, pos = 0;

        uint node = _g.m_tree[_g.select_occ(X, 1)];

        auto limit = _g.limits_rule(node);

        int r = dfs_cmp_suffix_aux(node,X,limit,itera,end,off,pos);

//        if(r == 0 && itera != end) return 1;

//        if(r == 0 && itera == end && pos < limit.second-limit.first+1) return -1;

        return r;
    }
    virtual int match_prefix(const uint &X, std::string::iterator &itera,std::string::iterator &end){

        uint off = 0, pos = 0;

        uint node = _g.m_tree[_g.select_occ(X, 1)];

        auto limit = _g.limits_rule(node);

        int r = dfs_cmp_prefix_aux(node,X,limit,itera,end,off,pos);

//        if(r == 0 && itera != end) return 1;

//        if(r == 0 && itera == end && pos < limit.second-limit.first+1) return -1;

        return r;

    }
};

#endif //IMPROVED_GRAMMAR_INDEX_SELFGRAMMARINDEXBSQ_H
