//
// Created by alpachec on 18-08-18.
//

#ifndef IMPROVED_GRAMMAR_INDEX_PATRICIA_TREE_H
#define IMPROVED_GRAMMAR_INDEX_PATRICIA_TREE_H


#include <utility>
#include <map>
#include <vector>
#include <cassert>
#include <sdsl/io.hpp>
#include <climits>

namespace m_patricia {



    struct node {

        typedef unsigned int ulong;
        typedef std::pair<ulong, node *> edge;

        ulong id;
        ulong ptr_leaf;
        bool leaf;
        char ch{};

        std::multimap<unsigned char, edge> childs;
        std::vector<ulong> ids;


        node(const ulong &id, const ulong &leaf, bool l = false, ulong m = 1) : id(id), ptr_leaf(leaf), leaf(l) {
            childs.clear();
            ids.clear();
            ids.emplace_back(id);
        }

        ~node() {
            for (auto &&item : childs) {
                delete item.second.second;
            }
        }

        ulong size_in_bytes()const {
            return sizeof(ulong) * 2 + childs.size() * 2 + ids.size() + 2;
        }

        void save(std::fstream& f)const{

            sdsl::serialize( id,f);
            sdsl::serialize( ptr_leaf,f);
            sdsl::serialize(leaf,f);
            sdsl::serialize(ch,f);

            sdsl::serialize(childs.size(),f);
            for (auto &&item : childs) {
                sdsl::serialize(item.first,f);
                sdsl::serialize(item.second.first,f);
                sdsl::serialize(item.second.second,f);
            }

            for (auto &&item : ids) {
                sdsl::serialize(item,f);

            }
        }

    };

    template<typename C>
    class patricia_tree {

    protected:

        node *_root;

        std::vector<C> dicc;

        node::ulong last_id;


    public:

        patricia_tree() : last_id(0) {
            _root = new node(last_id, 0, false);
        }

        ~patricia_tree() {
            if (_root != nullptr) {
                delete (_root);
                dicc.clear();
            }
        }

        /*
         * Insert an object in the tree
         * */
        bool insert(const C &s) {
            ulong pos = 0;
            auto match_node = find_match_node(_root, pos, s);

            if (match_node == _root) {
                dicc.emplace_back(s);
                node *new_leaf = new node(++last_id, dicc.size() - 1, true);
                node::edge e = std::make_pair(1, new_leaf);
                _root->childs.insert(std::make_pair(s[0], e));
                return true;
            }

            ulong real_pos_match = 0;
            auto s_match = dicc[match_node->ptr_leaf];
            while (real_pos_match < s.size() && real_pos_match < s_match.size() &&
                   s[real_pos_match] == s_match[real_pos_match])
                ++real_pos_match;


            if (real_pos_match == s.size() && real_pos_match == s_match.size()) { // the object already exists
                if (match_node->childs.size() == 1) {
                    match_node->ids.emplace_back((dicc[match_node->ptr_leaf]).to_ulong());
                }
                match_node->ids.emplace_back(s.to_ulong());
                return true;
            }

            ulong locus_pos = 0;
            auto locus = find_locus(_root, locus_pos, real_pos_match, s);

            if (real_pos_match == locus_pos) {
                dicc.emplace_back(s);
                node::edge e = std::make_pair(1, new node(++last_id, dicc.size() - 1, true));
                locus->childs.insert(std::make_pair(s[real_pos_match], e));
                return true;
            }


            auto it = locus->childs.find(s_match[locus_pos]);
            auto oldNode = it->second.second;

            if (locus_pos < real_pos_match && real_pos_match < locus_pos + it->second.first) {

                node *new_intern = new node(++last_id, oldNode->ptr_leaf, false);
                node::edge e_old = std::make_pair(locus_pos + it->second.first - real_pos_match, oldNode);
                new_intern->childs.insert(std::make_pair(s_match[real_pos_match], e_old));

                dicc.emplace_back(s);
                node *new_leaf = new node(++last_id, dicc.size() - 1, true);
                node::edge e_new = std::make_pair(1, new_leaf);
                new_intern->childs.insert(std::make_pair(s[real_pos_match], e_new));

                it->second.first = real_pos_match - locus_pos;
                it->second.second = new_intern;

                return true;
            }

            if (locus_pos < real_pos_match && real_pos_match >= locus_pos + it->second.first && oldNode->leaf) {

                node *new_intern = new node(++last_id, oldNode->ptr_leaf, false);
                node::edge e_old = std::make_pair(1, oldNode);
                new_intern->childs.insert(std::make_pair(s_match[real_pos_match], e_old));

                dicc.emplace_back(s);
                node *new_leaf = new node(++last_id, dicc.size() - 1, true);
                node::edge e_new = std::make_pair(1, new_leaf);
                new_intern->childs.insert(std::make_pair(s[real_pos_match], e_new));

                it->second.first = real_pos_match - locus_pos;
                it->second.second = new_intern;

                return true;
            }

            return false;

        }

        /*
         * Returns a const reference to the root;
         * */
        const node *root() const {
            return _root;
        }

        /*
         * Provide access to the dictionary
         * */
        const C &operator[](const ulong &i) {
            return dicc[i];
        }

        /*
         * Provide DFS method over the tree
         * */
        template<typename F>
        void dfs(const node *n, const F& f, const char &a, const ulong &j) const {
            if (!f(n, a, j))return;
            for (auto &&item :n->childs) {
                dfs(item.second.second, f, item.first, item.second.first);
            }
        }

        /*
         * Returns the number of repeted leaves
         * */
        size_t num_leaves_rep() const {

            size_t _num_leaves = 0;
            dfs(_root, [&_num_leaves, this](const node *n, const char &a, const ulong &j) {
                if (n->childs.empty() && n->ids.size() > 1 ) {
                    _num_leaves++;
                    return false;
                }
                return true;
            }, '_', 0);

            return _num_leaves;
        }
        /*
         * Returns the number of leaves
         * */
        size_t num_leaves() const{

            size_t _num_leaves = 0;
            dfs(_root, [&_num_leaves, this](const node *n, const char &a, const ulong &j) {
                if (n->childs.empty()) {
                    _num_leaves += n->ids.size();
                    // num_leaves++;
                    return false;
                }
                return true;
            }, '_', 0);

            return _num_leaves;
        }

        /*
         * Returns the total of nodes in the tree
         * */
        size_t num_total_nodes() const {
            size_t num = 0;
            dfs(_root, [&num, this](const node *n, const char &a, const ulong &j) {

                if (n->childs.empty() && !n->ids.empty()) {
                    num += n->ids.size();
                    return false;
                }
                num++;
                return true;
            }, '_', 0);
            return num;
        }

        /*
         * Returns the number of internal nodes in the tree
         * */
        size_t num_internal_nodes() const {
            size_t num = 0;
            dfs(_root, [&num, this](const node *n, const char &a, const ulong &j) {
                if (!(n->childs.empty())) {
                    num++;
                    return true;
                }

                return false;
            }, '_', 0);
            return num;
        }

        /*
         * Clear the structure destroying al the nodes
         * */
        void clear() {
            delete _root;
            dicc.clear();
            _root = new node(last_id, 0, false);
        }

        /*
         * Print the tree
         * */
        void print() {
            dfs(_root, [this](const node *n, const char &a, const ulong &j) {

                std::cout<<"<"<<n->id<<">\n";
                for (auto &&item :n->childs) {
                    std::cout<<"\t< "<<item.first<<"("<<uint(item.first)<<"),<"<<item.second.first<<", "<<item.second.second->id<<"> >\n";
                }
                if(n->childs.empty())
                    std::cout<<"("<<n->ids.size()<<")\n";

                return true;

            }, '_', 0);
        }

        /*
         * Return the size of the structure in bytes
         * */
        ulong size_in_bytes() {
            ulong num = 0;
            dfs(_root, [&num, this](const node *n, const char &a, const ulong &j)->bool{
                num += n->size_in_bytes();
                return true;
            }, '_', 0);
            return num;
        }

        void save(std::fstream& f)
        {
            sdsl::serialize(_root,f);
            sdsl::serialize(last_id,f);
            /*for (auto &&item : dicc) {
                sdsl::serialize(item,f);
            }*/

            dfs(root(),[&f](const node *n, const char &a, const ulong &j)->bool{
                n->save(f);
                return true;
            },'_',0);

        }

    protected:

        node *find_match_node(node *n, ulong &pos, const C &s) {
            if (n->leaf) return n;

            if (pos > s.size()) return n;

            auto it = n->childs.find((unsigned char) s[pos]);

            if (it == n->childs.end()) return n;

            pos += it->second.first;

            return find_match_node(it->second.second, pos, s);
        }


        node *find_locus(node *n, ulong &pos, const ulong &limit, const C &s) {

            assert(pos <= limit && pos <= s.size() && !n->leaf);

            // if(n->leaf) return n;

            if (pos == limit) return n;

            auto it = n->childs.find((unsigned char) s[pos]);

            if (it == n->childs.end()) return n;

            if (it->second.second->childs.empty()) {
                return n;
            }

            if (pos + it->second.first > limit)
                return n;

            pos += it->second.first;

            return find_locus(it->second.second, pos, limit, s);

        }


    };


    class string_pairs:std::pair<size_t ,size_t >{
        const std::string* text;
        uint id;
        uint c{};

    public:
        string_pairs():text(nullptr),id(0),c(0){
            first = 0;
            second = 0;
        }
        ~string_pairs() = default;

        string_pairs(const std::string& _text, uint _id):text(&_text),id(_id){
            first = 0;
            second = 0;
        }

        string_pairs(const string_pairs& P) : pair(P) {
            text = P.text;
            id = P.id;
            c = P.c;
            first = P.first;
            second = P.second;
        }

        string_pairs& operator=(const string_pairs& s){
            text = s.text;
            id = s.id;
            c = s.c;
            first = s.first;
            second = s.second;
            return *this;
        }

        uint get_id() const{ return id;}
        void set_left(size_t t){ first = t;}
        void set_right(size_t t){ second = t;}
        size_t get_left(){ return first;}
        size_t get_right(){ return second;}

        char operator[](size_t i) const{

            assert(i>=0 && i <= size());

            if(i == size())
                return 0;
            return (*text)[first + i];
            /*
            auto j = i/(sizeof(char)*CHAR_BIT);
            auto r = i%(sizeof(char)*CHAR_BIT);

            auto _ch =(*text)[first+j];

            auto pot = 1 << (sizeof(_ch)*CHAR_BIT-1);
            auto tt = (pot >> r);
            auto yy = _ch & (pot >> r);
            /// auto t = (_ch & (1 << r));

            return (_ch & (pot >> r))?'1':'0';
             */
        }

        std::string toString() const{

            std::string s;
            s.resize(second-first+1);
            std::copy(text->begin()+first,text->begin()+second+1,s.begin());
            // s+= "(" + std::to_string(c) + ")";
            return s;
        }


        void operator++(){c++;}

        size_t size() const {
            return (second-first+1);
            ///return (second-first+1)*sizeof(char)*CHAR_BIT;
        }


        bool empty(){ return second < first;}

        ulong to_ulong() const { return id;}


    };



    class rev_string_pairs:std::pair<size_t ,size_t >{
        const std::string* text;
        uint id;
        uint c{};

    public:
        rev_string_pairs():text(nullptr),id(0),c(0){
            first = 0;
            second = 0;
        }
        ~rev_string_pairs() = default;

        rev_string_pairs(const std::string& _text, uint _id):text(&_text),id(_id){
            first = 0;
            second = 0;
        }

        rev_string_pairs(const rev_string_pairs& P) : pair(P) {
            text = P.text;
            id = P.id;
            c = P.c;
            first = P.first;
            second = P.second;
        }

        rev_string_pairs& operator=(const rev_string_pairs& s){
            text = s.text;
            id = s.id;
            c = s.c;
            first = s.first;
            second = s.second;
            return *this;
        }

        uint get_id() const{ return id;}
        void set_left(size_t t){ first = t;}
        void set_right(size_t t){ second = t;}
        size_t get_left(){ return first;}
        size_t get_right(){ return second;}

        char operator[](size_t i) const{

            assert(i>=0 && i <= size());

            if(i == size())
                return 0;
            return (*text)[second - i];
            /*
            auto j = i/(sizeof(char)*CHAR_BIT);
            auto r = i%(sizeof(char)*CHAR_BIT);

            auto _ch =(*text)[first+j];

            auto pot = 1 << (sizeof(_ch)*CHAR_BIT-1);
            auto tt = (pot >> r);
            auto yy = _ch & (pot >> r);
            /// auto t = (_ch & (1 << r));

            return (_ch & (pot >> r))?'1':'0';
             */
        }

        std::string toString() const{

            std::string s;
            s.resize(second-first+1);
            std::copy(text->begin()+first,text->begin()+second+1,s.begin());
            std::reverse(s.begin(),s.end());
            // s+= "(" + std::to_string(c) + ")";
            return s;
        }


        void operator++(){c++;}

        size_t size() const {
            return (second-first+1);
            ///return (second-first+1)*sizeof(char)*CHAR_BIT;
        }


        bool empty(){ return second < first;}

        ulong to_ulong() const { return id;}


    };






}

#endif //IMPROVED_GRAMMAR_INDEX_PATRICIA_TREE_H
