//
// Created by inspironXV on 8/16/2018.
//

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
void SelfGrammarIndex::expand_grammar_sfx(const size_t & sfx, std::string & s, const size_t & l) const
{


    if(l == 0)
    {
        s = "";
        return;
    }
    s.resize(l);

    const auto& parser_tree = _g.get_parser_tree();

    size_t sfx_preorder = grid.first_label_col(sfx);
    size_t X = _g[sfx_preorder];
    size_t pos = 0;
    expand_prefix(X,s,l,pos);



    if(pos < l){

        auto node = parser_tree[sfx_preorder];
        auto parent = parser_tree.parent(node);
        auto child_r = parser_tree.childrank(node);
        auto n  = parser_tree.children(parent);
        child_r ++;

        while(pos < l && child_r <= n ){
            //std::string s1;
            size_t r = parser_tree.child(parent,child_r);
            r = parser_tree.pre_order(r);
            r = _g[r];
            expand_prefix(r,s,l,pos);
            child_r ++;
        }

    }

    s[pos] = '\0';
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

void SelfGrammarIndex::find_second_occ(long int & offset, unsigned int & node, sdsl::bit_vector & occ) const{

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
            occ[S.front().second ] = true;
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
                size_t i_pre_parent = _g.select_occ(Xi,i);
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

void SelfGrammarIndex::display_trie(const std::size_t & i , const std::size_t & j, std::string & str) {

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

    long long int off = j-i+1;
    str.resize(off);
    size_t pos = 0;
  //  uint t_X =(uint)_g[Tg.pre_order(current_node)];
  //  frules << t_X <<"\n";
    bp_expand_prefix(_g[Tg.pre_order(current_node)],str,(size_t)off,pos);

    size_t current_leaf = Tg.leafrank(current_node)+1;

    while(!s_path.empty() && pos < off  ){

        if(current_leaf == s_path.top().second )
        {
            if(s_path.top().first == 0)
                break;

            current_leaf = s_path.top().first+1;
            s_path.pop();
        }else{


//            uint t_X2 =(uint)_g[Tg.pre_order(Tg.leafselect(current_leaf))];
//            frules << t_X2 <<"\n";


            bp_expand_prefix(_g[Tg.pre_order(Tg.leafselect(current_leaf))],str,(size_t)off,pos);
            ++current_leaf;
        }
    }

}

size_t SelfGrammarIndex::size_in_bytes() const {
//    std::cout<<"SelfGrammarIndex::size_in_bytes()\n";
    return _g.size_in_bytes() + grid.size_in_bytes();
}

int SelfGrammarIndex::cmp_prefix(const compressed_grammar::g_long & X_i, std::string::iterator & itera, std::string::iterator & end) const{

    const auto& Tg = _g.get_parser_tree();

    std::deque<std::pair<uint,std::pair<uint,uint>>> Q;

    uint node  = Tg[_g.select_occ(X_i,1)];

    uint current_leaf = Tg.leafrank(node);
    uint last_leaf = current_leaf+Tg.leafnum(node)-1;

    //Q.emplace_front(node,std::make_pair(current_leaf,last_leaf));

    while(current_leaf <= last_leaf)
    {
        // current leaf is a Terminal rule?
        auto X = _g[Tg.pre_order(Tg.leafselect(current_leaf))];

        if(_g.isTerminal(X))
        {
            unsigned char a_th = _g.terminal_simbol(X); // a_th symbol in the sorted alphabet
            if(a_th < (unsigned char)(*itera)) return 1;
            if(a_th > (unsigned char)(*itera)) return -1;
            ++itera;
            if(itera == end) return 0;
            current_leaf++;

            while(current_leaf > last_leaf && !Q.empty())
            {
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
            auto node_first_occ = Tg[_g.select_occ(X,1)];
            current_leaf = Tg.leafrank(node_first_occ);
            last_leaf = current_leaf+Tg.leafnum(node_first_occ)-1;
        }
    }
    return 0;

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

int SelfGrammarIndex::cmp_suffix(const compressed_grammar::g_long & X_i, std::string::iterator & itera, std::string::iterator & end) const {

    assert(X_i > 0 && X_i < _g.n_rules());



    const auto& Tg = _g.get_parser_tree();

    std::deque<std::pair<uint,std::pair<uint,uint>>> Q;

    uint node  = Tg[_g.select_occ(X_i,1)];

    uint current_leaf = Tg.leafrank(node);
    uint last_leaf = current_leaf+Tg.leafnum(node)-1;
    swap(current_leaf,last_leaf);

    //Q.emplace_front(node,std::make_pair(current_leaf,last_leaf));

    while(current_leaf >= last_leaf)
    {
        // current leaf is a Terminal rule?
        auto X = _g[Tg.pre_order(Tg.leafselect(current_leaf))];

        if(_g.isTerminal(X))
        {
            unsigned char a_th = _g.terminal_simbol(X); // a_th symbol in the sorted alphabet
            if(a_th < (unsigned char)(*itera)) return  1;
            if(a_th > (unsigned char)(*itera)) return -1;
            --itera;
            if(itera == end-1) return 0;
            --current_leaf;
            while(current_leaf < last_leaf && !Q.empty()){
                current_leaf = Q.front().second.first-1;
                last_leaf = Q.front().second.second;
                Q.pop_front();
            }

        }
        else
        {
            //Save actua state;
            Q.emplace_front(std::make_pair(node,std::make_pair(current_leaf,last_leaf)));
            // Jump to first occ
            auto node_first_occ = Tg[_g.select_occ(X,1)];
            current_leaf = Tg.leafrank(node_first_occ);
            last_leaf = current_leaf+Tg.leafnum(node_first_occ)-1;
            swap(current_leaf,last_leaf);
        }
    }
    return 0;

}

bool SelfGrammarIndex::bp_expand_prefix(const compressed_grammar::g_long &X_i,std::string & s, const size_t & l,size_t & pos) const
{
   //assert(X_i > 0 && X_i < _g.n_rules());

    if(_g.isTerminal(X_i))
    {
        unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
        s[pos]= a_th;
        return l == ++pos;

    }


    const auto& Tg = _g.get_parser_tree();
    const auto& tree = _g.get_left_trie();
    const auto& bp_rep = tree.get_tree();
    const auto& seq = tree.get_seq();
    const auto& inv_seq = tree.get_seq_inv();


    /*
    * Find the left most path for X_i in left_trie
    *
    * */


    /*
     * Finding the preorder of the node with tag X_i in left most path trie
     * */
    size_t  pre_tlmp_xi = inv_seq[X_i] + 1;
    /*
     * Finding the node with preorder pre_tlmp_xi in left most path trie
     * */
    size_t  node_xi_left_path = bp_rep[pre_tlmp_xi];

    /*
     * Finding child of the root that is ancestor of node_xi_left_path
     * */

    int depht_node_xi = (int)bp_rep.depth(node_xi_left_path);
    --depht_node_xi;
    auto current  = bp_rep.levelancestor(node_xi_left_path,--depht_node_xi);
   /*
    * root's child ancestor
    * */

    size_t  X_a = seq[bp_rep.pre_order(current)-1];

    if(_g.isTerminal(X_a)){

        unsigned char a_th = _g.terminal_simbol(X_a); // a_th symbol in the sorted alphabet
        s[pos]= a_th;

        if(l == ++pos){
            return true;
        }
    }


    while(depht_node_xi > 0)
    {
        auto ancestor_l_1  = bp_rep.levelancestor(node_xi_left_path,depht_node_xi-1);

        size_t  preoreder_trie = bp_rep.pre_order(ancestor_l_1);
        size_t  X = seq[preoreder_trie-1];
        size_t  preorder_parser_tree = _g.select_occ(X,1);

        size_t  u = Tg[preorder_parser_tree];



        size_t l_rp = Tg.leafrank(u);
        size_t n_limit = l_rp + Tg.leafnum(u)-1;

        size_t v = Tg.fchild(u);
        size_t lnum_s_ch = Tg.leafnum(v);

        for(size_t i = l_rp + lnum_s_ch; i <= n_limit; ++i)
        {
            size_t leaf_t = Tg.leafselect(i);

            if( bp_expand_prefix(_g[Tg.pre_order(leaf_t)],s,l,pos) )
            {
                return true;
            }
        }



        /*size_t  n_ch = Tg.children(u);

        ///size_t  begin = (top == current)?1:2;
        for (size_t  i = 2 ; i <= n_ch ; ++i) {
            size_t  ch = Tg.child(u,i);

            if( bp_expand_prefix(_g[Tg.pre_order(ch)],s,l,pos) )
            {
                return true;
            }
        }*/

        --depht_node_xi;
    }

    return false;
}

bool SelfGrammarIndex::bp_expand_suffix(const compressed_grammar::g_long &X_i,std::string & s, const size_t & l,size_t & pos) const
{
//    assert(X_i > 0 && X_i < _g.n_rules());

    if(_g.isTerminal(X_i))
    {
        unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
        s[pos]= a_th;
        return l == ++pos;

    }

    const auto& Tg = _g.get_parser_tree();
    const auto& tree = _g.get_right_trie();
    const auto& bp_rep = tree.get_tree();
    const auto& seq = tree.get_seq();
    const auto& inv_seq = tree.get_seq_inv();
    /*
    * Find the left most path for X_i in left_trie
    *
    * */

    /*
     * Finding the preorder of the node with tag X_i in left most path trie
     * */
    size_t  pre_tlmp_xi = inv_seq[X_i] + 1;
    /*
     * Finding the node with preorder pre_tlmp_xi in left most path trie
     * */
    size_t  node_xi_left_path = bp_rep[pre_tlmp_xi];

    /*
     * Finding child of the root that is ancestor of node_xi_left_path
     * */

    int depht_node_xi = (int)bp_rep.depth(node_xi_left_path);
    --depht_node_xi;
    auto current  = bp_rep.levelancestor(node_xi_left_path,--depht_node_xi);
    /*
     * root's child ancestor
     * */

    size_t  X_a = seq[bp_rep.pre_order(current)-1];

    if(_g.isTerminal(X_a)){

        unsigned char a_th = _g.terminal_simbol(X_a); // a_th symbol in the sorted alphabet
        s[pos]= a_th;

        if(l == ++pos)
            return true;
    }


    while(depht_node_xi > 0)
    {
        auto ancestor_l_1  = bp_rep.levelancestor(node_xi_left_path,depht_node_xi-1);
        size_t  preoreder_trie = bp_rep.pre_order(ancestor_l_1);
        size_t  X = seq[preoreder_trie-1];
        size_t  preorder_parser_tree = _g.select_occ(X,1);

        size_t  u = Tg[preorder_parser_tree];
        size_t  n_ch = Tg.children(u);

        ///size_t  begin = (top == current)?1:2;
        for (size_t  i = n_ch-1 ; i > 0 ; --i) {
            size_t  ch = Tg.child(u,i);

            if( bp_expand_suffix(_g[Tg.pre_order(ch)],s,l,pos) )
            {
                return true;
            }
        }

        --depht_node_xi;
    }


    return false;

}

int
SelfGrammarIndex::bp_cmp_prefix(const compressed_grammar::g_long & X_i, std::string::iterator & itera, std::string::iterator & end) const {
//    assert(X_i > 0 && X_i < _g.n_rules());

    if(_g.isTerminal(X_i))
    {
        unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
        if(a_th < (unsigned char)(*itera)) return 1;
        if(a_th > (unsigned char)(*itera)) return  -1;
        ++itera;
        ///if(itera == end) return 0;
        return 0;
    }


    const auto& Tg = _g.get_parser_tree();
    const auto& tree = _g.get_left_trie();
    const auto& bp_rep = tree.get_tree();
    const auto& seq = tree.get_seq();
    const auto& inv_seq = tree.get_seq_inv();

    /*
    * Find the left most path for X_i in left_trie
    *
    * */


    /*
     * Finding the preorder of the node with tag X_i in left most path trie
     * */
    size_t  pre_tlmp_xi = inv_seq[X_i] + 1;
    /*
     * Finding the node with preorder pre_tlmp_xi in left most path trie
     * */
    size_t  node_xi_left_path = bp_rep[pre_tlmp_xi];

    /*
     * Finding child of the root that is ancestor of node_xi_left_path
     * */

    int depht_node_xi = (int)bp_rep.depth(node_xi_left_path);
    --depht_node_xi;
    size_t current  = bp_rep.levelancestor(node_xi_left_path,--depht_node_xi);
    /*
     * root's child ancestor
     * */

    size_t  X_a = seq[bp_rep.pre_order(current)-1];

    if(_g.isTerminal(X_a)){

        unsigned char a_th = _g.terminal_simbol(X_a); // a_th symbol in the sorted alphabet

        if(a_th < (unsigned char)(*itera)) return 1;
        if(a_th > (unsigned char)(*itera)) return -1;
        ++itera;
        if(itera == end) return 0;
    }

    while(depht_node_xi > 0)
    {
        size_t ancestor_l_1  = bp_rep.levelancestor(node_xi_left_path,depht_node_xi-1);
        size_t  preoreder_trie = bp_rep.pre_order(ancestor_l_1);
        size_t  X = seq[preoreder_trie-1];
        size_t  preorder_parser_tree = _g.select_occ(X,1);

        size_t  u = Tg[preorder_parser_tree];
        size_t  n_ch = Tg.children(u);

        for (size_t  i = 2 ; i <= n_ch ; ++i) {
            size_t  ch = Tg.child(u,i);

            int r = bp_cmp_prefix(_g[Tg.pre_order(ch)],itera,end);

            if( r != 0 || itera == end)
                return r;
        }

        --depht_node_xi;
    }


    return 0;
}

int
SelfGrammarIndex::bp_cmp_suffix(const compressed_grammar::g_long & X_i, std::string::iterator & itera, std::string::iterator & end) const{

    assert(X_i > 0 && X_i < _g.n_rules());

    if(_g.isTerminal(X_i))
    {
        unsigned char a_th = _g.terminal_simbol(X_i); // a_th symbol in the sorted alphabet
        if(a_th < (unsigned char)(*itera)) return 1;
        if(a_th > (unsigned char)(*itera)) return  -1;
        --itera;
        return 0;
        ////if(itera == end) return 0;

    }

    const auto &Tg = _g.get_parser_tree();
    const auto &tree = _g.get_right_trie();
    const auto &bp_rep = tree.get_tree();
    const auto &seq = tree.get_seq();
    const auto &inv_seq = tree.get_seq_inv();
    /*
    * Find the left most path for X_i in left_trie
    *
    * */

    /*
     * Finding the preorder of the node with tag X_i in left most path trie
     * */
    size_t  pre_tlmp_xi = inv_seq[X_i] + 1;
    /*
     * Finding the node with preorder pre_tlmp_xi in left most path trie
     * */
    size_t  node_xi_left_path = bp_rep[pre_tlmp_xi];

    /*
     * Finding child of the root that is ancestor of node_xi_left_path
     * */

    int depht_node_xi = (int)bp_rep.depth(node_xi_left_path);
    --depht_node_xi;
    size_t  current  = bp_rep.levelancestor(node_xi_left_path,--depht_node_xi);
    /*
     * root's child ancestor
     * */

    size_t  X_a = seq[bp_rep.pre_order(current)-1];

    if(_g.isTerminal(X_a)){

        unsigned char a_th = _g.terminal_simbol(X_a); // a_th symbol in the sorted alphabet

        if(a_th < (unsigned char)(*itera)) return 1;
        if(a_th > (unsigned char)(*itera)) return  -1;
        --itera;
            if(itera == end-1) return 0;
    }



    while(depht_node_xi > 0)
    {
        size_t  ancestor_l_1  = bp_rep.levelancestor(node_xi_left_path,depht_node_xi-1);
        size_t  preoreder_trie = bp_rep.pre_order(ancestor_l_1);
        size_t  X = seq[preoreder_trie-1];
        size_t  preorder_parser_tree = _g.select_occ(X,1);

        size_t  u = Tg[preorder_parser_tree];
        size_t  n_ch = Tg.children(u);

        ///size_t  begin = (top == current)?1:2;
        for (size_t  i = n_ch-1 ; i > 0 ; --i) {
            size_t  ch = Tg.child(u,i);

            int r = bp_cmp_suffix(_g[Tg.pre_order(ch)],itera,end);

            if( r != 0 || itera == end-1 )
                return r;
        }

        --depht_node_xi;
    }


    return 0;

}

int SelfGrammarIndex::bp_cmp_suffix_grammar(const size_t & sfx, std::string::iterator & iterator1, std::string::iterator & iterator2) const {

    if(iterator1==iterator2) return 1;

    const auto& parser_tree = _g.get_parser_tree();

    size_t sfx_preorder = grid.first_label_col(sfx);
    size_t X = _g[sfx_preorder];

    int r = bp_cmp_prefix(X,iterator1,iterator2);

    if(r == 0)
    {
        size_t node = parser_tree[sfx_preorder];
        size_t parent = parser_tree.parent(node);
        auto child_r = parser_tree.childrank(node);
        auto n  = parser_tree.children(parent);
        child_r ++;
        while(r == 0 && iterator1!=iterator2 && child_r <= n ){
            size_t rnode = parser_tree.child(parent,child_r);
            rnode = parser_tree.pre_order(rnode);
            auto Y = _g[rnode];
            r = bp_cmp_prefix(Y,iterator1,iterator2);
            child_r ++;
        }
        if(r == 0 && child_r > n && iterator1 != iterator2) return  1; // the sufix is prefix of the string



//        size_t  last_child = parser_tree.lchild(parent);
//        size_t  child_n = parser_tree.nsibling(node);
//
//        while(r == 0 && iterator1!=iterator2 && child_n <= last_child ){
//            size_t rnode = parser_tree.pre_order(child_n);
//            size_t Y = _g[rnode];
//            r = bp_cmp_prefix(Y,iterator1,iterator2);
//
//            if(child_n != last_child)
//                child_n = parser_tree.nsibling(child_n);
//            else
//                child_n = last_child+1;
//        }
//
//        if(r == 0 && child_n > last_child && iterator1 != iterator2) return  1; // the sufix is prefix of the string



    }

    return r;
}

void SelfGrammarIndex::locate_ch(const char & ch, std::vector<uint> & occ) const{

    const auto& alp = _g.get_alp();

    auto p = std::find(alp.begin(),alp.end(),ch);

    if(p == alp.end()) return;

    unsigned int rk = p - alp.begin()+1;
    auto X_a = _g.terminal_rule(rk);

    const auto& Tg = _g.get_parser_tree();
    size_t n_s_occ = _g.n_occ(X_a);
    for (size_t i = 1; i <= n_s_occ; ++i)
    {
        size_t node_occ_pre = _g.select_occ(X_a,i);
        size_t current = Tg[node_occ_pre];
        auto current_parent = Tg.parent(current);
        long int p_offset = _g.offsetText(current) - _g.offsetText(current_parent) ;
        find_second_occ(p_offset,current_parent,occ);

    }
}

void SelfGrammarIndex::locate_ch(const char & ch, sdsl::bit_vector & occ) const {

    const auto& alp = _g.get_alp();

    auto p = std::find(alp.begin(),alp.end(),ch);

    if(p == alp.end()) return;

    unsigned int rk = p - alp.begin()+1;
    auto X_a = _g.terminal_rule(rk);

    const auto& Tg = _g.get_parser_tree();
    size_t n_s_occ = _g.n_occ(X_a);
    for (size_t i = 1; i <= n_s_occ; ++i)
    {
        size_t node_occ_pre = _g.select_occ(X_a,i);
        size_t current = Tg[node_occ_pre];
        auto current_parent = Tg.parent(current);
        long int p_offset = _g.offsetText(current) - _g.offsetText(current_parent) ;
        find_second_occ(p_offset,current_parent,occ);

    }
}

bool SelfGrammarIndex::expand_prefix2(const size_t & node, std::string & s, const size_t & l, size_t &pos) const {

    const auto& Tg = _g.get_parser_tree();

    std::deque<std::pair<uint,std::pair<uint,uint>>> Q;



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

            if(current_leaf > last_leaf && !Q.empty())
            {
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

void SelfGrammarIndex::build_basics_bal(const std::string & text, fstream& R, fstream&C, fstream& suffixes, fstream& basics, fstream& repair_g
#ifdef MEM_MONITOR
        ,mem_monitor& mm
#endif
) {

    /*
* Building grammar by repair algorithm
*
* */
    std::cout<<"Building grammar by repair algorithm (SGI)"<<std::endl;


    grammar not_compressed_grammar;
    not_compressed_grammar.buildBalRepair(text,R,C
#ifdef MEM_MONITOR
            ,mm
#endif
    );


    std::cout<<"\t number of rules "<<not_compressed_grammar.n_rules()<<std::endl;
    std::cout<<"\t total size of rules "<<not_compressed_grammar.get_size()<<std::endl;
    //std::cout<<"\t size of the representation "<<not_compressed_grammar.size_in_bytes()*1/(1024*1024)<<"(mb)"<<std::endl;

    //  not_compressed_grammar.print(text);


    /*
     * Building compressed grammar
     *
    * */
    std::cout<<"Building compressed grammar"<<std::endl;

    ///not_compressed_grammar.print(text);
    _g.code = code;
    _g.build(not_compressed_grammar
#ifdef MEM_MONITOR
            ,mm
#endif
    );
    //  _g.print_size_in_bytes();
    std::cout<<"total size of compressed grammar*******************"<<_g.size_in_bytes()*1.0/1024/1024<<std::endl;
    /*
     * Build sufix of grammar
     *
     * */
    std::cout<<"Compute suffix grammar"<<std::endl;

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
    std::cout<<"\t number of suffixes "<<num_sfx<<std::endl;

    /*
     * Sorting suffixes
     *
     * */

    std::cout<<"Building structures for sorting"<<std::endl;
//    sdsl::int_vector<> SA(text.size(),0);
//    sdsl::algorithm::calculate_sa( (unsigned char*)text.c_str(),text.size(),SA);
//    sdsl::inv_perm_support<> SA_1(&SA);
//    sdsl::lcp_wt<> LCP;
//    sdsl::construct_im(LCP,text.c_str(),sizeof(char));
//    sdsl::rmq_succinct_sada<true,sdsl::bp_support_sada<>> rmq(&LCP);

    sdsl::int_vector<> SA;
    sdsl::int_vector<> SA_1;
    sdsl::lcp_bitcompressed<> LCP;
    sdsl::rmq_succinct_sada<> rmq;

    // Computes the text reverse
    uint32_t text_size = text.length();

    auto *rev_text = new unsigned char[text_size + 1];
    for (uint32_t i = 0; i < text_size; i++) {
        rev_text[i] = text[text_size - i - 1];
    }
    rev_text[text_size] = 0;
    sdsl::cache_config config(false, ".", "cache");
    sdsl::store_to_file((const char *)rev_text, sdsl::conf::KEY_TEXT);
    sdsl::register_cache_file(sdsl::conf::KEY_TEXT, config);

    sdsl::construct(LCP, sdsl::conf::KEY_TEXT, config, 1);
    for (uint32_t i = 0; i < LCP.size(); i++) {
        // cout << "LCP[i] = " << m_lcp[i] << endl;
    }

    if (sdsl::cache_file_exists(sdsl::conf::KEY_SA, config)) {
        sdsl::load_from_cache(SA, sdsl::conf::KEY_SA, config);
        SA_1 = SA;

        for (uint32_t  i = 0; i < SA.size(); i++) {
            // cout << "SA[i] = " << m_SA[i] << endl;
            SA_1[SA[i]] = i;
        }
        sdsl::util::clear(SA);
    }

    // Builds the RMQ Support.
    rmq = sdsl::rmq_succinct_sada<>(&LCP);

    sdsl::remove(sdsl::cache_file_name(sdsl::conf::KEY_SA, config));
    sdsl::remove(sdsl::cache_file_name(sdsl::conf::KEY_TEXT, config));
    sdsl::remove(sdsl::cache_file_name(sdsl::conf::KEY_LCP, config));


    std::cout<<"sorting suffixes ........."<<std::endl;
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


    std::cout<<"end sorting suffixes "<<std::endl;

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

    std::cout<<"Building grid for 2d range search first occ"<<std::endl;

    std::vector<std::pair<std::pair<range_search2d::bin_long ,range_search2d::bin_long >,range_search2d::bin_long>> grid_points(num_sfx);
    size_t bpair = 0;
    for (auto && s  :grammar_sfx ) {
        grid_points[bpair] = make_pair(make_pair(s.second.first,bpair+1),s.second.second);
        ++bpair;
    }
    grid.code = code;
    grid.build(grid_points.begin(),grid_points.end(),not_compressed_grammar.n_rules(),num_sfx);
    //std::cout<<"***************************grid size "<<grid.size_in_bytes()*1.0/1024/1024<<std::endl;
    ///grid.print_size();
    ////grid.print();

    /*
     * Building Sampled Patricia Trees for suffixes.
     *
     * Sampling by log(u)log(log(n))/log(n)
     * where n is the number of symbols in the repair grammar and u is the length of the original text
     * */

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

void SelfGrammarIndex::find_second_occ_rec(long int off, unsigned int & prenode, std::vector<uint> & occ) const {

    size_t Xi = _g[prenode];
    size_t n_s_occ = _g.n_occ(Xi);
    find_second_occ_rec_aux(off,prenode,occ);
    for (size_t i = 2; i <= n_s_occ; ++i)
    {
        uint pre_node_i = _g.select_occ(Xi,i);
        find_second_occ_rec_aux(off,pre_node_i,occ);
    }

}

void SelfGrammarIndex::find_second_occ_rec_aux(long int off, unsigned int & prenode, std::vector<uint> & occ) const {

    if(prenode == 1)
    {
        occ.emplace_back(off);
        return;
    }

    const auto& Tg = _g.get_parser_tree();

    uint node = Tg[prenode];

    uint parent = Tg.parent(node);

    uint pre_parent = Tg.pre_order(parent);

    size_t Xi = _g[pre_parent];

    size_t n_s_occ = _g.n_occ(Xi);

    long int p_offset = off + _g.offsetText(node) - _g.offsetText(parent);

    find_second_occ_rec(p_offset,pre_parent,occ);

    for (size_t i = 2; i <= n_s_occ; ++i)
    {
        uint pre_parent_i = _g.select_occ(Xi,i);
        find_second_occ_rec(p_offset,pre_parent_i,occ);
    }
}

unsigned long SelfGrammarIndex::size_basics_in_bytes() const {
    return size_in_bytes();
}

unsigned long SelfGrammarIndex::size_tries_in_bytes() const {
    return _g.get_compact_trie_left_size() + _g.get_compact_trie_right_size();
}

bool SelfGrammarIndex::expand_prefix_slp(const compressed_grammar::g_long & X, std::string & s, const size_t & l,
                                         size_t &pos) const {



    uint pnode = _g.select_occ(X,1);
    uint node = _g.m_tree[pnode];
    uint len = _g.len_rule(node);
    rule_trav rt(X,len,node,_MAX_PROOF);

    while(true){

        if(rt.len_rule[rt.level] > 1){
            //la regla actual no es terminal
            rt.label_last_processed[rt.level] = _g.label_i_child(rt.rules[rt.level].node,++(rt.last_processed[rt.level]));


            rt.level++;

            rt.rules[rt.level].id = rt.label_last_processed[rt.level-1];

            if(!_g.isTerminal(rt.rules[rt.level].id)){
                rt.rules[rt.level].node = _g.m_tree[_g.select_occ(rt.rules[rt.level].id,1)];
                rt.len_rule[rt.level] = _g.len_rule(rt.rules[rt.level].node);
            }
        }

        else{
            s[rt.read] = _g.terminal_simbol(rt.rules[rt.level].id);
            rt.read++;
            if(rt.read == l) break;

            do{
                rt.last_processed[rt.level] = 0;
                rt.len_rule[rt.level] = 0;
                rt.label_last_processed[rt.level] = 0;
                rt.level--;

            }while(rt.last_processed[rt.level] == rt.len_rule[rt.level]);
        }
    }

    return false;
}
bool SelfGrammarIndex::expand_suffix_slp(const compressed_grammar::g_long & X, std::string & s, const size_t & l,
                                         size_t &pos) const {



    uint pnode = _g.select_occ(X,1);
    uint node = _g.m_tree[pnode];
    //std::pair<uint,uint> limits = _g.limits_rule(node);
    uint len = _g.len_rule(node);
    rule_trav rt(X,len,node,_MAX_PROOF);
    rt.last_processed[rt.level] = rt.len_rule[rt.level]+1;

    while(true){

        if(rt.len_rule[rt.level] > 1){//la regla actual no es terminal
            rt.label_last_processed[rt.level] = _g.label_i_child(rt.rules[rt.level].node,--rt.last_processed[rt.level]);
            rt.level++;
            rt.rules[rt.level].id = rt.label_last_processed[rt.level-1];
            if(!_g.isTerminal(rt.rules[rt.level].id )){

                rt.rules[rt.level].node = _g.m_tree[_g.select_occ(rt.rules[rt.level].id,1)];
                rt.len_rule[rt.level] = _g.len_rule(rt.rules[rt.level].node);
                rt.last_processed[rt.level] = rt.len_rule[rt.level]+1;
            }

        }

        else{
            s[rt.read] = _g.terminal_simbol(rt.rules[rt.level].id);
            rt.read++;
            if(rt.read == l) break;

            do{
                rt.last_processed[rt.level] = 0;
                rt.len_rule[rt.level] = 0;
                rt.label_last_processed[rt.level] = 0;
                rt.level--;

            }while(rt.last_processed[rt.level] == 1);
        }
    }

    return false;
}

void SelfGrammarIndex::build_bitvector_occ(sdsl::bit_vector &B) const
{

    size_t size_gr = _g.m_tree.subtree(_g.m_tree.root());

    size_t n_cols = grid.n_columns();


    sdsl::bit_vector T(size_gr*n_cols,0);


    for (int i = 1; i <= n_cols ; ++i) {

        uint preorder_node = grid.first_label_col(i);
        uint begin = i*n_cols;

        track_occ(preorder_node,T,begin);
    }
    B.resize(T.size());
    std::copy(T.begin(),T.end(),B.begin());


}

void SelfGrammarIndex::track_occ(uint &pnode, sdsl::bit_vector& B, const uint& begin) const{

    if(pnode == 1)
        return;

    uint pos_node = _g.m_tree[pnode];

    uint parent = _g.m_tree.parent(pos_node);

    uint preorder_parent = _g.m_tree.pre_order(parent);

    if(_g.m_tree.isleaf(pos_node) == 1)
        B[ begin + preorder_parent - 1 ] = true;

    uint label = _g[preorder_parent];

    uint n_occ = _g.n_occ(label);

    for (int i = 2; i <= n_occ ; ++i)
    {
        uint p = _g.select_occ(label,i);
        track_occ(p,B,begin);
    }

    track_occ(preorder_parent,B,begin);
}

void SelfGrammarIndex::build_basics(
        const std::string & text,
        fstream& suffixes,
        fstream& basics,
        fstream& repair_g,
        grammar &not_compressed_grammar,
        std::vector< std::pair< std::pair<size_t ,size_t >,std::pair<size_t ,size_t > > >& grammar_sfx
#ifdef MEM_MONITOR
        , mem_monitor& mm
#endif
        ) {
    build_basics(text,not_compressed_grammar,grammar_sfx
#ifdef MEM_MONITOR
            ,mm
#endif
    );

    if(!basics.is_open()){ std::cout<<"ERROR OPENING BASIC FILE"<<std::endl;}
    if(!suffixes.is_open()){ std::cout<<"ERROR OPENING SUFFIX FILE"<<std::endl;}
    if(!repair_g.is_open()){ std::cout<<"ERROR OPENING GRAMMAR FILE"<<std::endl;}
    unsigned long num_sfx = grammar_sfx.size();

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


uint64_t SelfGrammarIndex::estimate_size_of_ed_graph(const uint64_t& len, const uint64_t& n_missmatch) {
    auto editD = [](const std::string & s1, const std::string & s2){
        if(s1.size() != s2.size()) return -1L;
        long D = 0L;
        for (uint64_t i = 0; i <s1.size() ; ++i) {
            if(s1[i] != s2[i])
                ++D;
        }
        return D;
    };

    std::vector<std::string> p_occ;
    uint64_t sfx = grid.n_columns();
    {
        sdsl::int_vector<> off (sfx,0);
        sdsl::int_vector<> off_parent (sfx,0);
        sdsl::int_vector<> end_parent (sfx,0);

        for (uint64_t i = 0; i < sfx ; ++i) {
            // compute de start position of the suffix and store the position in off...
            uint64_t preorder_node = grid.first_label_col(i+1);
            auto node = _g.m_tree[preorder_node];
            auto lf = _g.m_tree.leafrank(node);
            off[i] = _g.select_L(lf);
            auto parent = _g.m_tree.parent(node);
            auto pre_parent = _g.m_tree.pre_order(parent);
            off_parent[i] = _g.offsetText(pre_parent);
            if(lf + 1 == sfx)
                end_parent[i] = _g.get_size_text();
            else
                end_parent[i] = (_g.select_L(lf+1)) - 1;
        }
        for(uint64_t i = 0; i < sfx; i++)
        {
            //            extract substring S = T[off - len + 1 ,...,off + len - 1]
            std::string S;
            //            check do not analyze substring outside the primary occ (outside of the parent)
            uint64_t lw = off[i] - len < off_parent[i] ? off_parent[i]:off[i] - len ;
            uint64_t hi = off[i] + len > end_parent[i] ? end_parent[i]:off[i] + len - 1;
            display(lw,hi,S);
            //            and store it in pocc
            p_occ.push_back(S);
        }

    }
    std::set<std::tuple<uint64_t,uint64_t>> edges;
    //init matrix
//        M.resize(sfx);
//        for (uint64_t i = 0; i < sfx; ++i) {
//            M[i].resize(sfx);
//            for (uint64_t j = 0; j < sfx ; ++j) {
//                M[i][j] = 0;
//            }
//        }
    for(uint64_t i = 0; i < sfx; i++)
        for(uint64_t i2 = i+1; i2 < sfx; i++)
            for(long j = 0; j < len ; ++j) {
                std::string si;si.resize(len);
                std::copy(p_occ[i].begin()+j,p_occ[i].begin()+j+len,si.begin());
                for (long j2 = 0; j2 < len; ++j2) {
                    std::string si2;si2.resize(len);
                    std::copy(p_occ[i2].begin()+j2,p_occ[i2].begin()+j2+len,si2.begin());
                    // compute Edit distance between S[j ... j +off] y S[j2 ... j+off]
                    long D = editD(si,si2);
                    if(D <= n_missmatch){
                        //add point to the graph...
                        edges.insert(std::make_pair(i,i2));
//                            M[i][i2] = 1;
                    }
                }
            }
    p_occ.clear();
    std::vector<std::tuple<uint64_t,uint64_t>> G(edges.size());
    uint64_t i = 0;
    for (const auto &e : edges)
        G[i++] = e;
    edges.clear();
    //compute k2-tree
    sdsl::k2_tree<4> K(G,sfx);
//    return sdsl::size_in_bytes(K);
        return 0;
}



