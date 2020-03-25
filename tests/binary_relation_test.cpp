//
// Created by inspironXV on 8/17/2018.
//
#include <gtest/gtest.h>
#include <benchmark/benchmark.h>
#include <fstream>
#include "collections.cpp"
#include "../binary_relation.h"
#include "../simple_grid.h"

#define pp std::pair<std::pair<size_t,size_t>,std::pair<size_t,size_t>>

simple_grid<
        sdsl::wt_int<>,
        sdsl::int_vector<>,
        sdsl::rrr_vector<>,
        sdsl::rrr_vector<>::select_1_type,
        sdsl::rrr_vector<>::rank_1_type
        > s_grid;

simple_grid<
        sdsl::wt_int<>,
        sdsl::int_vector<>,
        sdsl::sd_vector<>,
        sdsl::sd_vector<>::select_1_type,
        sdsl::sd_vector<>::rank_1_type
> s_grid_sd;

simple_grid<
        sdsl::wm_int<>,
        sdsl::int_vector<>,
        sdsl::rrr_vector<>,
        sdsl::rrr_vector<>::select_1_type,
        sdsl::rrr_vector<>::rank_1_type
> s_grid_wm;



binary_relation bin;
std::vector<binary_relation::point> v;
std::vector<pp> random_q;

static void generate_queries(benchmark::State& state){
    std::srand(std::time(nullptr));
    size_t queries = state.range(0);
    uint points = v.size();

    for (auto _ : state)
    {
        random_q.clear();

        for (int i = 0; i < queries; ++i) {
            uint row1 = std::rand()%points+1;
            uint row2 = std::rand()%points+1;
            uint col1 = std::rand()%points+1;
            uint col2 = std::rand()%points+1;

            if(row1 > row2 )std::swap(row1,row2);

            if(col1 > col2 )std::swap(col1,col2);
            auto p1 = std::make_pair(row1,row2);
            auto p2 = std::make_pair(col1,col2);

            random_q.emplace_back(std::make_pair(p1,p2));
        }

    }
}

static void binary_relation_size(benchmark::State& state)
{
    size_t points = state.range(0);
    for (auto _ : state)
    {
        binary_relation bin;
        std::vector<binary_relation::point> v(points);
        std::srand(std::time(nullptr));

        for (binary_relation::bin_long i = 1; i <= points; ++i)
        {
            ////binary_relation::ulong row = std::rand()%points+1;
            ////binary_relation::ulong tag = std::rand()%points+1;
            binary_relation::point p = std::make_pair(std::make_pair(i,i),i);
            v[i-1] = p;
        }

        bin.build(v.begin(),v.end(),points,points);
        std::fstream fout("bin_rel_"+std::to_string(points),std::ios::out| std::ios::binary);
        bin.save(fout);
        fout.close();
        bin.print_size();
    }
}


static void simple_grid_build(benchmark::State& state)
{
    size_t points = state.range(0);
    for (auto _ : state)
    {
        std::srand(std::time(nullptr));

        for (binary_relation::bin_long i = 1; i <= points; ++i)
        {
            uint row = std::rand()%points+1;
            uint l = std::rand()%points+1;
            binary_relation::point p = std::make_pair(std::make_pair(row,i),l);
//            std::cout<<p.first.first<<","<<p.first.second<<std::endl;
            v.emplace_back(p);
        }


        bin.build(v.begin(),v.end(),points,points);
        bin.print_size();
        s_grid.build(v.begin(),v.end(),points,points);
        s_grid_wm.build(v.begin(),v.end(),points,points);
        s_grid_sd.build(v.begin(),v.end(),points,points);



//        std::fstream fout("bin_rel_"+std::to_string(points),std::ios::out| std::ios::binary);
//        bin.save(fout);
//        fout.close();
//        s_grid.print();
        sleep(4);
    }
}
static void lbr_range_query(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    uint points = v.size(), pocc = 0;

    for (auto _ : state)
    {
        for (int i = 0; i < random_q.size(); ++i) {
            uint row1 = random_q[i].first.first;
            uint row2 = random_q[i].first.second;
            uint col1 = random_q[i].second.first;
            uint col2 = random_q[i].second.second;

            std::vector<std::pair<size_t ,size_t >> X;

            bin.range2(row1,row2,col1,col2,X);
            pocc += X.size();
        }
//        std::vector<uint> R,X1;
//
//        for (int i = 0; i < v.size(); ++i) {
//            if(v[i].first.first  >= row1 &&  v[i].first.first  <= row2 &&
//               v[i].first.second >= col1 &&  v[i].first.second <= col2 )
//                R.emplace_back(v[i].first.second);
//        }
//
//        for (const auto &item : X) {
//            X1.emplace_back(item.second);
//        }
//        std::sort(X1.begin(),X1.end());
//        std::sort(R.begin(),R.end());
//        if(X1!=R){
//            std::cout<<row1<<" "<<row2<<" "<<col1<<" "<<col2<<"\n";
//            X.clear();
//            s_grid.range(row1,row2,col1,col2,X);
//
//        }
//
//
//        ASSERT_EQ(X1,R);

        state.counters["size"] = bin.size_in_bytes();
        state.counters["pocc"] = pocc;
    }
}
static void sg_range_query(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    uint points = v.size(), pocc = 0;

    for (auto _ : state)
    {
        for (int i = 0; i < random_q.size(); ++i) {
            uint row1 = random_q[i].first.first;
            uint row2 = random_q[i].first.second;
            uint col1 = random_q[i].second.first;
            uint col2 = random_q[i].second.second;

            std::vector<std::pair<uint ,uint>> X;

            s_grid.range(row1,row2,col1,col2,X);

            pocc += X.size();
        }
//        std::vector<uint> R,X1;
//
//        for (int i = 0; i < v.size(); ++i) {
//            if(v[i].first.first  >= row1 &&  v[i].first.first  <= row2 &&
//               v[i].first.second >= col1 &&  v[i].first.second <= col2 )
//                R.emplace_back(v[i].first.second);
//        }
//
//        for (const auto &item : X) {
//            X1.emplace_back(item.second);
//        }
//        std::sort(X1.begin(),X1.end());
//        std::sort(R.begin(),R.end());
//        if(X1!=R){
//            std::cout<<row1<<" "<<row2<<" "<<col1<<" "<<col2<<"\n";
//            X.clear();
//            s_grid.range(row1,row2,col1,col2,X);
//
//        }
//
//
//        ASSERT_EQ(X1,R);


    }
    state.counters["size"] = s_grid.size_in_bytes();
    state.counters["pocc"] = pocc;
}
static void sgsd_range_query(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    uint points = v.size();
    uint pocc = 0;

    for (auto _ : state)
    {
        for (int i = 0; i < random_q.size(); ++i) {
            uint row1 = random_q[i].first.first;
            uint row2 = random_q[i].first.second;
            uint col1 = random_q[i].second.first;
            uint col2 = random_q[i].second.second;

            std::vector<std::pair<uint ,uint>> X;

            s_grid_sd.range(row1,row2,col1,col2,X);
            pocc += X.size();
        }
//        std::vector<uint> R,X1;
//
//        for (int i = 0; i < v.size(); ++i) {
//            if(v[i].first.first  >= row1 &&  v[i].first.first  <= row2 &&
//               v[i].first.second >= col1 &&  v[i].first.second <= col2 )
//                R.emplace_back(v[i].first.second);
//        }
//
//        for (const auto &item : X) {
//            X1.emplace_back(item.second);
//        }
//        std::sort(X1.begin(),X1.end());
//        std::sort(R.begin(),R.end());
//        if(X1!=R){
//            std::cout<<row1<<" "<<row2<<" "<<col1<<" "<<col2<<"\n";
//            X.clear();
//            s_grid.range(row1,row2,col1,col2,X);
//
//        }
//
//
//        ASSERT_EQ(X1,R);


    }
    state.counters["size"] = s_grid_sd.size_in_bytes();
    state.counters["pocc"] = pocc;
}
static void sgwm_range_query(benchmark::State& state)
{
    std::srand(std::time(nullptr));

    uint points = v.size();
    uint pocc = 0;
    for (auto _ : state)
    {
        for (int i = 0; i < random_q.size(); ++i) {
            uint row1 = random_q[i].first.first;
            uint row2 = random_q[i].first.second;
            uint col1 = random_q[i].second.first;
            uint col2 = random_q[i].second.second;

            std::vector<std::pair<uint,uint>> X;

            s_grid_wm.range(row1,row2,col1,col2,X);
            pocc += X.size();
        }






//        std::vector<uint> R,X1;
//
//        for (int i = 0; i < v.size(); ++i) {
//            if(v[i].first.first  >= row1 &&  v[i].first.first  <= row2 &&
//               v[i].first.second >= col1 &&  v[i].first.second <= col2 )
//                R.emplace_back(v[i].first.second);
//        }
//
//        for (const auto &item : X) {
//            X1.emplace_back(item.second);
//        }
//        std::sort(X1.begin(),X1.end());
//        std::sort(R.begin(),R.end());
//        if(X1!=R){
//            std::cout<<row1<<" "<<row2<<" "<<col1<<" "<<col2<<"\n";
//            X.clear();
//            s_grid.range(row1,row2,col1,col2,X);
//
//        }
//
//
//        ASSERT_EQ(X1,R);


    }

    state.counters["size"] = s_grid_wm.size_in_bytes();
    state.counters["pocc"] = pocc;
}

//BENCHMARK(binary_relation_size)->Arg(6500000)->Unit(benchmark::kMillisecond);
BENCHMARK(simple_grid_build)  ->Arg(6000000)->Unit(benchmark::kMillisecond);
BENCHMARK(generate_queries)   ->Arg(1000)->Unit(benchmark::kMillisecond);
BENCHMARK(lbr_range_query)    ->Arg(6000000)->Unit(benchmark::kMillisecond);
BENCHMARK(sg_range_query)     ->Arg(6000000)->Unit(benchmark::kMillisecond);
BENCHMARK(sgsd_range_query)   ->Arg(6000000)->Unit(benchmark::kMillisecond);
BENCHMARK(sgwm_range_query)   ->Arg(6000000)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
