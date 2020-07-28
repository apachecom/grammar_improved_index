//
// Created by inspironXV on 8/16/2018.
//

#include "binary_relation.h"

binary_relation::binary_relation(const binary_relation &R) {
    SL = R.SL;
    SB = R.SB;
    XA = R.XA;
    XB = R.XB;
    xb_rank1 = bin_bit_vector_xb::rank_1_type(&XB);
    xb_sel1 =  bin_bit_vector_xb::select_1_type(&XB);
    xb_sel0 =  bin_bit_vector_xb::select_0_type(&XB);
    ////xa_sel1 = sdsl::rrr_vector<>::select_1_type(&XA);
    xa_rank1 = bin_bit_vector_xa::rank_1_type(&XA);
}

void binary_relation::build( std::vector<point>::iterator begin,std::vector<point>::iterator end,
 const binary_relation::bin_long & n_rows, const binary_relation::bin_long & n_cols) {

    /*
     * Sort points by rows (rules) then by cols(suffix)
     *
     * */

    sort(begin, end, [](const point &a, const point &b) -> bool
    {
        if ((a.first.first) < (b.first.first)) return true;

        if ((a.first.first) > (b.first.first)) return false;

        return (a.first.second) < (b.first.second);
    });


    /*
     * Creating bit_vectors XA and XB
     *
     * */
    bin_long n_points = (unsigned long) (end - begin);
    {

        std::vector<bin_long> card_rows(n_rows, 0);
        std::vector<bin_long> card_cols(n_cols, 0);
        /*
         * Computing the cardinal of every column and every row
         * */
        for (auto i = begin; i != end; ++i) {
            card_rows[i->first.first - 1]++;
            card_cols[i->first.second - 1]++;
        }

        /*
         * Building bit_vectors XA and XB
         *
         * */

        {
            sdsl::bit_vector xb(n_points + n_rows + 1, false);
            xb[0] = true;
            /*
             * Put a 0 in XB for each element in the ith row
             * then add 1
             * */
            size_t p = 1;
            for (uint j = 0; j < n_rows; ++j) {
                xb[p + card_rows[j]] = true;
                p += card_rows[j] + 1;
            }

            XB = bin_bit_vector_xb(xb);
        }

        {
            sdsl::bit_vector xa(n_points + n_cols + 1, 0);
            xa[0] = true;
            /*
             * Put a 0 in XA for each element in the ith column
             * then add 1
             * */
            size_t p = 1;
            for (uint j = 0; j < n_cols; ++j) {
                xa[p + card_cols[j]] = true;
                p += card_cols[j] + 1;
            }

            XA = bin_bit_vector_xa(xa);
        }
    }
    /*
     * Structures to support rank and select on XA an XB
     * */
    ////xa_sel1 = sdsl::rrr_vector<>::select_1_type(&XA);
    xb_sel1 = bin_bit_vector_xb::select_1_type(&XB);
    xb_sel0 = bin_bit_vector_xb::select_0_type(&XB);

    xa_rank1  = bin_bit_vector_xa::rank_1_type(&XA);
    xb_rank1  = bin_bit_vector_xb::rank_1_type(&XB);

    /*
     * Build a wavelet_tree on SB( index of the columns not empty ) and plain representation for SL(labels)
     * */

    std::ofstream sb_file("sb_file", std::ios::binary);
    sdsl::int_vector<> _sl(n_points,0);
    sdsl::int_vector<> _sb(n_points,0);
    bin_long j = 0;
    for (auto i = begin; i != end; ++i) {
        _sl[j] = i->second;
        _sb[j] = i->first.second;
        ++j;
    }

    SL = compressed_seq(_sl);
    sdsl::util::bit_compress(SL);

    sdsl::util::bit_compress(_sb);
    sdsl::serialize(_sb,sb_file);
    sb_file.close();

    std::string id_sb = sdsl::util::basename("sb_file") + "_";
    sdsl::cache_config file_conf_sb(false,"./",id_sb);
    sdsl::construct(SB,"sb_file",file_conf_sb,0);



    sdsl::wm_int<> SB_wm;
    sdsl::construct(SB_wm,"sb_file",file_conf_sb,0);


//    std::fstream f_wm_int("../files/wts/"+std::to_string(code)+"_wt_file_sb_grid-wm_int",std::ios::out|std::ios::binary);
//    std::fstream f_wt_int("../files/wts/"+std::to_string(code)+"_wt_file_sb_grid-wt_int",std::ios::out|std::ios::binary);


//    sdsl::serialize(SB,f_wt_int);
//    sdsl::serialize(SB_wm,f_wm_int);

}


void binary_relation::range2(binary_relation::bin_long & a1, binary_relation::bin_long & a2, binary_relation::bin_long & b1, binary_relation::bin_long & b2, std::vector<std::pair<size_t, size_t>> &Rel) {

    size_t p1,p2;
    p1 = map(a1);
    p2 = map(a2+1)-1;
    if(p1 > p2) return;
    auto res = SB.range_search_2d2(p1,p2,b1,b2);

    for ( auto point : res.second ){
        Rel.emplace_back(std::make_pair(point.first,point.second));
    }
}


void binary_relation::range(binary_relation::bin_long & a1, binary_relation::bin_long & a2, binary_relation::bin_long & b1, binary_relation::bin_long & b2, std::vector<std::pair<size_t, size_t>> &Rel) {

    size_t p1,p2;
    p1 = map(a1);
    p2 = map(a2+1)-1;
    if(p1 > p2) return;
    auto res = SB.range_search_2d2(p1,p2,b1,b2);

    for ( auto point : res.second ){
        Rel.emplace_back(std::make_pair(unmap(point.first),point.second));
    }
}

binary_relation::bin_long binary_relation::labels(const size_t & a, const size_t & b) const{

    size_t m1 = map(a+1);
    size_t m = map(a);
    size_t p2 = SB.rank(m1,b);
    size_t p1 = SB.rank(m,b);
    if(p1 == p2) return 0;
    return SL[SB.select(p1+1,b)];

}

binary_relation::bin_long binary_relation::first_label_col(const size_t & sufx) const{

//    bin_long cols = xa_rank1(XA.size());
//    assert(0 < sufx && sufx <= cols );
//    if(SB.rank(SB.size(),sufx) == 0){
//        std::cout<<"Error"<<std::endl;
//    }
    return SL[SB.select(1,sufx)];
}

binary_relation::bin_long binary_relation::map(const bin_long & rule) const{
    assert(rule > 0);
    return xb_sel1(rule)-rule+1;
}

binary_relation::bin_long binary_relation::unmap(const bin_long & sufx) const {
    assert( XB.size() - xb_rank1(XB.size()) > sufx);
    return xb_sel0(sufx+1)-sufx;
}

void binary_relation::save(std::fstream & fin) const{

    sdsl::serialize(SB, fin);
    sdsl::serialize(SL, fin);
    sdsl::serialize(XA, fin);
    sdsl::serialize(XB, fin);

    sdsl::serialize(xb_rank1, fin);
    sdsl::serialize(xb_sel0, fin);
    sdsl::serialize(xb_sel1, fin);
    sdsl::serialize(xa_rank1, fin);
}

void binary_relation::load(std::fstream & fout) {
    sdsl::load(SB,fout);
    sdsl::load(SL,fout);
    sdsl::load(XA,fout);
    sdsl::load(XB,fout);
    sdsl::load(xb_rank1,fout);
    sdsl::load(xb_sel0,fout);
    sdsl::load(xb_sel1,fout);
    sdsl::load(xa_rank1,fout);

    xb_sel1 = bin_bit_vector_xb::select_1_type(&XB);
    xb_sel0 = bin_bit_vector_xb::select_0_type(&XB);
    xa_rank1  = bin_bit_vector_xa::rank_1_type(&XA);
    xb_rank1  = bin_bit_vector_xb::rank_1_type(&XB);
}
#ifdef DEBUG
void binary_relation::print_size() {
    std::cout<<"SB "<<sdsl::size_in_mega_bytes(SB) << std::endl;
    std::cout<<"\t SB length"<<SB.size()<< std::endl;
    std::cout<<"\t SB size alphabet sigma "<<SB.sigma<<std::endl;
    std::cout<<"SL "<<sdsl::size_in_mega_bytes(SL) << std::endl;
    std::cout<<"\t SL length "<<SL.size()<< std::endl;
    std::cout<<"XA "<<sdsl::size_in_mega_bytes(XA) << std::endl;
    std::cout<<"XB "<<sdsl::size_in_mega_bytes(XB) << std::endl;
    std::cout<<"XB rank 1 "<<sdsl::size_in_mega_bytes(xb_rank1)  << std::endl;
    std::cout<<"XB select 0 "<<sdsl::size_in_mega_bytes(xb_sel0) << std::endl;
    std::cout<<"XB select 1 "<<sdsl::size_in_mega_bytes(xb_sel1) << std::endl;
    std::cout<<"XA rank 1 "<<sdsl::size_in_mega_bytes(xa_rank1)  << std::endl;
    std::cout<<"total size of grid 2d range search************"<<size_in_bytes()*1.0/1024*1024<< std::endl;
}
#endif

binary_relation::bin_long binary_relation::n_columns() const {
    return xa_rank1(XA.size())-1;
}

unsigned long long binary_relation::size_in_bytes() const {
    return
            sdsl::size_in_bytes(SB) +
            sdsl::size_in_bytes(SL) +
//            sdsl::size_in_bytes(XA) +
            sdsl::size_in_bytes(XB) +
//            sdsl::size_in_bytes(xb_rank1)  +
            sdsl::size_in_bytes(xb_sel0) +
//            sdsl::size_in_bytes(xb_sel1) +
            sdsl::size_in_bytes(xa_rank1) ;
}

binary_relation &binary_relation::operator=(const binary_relation & R) {
    XB = R.XB;
    XA = R.XA;
    SB = R.SB;
    SL = R.SL;

    xb_sel1 = bin_bit_vector_xb::select_1_type(&XB);
    xb_sel0 = bin_bit_vector_xb::select_0_type(&XB);
    xa_rank1  = bin_bit_vector_xa::rank_1_type(&XA);
    xb_rank1  = bin_bit_vector_xb::rank_1_type(&XB);

    return *this;
}


