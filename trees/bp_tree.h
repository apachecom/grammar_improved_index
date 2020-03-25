//
// Created by inspironXV on 8/17/2018.
//

#ifndef IMPROVED_GRAMMAR_INDEX_BP_TREE_H
#define IMPROVED_GRAMMAR_INDEX_BP_TREE_H


#include <sdsl/int_vector.hpp>
#include <sdsl/bp_support_sada.hpp>


class bp_tree {

    typedef sdsl::bit_vector bv;
    typedef sdsl::bp_support_sada<> parenthesis_seq;
    typedef unsigned long bp_long;
public:
    bv bit_vector;
    parenthesis_seq bps;
    sdsl::rank_support_v<10,2> rank_10;
    sdsl::select_support_mcl<10,2> select_10;
    sdsl::select_support_mcl<0,1> select_0;


public:
    bp_tree() = default;
    ~bp_tree() = default;

    void build( const bv & );

    inline short root()const{ return 0; }
    /*
     * return the number of nodes in the subtree of the node
     * */
    inline bp_long subtree(const bp_long& v)const{
        return (bps.find_close(v)-v+1)/2;
    }

    inline bool isleaf(const bp_long & v)const{
        return !bit_vector[v];
    }
    /*
     * (preoreder) return the preorder of a node
     * */
    inline bp_long pre_order( const bp_long& v)const{
        return bps.rank(v);
    }
    /*
     * (preoreder select) return the i-th node in preorder
     * */
    inline bp_long operator[](const bp_long &  i)const{
        return bps.select(i);
    };
    /*
     *  return the i-th child of a node
     * */
    bp_long child(const bp_long& , const bp_long& )const;
    /*
     * return the rank of a node between its brothers
     * */
    bp_long childrank(const bp_long & )const;
    /*
     * return the number of children of a node
     * */
    bp_long children(const bp_long &v)const;
    /*
     * return the number of leaves in the subtree of a node
     * */
    inline bp_long leafnum(const bp_long & v)const {
        return leafrank(bps.find_close(v))-leafrank(v);
    }
    /*
     * return the rank between the leaf of the left most leaf of a node
     * */
    inline bp_long leafrank(const bp_long & v)const{
        return rank_10(v)+1;
    };
    /*
     * return the i-th leaf
     * */
    inline bp_long leafselect(const bp_long &i)const {
        return select_10(i);
    }
    /*
     * return the parent of a node
     * */
    inline bp_long parent(const bp_long& v)const{
        if(v == 0) return bps.size();
        return bps.enclose(v);
    }

    inline bp_long fchild(const bp_long& v) const{
        return v+1;
    }
    inline bp_long lchild(const bp_long& v )const{
        return bps.find_open(bps.find_close(v)-1);
    }

    inline bp_long nsibling(const bp_long& v)const{
        auto nb = bps.find_close(v)+1;
        if(bit_vector[nb] == 0) return bps.size();
        return nb;
    }

    inline bp_long psibling(const bp_long& v)const{
        if(bit_vector[v-1] == true) return bps.size();
        return  bps.find_open(v-1);
    }

    inline bp_long levelancestor(const bp_long& v, const int& d) const{
        return bps.level_anc(v,d);
    }

    size_t size_in_bytes() const;

    double size_in_mega_bytes() const;

    void print_size_in_bytes(const std::string &) const;


    void save(std::fstream &f) const;

    void load(std::fstream &f);

    inline bp_long depth(const bp_long& v) const{
        return 2*bps.rank(v)-(v+1);
    }



    /*
     * dfs over the tree
     * */
    template <typename K>
    void dfs_preorder(const bp_long &node, const K &f){

        auto keep = f(node,this);
        if(!keep)
            return;
        bp_long n  = children(node);
        for (bp_long i = 0; i <  n ; ++i) {
            dfs_preorder(child(node,i+1),f);
        }
    }

};


#endif //IMPROVED_GRAMMAR_INDEX_BP_TREE_H
