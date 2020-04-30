//
// Created by alpachec on 19-08-18.
//

#include <sdsl/lcp_bitcompressed.hpp>
#include <sdsl/rmq_succinct_sada.hpp>
#include "SelfGrammarIndexBS.h"



using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

void SelfGrammarIndexBS::build(const std::string & text) {


    /*
 * Building grammar by repair algorithm
 *
 * */
    std::cout<<"Building grammar by repair algorithm"<<std::endl;


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

    std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx;
    const auto &gtree = _g.get_parser_tree();
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
    sdsl::int_vector<> SA(text.size(),0);
    sdsl::algorithm::calculate_sa( (unsigned char*)text.c_str(),text.size(),SA);
    sdsl::inv_perm_support<> SA_1(&SA);
    sdsl::lcp_wt<> LCP;
    sdsl::construct_im(LCP,text.c_str(),sizeof(char));
    sdsl::rmq_succinct_sada<true,sdsl::bp_support_sada<>> rmq(&LCP);

//    std::cout<<"sorting suffixes ........."<<std::endl;
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

    std::vector<std::pair<std::pair<range_search2d::bin_long ,range_search2d::bin_long >,range_search2d::bin_long>> grid_points(num_sfx);
    size_t bpair = 0;
    for (auto && s  :grammar_sfx ) {
        grid_points[bpair] = make_pair(make_pair(s.second.first,bpair+1),s.second.second);
        ++bpair;
    }


    grid.code = code;
    grid.build(grid_points.begin(),grid_points.end(),not_compressed_grammar.n_rules(),num_sfx);
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



}
void SelfGrammarIndexBS::locate2( std::string & pattern, sdsl::bit_vector & occ){

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }


    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();

    const auto& g_tree = _g.get_parser_tree();
    size_t nnodes = g_tree.subtree(g_tree.root()) + 1;
    std::vector<bool> mark(nnodes,0);

    /*
     *
     * partitioning the pattern
     *
     * */
    for (size_t  i = 1; i <= p_n ; ++i)
    {
      /*      std::string p1,p2;
            p1.resize(i);
            p2.resize(p_n-i);

            std::copy(pattern.begin(),pattern.begin()+i,p1.begin());
            std::copy(pattern.begin()+i,pattern.end(),p2.begin());
            std::reverse(p1.begin(),p1.end());

        std::cout<<"p1:"<<p1<<std::endl;
        std::cout<<"p2:"<<p2<<std::endl;*/

        ///std::cout<<"************************************************************"<<std::endl;
        auto itera = pattern.begin() + i-1;
       ////// auto start = timer::now();

        grammar_representation::g_long lr = 1,hr = n_xj;
      ////  uint c = 0;
      ///  std::cout<<"[l,r] "<<lr<<" "<<hr<<std::endl;

        if(
                !lower_bound(lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {
                    auto begin = pattern.begin();
                    auto end = itera;

                    //auto r =  bp_cmp_suffix(a,end,begin);
                    auto r =  cmp_suffix(a,end,begin);
                    if(r == 0 && end != begin-1) return 1;
                    return r;


/*
                    std::string sufx;
                    sufx.resize(n_s);
                    size_t pos = 0;
                    expand_suffix(a,sufx,n_s,pos);

                    auto cmp_it = itera-1;

                    for (int j = 0; j < sufx.size() ; ++j) {
                        if(*cmp_it < sufx[j]) return -1;
                        if(*cmp_it > sufx[j]) return 1;
                        --cmp_it;
                    }
                    return 0;


                    */

                })	
        )continue;
        grammar_representation::g_long r1 = lr;
	
///c =0;
        lr = 1,hr = n_xj;
       //// std::cout<<"[l,r] "<<lr<<" "<<hr<<std::endl;

        if(
                !upper_bound(lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {
                    auto begin = pattern.begin();
                    auto end = itera;

                    //auto r =  bp_cmp_suffix(a,end,begin);
                    auto r = cmp_suffix(a,end,begin);
                    if(r == 0 && end != begin-1) return 1;

                    return r;
                 /*   size_t  n_s = itera - pattern.begin();
                    std::string sufx;
                    sufx.resize(n_s);
                    size_t pos = 0;
                    expand_suffix(a,sufx,n_s,pos);

                    auto cmp_it = itera-1;

                    for (int j = 0; j < sufx.size() ; ++j) {
                        if(*cmp_it < sufx[j]) return -1;
                        if(*cmp_it > sufx[j]) return 1;
                        --cmp_it;
                    }
                    return 0;*/
                })
         ) continue;
        grammar_representation::g_long r2 = hr;

     ////   std::cout<<"c->"<<c<<std::endl;
///c =0;
        grammar_representation::g_long ls = 1,hs = n_sj;
    ////    std::cout<<"[l,r] "<<ls<<" "<<hs<<std::endl;
        if(
                !lower_bound(ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {
          ////          c++;
                    auto end = pattern.end();
                    auto it2 = itera+1;
                    auto r = cmp_suffix_grammar(a,it2,end);
                    return r;
                  /*  size_t  n_s = pattern.end()-itera ;
                    std::string sufx;
                    sufx.resize(n_s);
                    expand_grammar_sfx(a,sufx,n_s);

                    auto cmp_it = itera;

                    for (int j = 0; j < sufx.size() ; ++j) {
                        if(*cmp_it < sufx[j]) return -1;
                        if(*cmp_it > sufx[j]) return 1;
                        ++cmp_it;
                    }
                    return 0;


                    */
                })
            )
        continue;
        grammar_representation::g_long c1 = ls;
       //// std::cout<<"c->"<<c<<std::endl;
        ls = 1,hs = n_sj;
        ////std::cout<<"[l,r] "<<ls<<" "<<hs<<std::endl;
      ////   c =0 ;

        if(
                !upper_bound(ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {

                    auto end = pattern.end();
                    auto it2 = itera+1;
                    //auto r =  bp_cmp_suffix_grammar(a,it2,end);
                    auto r =  cmp_suffix_grammar(a,it2,end);
                    return r;

                   /*size_t  n_s = pattern.end()-itera;
                    std::string sufx;
                    sufx.resize(n_s);
                    expand_grammar_sfx(a,sufx,n_s);

                    auto cmp_it = itera;

                    for (int j = 0; j < sufx.size() ; ++j) {
                        if(*cmp_it < sufx[j]) return -1;
                        if(*cmp_it > sufx[j]) return 1;
                        ++cmp_it;
                    }
                    return 0;
                    */
                })
            )
            continue;
        grammar_representation::g_long c2 = hs;
        ///std::cout<<"c->"<<c<<std::endl;
        /////auto stop = timer::now();
        ////unsigned long tttt = duration_cast<microseconds>(stop-start).count();
        //////std::cout<<"\tbinaries searches"<<std::endl;

        //////start = timer::now();
        std::vector< std::pair<size_t,size_t> > pairs;
        grid.range2(r1,r2,c1,c2,pairs);
        ////const auto& g_tree = _g.get_parser_tree();
        long len = itera-pattern.begin() +1;
        ///////std::cout<<"range founded "<<pairs.size()<<std::endl;
        for (auto &pair : pairs) {
            ////size_t p = grid.labels(pair.first, pair.second);
            size_t p = grid.first_label_col(pair.second);
            size_t pos_p = _g.offsetText(g_tree[p]);
            unsigned int parent = g_tree.parent(g_tree[p]);
            long int  l = (- len + pos_p) - _g.offsetText(parent);
            ////start = timer::now();
            if(mark[p] == false)
            {
                mark[p] = true;
                find_second_occ(l,parent,occ);
            }
            //// stop = timer::now();
            ///std::cout<<"\t\tsearch_grammar_tree_second_occ time:" <<duration_cast<nanoseconds>(stop-start).count()<<"(ns)"<<std::endl;
            //// std::cout<<"find_second_occ "<<std::endl;
            ////stop = timer::now();
            /////tttt = duration_cast<microseconds>(stop-start).count();
            ////std::cout<<"\t\tfind_second_occ"<<tttt<<std::endl;
        }

       //// stop = timer::now();
        ////////tttt = duration_cast<microseconds>(stop-start).count();
        ///////std::cout<<"\tfind_second_occ"<<tttt<<std::endl;
    }


}
void SelfGrammarIndexBS::locate( std::string & pattern, sdsl::bit_vector & occ){

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }


    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();
    /*
     *
     * partitioning the pattern
     *
     * */
    for (size_t  i = 1; i <= p_n ; ++i)
    {


       /* std::string p1,p2;
        p1.resize(i);
        p2.resize(p_n-i);

        std::copy(pattern.begin(),pattern.begin()+i,p1.begin());
        std::copy(pattern.begin()+i,pattern.end(),p2.begin());
        std::reverse(p1.begin(),p1.end());*/

        ///std::cout<<"************************************************************"<<std::endl;
        auto itera = pattern.begin() + i-1;
        ////// auto start = timer::now();

        grammar_representation::g_long lr = 1,hr = n_xj;
        ////  uint c = 0;
        ///  std::cout<<"[l,r] "<<lr<<" "<<hr<<std::endl;

        if(
                !lower_bound(lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {
                    ////        c++;
                    ///size_t  n_s = itera - pattern.begin();

                    auto begin = pattern.begin();
                    auto end = itera;
                    ///auto start = timer::now();

                    auto r =  bp_cmp_suffix(a,end,begin);
                    ///auto stop = timer::now();
                    ///auto tttt = duration_cast<microseconds>(stop-start).count();
                    ///std::cout<<"\t\t\t\tb1"<<tttt<<std::endl;
                    if(r == 0 && end != begin-1) return 1;
                    return r;


/*
                    std::string sufx;
                    sufx.resize(n_s);
                    size_t pos = 0;
                    expand_suffix(a,sufx,n_s,pos);

                    auto cmp_it = itera-1;

                    for (int j = 0; j < sufx.size() ; ++j) {
                        if(*cmp_it < sufx[j]) return -1;
                        if(*cmp_it > sufx[j]) return 1;
                        --cmp_it;
                    }
                    return 0;


                    */

                })
                )continue;
        grammar_representation::g_long r1 = lr;

        ///    std::cout<<"c->"<<c<<std::endl;
///c =0;
        lr = 1,hr = n_xj;
        //// std::cout<<"[l,r] "<<lr<<" "<<hr<<std::endl;

        if(
                !upper_bound(lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {
                    ///       c++;
                    auto begin = pattern.begin();
                    auto end = itera;


                    ///auto start = timer::now();
                    auto r =  bp_cmp_suffix(a,end,begin);
                    ///auto stop = timer::now();
                    ///auto tttt = duration_cast<microseconds>(stop-start).count();
                    ///std::cout<<"\t\t\t\tb2"<<tttt<<std::endl;
                    if(r == 0 && end != begin-1) return 1;

                    return r;
                    /*   size_t  n_s = itera - pattern.begin();
                       std::string sufx;
                       sufx.resize(n_s);
                       size_t pos = 0;
                       expand_suffix(a,sufx,n_s,pos);

                       auto cmp_it = itera-1;

                       for (int j = 0; j < sufx.size() ; ++j) {
                           if(*cmp_it < sufx[j]) return -1;
                           if(*cmp_it > sufx[j]) return 1;
                           --cmp_it;
                       }
                       return 0;*/
                })
                ) continue;
        grammar_representation::g_long r2 = hr;
        ////   std::cout<<"c->"<<c<<std::endl;
///c =0;
        grammar_representation::g_long ls = 1,hs = n_sj;
        ////    std::cout<<"[l,r] "<<ls<<" "<<hs<<std::endl;
        if(
                !lower_bound(ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {
                    ////          c++;
                    auto end = pattern.end();
                    auto it2 = itera+1;


                    ///auto start = timer::now();
                    auto r = bp_cmp_suffix_grammar(a,it2,end);
                    /// auto stop = timer::now();
                    /// auto tttt = duration_cast<microseconds>(stop-start).count();
                    ////  std::cout<<"\t\t\t\tb1"<<tttt<<std::endl;

                    return r;
                    /*  size_t  n_s = pattern.end()-itera ;
                      std::string sufx;
                      sufx.resize(n_s);
                      expand_grammar_sfx(a,sufx,n_s);

                      auto cmp_it = itera;

                      for (int j = 0; j < sufx.size() ; ++j) {
                          if(*cmp_it < sufx[j]) return -1;
                          if(*cmp_it > sufx[j]) return 1;
                          ++cmp_it;
                      }
                      return 0;


                      */
                })
                )
            continue;
        grammar_representation::g_long c1 = ls;
        //// std::cout<<"c->"<<c<<std::endl;
        ls = 1,hs = n_sj;
        ////std::cout<<"[l,r] "<<ls<<" "<<hs<<std::endl;
        ////   c =0 ;

        if(
                !upper_bound(ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
                {
                    /////            c++;

                    auto end = pattern.end();
                    auto it2 = itera+1;


                    ///auto start = timer::now();
                    auto r =  bp_cmp_suffix_grammar(a,it2,end);
                    ///auto stop = timer::now();
                    ///auto tttt = duration_cast<microseconds>(stop-start).count();
                    // //std::cout<<"\t\t\t\tb1"<<tttt<<std::endl;


                    return r;

                    /*size_t  n_s = pattern.end()-itera;
                     std::string sufx;
                     sufx.resize(n_s);
                     expand_grammar_sfx(a,sufx,n_s);

                     auto cmp_it = itera;

                     for (int j = 0; j < sufx.size() ; ++j) {
                         if(*cmp_it < sufx[j]) return -1;
                         if(*cmp_it > sufx[j]) return 1;
                         ++cmp_it;
                     }
                     return 0;
                     */
                })
                )
            continue;
        grammar_representation::g_long c2 = hs;
        ///std::cout<<"c->"<<c<<std::endl;
        /////auto stop = timer::now();

        ////unsigned long tttt = duration_cast<microseconds>(stop-start).count();
        /////std::cout<<"\tbinaries searches :"<<tttt<<std::endl;


        //////start = timer::now();
        std::vector< std::pair<size_t,size_t> > pairs;
        grid.range2(r1,r2,c1,c2,pairs);

        const auto& g_tree = _g.get_parser_tree();


        long len = itera-pattern.begin() +1;

        ///////std::cout<<"range founded "<<pairs.size()<<std::endl;

        for (auto &pair : pairs) {


            //size_t p = grid.labels(pair.first, pair.second);
            size_t p = grid.first_label_col(pair.second);
            size_t pos_p = _g.offsetText(g_tree[p]);
            unsigned int parent = g_tree.parent(g_tree[p]);
            long int  l = (- len + pos_p) - _g.offsetText(parent);


            ///start = timer::now();
            find_second_occ(l,parent,occ);
            //// stop = timer::now();
            ///std::cout<<"\t\tsearch_grammar_tree_second_occ time:" <<duration_cast<nanoseconds>(stop-start).count()<<"(ns)"<<std::endl;

            //// std::cout<<"find_second_occ "<<std::endl;

            ////stop = timer::now();
            /////tttt = duration_cast<microseconds>(stop-start).count();
            ////std::cout<<"\t\tfind_second_occ"<<tttt<<std::endl;


        }

        //// stop = timer::now();
        ////////tttt = duration_cast<microseconds>(stop-start).count();
        ///////std::cout<<"\tfind_second_occ"<<tttt<<std::endl;
    }


}
void SelfGrammarIndexBS::locate( std::string & pattern, std::vector<uint> & occ){

    if(pattern.size() == 1)
    {
        locate_ch(pattern[0],occ);
        return;
    }


    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();
    /*
     *
     * partitioning the pattern
     *
     * */
    for (size_t  i = 1; i <= p_n ; ++i)
    {


        auto itera = pattern.begin() + i-1;
        grammar_representation::g_long lr = 1,hr = n_xj;

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
        hr = n_xj;

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


        grammar_representation::g_long ls = 1,hs = n_sj;

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
        hs = n_sj;

        found = false;

        upper_bound(found,ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
        {
            auto end = pattern.end();
            auto it2 = itera+1;
            auto r =  bp_cmp_suffix_grammar(a,it2,end);
            return r;
        });

        if(!found)
            continue;


        binary_relation::bin_long c2 = hs;

        long len = itera-pattern.begin() +1;


        find_second_occ(r1,r2,c1,c2,len,occ);

//        const auto& g_tree = _g.get_parser_tree();
//        std::vector< std::pair<size_t,size_t> > pairs;
//        grid.range2(r1,r2,c1,c2,pairs);
//
//        long len = itera-pattern.begin() +1;
//
//        for (auto &pair : pairs) {
//            size_t p = grid.first_label_col(pair.second);
//            size_t pos_p = _g.offsetText(g_tree[p]);
//            unsigned int parent = g_tree.parent(g_tree[p]);
//            long int  l = (- len + pos_p) - _g.offsetText(parent);
//            find_second_occ(l,parent,occ);
//        }


    }


}
void SelfGrammarIndexBS::locateNoTrie( std::string & pattern, std::vector<uint> & occ){

//    if(pattern.size() == 1)
//    {
//        locate_ch(pattern[0],occ);
//        return;
//    }


    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();

    for (size_t  i = 1; i <= p_n ; ++i)
    {

        std::string::iterator itera = pattern.begin() + i-1;
        grammar_representation::g_long lr = 1,hr = n_xj;

        bool found = false;

        lower_bound(found,lr,hr,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
        {
            std::string::iterator begin = pattern.begin();
            std::string::iterator end = itera;
            auto r =  dfs_cmp_suffix(a,end,begin);
//            int r =1;
//            uint64_t t =_g.m_tree[_g.select_occ(a,1)];
//            int r = dfs_cmp_suffix_node( t,end,begin);

//            begin = pattern.begin();
//            end = itera;
//            auto r2 =  cmp_suffix(a,end,begin);
//            if(r!=r2)
//            {
//                begin = pattern.begin();
//                end = itera;
//                dfs_cmp_suffix(a,end,begin);
//            }
            if(r == 0 && end != begin-1) return 1;
            return r;
        });


        if(!found)
            continue;

        binary_relation::bin_long r1 = lr;
        hr = n_xj;

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


        grammar_representation::g_long ls = 1,hs = n_sj;

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
        hs = n_sj;

        found = false;

        upper_bound(found,ls,hs,[&itera,&pattern,this](const grammar_representation::g_long & a)->int
        {
            auto end = pattern.end();
            auto it2 = itera+1;
            auto r =  cmp_suffix_grammar(a,it2,end);
            return r;
        });

        if(!found)
            continue;



        binary_relation::bin_long c2 = hs;


        long len = itera-pattern.begin() +1;


        find_second_occ(r1,r2,c1,c2,len,occ);

//        const auto& g_tree = _g.get_parser_tree();
//        std::vector< std::pair<size_t,size_t> > pairs;
//        grid.range2(r1,r2,c1,c2,pairs);
//
//        long len = itera-pattern.begin() +1;
//
//        for (auto &pair : pairs) {
//            size_t p = grid.first_label_col(pair.second);
//            size_t pos_p = _g.offsetText(g_tree[p]);
//            unsigned int parent = g_tree.parent(g_tree[p]);
//            long int  l = (- len + pos_p) - _g.offsetText(parent);
//            find_second_occ(l,parent,occ);
//        }


    }


}
void SelfGrammarIndexBS::display(const std::size_t &i, const std::size_t &j, std::string &s) {
    SelfGrammarIndex::display( i,j,s);
}
void SelfGrammarIndexBS::display_trie(const std::size_t &i, const std::size_t &j, std::string &s) {
    SelfGrammarIndex::display_trie( i,j,s);
}
void SelfGrammarIndexBS::build(const SelfGrammarIndex::grammar_representation &G, const SelfGrammarIndex::range_search2d &R) {
    _g = G;
    grid = R;
}
size_t SelfGrammarIndexBS::size_in_bytes() const {
//    std::cout<<"SelfGrammarIndexBS::size_in_bytes()\n";
    return SelfGrammarIndex::size_in_bytes();
}
void SelfGrammarIndexBS::find_ranges_trie(std::string & pattern, std::vector<uint>&X) {

    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();

    for (size_t  i = 1; i <= p_n ; ++i) {

        auto itera = pattern.begin() + i - 1;
        grammar_representation::g_long lr = 1, hr = n_xj;

        bool found = false;
        lower_bound(found,lr, hr, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {

            auto begin = pattern.begin();
            auto end = itera;
            auto r = bp_cmp_suffix(a, end, begin);
            if (r == 0 && end != begin - 1) return 1;
            return r;
        });

        if (!found)
            continue;

//        binary_relation::bin_long r1 = lr;
//        hr = n_xj;
//        found = false;
//        upper_bound ( found,lr, hr, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
//
//            auto begin = pattern.begin();
//            auto end = itera;
//
//            auto r = bp_cmp_suffix(a, end, begin);
//
//            if (r == 0 && end != begin - 1) return 1;
//
//            return r;
//        });
//
//        if (!found)
//            continue;
//
//        binary_relation::bin_long r2 = hr;
//        grammar_representation::g_long ls = 1, hs = n_sj;
//
//        found  = false;
//        lower_bound(found,ls, hs, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
//            auto end = pattern.end();
//            auto it2 = itera + 1;
//            auto r = bp_cmp_suffix_grammar(a, it2, end);
//            return r;
//        });
//
//        if (!found)
//            continue;
//
//        binary_relation::bin_long c1 = ls;
//        hs = n_sj;
//
//        found = false;
//        upper_bound(found, ls, hs, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
//            auto end = pattern.end();
//            auto it2 = itera + 1;
//            auto r = bp_cmp_suffix_grammar(a, it2, end);
//            return r;
//        });
//
//
//        if (!found )
//            continue;
//
//        binary_relation::bin_long c2 = hs;
//
//        long len = itera-pattern.begin() +1;
   }
}
void SelfGrammarIndexBS::find_ranges_trie(std::string & pattern, std::vector<uint>&X, std::vector<range>& fr) {

    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();

    for (size_t  i = 1; i <= p_n ; ++i) {

        auto itera = pattern.begin() + i - 1;
        grammar_representation::g_long lr = 1, hr = n_xj;

        bool found = false;
//        lower_bound(found,lr, hr, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
//
//            auto begin = pattern.begin();
//            auto end = itera;
//            auto r = bp_cmp_suffix(a, end, begin);
//            if (r == 0 && end != begin - 1) return 1;
//            return r;
//        });
//
//        if (!found)
//            continue;
//
//        binary_relation::bin_long r1 = lr;
//        hr = n_xj;
//        found = false;
//        upper_bound ( found,lr, hr, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
//
//            auto begin = pattern.begin();
//            auto end = itera;
//
//            auto r = bp_cmp_suffix(a, end, begin);
//
//            if (r == 0 && end != begin - 1) return 1;
//
//            return r;
//        });
//
//        if (!found)
//            continue;
//
        binary_relation::bin_long r2 = hr;
        grammar_representation::g_long ls = 1, hs = n_sj;

        found  = false;
        lower_bound(found,ls, hs, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
            auto end = pattern.end();
            auto it2 = itera + 1;
            auto r = bp_cmp_suffix_grammar(a, it2, end);
            return r;
        });

        if (!found)
            continue;

        binary_relation::bin_long c1 = ls;
        hs = n_sj;

        found = false;
        upper_bound(found, ls, hs, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
            auto end = pattern.end();
            auto it2 = itera + 1;
            auto r = bp_cmp_suffix_grammar(a, it2, end);
            return r;
        });


        if (!found )
            continue;

        binary_relation::bin_long c2 = hs;

        long len = itera-pattern.begin() +1;

//        range r;
//
//        r.x1 = r1;
//        r.x2 = r2;
//        r.y1 = c1;
//        r.y2 = c2;
//        r.len = len;
//        fr.emplace_back(r);
    }
}
void SelfGrammarIndexBS::find_ranges(std::string &pattern, std::vector<uint> &X) {

    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();

    for (size_t  i = 1; i <= p_n ; ++i) {

        auto itera = pattern.begin() + i - 1;
        grammar_representation::g_long lr = 1, hr = n_xj;
//
//        if (
//                !lower_bound(lr, hr, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
//                    auto begin = pattern.begin();
//                    auto end = itera;
//                    auto r = bp_cmp_suffix(a, end, begin);
//                    if (r == 0 && end != begin - 1) return 1;
//                    return r;
//                })
//                )
            //continue;
//        binary_relation::bin_long r1 = lr;
//        lr = 1, hr = n_xj;
//        if (
//                !upper_bound(lr, hr, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
//                    auto begin = pattern.begin();
//                    auto end = itera;
//                    auto r = bp_cmp_suffix(a, end, begin);
//                    if (r == 0 && end != begin - 1) return 1;
//
//                    return r;
//                })
//                )
            //continue;
        binary_relation::bin_long r2 = hr;
        grammar_representation::g_long ls = 1, hs = n_sj;
        if (
                !lower_bound(ls, hs, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
                    auto end = pattern.end();
                    auto it2 = itera + 1;
                    auto r = bp_cmp_suffix_grammar(a, it2, end);
                    return r;
                })
                );
            //continue;
        binary_relation::bin_long c1 = ls;
        ls = 1, hs = n_sj;

        if (
                !upper_bound(ls, hs, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
                    auto end = pattern.end();
                    auto it2 = itera + 1;
                    auto r = bp_cmp_suffix_grammar(a, it2, end);
                    return r;
                })
                );
            //continue;


        binary_relation::bin_long c2 = hs;

    }

}
void SelfGrammarIndexBS::find_ranges_dfs(std::string & pattern, std::vector<uint>&X) {

    size_t p_n = pattern.size();
    size_t n_xj = _g.n_rules()-1;
    size_t n_sj = grid.n_columns();

    for (size_t  i = 1; i <= p_n ; ++i) {

        auto itera = pattern.begin() + i - 1;
        grammar_representation::g_long lr = 1, hr = n_xj;

        bool found = false;
        lower_bound(found,lr, hr, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {

            auto begin = pattern.begin();
            auto end = itera;
            auto r = dfs_cmp_suffix(a, end, begin);
            if (r == 0 && end != begin - 1) return 1;
            return r;
        });

        if (!found)
            continue;

//        binary_relation::bin_long r1 = lr;
//        hr = n_xj;
//        found = false;
//        upper_bound ( found,lr, hr, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
//
//            auto begin = pattern.begin();
//            auto end = itera;
//
//            auto r = bp_cmp_suffix(a, end, begin);
//
//            if (r == 0 && end != begin - 1) return 1;
//
//            return r;
//        });
//
//        if (!found)
//            continue;
//
//        binary_relation::bin_long r2 = hr;
//        grammar_representation::g_long ls = 1, hs = n_sj;
//
//        found  = false;
//        lower_bound(found,ls, hs, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
//            auto end = pattern.end();
//            auto it2 = itera + 1;
//            auto r = bp_cmp_suffix_grammar(a, it2, end);
//            return r;
//        });
//
//        if (!found)
//            continue;
//
//        binary_relation::bin_long c1 = ls;
//        hs = n_sj;
//
//        found = false;
//        upper_bound(found, ls, hs, [&itera, &pattern, this](const grammar_representation::g_long &a) -> int {
//            auto end = pattern.end();
//            auto it2 = itera + 1;
//            auto r = bp_cmp_suffix_grammar(a, it2, end);
//            return r;
//        });
//
//
//        if (!found )
//            continue;
//
//        binary_relation::bin_long c2 = hs;
//
//        long len = itera-pattern.begin() +1;
    }
}
