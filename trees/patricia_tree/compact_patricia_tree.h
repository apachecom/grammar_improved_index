//
// Created by inspironXV on 8/17/2018.
//

#ifndef IMPROVED_GRAMMAR_INDEX_COMPACT_PATRICIA_TREE_H
#define IMPROVED_GRAMMAR_INDEX_COMPACT_PATRICIA_TREE_H


#include <sdsl/inv_perm_support.hpp>
#include "../dfuds_tree.h"
#include "patricia_tree.h"

namespace m_patricia {

    class compact_patricia_tree {


    protected:
        typedef unsigned int ulong;
        typedef m_patricia::string_pairs K;
        typedef m_patricia::rev_string_pairs revK ;
        typedef m_patricia::patricia_tree<K> T;
        typedef dfuds::dfuds_tree tree;
        typedef sdsl::int_vector<> seq_alf;
        typedef sdsl::int_vector<> seq_jmp;


        tree m_tree;
        seq_alf seq;
        seq_jmp jumps;

    public:

        compact_patricia_tree();

        ~compact_patricia_tree();

        void build(const T &tree);
        void build(const m_patricia::patricia_tree<m_patricia::rev_string_pairs> &);

        ulong node_match(const K &) const;
        ulong node_match(const revK &) const;

        ulong node_locus(const K &, const ulong &, ulong &) const;
        ulong node_locus(const revK &, const ulong &, ulong &) const;

        void save(std::fstream &) const;

        void load(std::fstream &);

        const tree& get_tree()const ;
        const seq_alf& get_seq() const;
        const seq_jmp& get_jumps() const;


        ulong size_in_bytes() const;
        void print_size_in_bytes() const;
        compact_patricia_tree& operator =(const compact_patricia_tree&);
        ulong find_child_range(const ulong &, const K& , const ulong&, const int &);
        ulong find_child_range(const ulong &, const revK& , const ulong& ,const int&);



    protected:
        bool path(ulong &, const K &str, ulong &) const;
        bool path(ulong &, const revK &str, ulong &) const;

        void path(ulong &, const K &str, ulong &, const ulong &limit) const;
        void path(ulong &, const revK &str, ulong &, const ulong &limit) const;


    };

}
#endif //IMPROVED_GRAMMAR_INDEX_COMPACT_PATRICIA_TREE_H
