//
// Created by inspironXV on 8/17/2018.
//

#include "compact_patricia_tree.h"

using namespace m_patricia;

m_patricia::compact_patricia_tree::compact_patricia_tree() = default;

m_patricia::compact_patricia_tree::~compact_patricia_tree() = default;

void m_patricia::compact_patricia_tree::build(const m_patricia::compact_patricia_tree::T &tree) {


    size_t n = tree.num_total_nodes();
    size_t m_n = tree.num_leaves_rep();

    sdsl::int_vector<> _jumps((n+m_n)+1,0);
    sdsl::int_vector<> _s((n+m_n)+1,0);
    _s[0] = '_';
    _s[1] = '_';
    sdsl::bit_vector b_temp((n+m_n)*2-1,1);
    size_t   pre = 1,pre_s = 2, off = 0;

    tree.dfs(tree.root(),[&_jumps,&_s,&b_temp,&pre_s,&pre, &off](const m_patricia::node *n, const char &a, const ulong &j)->bool
    {

        size_t ch = n->childs.size();
        size_t  m = n->ids.size();

        /*
         * if there is a duplicated prefix
         *
         * */
        if( m > 1 && ch == 0){
            b_temp[off+m] = false;
            off+=m+1;
            //////_s[pre_s] = a;
            _jumps[pre] = j;
            ++pre;
            //////++pre_s;

            for (size_t  i = 0; i < n->ids.size() ; ++i) {
                b_temp[off] = false;
                _s[pre_s] = 1;//(int)'#';
                _jumps[pre] = 1;
                ++pre;
                ++off;
                ++pre_s;
            }

            return false;
        }

        if(ch == 0)
        {
            b_temp[off] = false;
            off++;
            _jumps[pre] = j;
            ++pre;

            return false;
        }


        b_temp[off+ch] = false;
        off += ch+1;

        for (auto &&item : n->childs) {
            _s[pre_s] = item.first;
            ++pre_s;
        }

        _jumps[pre] = j;
        ++pre;
        return ch != 0;

    },'_',0);

    m_tree.build(b_temp);
    jumps = seq_jmp(_jumps);
    sdsl::util::bit_compress(jumps);
    seq = seq_alf(_s);
    sdsl::util::bit_compress(seq);

}


void m_patricia::compact_patricia_tree::build(const m_patricia::patricia_tree<m_patricia::rev_string_pairs> &tree) {


    size_t n = tree.num_total_nodes();
    size_t m_n = tree.num_leaves_rep();

    sdsl::int_vector<> _jumps((n+m_n)+1,0);
    sdsl::int_vector<> _s((n+m_n)+1,0);
    _s[0] = '_';
    _s[1] = '_';
    sdsl::bit_vector b_temp((n+m_n)*2-1,1);
    size_t   pre = 1,pre_s = 2, off = 0;

    tree.dfs(tree.root(),[&_jumps,&_s,&b_temp,&pre_s,&pre, &off](const m_patricia::node *n, const char &a, const ulong &j)->bool
    {

        size_t ch = n->childs.size();
        size_t  m = n->ids.size();

        /*
         * if there is a duplicated prefix
         *
         * */
        if( m > 1 && ch == 0){
            b_temp[off+m] = false;
            off+=m+1;
           //// _s[pre_s] = a;
            _jumps[pre] = j;
            ++pre;
            ////++pre_s;

            for (size_t  i = 0; i < n->ids.size() ; ++i) {
                b_temp[off] = false;
                _s[pre_s] = 1;
                _jumps[pre] = 1;
                ++pre;
                ++off;
                ++pre_s;
            }

            return false;
        }

        if(ch == 0)
        {
            b_temp[off] = false;
            off++;
            _jumps[pre] = j;
            ++pre;
            return false;
        }


        b_temp[off+ch] = false;
        off += ch+1;

        for (auto &&item : n->childs) {
            _s[pre_s] = item.first;
            ++pre_s;
        }

        _jumps[pre] = j;
        ++pre;
        return ch != 0;

    },'_',0);

    m_tree.build(b_temp);
    jumps = seq_jmp(_jumps);
    sdsl::util::bit_compress(jumps);
    seq = seq_alf(_s);
    sdsl::util::bit_compress(seq);

}

compact_patricia_tree::ulong m_patricia::compact_patricia_tree::node_match(const m_patricia::compact_patricia_tree::K &str) const {
    ulong node = m_patricia::compact_patricia_tree::tree::root();
    compact_patricia_tree::ulong p = 0;
    path(node,str,p);
    return node;
}

compact_patricia_tree::ulong m_patricia::compact_patricia_tree::node_match(const m_patricia::compact_patricia_tree::revK &str) const {
    ulong node = m_patricia::compact_patricia_tree::tree::root();
    compact_patricia_tree::ulong p = 0;
    path(node,str,p);
    return node;
}

compact_patricia_tree::ulong m_patricia::compact_patricia_tree::node_locus(const m_patricia::compact_patricia_tree::K & str, const compact_patricia_tree::ulong & limit, compact_patricia_tree::ulong& pos_locus)const {
    compact_patricia_tree::ulong node = m_patricia::compact_patricia_tree::tree::root();
    pos_locus = 0;
    path(node,str,pos_locus,limit);
    return node;
}
compact_patricia_tree::ulong m_patricia::compact_patricia_tree::node_locus(const m_patricia::compact_patricia_tree::revK & str, const compact_patricia_tree::ulong & limit, compact_patricia_tree::ulong& pos_locus)const
{
    compact_patricia_tree::ulong node = m_patricia::compact_patricia_tree::tree::root();
    pos_locus = 0;
    path(node,str,pos_locus,limit);
    return node;
}

void m_patricia::compact_patricia_tree::save(std::fstream & f) const {
//    std::cout<<"saving cpatricia_tree"<<std::endl;
//    std::cout<<"\t sdsl::serialize(seq,f);"<<std::endl;
    sdsl::serialize(seq,f);
//    std::cout<<"\t sdsl::serialize(jumps,f);"<<std::endl;
    sdsl::serialize(jumps,f);
//    std::cout<<"\t m_tree.save(f);"<<std::endl;
    m_tree.save(f);

}

bool m_patricia::compact_patricia_tree::path(compact_patricia_tree::ulong & node, const m_patricia::compact_patricia_tree::K &str,
                                             compact_patricia_tree::ulong & p) const{

    size_t childrens = m_tree.children(node);

    if (childrens == 0)
    {
        return true ;
    }

    if(p >= str.size())
        return true;

    compact_patricia_tree::ulong l = m_tree.rank_1(node);
    compact_patricia_tree::ulong r = l + childrens ;

    auto t = std::find(seq.begin()+l-1,seq.begin()+r,(unsigned char)str[p]);
//    auto iii = *t;
    if(t > seq.begin()+r)
        return true;
    auto ppp = (compact_patricia_tree::ulong)(t-(seq.begin()+l-1)+1);

    if(ppp > childrens)
        return true;

    size_t child = m_tree.child(node, ppp );

    size_t ii = m_tree.pre_order(child);

    p += jumps[ii];

    node = child;

    return path(node,str,p);

}

bool m_patricia::compact_patricia_tree::path(compact_patricia_tree::ulong & node, const m_patricia::compact_patricia_tree::revK &str,
                                             compact_patricia_tree::ulong & p) const{

    size_t childrens = m_tree.children(node);

    if (childrens == 0)
    {
        return true ;
    }

    if(p >= str.size())
        return true;

    compact_patricia_tree::ulong l = m_tree.rank_1(node);
    compact_patricia_tree::ulong r = l + childrens ;

    auto t = std::find(seq.begin()+l-1,seq.begin()+r,(unsigned char)str[p]);

//    char iii = *t;

    if(t > seq.begin()+r)
        return true;

    auto ppp = (compact_patricia_tree::ulong)(t-(seq.begin()+l-1)+1);

    if(ppp > childrens)
        return true;

    size_t child = m_tree.child(node, ppp );


    size_t ii = m_tree.pre_order(child);

    p += jumps[ii];

    node = child;

    return path(node,str,p);

}

void m_patricia::compact_patricia_tree::path(compact_patricia_tree::ulong & node, const m_patricia::compact_patricia_tree::K & str, compact_patricia_tree::ulong & p, const compact_patricia_tree::ulong &limit) const{

    size_t childrens = m_tree.children(node);

    if (childrens == 0)
        return ;

    if(p > str.size())
        return ;

    compact_patricia_tree::ulong l = m_tree.rank_1(node);
    compact_patricia_tree::ulong r = l + childrens ;

    auto t = std::find(seq.begin()+l-1,seq.begin()+r,(unsigned char)str[p]);
//    char iii = *t;
    if(t > seq.begin()+r)
        return;

    auto ppp = (compact_patricia_tree::ulong)(t-(seq.begin()+l-1)+1);

    if(ppp > childrens)
        return;

    size_t child = m_tree.child(node, ppp );

    size_t ii = m_tree.pre_order(child);



    if( p+jumps[ii] > limit ){

        return ;
    }
    p += jumps[ii];

    node = child;

    path(node,str,p,limit);
}

void m_patricia::compact_patricia_tree::path(compact_patricia_tree::ulong & node, const m_patricia::compact_patricia_tree::revK & str, compact_patricia_tree::ulong & p, const compact_patricia_tree::ulong &limit) const{

    size_t childrens = m_tree.children(node);

    if (childrens == 0)
        return ;


    if(p > str.size())
        return;

    compact_patricia_tree::ulong l = m_tree.rank_1(node);
    compact_patricia_tree::ulong r = l + childrens ;

    auto t = std::find(seq.begin()+l-1,seq.begin()+r,(unsigned char)str[p]);
    //char iii = *t;
    if(t > seq.begin()+r)
        return;

    auto ppp = (compact_patricia_tree::ulong)(t-(seq.begin()+l-1)+1);

    if(ppp > childrens)
        return ;

    size_t child = m_tree.child(node, ppp );

    size_t ii = m_tree.pre_order(child);

    if( p+jumps[ii] > limit ){

        return ;
    }

    p += jumps[ii];

    node = child;

    path(node,str,p,limit);
}

void m_patricia::compact_patricia_tree::load(std::fstream &f) {

    sdsl::load(seq,f);
    sdsl::load(jumps,f);
    m_tree.load(f);

}

const m_patricia::compact_patricia_tree::tree &m_patricia::compact_patricia_tree::get_tree() const {
    return m_tree;
}

compact_patricia_tree::ulong m_patricia::compact_patricia_tree::size_in_bytes() const {

    return m_tree.size_in_bytes() + sdsl::size_in_bytes(jumps) + sdsl::size_in_bytes(seq);

}

void m_patricia::compact_patricia_tree::print_size_in_bytes() const {
    std::cout<<"PATRICIA TREE"<<std::endl;
    std::cout<<"\t tree size "<<m_tree.size_in_bytes()<<std::endl;
    std::cout<<"\t sequence size "<<sdsl::size_in_bytes(seq)<<std::endl;
    std::cout<<"\t tree size "<<sdsl::size_in_bytes(jumps)<<std::endl;

}

compact_patricia_tree &compact_patricia_tree::operator=(const m_patricia::compact_patricia_tree& T) = default;

const compact_patricia_tree::seq_alf &compact_patricia_tree::get_seq() const {
    return seq;
}

const compact_patricia_tree::seq_jmp &compact_patricia_tree::get_jumps() const {
    return jumps;
}

compact_patricia_tree::ulong
compact_patricia_tree::find_child_range(const compact_patricia_tree::ulong & node, const compact_patricia_tree::K & str,
                                        const compact_patricia_tree::ulong & p,const int &cp)
{


    auto childrens = m_tree.children(node);
    compact_patricia_tree::ulong l = m_tree.rank_1(node)-1;
    compact_patricia_tree::ulong r = l + childrens;
    auto mm2 = (unsigned char)str[p];
    auto t = std::lower_bound(seq.begin()+l,seq.begin()+r,mm2);
    unsigned char mm1 = (unsigned char)*t;


    if(mm1 == mm2 && t < seq.begin()+r){
        uint ppp = (t-seq.begin())-l+1;
        auto pchild = m_tree.child(node,ppp);
        if(cp > 0){// patron mayor que indice
            return m_tree.leafrank(pchild)+m_tree.leafnum(pchild) -1;
        }else{
            auto _l = m_tree.leafrank(pchild);
            if (_l == 1) {
                return 1;
            } else {
                return _l - 1;
            }
        }

    }

    if(t >= seq.begin()+r){ // there is not an element greater than str[p]
        return m_tree.leafrank(node) + m_tree.leafnum(node)-1;
    }

    if(t == seq.begin()+l){ // all the elements are greater than str[p]
        auto _l = m_tree.leafrank(node);
        if (_l == 1) {
            return 1;
        } else {
            return _l - 1;
        }
    }

    auto ppp = (compact_patricia_tree::ulong)(t-(seq.begin()+l)+1);

    return m_tree.leafrank( m_tree.child(node,ppp) ) -1;
}

compact_patricia_tree::ulong
compact_patricia_tree::find_child_range(const compact_patricia_tree::ulong & node, const compact_patricia_tree::revK & str,
                                        const compact_patricia_tree::ulong & p,const int &cp) {


    auto childrens = m_tree.children(node);

    compact_patricia_tree::ulong l = m_tree.rank_1(node)-1;
    compact_patricia_tree::ulong r = l + childrens;
    auto mm2 = (unsigned char)str[p];
    auto t = std::lower_bound(seq.begin()+l,seq.begin()+r,mm2);

    unsigned char mm1 = (unsigned char)*t;


    if(mm1 == mm2 && t < seq.begin()+r){
        uint ppp = (t-seq.begin())-l+1;
        auto pchild = m_tree.child(node,ppp);
        if(cp > 0){// patron mayor que indice
            return m_tree.leafrank(pchild)+m_tree.leafnum(pchild) -1;
        }else{
            auto _l = m_tree.leafrank(pchild);
            if (1 == _l) {
                return 1;
            } else {
                return _l - 1;
            }
        }

    }

    if(t >= seq.begin()+r){ // there is not an element greater than str[p]
        return m_tree.leafrank(node) + m_tree.leafnum(node)-1;
    }

    if(t == seq.begin()+l){ // all the elements are greater than str[p]
        auto _l = m_tree.leafrank(node);
        if (1 == _l) {
            return 1;
        } else {
            return _l - 1;
        }
    }

    auto ppp = (compact_patricia_tree::ulong)(t-(seq.begin()+l)+1);

    return m_tree.leafrank( m_tree.child(node,ppp) ) -1;


}
