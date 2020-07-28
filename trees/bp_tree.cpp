//
// Created by inspironXV on 8/17/2018.
//

#include "bp_tree.h"

void bp_tree::build(const bp_tree::bv &v) {

    bit_vector = bv(v);
    bps =   sdsl::bp_support_sada<>(&bit_vector);
    rank_10 = sdsl::rank_support_v<10,2> (&bit_vector);
    select_10 = sdsl::select_support_mcl<10,2>(&bit_vector);
    select_0  = bv::select_0_type(&bit_vector);
}
//
//short bp_tree::root() const{
//    return 0;
//}
//
//bp_tree::bp_long bp_tree::subtree(const bp_tree::bp_long & v) const{
//    assert(bit_vector[v] == true);
//    return (bps.find_close(v)-v+1)/2;
//}
//
//bool bp_tree::isleaf(const bp_tree::bp_long & v) const{
//    return !bit_vector[v];
//}
//
//bp_tree::bp_long bp_tree::pre_order(const bp_tree::bp_long &v) const{
//    return bps.rank(v);
//}
//
//bp_tree::bp_long bp_tree::operator[](const bp_tree::bp_long & i) const{
//    return bps.select(i);
//}

bp_tree::bp_long bp_tree::child(const bp_tree::bp_long & v, const bp_tree::bp_long & t) const{
    //assert(bit_vector[v+1] == 0);
    if(bit_vector[v+1]==0) return 0;
    if(t == 1) return  v+1;

    auto l = v+2;
    auto r = bps.find_close(v);

    for(int i = 2; i <= t ; ++i)
    {
        auto next = bps.rmq(l,r);
        if(next+1 >= r) return bit_vector.size();
        l = next+1;
    }

    return l;
}
/*
bp_tree::bp_long bp_tree::childrank(const bp_tree::bp_long &) {
    return 0;
}*/

bp_tree::bp_long bp_tree::children(const bp_tree::bp_long &v)const {
    if(bit_vector[v+1] == 0) return 0;

    auto l = v+1;
    bp_long ch = 0;
    while(l != bps.size())
    {   ch++;
        l = nsibling(l);
    }

    return ch;
}

//bp_tree::bp_long bp_tree::leafnum(const bp_tree::bp_long &v) const {
//
//    return leafrank(bps.find_close(v))-leafrank(v);
//}
//
//bp_tree::bp_long bp_tree::leafrank(const bp_tree::bp_long &v) const  {
//    return rank_10(v)+1;
//}

//bp_tree::bp_long bp_tree::leafselect(const bp_tree::bp_long & i) const {
//    return select_10(i);
//}

//bp_tree::bp_long bp_tree::parent(const bp_tree::bp_long & v)const {
//    if(v == 0) return bps.size();
//    return bps.enclose(v);
//}

//bp_tree::bp_long bp_tree::psibling(const bp_tree::bp_long & v) const{
//    if(bit_vector[v-1] == true) return bps.size();
//    return  bps.find_open(v-1);
//}
//
//bp_tree::bp_long bp_tree::nsibling(const bp_tree::bp_long & v) const{
//    auto nb = bps.find_close(v)+1;
//    if(bit_vector[nb] == 0) return bps.size();
//    return nb;
//}

//bp_tree::bp_long bp_tree::lchild(const bp_tree::bp_long & v) const{
//    //assert(bit_vector[v+1] == false);
//    //open(B, close(B, v) − 1)
//
//    return bps.find_open(bps.find_close(v)-1);
//}
//
//bp_tree::bp_long bp_tree::fchild(const bp_tree::bp_long & v)const {
//    //assert(bit_vector[v+1] == false);
//    return v+1;
//}
//
//bp_tree::bp_long bp_tree::levelancestor(const bp_tree::bp_long & v, const int& d) const {
//
//    if(d == 0) return v;
//    return bps.bwd_excess(v,-d-1)+1;
//}

size_t bp_tree::size_in_bytes() const {
    return  sdsl::size_in_bytes(bit_vector) +
            sdsl::size_in_bytes(bps)        +
            sdsl::size_in_bytes(select_10)  +
            sdsl::size_in_bytes(select_0)   +
            sdsl::size_in_bytes(rank_10)
            ;
}

double bp_tree::size_in_mega_bytes() const {
    return  sdsl::size_in_mega_bytes(bit_vector) +
            sdsl::size_in_mega_bytes(bps)        +
            sdsl::size_in_mega_bytes(select_10)  +
            sdsl::size_in_mega_bytes(select_0)   +
            sdsl::size_in_mega_bytes(rank_10)
            ;
}

void bp_tree::print_size_in_bytes(const std::string & s) const {
    std::cout<<s+"tree bitvector \t"<<sdsl::size_in_mega_bytes(bit_vector)<<"(bytes)"<<std::endl;
    std::cout<<s+"\t tree bitvector length\t"<<bit_vector.size()<<std::endl;
    std::cout<<s+"balanced parentheses sequence \t"<<sdsl::size_in_mega_bytes(bps)       <<"(bytes)"<<std::endl;
    std::cout<<s+"\t balanced parentheses sequence \t"<<bps.size()<<std::endl;
    std::cout<<s+"tree bitvector(select_00) \t"<<sdsl::size_in_mega_bytes(select_10) <<"(bytes)"<<std::endl;
    std::cout<<s+"tree bitvector(select_0) \t"<<sdsl::size_in_mega_bytes(select_0)  <<"(bytes)"<<std::endl;
    std::cout<<s+"tree bitvector(rank_00) \t"<<sdsl::size_in_mega_bytes(rank_10)   <<"(bytes)"<<std::endl;
}

void bp_tree::save(std::fstream &f) const {
    sdsl::serialize(bit_vector,f);
    sdsl::serialize(bps,f);
    sdsl::serialize(rank_10,f);
    sdsl::serialize(select_10,f);
    sdsl::serialize(select_0,f);
}

void bp_tree::load(std::fstream &f) {
    sdsl::load(bit_vector,f);
    bps.load(f,&bit_vector);
    sdsl::load(rank_10,f);
    sdsl::load(select_10,f);
    sdsl::load(select_0,f);

    bps =   sdsl::bp_support_sada<>(&bit_vector);
    rank_10 = sdsl::rank_support_v<10,2> (&bit_vector);
    select_10 = sdsl::select_support_mcl<10,2>(&bit_vector);
    select_0  = bv::select_0_type(&bit_vector);
}
//
//bp_tree::bp_long bp_tree::depth(const bp_tree::bp_long &v) const {
//    return 2*bps.rank(v)-(v+1);
//}
