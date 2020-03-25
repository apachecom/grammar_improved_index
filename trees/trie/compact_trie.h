//
// Created by inspironXV on 8/17/2018.
//

#ifndef IMPROVED_GRAMMAR_INDEX_COMPACT_TRIE_H
#define IMPROVED_GRAMMAR_INDEX_COMPACT_TRIE_H


#include "../bp_tree.h"
#include "Trie.h"
#include "../dfuds_tree.h"
#include <sdsl/inv_perm_support.hpp>


class compact_trie {

    public:
        typedef unsigned int c_trie_long;
        typedef _trie::Trie<std::vector<c_trie_long>> T;
        typedef bp_tree tree;
        typedef sdsl::int_vector<> permutation_seq;
        typedef sdsl::inv_perm_support<> inv_permutation_seq;

    //protected:
        tree m_tree;
        permutation_seq seq;
        inv_permutation_seq inv_seq;

    ///public:
        compact_trie();
        ~compact_trie();
        void build( const T&);
        /*
         * Return de preorder of the node with the label
         * passed
         * */
        c_trie_long label(const c_trie_long&)const;
        /*
         * Return de label of the node with i-th preorder passed
         * */
        c_trie_long preorder(const c_trie_long&) const;
        /*
         * return a constant reference to the tree
         * */
        const tree& get_tree() const;
        /*
         * return a constant reference to the sequence
         * */
        const permutation_seq& get_seq()const;
        /*
         * return a constant reference to the inverse permutation of the sequence
         * */
        const inv_permutation_seq& get_seq_inv()const;

        void save(std::fstream& f)const ;
        void load(std::fstream& f);

        compact_trie&operator=(const compact_trie& );

        void print_size_in_bytes(const std::string&) const;
        size_t size_in_bytes()const;
};


#endif //IMPROVED_GRAMMAR_INDEX_COMPACT_TRIE_H
