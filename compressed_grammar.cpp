//
// Created by inspironXV on 8/17/2018.
//

#include "compressed_grammar.h"

compressed_grammar::compressed_grammar() {

}

compressed_grammar::~compressed_grammar() {

}

compressed_grammar::g_long compressed_grammar::select_occ(const g_long & X, const g_long & j) const {
   //// assert(j > 0);
    ////return (j == 1 ? select1_Z(F[X]) : select0_Z(X_p.select(j - 1, X) + 1)) + 1;
    //auto a = rank1_Z(Z.size());
    if(j == 1 )
        return select1_Z(F[X])+1;


    return select0_Z( X_p.select(j - 1, X) + 1 ) + 1;

    ////return (j == 1 ? select1_Z(F[X]) : select0_Z(mX[make_pair((uint)X,(uint)j-1)] + 1)) + 1;
}

compressed_grammar::g_long compressed_grammar::operator[](const g_long & i)const {
   //// assert(i > 0 && i <= Z.size());


    /*
     * first occurrence
     *
     * */
    if(Z[i-1])
        return F_inv[rank1_Z(i)];
    /*
     * not first occurrence
     *
     * */
    return X_p[i-rank1_Z(i)-1];

}

compressed_grammar::g_long compressed_grammar::n_occ(const g_long & Xj)const {
    return X_p.rank(X_p.size(),Xj)+1;
}

void compressed_grammar::set_L(const compressed_grammar::l_vector & _L) {
    L = _L;
    select_L = compressed_grammar::l_vector::select_1_type(&L);
}

void compressed_grammar::set_Y(const compressed_grammar::y_vector & _Y) {
    Y = _Y;
    rank_Y = compressed_grammar::y_vector::rank_1_type(&Y);
}

void compressed_grammar::set_Z(const compressed_grammar::z_vector &_Z) {
    Z = _Z;
    rank1_Z     = z_vector::rank_1_type(&Z);
//    rank0_Z     = z_vector::rank_0_type(&Z);
    select1_Z   = z_vector::select_1_type(&Z);
    select0_Z   = z_vector::select_0_type(&Z) ;
}

void compressed_grammar::set_F(const compressed_grammar::compact_perm & _F) {
    F = _F;
    F_inv = inv_compact_perm(&F);
}

void compressed_grammar::set_X_p(const compressed_grammar::wavelet_tree &_Xp) {
    X_p = _Xp;
}

compressed_grammar::g_long compressed_grammar::offsetText(const g_long & node)const {

    return select_L(m_tree.leafrank(node));
}

bool compressed_grammar::isTerminal(const g_long & Xi) const{
    return Y[Xi];
}

void compressed_grammar::build(compressed_grammar::plain_grammar &grammar) {


    /*
     * Building bitvector for terminal rules
     *
     * */

    std::cout<<" * Building bitvector for terminal rules"<<std::endl;

    {
        sdsl::bit_vector _Y(grammar.n_rules() + 1,0);

        for (auto i = grammar.begin(); i != grammar.end(); ++i) {
            if (i->second.terminal)
                _Y[i->first] = true;
        }

        Y = sdsl::bit_vector(_Y);
        rank_Y = y_vector::rank_1_type(&Y);
        select_Y = y_vector::select_1_type(&Y);
    }

    /*
     * Building compressed grammar tree
     *
     * */
    std::cout<<" * Building compressed grammar tree"<<std::endl;

    {
        sdsl::bit_vector _L(grammar.text_size() + 1, 0);
        uint c_nodes = 0;
        std::set<uint> M;
        uint l_pos = 0;

        /*
         * Count the number of nodes of compact tree representation
         * Build bitvector L for marking the pos of every no terminal in text
         *
         * */
        std::cout << "* Count the number of nodes of compact tree representation\n";
//                " * Build bitvector L for marking the pos of every no terminal in text" << std::endl;

        grammar.dfs(grammar.get_initial_rule(), [&_L, &l_pos, &M, &c_nodes, this](rule &r) -> bool {

            uint id = r.id;
            if (M.find(id) != M.end()) {
                c_nodes++;
                _L[l_pos] = 1;
                l_pos += r.r - r.l + 1;
                return false;
            }
            M.insert(id);
            c_nodes++;
            if (r.terminal) {
                _L[l_pos] = 1;
                l_pos += r.r - r.l + 1;;
                return false;
            }
            return true;

        });

        L = l_vector(_L);
        select_L = l_vector::select_1_type(&L);
        rank_L = l_vector::rank_1_type(&L);
        /*
         * Building dfuds representation of parser tree
         * */

        std::cout << " * Building dfuds representation of parser tree" << std::endl;

        sdsl::bit_vector bv(2 * c_nodes - 1, 1);
        uint pos = 0;
        M.clear();

        /////std::string s;
        sdsl::bit_vector z(c_nodes, 0);
        uint i = 0, j = 1;
        sdsl::int_vector<> _F(grammar.n_rules() + 1, 0);

        std::ofstream xp_file("xp_file", std::ios::binary);
        sdsl::int_vector<> v_sq(c_nodes - grammar.n_rules());
        compressed_grammar::g_long vs_p = 0;

        grammar.dfs(grammar.get_initial_rule(), [this, &v_sq, &vs_p, &bv, &_F, &z, &M, &pos, &i, &j](const rule &r) -> bool
        {
            uint id = r.id;
            if (M.find(id) != M.end())
            {
                bv[pos] = false;
                pos++;

                i++;
                v_sq[vs_p] = id;
                vs_p++;
                return false;
            }

            z[i] = true;
            _F[id] = j;
             i++;
            j++;

            M.insert(id);

            if (r.terminal) {
                bv[pos] = false;
                pos++;
                return false;
            }

            uint n = r._rule.size();
            bv[pos + n] = false;
            pos += n + 1;
            return true;

        });


        m_tree.build(bv);
        std::cout<<"DFUDS TREE BUILDED\n";
        //m_tree.print();
        sdsl::util::bit_compress(v_sq);

        sdsl::serialize(v_sq, xp_file);
        xp_file.close();


        sdsl::construct(X_p, "xp_file", 0);

        /*sdsl::wt_gmr_rs<> X_p_gmr_rs;
        sdsl::wt_gmr<> X_p_gmr;
        sdsl::wt_ap<> X_p_ap_rs;

        sdsl::construct(X_p_gmr_rs, "xp_file", 0);
        sdsl::construct(X_p_gmr, "xp_file", 0);
        sdsl::construct(X_p_ap_rs, "xp_file", 0);

        fstream f_gmr_rs("../files/wts/"+std::to_string(code)+"_xt_file-gmr_rs",std::ios::out|std::ios::binary);
        fstream f_gmr("../files/wts/"   +std::to_string(code)+"_xt_file-gmr",std::ios::out|std::ios::binary);
        fstream f_ap("../files/wts/"    +std::to_string(code)+"_xt_file-ap",std::ios::out|std::ios::binary);

        sdsl::serialize(X_p_gmr_rs,f_gmr_rs);
        sdsl::serialize(X_p_gmr,f_gmr);
        sdsl::serialize(X_p_ap_rs,f_ap);*/


        /*{
            l_occ_xp = ls_occ_vector(v_sq.size(),0);
            std::set<uint> MM;

            for (size_t k = v_sq.size()-1; k >= 0 && MM.size() < X_p.sigma ; --k) {
                if(MM.find(v_sq[k]) == MM.end())
                {
                    l_occ_xp[k] = true;
                    MM.insert((uint)v_sq[k]);
                }

            }

        }*/





//        std::cout<<"X_p[k]"<<std::endl;
//        for (int k = 0; k < X_p.size() ; ++k) {
//            std::cout<<X_p[k]<<" ";
//        }
//        std::cout<<std::endl;


        F = compact_perm(_F);
  //////      std::cout<<std::endl;


        F_inv = inv_compact_perm(&F);
        sdsl::util::bit_compress(F);


//        std::cout<<"F[k]"<<std::endl;
//        for (int k = 0; k < F.size() ; ++k) {
//            std::cout<<F[k]<<" ";
//        }
//        std::cout<<std::endl;

//        std::cout<<"F_inv[k]"<<std::endl;
//        for (int k = 0; k < F_inv.size() ; ++k) {
//            std::cout<<F_inv[k]<<" ";
//        }
//        std::cout<<std::endl;


        Z = z;
        select1_Z = z_vector::select_1_type(&Z);
        select0_Z = z_vector::select_0_type(&Z);
//        rank0_Z  = z_vector::rank_0_type(&Z);
        rank1_Z  = z_vector::rank_1_type(&Z);

//        std::cout<<"Z[k]"<<std::endl;
//        for (int k = 0; k < Z.size() ; ++k) {
//            std::cout<<Z[k]<<" ";
//        }
//        std::cout<<std::endl;

    }

    std::cout<<"Building left_most_path(grammar) "<<std::endl;
    left_most_path(grammar);
    std::cout<<"Building right_most_path(grammar) "<<std::endl;
    right_most_path(grammar);



    auto _alp = grammar.get_map();

    for (auto &&  ii: _alp) {
        alp.push_back(ii.second);
    }
    std::sort(alp.begin(),alp.end());



    std::cout<<"Grammar end\n";

}

const compressed_grammar::parser_tree & compressed_grammar::get_parser_tree() const
{
    return m_tree;
}

size_t compressed_grammar::size_in_bytes() const{


    return size_t(
                               sdsl::size_in_bytes(X_p) +
                               sdsl::size_in_bytes(Z) +
                               sdsl::size_in_bytes(select0_Z) +
                               sdsl::size_in_bytes(select1_Z) +
                               sdsl::size_in_bytes(rank1_Z) +

                               sdsl::size_in_bytes(F) +
                               sdsl::size_in_bytes(F_inv) +
                               sdsl::size_in_bytes(Y) +
                               sdsl::size_in_bytes(rank_Y) +
                               sdsl::size_in_bytes(select_Y)+

                               sdsl::size_in_bytes(L) +
                               sdsl::size_in_bytes(select_L) +
                               sdsl::size_in_bytes(rank_L)+
                               m_tree.size_in_bytes() +
                               left_path.size_in_bytes() +

                               right_path.size_in_bytes() +
                               sdsl::size_in_bytes(alp)
                               );

}

void compressed_grammar::print_size_in_bytes() {
    std::cout<<"X_p \t"<<sdsl::size_in_mega_bytes(X_p)        <<"(bytes)"<<std::endl;
    std::cout<<"\t X_p(length) \t"<<X_p.size()<<std::endl;
    std::cout<<"\t X_p(sigma) \t"<<X_p.sigma<<std::endl;
    std::cout<<"Z \t"<<sdsl::size_in_mega_bytes(Z)          <<"(bytes)"<<std::endl;
    std::cout<<"\t Z(length) \t"<<Z.size()<<std::endl;
    std::cout<<"select0_Z \t"<<sdsl::size_in_mega_bytes(select0_Z)  <<"(bytes)"<<std::endl;
    std::cout<<"select1_Z \t"<<sdsl::size_in_mega_bytes(select1_Z)  <<"(bytes)"<<std::endl;
    std::cout<<"rank1_Z \t"<<sdsl::size_in_mega_bytes(rank1_Z)    <<"(bytes)"<<std::endl;
//    std::cout<<"rank0_Z \t"<<sdsl::size_in_mega_bytes(rank0_Z)    <<"(bytes)"<<std::endl;
    std::cout<<"F \t"<<sdsl::size_in_mega_bytes(F)          <<"(bytes)"<<std::endl;
    std::cout<<"\tF(length) \t"<<F.size()<<std::endl;
    std::cout<<"F_inv \t"<<sdsl::size_in_mega_bytes(F_inv)      <<"(bytes)"<<std::endl;
    std::cout<<"\tF_inv(length) \t"<<F_inv.size()<<std::endl;
    std::cout<<"Y \t"<<sdsl::size_in_mega_bytes(Y)          <<"(bytes)"<<std::endl;
    std::cout<<"\t Y (length) \t"<<Y.size()          <<std::endl;
    std::cout<<"rank_Y \t"<<sdsl::size_in_mega_bytes(rank_Y)     <<"(bytes)"<<std::endl;
    std::cout<<"L \t"<<sdsl::size_in_mega_bytes(L)          <<"(bytes)"<<std::endl;
    std::cout<<"select_L \t"<<sdsl::size_in_mega_bytes(select_L)   <<"(bytes)"<<std::endl;
    std::cout<<"parser tree \t"<<m_tree.size_in_bytes()          <<"(bytes)"<<std::endl;
    m_tree.print_size_in_bytes("\t\t");
    std::cout<<"left trie \t"<<left_path.size_in_bytes()          <<"(bytes)"<<std::endl;
    left_path.print_size_in_bytes("\t\t");
    std::cout<<"right trie \t"<<right_path.size_in_bytes()          <<"(bytes)"<<std::endl;
    right_path.print_size_in_bytes("\t\t");
}

void compressed_grammar::save(std::fstream &f) {

    std::cout<<"saving compressed_grammar\n";

    sdsl::serialize(X_p      ,f);
    sdsl::serialize(Z        ,f);
    sdsl::serialize(select0_Z,f);
    sdsl::serialize(select1_Z,f);
    sdsl::serialize(rank1_Z  ,f);

    sdsl::serialize(F        ,f);
    sdsl::serialize(F_inv    ,f);
    sdsl::serialize(Y        ,f);
    sdsl::serialize(rank_Y   ,f);
    sdsl::serialize(L        ,f);

    sdsl::serialize(rank_L   ,f);
    sdsl::serialize(select_L ,f);
    m_tree.save(f);
    left_path.save(f);
    right_path.save(f);

    size_t n = alp.size();
    sdsl::serialize(n ,f);
    for (auto &&  c: alp) {
        sdsl::serialize(c,f);
    }


}

void compressed_grammar::load(std::fstream & f) {

    sdsl::load(X_p      ,f);
    sdsl::load(Z        ,f);
    sdsl::load(select0_Z,f);
    sdsl::load(select1_Z,f);
    sdsl::load(rank1_Z  ,f);
//    sdsl::load(rank0_Z  ,f);

    select1_Z = z_vector::select_1_type(&Z);
    select0_Z = z_vector::select_0_type(&Z);
//    rank0_Z  = z_vector::rank_0_type(&Z);
    rank1_Z  = z_vector::rank_1_type(&Z);

    sdsl::load(F        ,f);
    sdsl::load(F_inv    ,f);

    F_inv = inv_compact_perm(&F);

    sdsl::load(Y        ,f);
    sdsl::load(rank_Y   ,f);
    rank_Y = y_vector::rank_1_type(&Y);
    select_Y = y_vector::select_1_type(&Y);
    sdsl::load(L        ,f);
    sdsl::load(rank_L ,f);
    sdsl::load(select_L ,f);
    select_L = l_vector::select_1_type(&L);
    rank_L = l_vector::rank_1_type(&L);
    m_tree.load(f);
    left_path.load(f);
    right_path.load(f);
    size_t n;
    sdsl::load(n,f);
    alp.clear();
    alp.resize(n);
    for (size_t i =0 ; i < n ; ++i) {
        unsigned char c ;
        sdsl::load(c,f);
        alp[i] = c;
    }

}


void compressed_grammar::left_most_path(){
    _trie::Trie<std::vector<g_long>> left_trie;
    auto num_leaf = m_tree.leafnum(m_tree.root());
    std::map<g_long, std::vector<g_long > > paths;
    for(int l = 0; l < num_leaf; ++l){
        std::vector<uint > current_path;
        auto current_leaf = m_tree.leafselect(l+1);
        auto Xj = (*this)[m_tree.pre_order(current_leaf)];

        if(paths.find(Xj) == paths.end())
        {
            paths[Xj] = {Xj};


        }else{

        }

    }



}

void compressed_grammar::left_most_path(const plain_grammar& grammar)
{
    _trie::Trie<std::vector<g_long>> left_trie;
    std::vector<uint> _stack(m_tree.subtree(m_tree.root()),0);
    g_long top = 0;
    std::map<g_long ,std::vector<g_long> > seconds_paths;

    std::cout<<"dfs_posorder for build trie\n"<<std::endl;

    m_tree.dfs_posorder(m_tree.root(),[&_stack,&top,&left_trie,&seconds_paths,this](const parser_tree::dfuds_long &node)->bool{
//        std::cout<<m_tree.pre_order(node)<<std::endl;
        parser_tree::dfuds_long rank_ch = m_tree.childrank(node);
        parser_tree::dfuds_long p = m_tree.pre_order(node);
        g_long Xj = (*this)[p];

//        std::cout<<"Xi:"<<Xj<<std::endl;
//        std::cout<<"node:"<<node<<std::endl;
//        sleep(1);
        if(m_tree.isleaf(node))
        {
//            std::cout<<"leaf case\n"<<std::endl;

            g_long first = select_occ(Xj,1);
            if(first == p)
            {
                std::vector<g_long > XX;
                XX.push_back(Xj);
                left_trie.insert(XX);
            }

            if(rank_ch == 1){

                if(first != p)
                {

                    for (auto &&  k: seconds_paths[Xj]) {
                        _stack[top] = k;
                        top++;
                    }
                    _stack[top] = p;
                    top++;

                    return false;
                }

                _stack[top]=p;
                top++;
            }

//            std::cout<<"leaf case end"<<std::endl;

            return false;
        }


//        std::cout<<"internal node case\n"<<std::endl;

        parser_tree::dfuds_long current = node;
//        std::cout<<"top:"<<top<<std::endl;
        int pointer = top-1;


        {
            std::vector<g_long > tt;
            std::vector<compact_trie::c_trie_long > sctt;
            tt.insert(tt.begin(),Xj);

            while(pointer >= 0  &&  m_tree.parent(m_tree[_stack[pointer]]) == current ){

                auto Xi = (*this)[_stack[pointer]];
//                std::cout<<"stack["<<pointer<<"]:"<<_stack[pointer]<<" ->rule:"<<Xi<<std::endl;
                auto pre = _stack[pointer];

                sctt.insert(sctt.begin(),pre);
                tt.insert(tt.begin(),Xi);
                current = m_tree[_stack[pointer]];
                pointer--;
                auto first = select_occ(Xi,1);

                if(m_tree.isleaf(current) && pre != first)
                    current = m_tree[first];
            }

//            std::cout<<"end while"<<std::endl;
            left_trie.insert(tt);
//            std::cout<<"list inserted\n";
            seconds_paths[Xj] = sctt;
//            std::cout<<"internal case end"<<std::endl;
        }



        if(rank_ch == 1){
            _stack[top] = p;
            top++;
            return true;
        }

        top = pointer+1;

        return false;
    });

    _stack.clear();
//    std::cout<<"trie constructed\n"<<std::endl;
//    std::cout<<"building compact trie\n"<<std::endl;

//    left_trie.print();

    left_path.build(left_trie);
//    left_path.print();
}
/*

void compressed_grammar::left_most_path(const plain_grammar& grammar)
{
    _trie::Trie<std::vector<g_long>> left_trie;
    std::vector<uint> _stack(m_tree.subtree(m_tree.root()),0);
    g_long top = 0;
    std::map<g_long ,std::vector<g_long> > seconds_paths;

    m_tree.dfs_posorder(m_tree.root(),[&_stack,&top,&left_trie,&seconds_paths,this](const parser_tree::dfuds_long &node)->bool{
        std::cout<<m_tree.pre_order(node)<<std::endl;
        parser_tree::dfuds_long rank_ch = m_tree.childrank(node);
        parser_tree::dfuds_long p = m_tree.pre_order(node);
        g_long Xj = (*this)[p];

        std::cout<<"Xj:"<<Xj<<std::endl;
        std::cout<<"preorder:"<<p<<std::endl;

        if(m_tree.isleaf(node))
        {
            std::cout<<"node leaf"<<std::endl;


            g_long first = select_occ(Xj,1);

            std::cout<<"preorder 1 occ:"<<first<<std::endl;
            if(first == p)
            {

                std::cout<<"node leaf 1 occ"<<std::endl;

                std::vector<g_long > XX;
                XX.push_back(Xj);
                left_trie.insert(XX);
            }

            if(rank_ch == 1){


                std::cout<<"node leaf 1 occ first child"<<std::endl;

                if(first != p)
                {

                    for (auto &&  k: seconds_paths[Xj]) {
                        _stack[top] = k;
                        top++;
                    }
                    _stack[top] = p;
                    top++;

                    return false;
                }

                _stack[top]=p;
                top++;
            }

            return false;
        }


        std::cout<<"no leaf node\n";
        parser_tree::dfuds_long current = node;
        int pointer = top-1;


        {
            std::vector<g_long > tt;
            std::vector<compact_trie::c_trie_long > sctt;
            tt.insert(tt.begin(),Xj);

            while(pointer >= 0  &&  m_tree.parent(m_tree[_stack[pointer]]) == current ){

                auto Xi = (*this)[_stack[pointer]];
                auto pre = _stack[pointer];

                sctt.insert(sctt.begin(),pre);
                tt.insert(tt.begin(),Xi);
                current = m_tree[_stack[pointer]];
                pointer--;
                auto first = select_occ(Xi,1);

                if(m_tree.isleaf(current) && pre != first)
                    current = m_tree[first];
            }


            left_trie.insert(tt);
            seconds_paths[Xj] = sctt;

        }



        if(rank_ch == 1){
            _stack[top] = p;
            top++;
            return true;
        }

        top = pointer+1;

        return false;
    });

    _stack.clear();



    ///////left_trie.print();
    std::cout<<"building left most path\n";
    left_path.build(left_trie);

}
*/

void compressed_grammar::right_most_path(const plain_grammar& grammar)
{
    _trie::Trie<std::vector<compact_trie::c_trie_long>> right_trie;
    std::vector<g_long> _stack(m_tree.subtree(m_tree.root()),0);
    g_long top = 0;
    std::map<g_long ,std::vector<g_long> > seconds_paths;

    m_tree.dfs_posorder(m_tree.root(),[&_stack,&top,&right_trie,&seconds_paths,this](const parser_tree::dfuds_long &node)->bool{

        uint rank_ch = m_tree.childrank(node);
        uint n_ch = m_tree.children(m_tree.parent(node));
        uint p = m_tree.pre_order(node);
        auto Xj = (*this)[p];

        if(m_tree.isleaf(node))
        {
            g_long first = select_occ(Xj,1);
            if(first == p)
            {
                std::vector<g_long > XX;
                XX.push_back(Xj);
                right_trie.insert(XX);
            }

            if(rank_ch == n_ch)
            {

                if(first != p)
                {
                    for (auto &&  k: seconds_paths[Xj]) {
                        _stack[top] = k;
                        top++;
                    }
                    _stack[top] = p;
                    top++;
                    return false;
                }
                _stack[top]=p;
                top++;
            }
            return false;
        }

        uint current = node;
        int pointer = top-1;
        {
            std::vector<compact_trie::c_trie_long > tt;
            std::vector<compact_trie::c_trie_long > sctt;
            tt.insert(tt.begin(),Xj);

            while(pointer >= 0  &&  m_tree.parent(m_tree[_stack[pointer]]) == current ){

                auto Xi = (*this)[_stack[pointer]];
                auto pre = _stack[pointer];

                sctt.insert(sctt.begin(),pre);
                tt.insert(tt.begin(),Xi);
                current = m_tree[_stack[pointer]];
                pointer--;
                auto first = select_occ(Xi,1);

                if(m_tree.isleaf(current) && pre != first)
                    current = m_tree[first];

            }

            right_trie.insert(tt);
            seconds_paths[Xj] = sctt;
        }


        if(rank_ch == n_ch){
            _stack[top] = p;
            top++;
            return true;
        }

        top = pointer+1;


        return false;
    });

 /////////   right_trie.print();

    _stack.clear();
    right_path.build(right_trie);
}

compressed_grammar::g_long compressed_grammar::n_rules() const {
    return F.size() ;
}

compressed_grammar::g_long compressed_grammar::pre_right_trie(const g_long & pre_g_tree) const {
    return left_path.preorder(pre_g_tree);


}

compressed_grammar::g_long compressed_grammar::pre_left_trie(const g_long & pre_g_tree) const {
    return right_path.preorder(pre_g_tree);;
}

const compact_trie &compressed_grammar::get_right_trie() const {
    return right_path;
}

const compact_trie &compressed_grammar::get_left_trie() const {
    return left_path;
}

//unsigned char compressed_grammar::terminal_simbol(const g_long &i) const{
//    return alp[rank_Y(i)];
//}

compressed_grammar::g_long compressed_grammar::get_size_text() const {
    return L.size();
}
/*
bool compressed_grammar::isLastOcc(const compressed_grammar::g_long &j) const {
    if(Z[j-1]) return false;
    return l_occ_xp[rank0_Z(j-1)];
}
 */

compressed_grammar &compressed_grammar::operator=(const compressed_grammar & G) {

    m_tree = G.m_tree;
    Z = G.Z;

    select1_Z = z_vector::select_1_type(&Z);
    select0_Z = z_vector::select_0_type(&Z);
//    rank0_Z  = z_vector::rank_0_type(&Z);
    rank1_Z  = z_vector::rank_1_type(&Z);

    X_p = G.X_p;
    F = G.F;
    F_inv = inv_compact_perm(&F);
    L = G.L;
    select_L = l_vector::select_1_type(&L);
    Y = G.Y;
    rank_Y = y_vector::rank_1_type(&Y);
    select_Y = y_vector::select_1_type(&Y);
    left_path = G.left_path;
    right_path = G.right_path;
    ///l_occ_xp = G.l_occ_xp;
    alp = G.alp;

    return *this;

}

const std::vector<unsigned char> compressed_grammar::get_alp() const {
    return alp;
}

compressed_grammar::g_long compressed_grammar::terminal_rule(const compressed_grammar::g_long &i) const {
    return select_Y(i);
}

bool compressed_grammar::is_first_occ(const compressed_grammar::g_long & j) const {
    return Z[j-1];
}


const compressed_grammar::wavelet_tree &compressed_grammar::get_Xp() const {
    return X_p;
}


/*

const sdsl::wt_ap<> &compressed_grammar::get_Xp() const {
    return X_p;
}
*/



const compressed_grammar::compact_perm &compressed_grammar::get_F() const {
    return F;
}

void compressed_grammar::right_most_path() {

}

void compressed_grammar::load_z(std::fstream &f) {

    sdsl::load(Z        ,f);

    select1_Z = z_vector::select_1_type(&Z);
    select0_Z = z_vector::select_0_type(&Z);
//    rank0_Z  = z_vector::rank_0_type(&Z);
    rank1_Z  = z_vector::rank_1_type(&Z);

}

void compressed_grammar::load_y(std::fstream &f) {
    sdsl::load(Y        ,f);
    rank_Y = y_vector::rank_1_type(&Y);
    select_Y = y_vector::select_1_type(&Y);
}

void compressed_grammar::load_l(std::fstream &f) {
    sdsl::load(L        ,f);
    select_L = l_vector::select_1_type(&L);
    rank_L = l_vector::rank_1_type(&L);
}

void compressed_grammar::load_F(std::fstream &f) {
    sdsl::load(F        ,f);
    F_inv = inv_compact_perm(&F);
}

void compressed_grammar::load_X_p(std::fstream &f) {
    sdsl::load(X_p      ,f);
}

void compressed_grammar::load_ltrie(std::fstream &f) {

    left_path.load(f);
}

void compressed_grammar::load_rtrie(std::fstream &f) {

    right_path.load(f);
}

void compressed_grammar::load_alp(std::fstream &f) {
    sdsl::load(alp,f);
}

void compressed_grammar::load_mtree(std::fstream &f) {
    m_tree.load(f);
}


