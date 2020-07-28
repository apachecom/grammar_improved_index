//
// Created by inspironXV on 8/17/2018.
//

#include "dfuds_tree.h"

using namespace dfuds;

void dfuds_tree::build(const sdsl::bit_vector &v) {

    auto _bv = sdsl::bit_vector(v.size()+3);
    _bv[0]=1;_bv[1]=1;_bv[2]=0;

    for (int i = 0; i < v.size(); ++i) {
        _bv[3+i] = v[i];
    }
    bit_vector = bv(_bv);
    bps =   sdsl::bp_support_sada<>(&bit_vector);
    rank_00 = sdsl::rank_support_v<00,2> (&bit_vector);
    select_00 = sdsl::select_support_mcl<00,2>(&bit_vector);
    select_0  = bv::select_0_type(&bit_vector);


}

//inline short dfuds_tree::root()const {
//    return 3;
//}

//inline dfuds_tree::dfuds_long dfuds_tree::subtree(const dfuds_tree::dfuds_long & v)const {
//
//    return ((bps.fwd_excess(v - 1, -1) - v) / 2 + 1);
//    ////return 0;
//}

//inline bool dfuds_tree::isleaf(const dfuds_tree::dfuds_long & v) const{
//    return !bit_vector[v];
//}

//inline dfuds_tree::dfuds_long dfuds_tree::pre_order(const dfuds_tree::dfuds_long & v)const {
//    ////assert(v > 2);
//    return  v-bps.rank(v)+bit_vector[v] ;
//}

//inline dfuds_tree::dfuds_long dfuds_tree::operator[](const dfuds_tree::dfuds_long & i) const{
//    return select_0(i)+1;
//}

//inline dfuds_tree::dfuds_long dfuds_tree::childrank(const dfuds_tree::dfuds_long & v) const{
//    if(v == 3) return 0;
//
//    size_t bp_parent = bps.find_open(v-1);
//
//    return succ0(bp_parent) - bp_parent;
//
//}

//inline dfuds_tree::dfuds_long dfuds_tree::children(const dfuds_tree::dfuds_long &v) const{
//
//    if(!bit_vector[v]) return  0;
//
//    size_t zeros = v - bps.rank(v)+1 ;
//    return select_0(zeros+1) - v;
//}



//inline dfuds_tree::dfuds_long dfuds_tree::leafnum(const dfuds_tree::dfuds_long &v)const {
//    return leafrank( bps.fwd_excess(v-1,-1) +1)-leafrank(v);
//}
//
//inline dfuds_tree::dfuds_long dfuds_tree::leafrank(const dfuds_tree::dfuds_long &v) const {
//    return rank_00(v)+1;
//}

//inline dfuds_tree::dfuds_long dfuds_tree::leafselect(const dfuds_tree::dfuds_long & i) const{
//    return select_00(i);
//}

//inline dfuds_tree::dfuds_long dfuds_tree::child(const dfuds_tree::dfuds_long & v, const dfuds_tree::dfuds_long & t)const {
//    if(!bit_vector[v] /*|| t > children(v)*/) return 0;
//
//    size_t close = bps.find_close(succ0(v)-t);
//
//    if( close == bps.size())return 0;
//
//    return close+1;
//}

//inline dfuds_tree::dfuds_long dfuds_tree::parent(const dfuds_tree::dfuds_long & v) const{
//    ///assert(v >= 3 && v < bit_vector.size());
//    if(v == 3) return 0;
//    /*auto p = bps.find_open(v-1);
//    while(bit_vector[p-1]!=0)--p;
//    return p;*/
//    return pred0( bps.find_open(v-1) )+1;
//
//}
//
//inline dfuds_tree::dfuds_long dfuds_tree::pred0(const dfuds_tree::dfuds_long &i)const {
//    if(bit_vector[i]==0)
//        return i;
//    return select_0(i - bps.rank(i)+1);
//}
//
//inline dfuds_tree::dfuds_long dfuds_tree::succ0(const dfuds_tree::dfuds_long &i)const {
//    size_t zeros = i - bps.rank(i)+1;
//    return select_0(zeros+bit_vector[i]);
//}


size_t dfuds_tree::size_in_bytes() const{

    return  sdsl::size_in_bytes(bit_vector) +
            sdsl::size_in_bytes(bps)        +
            sdsl::size_in_bytes(select_00)  +
            sdsl::size_in_bytes(select_0)   +
            sdsl::size_in_bytes(rank_00)
            ;

}

double dfuds_tree::size_in_mega_bytes() const
{
    return  sdsl::size_in_mega_bytes(bit_vector) +
            sdsl::size_in_mega_bytes(bps)        +
            sdsl::size_in_mega_bytes(select_00)  +
            sdsl::size_in_mega_bytes(select_0)   +
            sdsl::size_in_mega_bytes(rank_00)
            ;
}

void dfuds_tree::print_size_in_bytes(const std::string & s) const {
    std::cout<<s+"tree bitvector \t"<<sdsl::size_in_mega_bytes(bit_vector)<<"(bytes)"<<std::endl;
    std::cout<<s+"\t tree bitvector length\t"<<bit_vector.size()<<std::endl;
    std::cout<<s+"balanced parentheses sequence \t"<<sdsl::size_in_mega_bytes(bps)       <<"(bytes)"<<std::endl;
    std::cout<<s+"\t balanced parentheses sequence \t"<<bps.size()<<std::endl;
    std::cout<<s+"tree bitvector(select_00) \t"<<sdsl::size_in_mega_bytes(select_00) <<"(bytes)"<<std::endl;
    std::cout<<s+"tree bitvector(select_0) \t"<<sdsl::size_in_mega_bytes(select_0)  <<"(bytes)"<<std::endl;
    std::cout<<s+"tree bitvector(rank_00) \t"<<sdsl::size_in_mega_bytes(rank_00)   <<"(bytes)"<<std::endl;
}


void dfuds_tree::save(std::fstream& f)const{
    sdsl::serialize(bit_vector,f);
    sdsl::serialize(bps,f);
    sdsl::serialize(rank_00,f);
    sdsl::serialize(select_00,f);
    sdsl::serialize(select_0,f);

}
void dfuds_tree::load(std::fstream& f){
    sdsl::load(bit_vector,f);
    bps.load(f,&bit_vector);
    sdsl::load(rank_00,f);
    sdsl::load(select_00,f);
    sdsl::load(select_0,f);

    bps =   sdsl::bp_support_sada<>(&bit_vector);
    rank_00 = sdsl::rank_support_v<00,2> (&bit_vector);
    select_00 = sdsl::select_support_mcl<00,2>(&bit_vector);
    select_0  = bv::select_0_type(&bit_vector);

}
//
//inline dfuds_tree::dfuds_long dfuds_tree::rank_1(const dfuds_tree::dfuds_long & i) const{
//    return bps.rank(i);
//}

void dfuds_tree::print() const {

    for (unsigned long i : bit_vector) {
        std::cout<<i;
    }
    std::cout<<std::endl;
    for (unsigned long i : bit_vector) {
        if(i)
            std::cout<<'(';
        else
            std::cout<<')';
    }
    std::cout<<"\n";

}

dfuds_tree &dfuds_tree::operator=(const dfuds_tree& T) {
    bit_vector = T.bit_vector;
    bps =   sdsl::bp_support_sada<>(&bit_vector);
    rank_00 = sdsl::rank_support_v<00,2> (&bit_vector);
    select_00 = sdsl::select_support_mcl<00,2>(&bit_vector);
    select_0  = bv::select_0_type(&bit_vector);
    return *this;
}

//inline dfuds_tree::dfuds_long dfuds_tree::nsibling(const dfuds_tree::dfuds_long & v) const {
//    //if(bit_vector[bps.find_open(v-1)-1] != 1) return 0;
//    return bps.fwd_excess(v-1,-1)+1;
//}

//inline dfuds_tree::dfuds_long dfuds_tree::lchild(const dfuds_tree::dfuds_long & v) const {
//    if(bit_vector[v] == false) return 0;
//    return bps.find_close(v)+1;
//}
//inline dfuds_tree::dfuds_long dfuds_tree::fchild(const dfuds_tree::dfuds_long & v) const {
//    if(bit_vector[v] == false) return 0;
//    return succ0(v)+1;
//}


//inline bool dfuds_tree::is_ancestor(const dfuds_tree::dfuds_long & u,const dfuds_tree::dfuds_long & v) const {
//    return (u < v) && (v < bps.fwd_excess(u-1,-1));
//}
