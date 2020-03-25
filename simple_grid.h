//
// Created by alejandro on 15-06-19.
//

#ifndef IMPROVED_GRAMMAR_INDEX_SIMPLE_GRID_H
#define IMPROVED_GRAMMAR_INDEX_SIMPLE_GRID_H

#include <sdsl/wavelet_trees.hpp>
#include <vector>

template<
        typename wt = sdsl::wt_int<>,
        typename cvec = sdsl::int_vector<>,
        typename bv = sdsl::bit_vector,
        typename bv_select_1 = sdsl::bit_vector::select_1_type,
        typename bv_rank_1 = sdsl::bit_vector::rank_1_type
                >
class simple_grid {


    typedef std::pair< std::pair< unsigned int,  unsigned int> , unsigned int> point;

protected:

    wt grid;
    cvec SL;
    bv bit_r;
    bv_select_1 bit_r_sel1;
    bv_rank_1 bit_r_rank1;

public:
    simple_grid(){}
    ~simple_grid(){}


    void build( std::vector<point>::iterator,std::vector<point>::iterator ,const uint& , const uint & );

    void range(const uint&,const uint&, const uint&, const uint&, std::vector<std::pair<uint,uint>>& );

    size_t size_in_bytes();

    void save(std::fstream&);

    void load(std::fstream&);

    void print();



    uint n_columns(){ return SL.size();}

    simple_grid<wt,cvec,bv,bv_select_1,bv_rank_1>& operator=( const simple_grid<wt,cvec,bv,bv_select_1,bv_rank_1>&);

    const wt& get_wt();
    const cvec& get_sl();
};



template<typename wt, typename cvec, typename bv, typename bv_select_1, typename bv_rank_1>
void simple_grid<wt, cvec,bv,bv_select_1, bv_rank_1>::build(std::vector<point>::iterator begin, std::vector<point>::iterator end,
        const uint & n_rows, const uint & n_cols)
{
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
     * building bitvector for multicolumns
     *
     * */
    uint n_points = (unsigned long) (end - begin);;

    std::vector<uint> card_rows(n_rows, 0);
    /*
     * Computing the cardinal of every column and every row
     * */
    for (auto i = begin; i != end; ++i) {
        card_rows[i->first.first - 1]++;
    }



    {
        sdsl::bit_vector _XB(n_points + n_rows + 1, 0);
        _XB[0] = true;
        /*
         * Put a 0 in XB for each element in the ith row
         * then add 1
         * */
        size_t p = 1;
        for (uint j = 0; j < n_rows; ++j) {
            _XB[p + card_rows[j]] = true;
            p += card_rows[j] + 1;
        }

        bit_r = bv(_XB);
        bit_r_sel1  = bv_select_1(&bit_r);
        bit_r_rank1 = bv_rank_1(&bit_r);
    }

    /*
     * Build a wavelet_tree on SB( index of the columns not empty ) and plain representation for SL(labels)
     * */


    std::ofstream sb_file("sb_file", std::ios::binary);
    sdsl::int_vector<> _sl(n_points,0);
    sdsl::int_vector<> _sb(n_points,0);
    uint j = 0;
    for (auto i = begin; i != end; ++i) {
        _sl[j] = i->second;
        _sb[j] = i->first.second;
        ++j;
    }

    SL = cvec(_sl);
    sdsl::util::bit_compress(SL);

    sdsl::util::bit_compress(_sb);
    sdsl::serialize(_sb,sb_file);
    sb_file.close();

    std::string id_sb = sdsl::util::basename("sb_file") + "_";
    sdsl::cache_config file_conf_sb(false,"./",id_sb);
    sdsl::construct(grid,"sb_file",file_conf_sb,0);

}
template<typename wt, typename cvec, typename bv, typename bv_select_1, typename bv_rank_1>
void simple_grid<wt, cvec,bv,bv_select_1, bv_rank_1>::range(const uint &r_1, const uint & r_2, const uint & c_1, const uint & c_2, std::vector<std::pair<uint,uint>>& R) {

        uint sr_1 = bit_r_sel1(r_1);
        uint sr_2 = bit_r_sel1(r_2+1);

        int xr_1 = sr_1 - bit_r_rank1(sr_1);
        int xr_2 = sr_2 - bit_r_rank1(sr_2) - 1;

        if( xr_2 < 0 ) return;
//
//        std::cout<<xr_1<<" "<<xr_2<<" "<<c_1<<" "<<c_2<<"\n";

        auto res = grid.range_search_2d2(xr_1,xr_2,c_1,c_2);

        for ( auto point : res.second ){
            R.emplace_back(std::make_pair(point.first,point.second));
        }
}

template<typename wt, typename cvec, typename bv, typename bv_select_1, typename bv_rank_1>
void simple_grid<wt, cvec,bv,bv_select_1, bv_rank_1>::save(std::fstream & f) {
    sdsl::serialize(grid,f);
    sdsl::serialize(SL,f);
    sdsl::serialize(bit_r);
}

template<typename wt, typename cvec, typename bv, typename bv_select_1, typename bv_rank_1>
void simple_grid<wt, cvec,bv,bv_select_1, bv_rank_1>::load(std::fstream & f) {
    sdsl::load(grid,f);
    sdsl::load(SL,f);
    sdsl::load(bit_r);
    bit_r_sel1 = bv_select_1(&bit_r);
}

template<typename wt, typename cvec, typename bv, typename bv_select_1, typename bv_rank_1>
simple_grid<wt, cvec,bv,bv_select_1, bv_rank_1> &simple_grid<wt, cvec,bv,bv_select_1, bv_rank_1>::operator=(const simple_grid<wt, cvec, bv, bv_select_1, bv_rank_1> & G) {

    grid = G.grid;
    SL = G.SL;
    bit_r = G.bit_r;
    bit_r_sel1 = bv_select_1(&bit_r);
    return *this;
}

template<typename wt, typename cvec, typename bv, typename bv_select_1, typename bv_rank_1>
const wt &simple_grid<wt, cvec,bv,bv_select_1, bv_rank_1>::get_wt() {
    return grid;
}

template<typename wt, typename cvec, typename bv, typename bv_select_1, typename bv_rank_1>
const cvec &simple_grid<wt, cvec,bv,bv_select_1, bv_rank_1>::get_sl() {
    return SL;
}

template<typename wt, typename cvec, typename bv, typename bv_select_1, typename bv_rank_1>
size_t simple_grid<wt, cvec,bv,bv_select_1, bv_rank_1>::size_in_bytes() {
    return sdsl::size_in_bytes(grid)+sdsl::size_in_bytes(SL)
    +sdsl::size_in_bytes(bit_r)+sdsl::size_in_bytes(bit_r_sel1);
}

template<typename wt, typename cvec, typename bv, typename bv_select_1, typename bv_rank_1>
void simple_grid<wt, cvec,bv,bv_select_1, bv_rank_1>::print() {
    std::cout<<"wt: ";
    for (int i = 0; i < grid.size(); ++i) {
        std::cout<<grid[i]<<" ";
    }
    std::cout<<"\nlabels: ";
    for (int i = 0; i < SL.size(); ++i) {
        std::cout<<SL[i]<<" ";
    }
    std::cout<<"\nbit_v: ";
    for (int i = 0; i < bit_r.size(); ++i) {
        std::cout<<bit_r[i]<<" ";
    }
    std::cout<<std::endl;
}


#endif //IMPROVED_GRAMMAR_INDEX_SIMPLE_GRID_H
