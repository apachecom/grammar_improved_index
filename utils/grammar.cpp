//
// Created by inspironXV on 8/17/2018.
//

#include <algorithm>
#include <sdsl/int_vector.hpp>
#include <sdsl/wavelet_trees.hpp>
#include <sdsl/inv_perm_support.hpp>
#include <sdsl/lcp_bitcompressed.hpp>
#include <sdsl/rmq_succinct_sada.hpp>

#include "grammar.h"


typedef std::vector<std::pair<uint, uint>> rvect;
typedef std::vector<uint> lvect;

grammar::grammar():_size(0),initial_rule(0) {

}

grammar::~grammar() {

}

void grammar::buildRepair(const std::string &text
#ifdef MEM_MONITOR
    ,mem_monitor& mm
#endif

) {


#ifdef PRINT_LOGS
    std::cout<<"BUILD_CFG_GRAMMAR_1_RE_PAIR"<<std::endl;
#endif

#ifdef MEM_MONITOR
    auto start = timer::now();
    mm.event(BUILD_CFG_GRAMMAR_1_RE_PAIR);
#endif

    unsigned int max_key;
    auto  utext = (u_char *)text.c_str();
    int * ctext;
    unsigned int clength;
    unsigned int  length;
    length = sizeof(u_char) * text.length();
    unsigned char * symbols;
    unsigned int terminals;
    Tdiccarray *dicc; uint cdicc;
    RePair compressor;

    compressor.compress(utext, length, &ctext, &clength, &symbols, &terminals, &dicc, &cdicc);
    uint rules = terminals+cdicc;

    repair_grammar_size = 2*cdicc+terminals;

    std::map<unsigned char,rule::r_long > inv_alp;

    for (int k = 0; k < terminals; ++k) {
        alp[k] = symbols[k];
        inv_alp[symbols[k]] = k;
    }
    _size = 0;

    for (rule::r_long i=0 ; i < terminals ; i++){
        _grammar[i] = rule(i,true);
        _grammar[i]._rule.emplace_back(inv_alp[symbols[i]]);
        _size += 1;
    }
    for (rule::r_long i=0;i<cdicc; i++)
    {
        vector<rule::r_long> rightHand;
        rightHand.emplace_back((rule::r_long)dicc->rules[i].rule.left);
        rightHand.emplace_back((rule::r_long)dicc->rules[i].rule.right);

        _grammar[i+terminals] = rule(i+terminals,false);
        _grammar[i+terminals]._rule = rightHand;
        _size += _grammar[i+terminals]._rule.size();

        if(i+terminals > max_key)
            max_key = i+terminals;
    }

    free(symbols);
    Dictionary::destroyDicc(dicc);
    //initial rule.
    vector<rule::r_long> S;
    S.resize(clength);

    for (uint j = 0; j < clength ; ++j) {
        S[j] = (uint)ctext[j];
    }
    _size += clength;
    if(rules > max_key) max_key = rules;

    _grammar[rules] = rule(rules,false);
    _grammar[rules]._rule = S;
    initial_rule = rules;
    free(ctext);

#ifdef MEM_MONITOR
    auto stop = timer::now();
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_1_RE_PAIR] = duration_cast<microseconds>(stop-start).count();;
#endif

   preprocess(text

#ifdef MEM_MONITOR
           , mm
#endif
   );
}

void grammar::buildBalRepair( const std::string &text,std::fstream & in_grammar,  std::fstream & in_first_rule
#ifdef MEM_MONITOR
        ,mem_monitor& mm
#endif

) {

#ifdef MEM_MONITOR
    auto start = timer::now();
    mm.event(BUILD_CFG_GRAMMAR_1_RE_PAIR);
#endif

    unsigned int max_key;
    auto  utext = (u_char *)text.c_str();

    unsigned int  length;
    length = sizeof(u_char) * text.length();
    unsigned char * symbols;
    unsigned int terminals;


    if (!in_grammar.is_open()) {
        std::cout << "Problem grammar file error\n";
        return;
    }

    //int terminals;

    in_grammar.read((char *) &terminals, sizeof(terminals));

//            std::cout << "alph size" << terminals << "\n";

    symbols = new unsigned char[terminals];

    for (int i = 0; i < terminals; ++i) {
        char b;
        in_grammar.read((char *) &b, sizeof(char));

        symbols[i] = b;
//                std::cout<<symbols[i]<<" ";
    }
//            std::cout<<"\n";

    rvect V;
    lvect L;
//
//            V.resize(terminals);
//            for (int l = 0; l < terminals; ++l) {
//                V[l] = std::make_pair(l, 0);
//            }

    while (!in_grammar.eof()) {

        int p1, p2;
        in_grammar.read((char *) &p1, sizeof(int));
        in_grammar.read((char *) &p2, sizeof(int));
        V.push_back(std::make_pair(p1, p2));


    }
    V.pop_back();
    L.resize(V.size());

    for (int k = 0; k < L.size(); ++k) {
        L[k] = 0;
    }


    std::vector<uint> SS;
    while (!in_first_rule.eof()) {
        int X;
        in_first_rule.read((char *) &X, sizeof(int));
        SS.push_back(X);
    }
    SS.pop_back();
//

//					// Variables for the compressed sequence
    int *ctext = new int[SS.size()];
    uint clength = SS.size();
//            std::cout<<"S: "<<std::endl;
    for (size_t i = 0; i < clength; ++i){
        ctext[i] = SS[i];
//                std::cout<<ctext[i]<<" ";
    }
//            std::cout<<std::endl;
    SS.clear();

    // Variables for the dictionary rules
    uint cdicc = V.size();

//            RePair compressor;
//            compressor.compress(text, length, &ctext, &clength, &symbols, &terminals, &dicc, &cdicc);





    uint rules = terminals+cdicc;

    repair_grammar_size = 2*cdicc+terminals;

    std::map<unsigned char,rule::r_long > inv_alp;

    for (int k = 0; k < terminals; ++k) {
        alp[k] = symbols[k];
//        std::cout<<symbols[k]<<" ";
        inv_alp[symbols[k]] = k;
    }
//    std::cout<<std::endl;

    _size = 0;

    for (rule::r_long i=0 ; i < terminals ; i++){
        _grammar[i] = rule(i,true);
        _grammar[i]._rule.emplace_back(inv_alp[symbols[i]]);
        _size += 1;
    }
    for (rule::r_long i=0;i<cdicc; i++)
    {
        vector<rule::r_long> rightHand;
        rightHand.emplace_back((rule::r_long)V[i].first);
        rightHand.emplace_back((rule::r_long)V[i].second);

        _grammar[i+terminals] = rule(i+terminals,false);
        _grammar[i+terminals]._rule = rightHand;
        _size += _grammar[i+terminals]._rule.size();

        if(i+terminals > max_key)
            max_key = i+terminals;
    }

    free(symbols);
//    Dictionary::destroyDicc(dicc);
    //initial rule.
    vector<rule::r_long> S;
    S.resize(clength);

    for (uint j = 0; j < clength ; ++j) {
        S[j] = (uint)ctext[j];
    }
    _size += clength;
    if(rules > max_key) max_key = rules;

    _grammar[rules] = rule(rules,false);
    _grammar[rules]._rule = S;
    initial_rule = rules;
    free(ctext);
#ifdef MEM_MONITOR
    auto stop = timer::now();
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_1_RE_PAIR] = duration_cast<microseconds>(stop-start).count();;

#endif

    preprocess(text
#ifdef MEM_MONITOR
            , mm
#endif
    );

}
grammar::grammar_iterator grammar::begin() {
    return _grammar.begin();
}

grammar::grammar_iterator grammar::end() {
    return _grammar.end();
}

void grammar::preprocess(const std::string & text

#ifdef MEM_MONITOR
                         ,mem_monitor& mm
#endif
) {

    /*
     * check for every terminal symbol that exist a rule for it
     *
     * */


#ifdef PRINT_LOGS
    std::cout<<"BUILD_CFG_GRAMMAR_2_PREP_1_TERMINAL_RULE"<<std::endl;
#endif
#ifdef MEM_MONITOR
    auto start = timer::now();
    mm.event(BUILD_CFG_GRAMMAR_2_PREP_1_TERMINAL_RULE);
#endif
    rule::r_long max_rule = initial_rule;
    {
        sdsl::bit_vector mark(alp.size(),0);
        for (  auto &&i : _grammar ) {
            if(i.second.terminal)
            {
                mark[i.second._rule[0]] = true;
            }
        }



        if(mark != sdsl::bit_vector(alp.size(),1)){
            for ( rule::r_long i = 0; i < mark.size(); ++i)
            {
                if(!mark[i])
                {
                    _grammar[++max_rule] = rule(max_rule,true);
                    _grammar[max_rule]._rule[0] = alp[i];
                }
            }
        }
    }


#ifdef PRINT_LOGS
    std::cout<<BUILD_CFG_GRAMMAR_2_PREP_1_TERMINAL_RULE<<std::endl;
#endif
#ifdef MEM_MONITOR
    auto stop = timer::now();
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_1_TERMINAL_RULE] = duration_cast<microseconds>(stop-start).count();;

    #endif
    /*
     * REMOVE EVERY RULE WITH LENGHT EQUAL TO 1 WHIT A NO TERMINAL SIMBOL ON THE RIGHT HAND
     * AND ITS OCCURRENCY ARE REPLACED WHIT THE RULE FOR THE TERMINAL GENERATED
     *
     * */
#ifdef PRINT_LOGS
    std::cout<<BUILD_CFG_GRAMMAR_2_PREP_2_REMOVE_RULE_LEN_1<<std::endl;
#endif
#ifdef MEM_MONITOR
    start = timer::now();
    mm.event(BUILD_CFG_GRAMMAR_2_PREP_2_REMOVE_RULE_LEN_1);
#endif
    {
        sdsl::bit_vector mark(max_rule+1,0);
        for (auto &&iter : _grammar)
        {
            if(!iter.second.terminal && iter.second._rule.size() == 1)
                mark[iter.first] = true;
        }

        for (auto &&iter : _grammar)
        {
            rule::r_long j = 0, n = iter.second._rule.size();
            while(j < n)
            {
                rule::r_long r = iter.second._rule[j];
                if(mark[r] == true){
                    iter.second._rule[j] = _grammar[r]._rule[0];
                }
                else{
                    ++j;
                }

            }
        }
        for (auto &&iter : _grammar)
        {
            if(mark[iter.first] == true)
                _grammar.erase(iter.first);
        }
    }

#ifdef MEM_MONITOR
    stop = timer::now();
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_2_REMOVE_RULE_LEN_1] = duration_cast<microseconds>(stop-start).count();;

    #endif

#ifdef PRINT_LOGS
    std::cout<<BUILD_CFG_GRAMMAR_2_PREP_3_GRAMMAR_REPLACE_RULE_OCC_1<<std::endl;
#endif
#ifdef MEM_MONITOR
    start = timer::now();
    mm.event(BUILD_CFG_GRAMMAR_2_PREP_3_GRAMMAR_REPLACE_RULE_OCC_1);
#endif
    /*
     * DELETE EVERY RULE WHERE ITS LEFT HAND NO-TERMINAL ITS METIONED ONLY ONE TIME IN THE GRAMMAR
     * */

    {

        std::map<rule::r_long, rule::r_long> occ;
        for (auto &&iter : _grammar) {
            if (!iter.second.terminal) {
                for (int i = 0; i < iter.second._rule.size(); ++i) {
                    occ[iter.second._rule[i]]++;
                }
            }
        }

        std::set<rule::r_long> visit;
        replace(initial_rule,occ,visit);

        _size = 0;
        for (auto &&iter : _grammar) {
            _size+=iter.second._rule.size();
        }

    }

#ifdef MEM_MONITOR
    stop = timer::now();
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_3_GRAMMAR_REPLACE_RULE_OCC_1] = duration_cast<microseconds>(stop-start).count();;

#endif

    /*
     * Sort the rules by the reverse
     *
     * */


#ifdef PRINT_LOGS
    std::cout<<BUILD_CFG_GRAMMAR_2_PREP_4_CALCULATE_TEXT_OFF_RULE<<std::endl;
#endif
#ifdef MEM_MONITOR
    start = timer::now();
    mm.event(BUILD_CFG_GRAMMAR_2_PREP_4_CALCULATE_TEXT_OFF_RULE);
#endif
    {
        rule::r_long t = 0;
        std::set<rule::r_long >M;
        compute_offset_text(initial_rule,t,M);
        if(_grammar[initial_rule].r + 1 != text.size())
            std::cout<<"grammar error"<<std::endl;

    }

#ifdef MEM_MONITOR
    stop = timer::now();
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_4_CALCULATE_TEXT_OFF_RULE] = duration_cast<microseconds>(stop-start).count();;
#endif


#ifdef PRINT_LOGS
    std::cout<<BUILD_CFG_GRAMMAR_2_PREP_5_BUILD_EDA_SA_LCP_RMQ_SORT<<std::endl;
#endif
#ifdef MEM_MONITOR
    start = timer::now();
    mm.event(BUILD_CFG_GRAMMAR_2_PREP_5_BUILD_EDA_SA_LCP_RMQ_SORT);
#endif


    sdsl::int_vector<> m_SA;
    sdsl::int_vector<> m_ISA;
    sdsl::lcp_bitcompressed<> m_lcp;
    sdsl::rmq_succinct_sada<> m_rmq;

    // Computes the text reverse
    uint32_t text_size = text.length();

    auto *rev_text = new unsigned char[text_size + 1];
    for (uint32_t i = 0; i < text_size; i++) {
        rev_text[i] = text[text_size - i - 1];
    }
    rev_text[text_size] = 0;
    sdsl::cache_config config(false, ".", "cache_reverse");
    sdsl::store_to_file((const char *)rev_text, sdsl::conf::KEY_TEXT);
    sdsl::register_cache_file(sdsl::conf::KEY_TEXT, config);

    sdsl::construct(m_lcp, sdsl::conf::KEY_TEXT, config, 1);
//    std::cout<<"LCP.size()"<<m_lcp.size()<<std::endl;
//    for (uint32_t i = 0; i < m_lcp.size(); i++) {
//         cout << "LCP[i] = " << m_lcp[i] << endl;
//    }

    if (sdsl::cache_file_exists(sdsl::conf::KEY_SA, config)) {
        sdsl::load_from_cache(m_SA, sdsl::conf::KEY_SA, config);
        m_ISA = m_SA;

        for (uint32_t  i = 0; i < m_SA.size(); i++) {
//             cout << "SA[i] = " << m_SA[i] << endl;
            m_ISA[m_SA[i]] = i;
        }
        sdsl::util::clear(m_SA);
    }

    // Builds the RMQ Support.
    m_rmq = sdsl::rmq_succinct_sada<>(&m_lcp);

    sdsl::remove(sdsl::cache_file_name(sdsl::conf::KEY_SA, config));
    sdsl::remove(sdsl::cache_file_name(sdsl::conf::KEY_TEXT, config));
    sdsl::remove(sdsl::cache_file_name(sdsl::conf::KEY_LCP, config));

//    std::string rev_text = text;
//    std::reverse(rev_text.begin(),rev_text.end());
//    sdsl::int_vector<> SA(rev_text.size(),0);
//    sdsl::algorithm::calculate_sa( (unsigned char*)rev_text.c_str(),rev_text.size(),SA);
//
//
//    sdsl::inv_perm_support<> SA_1(&SA);
//    sdsl::lcp_wt<> LCP;
//    sdsl::construct_im(LCP, rev_text.c_str(),sizeof(unsigned char));
//    sdsl::rmq_succinct_sada<true,sdsl::bp_support_sada<>> rmq(&LCP);

    std::vector<rule::r_long> rules(_grammar.size(),0);
    rule::r_long j =0;
    for (  auto &&r: _grammar) {
        rules[j] = r.first;
        ++j;
    }
#ifdef MEM_MONITOR
    stop = timer::now();
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_5_BUILD_EDA_SA_LCP_RMQ_SORT] = duration_cast<microseconds>(stop-start).count();;

#endif



#ifdef PRINT_LOGS
    std::cout<<BUILD_CFG_GRAMMAR_2_PREP_6_SORT_REV_RULES<<std::endl;
#endif
#ifdef MEM_MONITOR
    mm.event(BUILD_CFG_GRAMMAR_2_PREP_6_SORT_REV_RULES);
    start = timer::now();
#endif

    std::sort(rules.begin(),rules.end(),[this,text_size,&rev_text,&m_ISA,&m_lcp,&m_rmq](const rule::r_long & a, const rule::r_long &b )->bool{

        rule::r_long a_pos = text_size - _grammar[a].r - 1;
        rule::r_long b_pos = text_size - _grammar[b].r - 1;

        rule::r_long size_a = _grammar[a].r - _grammar[a].l +1;
        rule::r_long size_b = _grammar[b].r - _grammar[b].l +1;

        if(a_pos == b_pos)
            return size_a < size_b;

        auto sa_1_a = m_ISA[a_pos];
        auto sa_1_b = m_ISA[b_pos];

        int min= m_lcp[m_rmq(std::min(sa_1_a,sa_1_b)+2,std::max(sa_1_a,sa_1_b)+1)];

        if(std::min(size_a,size_b) <= min){
            return size_a < size_b;
        }

        return sa_1_a < sa_1_b;

//        uint32_t rmq;
//        if (m_ISA[a_pos] < m_ISA[b_pos]) {
//            rmq = m_rmq(m_ISA[a_pos] + 1, m_ISA[b_pos]);
//        } else {
//            rmq = m_rmq(m_ISA[b_pos] + 1, m_ISA[a_pos]);
//        }
//        if (size_a <= m_lcp[rmq] && size_b <= m_lcp[rmq]) {
//            return size_a < size_b;
//        } else if (size_a <= m_lcp[rmq]) {
//            return true;
//        } else if (size_b <= m_lcp[rmq]) {
//            return false;
//        } else {
//            /***
//             * Neither is a prefix of the other. Use ISA to find
//             *the order
//             ***/
//            return m_ISA[a_pos] < m_ISA[b_pos];
//        }
    });


#ifdef MEM_MONITOR
    stop = timer::now();
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_6_SORT_REV_RULES] = duration_cast<microseconds>(stop-start).count();;

#endif


#ifdef PRINT_LOGS
    std::cout<<BUILD_CFG_GRAMMAR_2_PREP_7_UPDATE_IDS<<std::endl;
#endif
#ifdef MEM_MONITOR
    start = timer::now();
    mm.event(BUILD_CFG_GRAMMAR_2_PREP_7_UPDATE_IDS);
#endif
    std::vector<rule::r_long> inv_rules(max_rule+1,0);
    for (int k = 0; k < rules.size(); ++k) {
        inv_rules[rules[k]] = k;
    }

    std::map<rule::r_long ,rule> old_g = _grammar;
    _grammar.clear();
    rule::r_long ii = 0;

    for (  auto && i: rules)
    {
        _grammar[ii+1] = old_g[i];
        _grammar[ii+1].id = ii+1;

        if(! (_grammar[ii+1].terminal) )
        {
            for (rule::r_long j = 0; j < _grammar[ii+1]._rule.size();++j) {
                _grammar[ii+1]._rule[j] = inv_rules[_grammar[ii+1]._rule[j]]+1;
            }
        }
        ++ii;
    }


#ifdef PRINT_LOGS
    std::cout<<BUILD_CFG_GRAMMAR_2_PREP_7_UPDATE_IDS<<std::endl;
#endif
#ifdef MEM_MONITOR
    stop = timer::now();
    CLogger::GetLogger()->model[BUILD_CFG_GRAMMAR_2_PREP_7_UPDATE_IDS] = duration_cast<microseconds>(stop-start).count();;
#endif

    ii = 1;
    for (  auto && i: rules)
    {
        if(i == initial_rule)
        {
            initial_rule = ii;
            return;
        }
        ++ii;
    }
}

void grammar::replace(const rule::r_long & id, std::map<rule::r_long, rule::r_long> & occ, std::set<rule::r_long >& M) {

    if(_grammar[id].terminal)return;

    if(M.find(id) != M.end())
        return;

    int i = 0;

    while(i < _grammar[id]._rule.size()){

        uint r =  _grammar[id]._rule[i];
        auto focc = occ.find(r);

        if( focc != occ.end() && focc->second < 2 && _grammar[focc->first].terminal == false){//remove

            auto it = _grammar[id]._rule.begin();
            std::copy(_grammar[r]._rule.begin(),_grammar[r]._rule.end(),std::inserter( _grammar[id]._rule,it+i+1));
            it = _grammar[id]._rule.begin();

            _grammar[id]._rule.erase(it+i);
            _grammar.erase(focc->first);

        }else{

            replace(_grammar[id]._rule[i],occ,M);
            ++i;
        }
    }
    M.insert(id);

}

const rule::r_long& grammar::get_size() {
    return _size;
}

rule::r_long grammar::n_rules() {
    return _grammar.size();
}

rule::r_long grammar::size_in_bytes() {
    return sizeof(rule::r_long)*_size + (sizeof(char)+1)*alp.size() + 1 + _grammar.size()*(3*sizeof(rule::r_long)+1) ;
}

void grammar::print(const std::string& text)
{

    for (auto &&  item: _grammar) {

        std::cout<<item.first<<"->";
        for (auto &&  r: item.second._rule) {
            std::cout<<r<<" ";
        }
        std::cout<<"<"<<item.second.l<<","<<item.second.r<<">";
        std::string s;
        s.resize(item.second.r - item.second.l+1);
        std::copy(text.begin()+item.second.l,text.begin()+item.second.r+1,s.begin());
        std::cout<<"("<<s<<")\n";

    }
}

void grammar::compute_offset_text(const rule::r_long & Xj,  rule::r_long & pos, std::set<rule::r_long>& mark)
{

    if(mark.find(Xj) != mark.end()) {
        pos+=_grammar[Xj].r-_grammar[Xj].l+1;
        return;
    }

    if (_grammar[Xj].terminal == true)
    {
        _grammar[Xj].l =pos;
        _grammar[Xj].r =pos;
        ++pos;
        mark.insert(Xj);
        return;
    }

    _grammar[Xj].l = pos;

    for(int i = 0 ; i < _grammar[Xj]._rule.size(); i++){
        compute_offset_text(_grammar[Xj]._rule[i],pos,mark);
    }
    _grammar[Xj].r =pos-1;
    mark.insert(Xj);

}

const rule::r_long& grammar::get_initial_rule()
{
    return initial_rule;
}

rule::r_long grammar::text_size()
{
    return _grammar[initial_rule].r;
}

void grammar::save(std::fstream &f)
{
    sdsl::serialize(initial_rule,f);
//    std::cout<<"_size:"<<_size<<std::endl;
    sdsl::serialize(_size,f);
    rule::r_long n = alp.size();
    sdsl::serialize(n,f);

    for (auto &&  c: alp) {
        sdsl::serialize(c.first,f);
        sdsl::serialize(c.second,f);
    }

    rule::r_long ng =  _grammar.size();
//    std::cout<<"ng:"<<ng<<std::endl;

    sdsl::serialize(ng,f);

    for (auto && r : _grammar) {
        sdsl::serialize(r.first,f);
        r.second.save(f);
    }

}

void grammar::load(std::fstream &f)
{

    sdsl::load(initial_rule,f);
    sdsl::load(_size,f);

//    std::cout<<"_size:"<<_size<<std::endl;
    rule::r_long n = 0 ;
    sdsl::load(n,f);
    alp.clear();
    for (int i = 0; i < n; ++i) {
        unsigned int key; unsigned char v;
        sdsl::load(key,f);
        sdsl::load(v,f);
        alp[key] = v;
    }
    n = 0;
    sdsl::load(n,f);

//    std::cout<<"ng:"<<n<<std::endl;
    for (int i = 0; i < n; ++i) {
        rule::r_long id;
        sdsl::load(id,f);
        rule r;
        r.load(f);
        _grammar[id] = r;
    }
}

const rule &grammar::operator[](const rule::r_long& id) {
    return _grammar[id];
}

const std::map<unsigned int,unsigned char> &grammar::get_map() const {
    return alp;
}

rule::r_long grammar::num_terminals() const{
    return alp.size();
}

void grammar::write_text(fstream &out) {
    dfs(initial_rule,[&out,this](const rule& X)->bool{
        if(X.terminal){
            out << alp[X._rule[0]];
            return false;
        }
        return true;
    });
}
