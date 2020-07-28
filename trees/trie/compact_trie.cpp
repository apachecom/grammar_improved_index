//
// Created by inspironXV on 8/17/2018.
//

#include "compact_trie.h"

compact_trie::compact_trie() = default;

compact_trie::~compact_trie() = default;

compact_trie::c_trie_long compact_trie::label(const c_trie_long & i) const {
    return seq[i];
}

void compact_trie::build(const compact_trie::T & trie) {

    /*
     * counting number of nodes
     * */
    c_trie_long num_nodes = trie.get_num_nodes();

    /*
     * building bitvector and sequence
     *
     * */
//    std::cout<<"building bitvector and sequence"<<std::endl;
    {
        sdsl::bit_vector _bv(num_nodes*2,false);
        c_trie_long pos = 0;
        sdsl::int_vector<> v(num_nodes,0);
        c_trie_long  pos_s = 0;

        trie.dfs_ba(trie.get_root(),[&_bv, &pos,&pos_s,&v](const trie::node* n, const trie::node::key& a, const bool& b){
            _bv[pos] = b;
            if (b != 0)
            {
                v[pos_s] = (c_trie_long)a;
                pos_s++;
            }
            pos++;
        },0);
        m_tree.build(_bv);
        seq = permutation_seq(v);
        inv_seq = inv_permutation_seq(&seq);
        sdsl::util::bit_compress(seq);

    }

}

compact_trie::c_trie_long compact_trie::preorder(const c_trie_long & label) const {
    return inv_seq[label];
}

const compact_trie::tree& compact_trie::get_tree() const {
    return m_tree;
}

const compact_trie::permutation_seq& compact_trie::get_seq() const {
    return seq;
}

const compact_trie::inv_permutation_seq& compact_trie::get_seq_inv() const{
    return inv_seq;
}

void compact_trie::load(std::fstream &f) {

    sdsl::load(seq,f);
    sdsl::load(inv_seq,f);
    inv_seq = inv_permutation_seq(&seq);
    m_tree.load(f);

}

void compact_trie::save(std::fstream &f)const
{
//    std::cout<<"\t\tsaving compac_trie"<<std::endl;
    sdsl::serialize(seq,f);
//    std::cout<<"\t\t\tsdsl::serialize(seq,f);"<<std::endl;
    sdsl::serialize(inv_seq,f);
//    std::cout<<"\t\t\tsdsl::serialize(inv_seq,f);"<<std::endl;
    m_tree.save(f);
//    std::cout<<"\t\t\tm_tree.save(f);"<<std::endl;
}

void compact_trie::print_size_in_bytes(const std::string &s) const {


    std::cout<<s+"tree bitvector compact trie"<<std::endl;
    m_tree.print_size_in_bytes(s);
    std::cout<<s+"sequence \t"<<sdsl::size_in_bytes(seq)<<"(bytes)"<<std::endl;
    std::cout<<s+"\t inv sequence \t"<<sdsl::size_in_bytes(inv_seq)<<"(bytes)"<<std::endl;

}

size_t compact_trie::size_in_bytes() const {
    return m_tree.size_in_bytes() +
           sdsl::size_in_bytes(seq) +
           sdsl::size_in_bytes(inv_seq);
}

compact_trie &compact_trie::operator=(const compact_trie &T) {

    m_tree = T.m_tree;
    seq = T.seq;
    inv_seq = inv_permutation_seq(&seq);
    return *this;

}
