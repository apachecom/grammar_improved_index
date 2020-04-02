//
// Created by alpachec on 18-08-18.
//

#ifndef IMPROVED_GRAMMAR_INDEX_TRIE_H
#define IMPROVED_GRAMMAR_INDEX_TRIE_H
#include <map>
#include <fstream>
#include <sdsl/io.hpp>

namespace _trie{




    struct node{

//        typedef unsigned int ulong;
        typedef int key;
        typedef node* pnode;



        uint  id;
        bool leaf;

        std::map<key,pnode> childs;

        node(uint& _id, bool l = false):id(_id), leaf(l)
        {

        }
        ~node()
        {
            for (auto &&item : childs) {
                delete(item.second);
            }
        }

        uint size_in_bytes(){
            return (sizeof(pnode)+sizeof(key))*childs.size() + 1 + sizeof(uint);
        }

        void save(std::fstream& f) const
        {
            sdsl::serialize(id,f);
            sdsl::serialize(leaf,f);
            auto n = childs.size();
            sdsl::serialize(n,f);

            for (auto &&item : childs) {
                sdsl::serialize(item.first,f);
                sdsl::serialize(item.second,f);
            }
        }


    };

    template <typename C>
    class Trie {

        protected:
            node* root;
            uint last_id;
            uint num_nodes;
        public:
            Trie()
            {
                last_id = 0;
                root = new node(last_id);
                num_nodes = 1;
            }
            ~Trie()
            {
                delete root;
            }
            const node* get_root() const
            {
                return root;
            }
            uint get_num_nodes() const
            {
                return num_nodes;
            }
            bool insert(const C& s)
            {
                node* n = root;
                uint pos = 0;
                path(n,s,pos,[this](node* n,const C& s, const uint &pos)->bool
                {

                    node* current = n;
                    for(uint i = pos; i < s.size(); ++i)
                    {
                        current->childs[s[i]] = new node(++last_id);
                        current = current->childs[s[i]];
                        num_nodes++;
                    }
                    current->leaf = true;

                    return true;
                });

                return true;
            }

            template<typename F>
            void dfs(const node* node, const F& f,const node::key& a) const
            {
                if(!f(node,a))
                    return;
                for (auto &&item :node->childs) {
                    dfs(item.second,f,item.first);
                }

            }

            template<typename F>
            void dfs_ba(const node* node, const F& f,const node::key& a) const
            {
                f(node,a,true);

                for (auto &&item :node->childs) {
                    dfs_ba(item.second,f,item.first);
                }

                f(node,a,false);

            }

            void save( std::fstream& f)
            {
                sdsl::serialize(last_id,f);
                sdsl::serialize(num_nodes,f);

                dfs(root,[&f]( const node* n,  const node::key & a)->bool{
                    sdsl::serialize(a,f);
                    n->save(f);
                    return true;

                },'_');
            }

            void print()
            {
                dfs(root,[]( const node* n,  const node::key & a)->bool{
                    std::cout<<"<"<<n->id<<">\n";
                    for (auto &&item :n->childs) {
                        std::cout<<"\t< "<<item.first<<","<<item.second->id<<" >\n";
                    }
                    return true;

                },'_');
            }


        protected:

            template<typename F>
                    bool path(node* node,const C& s, uint& pos, const F& f)
            {
                if(pos == s.size()) return true;

                if(node->childs.find(s[pos]) == node->childs.end()){
                    f(node,s,pos);
                    return false;
                }
                node = node->childs[s[pos]];
                return path(node,s,++pos,f);
            }
    };


}




#endif //IMPROVED_GRAMMAR_INDEX_TRIE_H
