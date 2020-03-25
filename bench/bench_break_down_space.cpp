//
// Created by root on 23-05-19.
//


#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "../SelfGrammarIndexBS.h"
#include "../bench/utils.h"
#include "../bench/repetitive_collections.h"

#define COLLECTION 1

SelfGrammarIndex *idx = load_idx(3, 8, 1, 1, 1, COLLECTION);

static void bkd_PT_suffixes_dfuds_tree_select_00(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_suffixes().get_tree().select_00);

}
static void bkd_PT_suffixes_dfuds_tree_select_0(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_suffixes().get_tree().select_0);

}
static void bkd_PT_suffixes_dfuds_tree_rank_00(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_suffixes().get_tree().rank_00);

}
static void bkd_PT_suffixes_dfuds_tree_bps(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_suffixes().get_tree().bps);

}
static void bkd_PT_suffixes_dfuds_tree_bitvector(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_suffixes().get_tree().bit_vector);

}
static void bkd_PT_suffixes_cmp_int_vector_jmps(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_suffixes().get_jumps());
}
static void bkd_PT_suffixes_cmp_int_vector_labels(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_suffixes().get_seq());
}
///////////////////////////PT RULES///////////////////////////////////////
static void bkd_PT_rules_dfuds_tree_select_00(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_rules().get_tree().select_00);

}
static void bkd_PT_rules_dfuds_tree_select_0(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_rules().get_tree().select_0);

}
static void bkd_PT_rules_dfuds_tree_rank_00(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_rules().get_tree().rank_00);

}
static void bkd_PT_rules_dfuds_tree_bps(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_rules().get_tree().bps);

}
static void bkd_PT_rules_dfuds_tree_bitvector(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_rules().get_tree().bit_vector);

}
static void bkd_PT_rules_cmp_int_vector_jmps(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_rules().get_jumps());
}
static void bkd_PT_rules_cmp_int_vector_labels(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);
    int s = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_pt_rules().get_seq());
}
/////////////////////////////GRID//////////////////////////////////////////////
static void bkd_rrr_vector_XB_select_0(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grid().xb_sel0);
}
static void bkd_rrr_vector_XB_select_1(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grid().xb_sel1);
}
static void bkd_rrr_vector_XB_rank(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grid().xb_rank1);
}
static void bkd_rrr_vector_XB(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grid().XB);
}
static void bkd_rrr_vector_XA_rank(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grid().xa_rank1);

}
static void bkd_rrr_vector_XA(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grid().XA);

}
static void bkd_int_vector_comp_SL(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grid().SL);

}
static void bkd_wt_int_SB(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grid().SB);

}
static void bkd_alp(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().alp);

}
///////////////////////TRIES///////////////////////////////////
static void bkd_right_trie_bp_rank_10(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().right_path.m_tree.rank_10);

}
static void bkd_right_trie_bp_select_0(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().right_path.m_tree.select_0);

}
static void bkd_right_trie_bp_select_10(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().right_path.m_tree.select_10);

}
static void bkd_right_trie_bp_bp_sup(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().right_path.m_tree.bps);

}
static void bkd_right_trie_bp_bitvector(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().right_path.m_tree.bit_vector);

}
static void bkd_right_trie_inv_labels(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().right_path.inv_seq);

}
static void bkd_right_trie_labels(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().right_path.seq);

}
static void bkd_left_trie_bp_rank_10(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().left_path.m_tree.rank_10);

}
static void bkd_left_trie_bp_select_0(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().left_path.m_tree.select_0);

}
static void bkd_left_trie_bp_select_10(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().left_path.m_tree.select_10);

}
static void bkd_left_trie_bp_bp_sup(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().left_path.m_tree.bps);

}
static void bkd_left_trie_bp_bitvector(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().left_path.m_tree.bit_vector);

}
static void bkd_left_trie_inv_labels(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().left_path.inv_seq);

}
static void bkd_left_trie_labels(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().left_path.seq);

}
/////////////////////////BASICS/////////
static void bkd_dfuds_select_0(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().m_tree.select_0);

}
static void bkd_dfuds_rank_00(benchmark::State &state) {

    for (auto _ : state) {

    }
    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().m_tree.rank_00);

}
static void bkd_dfuds_select_00(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().m_tree.select_00);

}
static void bkd_dfuds_bp_sup(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().m_tree.bps);

}
static void bkd_dfuds_bitvector(benchmark::State &state) {

    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().m_tree.bit_vector);

}
static void bkd_sd_vector_L_select_1(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().select_L);

}
static void bkd_sd_vector_L_rank(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().rank_L);

}
static void bkd_sd_vector_L(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().L);

}
static void bkd_sd_vector_Y_select_1(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().select_Y);

}
static void bkd_sd_vector_Y_rank(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().rank_Y);

}
static void bkd_sd_vector_Y(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().Y);

}
static void bkd_sd_vector_Z_select_0(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().select0_Z);

}
static void bkd_sd_vector_Z_select_1(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().select1_Z);

}
static void bkd_sd_vector_Z_rank(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().rank1_Z);

}
static void bkd_sd_vector_Z(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = idx->get_grammar().get_Z_size();

}
static void bkd_wt_gmr_X(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = idx->get_grammar().get_X_size();

}
static void bkd_F(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"] = sdsl::size_in_bytes(idx->get_grammar().F);

}
static void bkd_invF(benchmark::State &state) {
    for (auto _ : state) {

    }
    int code = state.range(0);

    state.counters["size"]   = sdsl::size_in_bytes(idx->get_grammar().get_F_inv());

}
static void total_idx(benchmark::State &state) {


    for (auto _ : state) {

    }
    int code = state.range(0);
    int i = state.range(1);
    int s = state.range(2);
    SelfGrammarIndex *idxII = load_idx(i, s, 1, 1, 1, code);
    state.counters["size"]   = idxII->size_in_bytes();
    delete idxII;

}

BENCHMARK(bkd_dfuds_select_0)                    ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_dfuds_rank_00)                     ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_dfuds_select_00)                   ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_dfuds_bp_sup)                      ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_dfuds_bitvector)                   ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_sd_vector_L_select_1)              ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_sd_vector_L_rank)                  ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_sd_vector_L)                       ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_sd_vector_Y_select_1)              ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_sd_vector_Y_rank)                  ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_sd_vector_Y)                       ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_sd_vector_Z_select_0)              ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_sd_vector_Z_select_0)              ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_sd_vector_Z_select_1)              ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_sd_vector_Z_rank)                  ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_sd_vector_Z)                       ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_wt_gmr_X)                          ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_F)                                 ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_invF)                              ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_right_trie_bp_rank_10)             ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_right_trie_bp_select_0)            ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_right_trie_bp_select_10)           ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_right_trie_bp_bp_sup)              ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_right_trie_bp_bitvector)           ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_right_trie_inv_labels)             ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_right_trie_labels)                 ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_left_trie_bp_rank_10)              ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_left_trie_bp_select_0)             ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_left_trie_bp_select_10)            ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_left_trie_bp_bp_sup)               ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_left_trie_bp_bitvector)            ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_left_trie_inv_labels)              ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_left_trie_labels)                  ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_rrr_vector_XB_select_0)            ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_rrr_vector_XB_select_1)            ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_rrr_vector_XB_rank)                ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_rrr_vector_XB)                     ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_rrr_vector_XA_rank)                ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_rrr_vector_XA)                     ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_int_vector_comp_SL)                ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_wt_int_SB)                         ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_alp)                               ->Args({COLLECTION})->Unit(benchmark::kMicrosecond);

BENCHMARK(bkd_PT_rules_dfuds_tree_select_00)     ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_rules_dfuds_tree_select_0)      ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_rules_dfuds_tree_select_0)      ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_rules_dfuds_tree_rank_00)       ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_rules_dfuds_tree_bps)           ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_rules_dfuds_tree_bitvector)     ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_rules_cmp_int_vector_jmps)      ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_rules_cmp_int_vector_labels)    ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_suffixes_dfuds_tree_select_00)  ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_suffixes_dfuds_tree_select_0)   ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_suffixes_dfuds_tree_select_0)   ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_suffixes_dfuds_tree_rank_00)    ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_suffixes_dfuds_tree_bps)        ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_suffixes_dfuds_tree_bitvector)  ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_suffixes_cmp_int_vector_jmps)   ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(bkd_PT_suffixes_cmp_int_vector_labels) ->Args({COLLECTION,8})->Unit(benchmark::kMicrosecond);

BENCHMARK(total_idx)                             ->Args({COLLECTION,1,0})->Unit(benchmark::kMicrosecond);
BENCHMARK(total_idx)                             ->Args({COLLECTION,3,4})->Unit(benchmark::kMicrosecond);
BENCHMARK(total_idx)                             ->Args({COLLECTION,3,8})->Unit(benchmark::kMicrosecond);
BENCHMARK(total_idx)                             ->Args({COLLECTION,3,16})->Unit(benchmark::kMicrosecond);
BENCHMARK(total_idx)                             ->Args({COLLECTION,3,32})->Unit(benchmark::kMicrosecond);
BENCHMARK(total_idx)                             ->Args({COLLECTION,3,64})->Unit(benchmark::kMicrosecond);

BENCHMARK_MAIN();

