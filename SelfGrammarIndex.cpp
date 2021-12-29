//
// Created by inspironXV on 8/16/2018.
//

#include <sdsl/lcp_bitcompressed.hpp>
#include <sdsl/rmq_succinct_sada.hpp>
#include "SelfGrammarIndex.h"

#define _MAX_PROOF 1000

//std::fstream frules("rules_to_extract",std::ios::out|std::ios::binary);

using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

void SelfGrammarIndex::build(const std::string& text
#ifdef MEM_MONITOR
        , mem_monitor& mm
#endif
)
{
    grammar not_compressed_grammar;
    std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx;
    build_basics(text,not_compressed_grammar,grammar_sfx
#ifdef MEM_MONITOR
            , mm
#endif
                         );

}
void SelfGrammarIndex::save(std::fstream &fstream1) {

    _g.save(fstream1);
    grid.save(fstream1);
}

void SelfGrammarIndex::load(std::fstream &fin) {

    _g.load(fin);
    grid.load(fin);


}

bool SelfGrammarIndex::expand_prefix(const grammar_representation::g_long & X, std::string & s, const size_t & l,size_t & pos )const
{
    const auto& Tg = _g.get_parser_tree();

    std::deque<std::pair<uint,std::pair<uint,uint>>> Q;

    uint node  = Tg[_g.select_occ(X,1)];

    uint current_leaf = Tg.leafrank(node);
    uint last_leaf = current_leaf+Tg.leafnum(node)-1;
    //Q.emplace_front(node,std::make_pair(current_leaf,last_leaf));
    while(current_leaf <= last_leaf)
    {
        // current leaf is a Terminal rule?
        auto X_i = _g[Tg.pre_order(Tg.leafselect(current_leaf))];
        if(_g.isTerminal(X_i))
        {
            unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
            s[pos]= a_th;
            if(l == ++pos) return true;
            current_leaf++;
            while(current_leaf > last_leaf && !Q.empty()){
                current_leaf = Q.front().second.first+1;
                last_leaf = Q.front().second.second;
                Q.pop_front();
            }
        }
        else
        {
            //Save actua state;
            Q.emplace_front(std::make_pair(node,std::make_pair(current_leaf,last_leaf)));
            // Jump to first occ
            auto node_first_occ = Tg[_g.select_occ(X_i,1)];
            current_leaf = Tg.leafrank(node_first_occ);
            last_leaf = current_leaf+Tg.leafnum(node_first_occ)-1;
        }
    }
    return true;
}

bool SelfGrammarIndex::expand_suffix(const grammar_representation::g_long  &X_i, std::string &s, const size_t & l,size_t & pos ) const
{
    const auto& Tg = _g.get_parser_tree();

    std::deque<std::pair<uint,std::pair<uint,uint>>> Q;

    uint node  = Tg[_g.select_occ(X_i,1)];

    uint first_leaf = Tg.leafrank(node);

    long int  current_leaf= first_leaf+Tg.leafnum(node)-1;

    //Q.emplace_front(node,std::make_pair(current_leaf,last_leaf));

    while(current_leaf >= first_leaf )
    {
        // current leaf is a Terminal rule?
        auto X = _g[Tg.pre_order(Tg.leafselect(current_leaf))];

        if(_g.isTerminal(X))
        {

            unsigned char a_th = _g.terminal_simbol(X); // a_th symbol in the sorted alphabet
            s[pos] = a_th;
            if(l == ++pos)
                return true;
            --current_leaf;

            while(current_leaf < first_leaf && !Q.empty()){
                current_leaf = Q.front().second.first-1;
                first_leaf = Q.front().second.second;
                Q.pop_front();
            }
        }
        else
        {
            //Save actua state;
            Q.emplace_front(std::make_pair(node,std::make_pair(current_leaf,first_leaf)));
            // Jump to first occ
            auto node_first_occ = Tg[_g.select_occ(X,1)];
            first_leaf= Tg.leafrank(node_first_occ);
            current_leaf = first_leaf+Tg.leafnum(node_first_occ)-1;
        }
    }

    return true;

}

void SelfGrammarIndex::find_second_occ(long int & offset, unsigned int & node, std::vector<uint> & occ) const{
;
    const auto& Tg = _g.get_parser_tree();
    std::deque<std::pair< uint, long int >> S;

    {
        size_t pre = Tg.pre_order(node);
        size_t Xi = _g[pre];
        size_t n_s_occ = _g.n_occ(Xi);
        for (size_t i = 1; i <= n_s_occ; ++i)
        {
            size_t pre_parent = _g.select_occ(Xi,i);
            S.emplace_back(pre_parent,offset);
        }
    }

    ////uint steps = 0;
    while(!S.empty())
    {
        ///std::pair<size_t , long int > front_pair = S.front();

        if(S.front().first == 1)
        {
            occ.push_back((uint)(S.front().second));
        }
        else
        {
            auto _node = Tg[S.front().first];
            size_t parent = Tg.parent(_node);
            size_t pre_parent = Tg.pre_order(parent);
            size_t Xi = _g[pre_parent];
            size_t n_s_occ = _g.n_occ(Xi);
            long int p_offset = S.front().second + _g.offsetText(_node) - _g.offsetText(parent);
            for (size_t i = 1; i <= n_s_occ; ++i)
            {
                size_t i_pre_parent;
                i_pre_parent = _g.select_occ(Xi, i);
                S.emplace_back(i_pre_parent,p_offset);
            }
        }

        S.pop_front();
    }

}

void SelfGrammarIndex::display(const std::size_t & i, const std::size_t & j, std::string & str) {

    const auto& Tg = _g.get_parser_tree();
    long long int p = i;

    std::stack<std::pair<size_t ,size_t >> s_path;
    dfuds::dfuds_tree::dfuds_long root = Tg.root();

    s_path.push(make_pair(0,Tg.leafnum(root)+1));
    // auto start = timer::now();

    auto current_node = root;
    bool notend = true;

    while(notend)
    {

        dfuds::dfuds_tree::dfuds_long ls = Tg.leafrank(current_node);
        dfuds::dfuds_tree::dfuds_long hs = ls + Tg.leafnum(current_node)-1;
        dfuds::dfuds_tree::dfuds_long  l = Tg.find_child(current_node,ls,hs,[&p,&notend,this](const dfuds::dfuds_tree::dfuds_long &child)->bool{

            //size_t pos_m = _g.offsetText(child);
            size_t pos_m = _g.select_L(child);

            if(pos_m == p)  notend = false;

            return p < pos_m;
            return true;

        });



        current_node = Tg.leafselect(l);
        if(notend)
        {
            auto X_j = _g[Tg.pre_order(current_node)];
            p -= (long long int)_g.select_L(l);
            auto occ_p = _g.select_occ(X_j,1);
            current_node = Tg[occ_p];
            size_t l2_r = Tg.leafrank(current_node);

            p += (long long int)_g.select_L(l2_r);
            s_path.push(make_pair(l,l2_r + Tg.leafnum(current_node)));
        }



    }

    auto off = (long long int)(j-i+1);
    str.resize(off);
    size_t pos = 0;
    expand_prefix(_g[Tg.pre_order(current_node)],str,(size_t)off,pos);
    size_t current_leaf = Tg.leafrank(current_node)+1;

    while(!s_path.empty() && pos < off  ){

        if(current_leaf == s_path.top().second )
        {
            if(s_path.top().first == 0)
                break;

            current_leaf = s_path.top().first+1;
            s_path.pop();
        }else{
            expand_prefix(_g[Tg.pre_order(Tg.leafselect(current_leaf))],str,(size_t)off,pos);
            ++current_leaf;
        }
    }

}

size_t SelfGrammarIndex::size_in_bytes() const {
//    std::cout<<"SelfGrammarIndex::size_in_bytes()\n";
    return _g.size_in_bytes() + grid.size_in_bytes();
}

int SelfGrammarIndex::cmp_suffix_grammar(const size_t & sfx, std::string::iterator & iterator1, std::string::iterator & iterator2)
{

    if(iterator1 == iterator2)
    {
        return 1;
    }


    const auto& parser_tree = _g.get_parser_tree();

    size_t sfx_preorder = grid.first_label_col(sfx);
    size_t X = _g[sfx_preorder];

    auto r = dfs_cmp_prefix(X,iterator1,iterator2);

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
            r = dfs_cmp_prefix(Y,iterator1,iterator2);
            child_r ++;
        }

        if(r == 0 && child_r > n && iterator1 != iterator2) return  1; // the sufix is prefix of the string
    }

    return r;
}


void SelfGrammarIndex::build_basics(const std::string & text, fstream& suffixes, fstream& basics, fstream& repair_g
#ifdef MEM_MONITOR
                                    ,mem_monitor &mm
#endif
) {
    grammar not_compressed_grammar;
    std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > > grammar_sfx;

    build_basics(text,not_compressed_grammar,grammar_sfx
#ifdef MEM_MONITOR
    ,mm
#endif
    );
    unsigned long num_sfx = grammar_sfx.size();

    if(!basics.is_open()){ std::cout<<"ERROR OPENING BASIC FILE"<<std::endl;}
    if(!suffixes.is_open()){ std::cout<<"ERROR OPENING SUFFIX FILE"<<std::endl;}
    if(!repair_g.is_open()){ std::cout<<"ERROR OPENING GRAMMAR FILE"<<std::endl;}

    sdsl::serialize(num_sfx,suffixes);


    for (auto && e : grammar_sfx ) {

        size_t p = e.first.first;
        sdsl::serialize(p,suffixes);
        p = e.first.second;
        sdsl::serialize(p,suffixes);
        p = e.second.first;
        sdsl::serialize(p,suffixes);
        p = e.second.second;
        sdsl::serialize(p,suffixes);

    }

    save(basics);

    not_compressed_grammar.save(repair_g);


}
void SelfGrammarIndex::load_basics(fstream & f) {
    _g.load(f);
    grid.load(f);
}


void SelfGrammarIndex::build_basics(
        const std::string & text,
        grammar &not_compressed_grammar,
        std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > >& grammar_sfx
#ifdef MEM_MONITOR
        ,  mem_monitor& mm
#endif
){


    /*
 * Building grammar by repair algorithm
 *
 * */

#ifdef PRINT_LOGS
    std::cout<<BUILD_CFG_GRAMMAR<<std::endl;
#endif
#ifdef MEM_MONITOR
    auto start = timer::now();
    mm.event(BUILD_CFG_GRAMMAR);
#endif
    not_compressed_grammar.buildRepair(text
#ifdef MEM_MONITOR
            ,mm
#endif
    );
#ifdef MEM_MONITOR
    auto stop = timer::now();
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR] = duration_cast<microseconds>(stop-start).count();

#endif
    /*
     * Building compressed grammar
     *
    * */

#ifdef PRINT_LOGS
    std::cout<<BUILD_COMPRESSED_GRAMMAR<<std::endl;
#endif
#ifdef MEM_MONITOR
    start = timer::now();
    mm.event(BUILD_COMPRESSED_GRAMMAR);
#endif
    _g.code = code;
    _g.build(not_compressed_grammar
#ifdef MEM_MONITOR
            , mm
#endif
    );
#ifdef MEM_MONITOR
    stop = timer::now();
    CLogger::GetLogger()->model[BUILD_COMPRESSED_GRAMMAR] = duration_cast<microseconds>(stop-start).count();;
#endif
    /*
     * Build sufix of grammar
     *
     * */

#ifdef PRINT_LOGS
    std::cout<<BUILD_COMPUTE_GRAMMAR_SFX<<std::endl;
#endif
#ifdef MEM_MONITOR
    mm.event(BUILD_COMPUTE_GRAMMAR_SFX);
    start = timer::now();
#endif

    if(!grammar_sfx.empty()) grammar_sfx.clear();
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
#ifdef MEM_MONITOR
    stop = timer::now();
    CLogger::GetLogger()->model[BUILD_COMPUTE_GRAMMAR_SFX] = duration_cast<microseconds>(stop-start).count();;
#endif

    /*
     * Sorting suffixes
     *
     * */

#ifdef PRINT_LOGS
    std::cout<<BUILD_COMPUTE_SORT_EDA_SA_LCP_RMQ<<std::endl;
#endif
#ifdef MEM_MONITOR
    mm.event(BUILD_COMPUTE_SORT_EDA_SA_LCP_RMQ);
    start = timer::now();
#endif

//    sdsl::int_vector<> SA_(text.size(),0);
//    sdsl::algorithm::calculate_sa( (unsigned char*)text.c_str(),text.size(),SA_);
//    sdsl::inv_perm_support<> SA_1(&SA);
//    sdsl::lcp_wt<> LCP;
//    sdsl::construct_im(LCP,text.c_str(),sizeof(char));
//    sdsl::rmq_succinct_sada<true,sdsl::bp_support_sada<>> rmq(&LCP);

    sdsl::int_vector<> SA;
    sdsl::int_vector<> SA_1;
    sdsl::lcp_bitcompressed<> LCP;
    sdsl::rmq_succinct_sada<> rmq;

    uint32_t text_size = text.size();
    sdsl::cache_config config(false, ".", "cache-normal");
    sdsl::store_to_file((const char *)text.c_str(), sdsl::conf::KEY_TEXT);
    sdsl::register_cache_file(sdsl::conf::KEY_TEXT, config);

    sdsl::construct(LCP, sdsl::conf::KEY_TEXT, config, 1);
//    std::cout<<"TEXT.size()"<<text.size()<<std::endl;
//    std::cout<<"LCP.size()"<<LCP.size()<<std::endl;
//    for (uint32_t i = 0; i < LCP.size(); i++) {
//         cout << "LCP[i] = " << LCP[i] << endl;
//    }

    if (sdsl::cache_file_exists(sdsl::conf::KEY_SA, config)) {
        sdsl::load_from_cache(SA, sdsl::conf::KEY_SA, config);
        SA_1 = SA;
        for (uint32_t i = 0; i < SA.size(); i++) {
//            cout << "SA[i] = " << SA[i] << endl;
            SA_1[SA[i]] = i;
        }

        sdsl::util::clear(SA);
    } else {
        std::cout<<"ERROR CACHE NO ENCONTRADA"<<std::endl;

    }
//    std::cout<<"SA_1.size()"<<SA_1.size()<<std::endl;


    // Builds the RMQ Support.
    rmq = sdsl::rmq_succinct_sada<>(&LCP);
//    std::cout<<"RMQ.size()"<<rmq.size()<<std::endl;


    sdsl::remove(sdsl::cache_file_name(sdsl::conf::KEY_SA, config));
    sdsl::remove(sdsl::cache_file_name(sdsl::conf::KEY_TEXT, config));
    sdsl::remove(sdsl::cache_file_name(sdsl::conf::KEY_LCP, config));



#ifdef MEM_MONITOR
    stop = timer::now();
    CLogger::GetLogger()->model[BUILD_COMPUTE_SORT_EDA_SA_LCP_RMQ] = duration_cast<microseconds>(stop-start).count();;
#endif

#ifdef PRINT_LOGS
    std::cout<<BUILD_SORT_GRAMMAR_SFX<<std::endl;
#endif
#ifdef MEM_MONITOR
    start = timer::now();
    mm.event(BUILD_SORT_GRAMMAR_SFX);
#endif
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
//
//                  auto sa_1_a = SA_1[a_pos];
//                  auto sa_1_b = SA_1[b_pos];
//
//                  int min= LCP[rmq(std::min(sa_1_a,sa_1_b)+2,std::max(sa_1_a,sa_1_b)+1)];
//                  /*
//                   * Check if one is prefix of the other
//                   * return the shortest
//                   * */
//
//                  if(std::min(size_a,size_b) <= min){
//                      return size_a < size_b;
//                  }
//
//                  /*
//                   * then return the lowest lexicographical
//                   * */
//
//                  return sa_1_a < sa_1_b;
//
    //
                      uint32_t _rmq;
                      if (SA_1[a_pos] < SA_1[b_pos]) {
                          _rmq = rmq(SA_1[a_pos] + 1, SA_1[b_pos]);
                      } else {
                          _rmq = rmq(SA_1[b_pos] + 1, SA_1[a_pos]);
                      }
                      if (size_a <= LCP[_rmq] && size_b <= LCP[_rmq]) {
                          return size_a < size_b;
                      } else if (size_a <= LCP[_rmq]) {
                          return true;
                      } else if (size_b <= LCP[_rmq]) {
                          return false;
                      } else {
                          /***
                           * Neither is a prefix of the other. Use ISA to find
                           *the order
                           ***/
                          return SA_1[a_pos] < SA_1[b_pos];
                      }



              });


#ifdef MEM_MONITOR
    stop = timer::now();
    CLogger::GetLogger()->model[BUILD_SORT_GRAMMAR_SFX] = duration_cast<microseconds>(stop-start).count();;
#endif

#ifdef PRINT_LOGS
    std::cout<<BUILD_GRID<<std::endl;
#endif
#ifdef MEM_MONITOR
    start = timer::now();
    mm.event(BUILD_GRID);
#endif

    std::vector<std::pair<std::pair<range_search2d::bin_long ,range_search2d::bin_long >,range_search2d::bin_long>> grid_points(num_sfx);
    size_t bpair = 0;
    for (auto && s  :grammar_sfx ) {
        grid_points[bpair] = make_pair(make_pair(s.second.first,bpair+1),s.second.second);
        ++bpair;
    }


    grid.code = code;
    grid.build(grid_points.begin(),grid_points.end(),not_compressed_grammar.n_rules(),num_sfx);
#ifdef MEM_MONITOR
    stop = timer::now();
    CLogger::GetLogger()->model[BUILD_GRID] = duration_cast<microseconds>(stop-start).count();;
#endif


}


