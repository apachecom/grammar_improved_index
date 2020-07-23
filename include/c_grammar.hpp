//
// Created by alejandro on 23-07-20.
//

#ifndef IMPROVED_GRAMMAR_INDEX_C_GRAMMAR_H
#define IMPROVED_GRAMMAR_INDEX_C_GRAMMAR_H


#include "../trees/dfuds_tree.h"



namespace g_index{


    template <

            typename wavelet_tree,
            typename l_vector,
            typename z_vector,
            typename y_vector,
            typename inv_compact_perm,
            typename parser_tree = dfuds::dfuds_tree

    >
    class c_grammar{



    public:
        typedef uint32_t g_long;
        typedef sdsl::int_vector<> compact_perm;

    protected:
        parser_tree m_tree;

        z_vector Z; // Z bitmap with the first position of symbols in preorder sequence
        typename z_vector::rank_1_type rank1_Z;
        //z_vector::rank_0_type rank0_Z;
        typename z_vector::select_1_type select1_Z;
        typename z_vector::select_0_type select0_Z;

        wavelet_tree X_p;  // X sequence of preorder grammar tree symbols removing first aparation


        compact_perm F; // is a permutation of Xj such that F[i] = Xj only and only if Xj is the ith diferent symbol in preorder
        inv_compact_perm F_inv; //Inv-Permutation of F

        y_vector Y; // marks the rules Xi -> a
        typename y_vector::rank_1_type rank_Y;
        typename y_vector::select_1_type select_Y;

        l_vector L; // marks the init position of each Xi in T
        typename l_vector::select_1_type select_L;
        typename l_vector::rank_1_type rank_L;

        /*
         * alphabeth
         * */


        std::vector<unsigned char> alp;
    };

}

#endif //IMPROVED_GRAMMAR_INDEX_C_GRAMMAR_H
