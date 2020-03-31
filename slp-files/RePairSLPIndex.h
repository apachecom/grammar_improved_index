/* RePairSLPIndex.h
 * Copyright (C) 2010, Miguel A. Martinez-Prieto
 * all rights reserved.
 *
 * SLP-based index on Re-Pair compression based on:
 *    Francisco Claude and Gonzalo Navarro.
 *    Self-Indexed Grammar-Based Compression.
 *    Fundamenta Informaticae
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *
 * Contacting the author:
 *   Miguel A Martinez-Prieto:  migumar2@infor.uva.es
 */


#ifndef _REPAIRSLPINDEX_H
#define _REPAIRSLPINDEX_H

#include <algorithm>
#include <stdio.h>

#include "RePair/RePair.h"
#include "BinRel/BinaryRelation.h"
#include "BinRel/Point.h"
#include "BinRel/RuleTrav.h"
#include "BinRel/FullQuery.h"
#include "Utils/SuffixArray.h"
#include "Utils/deltacodes.h"
#include "Samplings/sampling.h"

#include <factorization.h>
#include <factorization_var.h>
#include <PermutationBuilder.h>

typedef std::vector<std::pair<uint, uint>> rvect;
typedef std::vector<uint> lvect;

using namespace cds_utils;


uint calc_len(const uint &X, rvect &D, lvect &L, const int & t) {

    if (X < t) return 1;

    if (L[X-t] != 0)
        return L[X-t];

//
//    L[D[X-t].first - t] = calc_len(D[X-t].first, D, L,t);
//    L[D[X-t].second - t] = calc_len(D[X-t].second, D, L,t);

    L[X -t] = calc_len(D[X-t].first, D, L,t) + calc_len(D[X-t].second, D, L,t);
    return L[X -t];
}


namespace cds_static {
    /** RePairSLPIndex
     *   Implements a compressed index on an Straight Line Program (SLP)
     *   representation of the text. The SLP is obtained with the Re-Pair
     *   algorithm by following the work of Claude & Navarro.
     *
     *   @author Miguel A. Martinez-Prieto
     */
    class RePairSLPIndex {
        static const size_t PROOF = 100;
        static const size_t NOTFOUND = 0;
        static const size_t LOCATE_SIZE = 32768;
        static const uint BLOCK_SAMPLING = 16;

        static const size_t DELTA_SAMPLING = 16;
        static const size_t PERM_EPS = 8;

    public:
        /** Generic constructor */
        RePairSLPIndex();

        void build(uchar *text, uint length, uint sampling_length, char *filename);

        void build(uchar *text, uint length, uint sampling_length, char *filename, std::fstream &in_grammar,
                   std::fstream &in_first_rule) {

            if (!in_grammar.is_open()) {
                std::cout << "Problem grammar file error\n";
                return;
            }

            //int terminals;

            in_grammar.read((char *) &terminals, sizeof(terminals));

//            std::cout << "alph size" << terminals << "\n";

            unsigned char *symbols = new unsigned char[terminals];

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

            for (int j = 0; j < V.size(); ++j){
                calc_len(j+terminals, V, L,terminals);
//                std::cout<<j+terminals<<"-> ( "<<V[j].first<<", "<<V[j].second<<")"<<L[j]<<std::endl;
            }


            std::vector<uint> S;
            while (!in_first_rule.eof()) {
                int X;
                in_first_rule.read((char *) &X, sizeof(int));
                S.push_back(X);
            }
            S.pop_back();
//
			this->filename = string(filename);
            textlength = length;

//					// Variables for the compressed sequence
            int *ctext = new int[S.size()];
            uint clength = S.size();
//            std::cout<<"S: "<<std::endl;
            for (size_t i = 0; i < clength; ++i){
                ctext[i] = S[i];
//                std::cout<<ctext[i]<<" ";
            }
//            std::cout<<std::endl;
            S.clear();

            // Variables for the dictionary rules
           uint cdicc = V.size();

//            RePair compressor;
//            compressor.compress(text, length, &ctext, &clength, &symbols, &terminals, &dicc, &cdicc);
            uint rules = terminals+cdicc;

            // *************************************************************
            // Lexicographic dictionary ordering
            //  - The sa_inv vector stores the lexicographic order of each
            //    in the position (it uses the text suffix array)
            //  - A new auxiliar vector (of Rules) joins symbol and pair
            //    dictionaries. Their first ocurrence is update by using the
            //	  information in sa_inv.
            // *************************************************************
            uint maxlenrule = 1;
            vector<Rule> grammar(rules+1);

            for (uint i=0;i<terminals;i++)
                grammar[i+1] = Rule((int)symbols[i], -1, i+1, 1);

            for (uint i=0;i<cdicc; i++)
            {
//                grammar[i+terminals+1] = Rule(dicc->rules[i].rule.left+1, dicc->rules[i].rule.right+1, i+terminals+1, dicc->rules[i].len);
                grammar[i+terminals+1] = Rule(V[i].first + 1, V[i].second + 1, i+terminals+1, L[i]);
                if (grammar[i+terminals+1].len > maxlenrule) maxlenrule = grammar[i+terminals+1].len;
            }

            free(symbols);
//            Dictionary::destroyDicc(dicc);

            for (uint i=1; i<=rules; i++)
            {
                char *s = new char[grammar[i].len+1];
                expandRule(grammar[i], s, &grammar);
                s[grammar[i].len] = '\0';
                grammar[i].sequence = s;
            }

            sort (grammar.begin()+1, grammar.end(), sorting);
            std::cout<<"Gramatica ordenada\n";
//            for (int m = 0; m < rules; ++m) {
//                grammar[m+1].print();
//            }

            // *************************************************************
            // Building an 'inverted' grammar to update dictionary and
            // sequence with the new ordering.
            // The lenrules structure (on DAC) and the bitsequences C and Y
            // are obtained in this process.
            // *************************************************************
            uint *grammar_inv = new uint[rules+1];
            uint *lengths = new uint[rules];

            BitString *bsC = new BitString(256);
            Y = new uint[terminals];
            uint cterms = 0;

            for (uint i=1; i<=rules; i++)
            {
                grammar_inv[grammar[i].id] = i;
                grammar[i].id = i;

                lengths[i-1] = grammar[i].len;

                if (grammar[i].len == 1)
                {
                    bsC->setBit(grammar[i].left,true);

                    Y[cterms] = i;
                    cterms++;
                }
            }



            lenrules = new factorization(lengths, terminals+cdicc);
            C = new BitSequenceRG(*bsC,20);

            std::cout<<"Creadas estructura lenrules y C\n";

//            std::cout<<"lengths: ";
//            for (int k = 0; k < rules; ++k) {
//                std::cout<<lengths[k]<< " ";
//            }
//            std::cout<<std::endl;
//            std::cout<<"C: "<<std::endl;
//            for (int n = 0; n < 256; ++n) {
//                std::cout<<C->access(n);
//            }std::cout<<std::endl;
//            std::cout<<"Y: ";
//            for (int n = 0; n < terminals; ++n) {
//                std::cout<<Y[n]<<" ";
//            }std::cout<<std::endl;
//

//            std::cout<<"lenrules: ";
//            for (int k = 0; k < rules; ++k) {
//                std::cout<<lenrules->access(k+1)<< " ";
//            }
//            std::cout<<std::endl;

            delete [] lengths;
            delete bsC;

            // *************************************************************
            // Updating symbols in the dictionary and the sequence.
            // *************************************************************
            for (uint i=1; i<=rules; i++)
            {
                if (grammar[i].len != 1)
                {
                    grammar[i].left = grammar_inv[grammar[i].left];
                    grammar[i].right = grammar_inv[grammar[i].right];
                }
            }

            for (uint i=0; i<clength; i++)
                ctext[i] = grammar_inv[ctext[i]+1];

            // *************************************************************
            // Reverse dictionary ordering
            // *************************************************************
            vector<Rule> grammar_rev(rules+1);
            char *xrule = new char[maxlenrule+1];

            for (uint i=1; i<=rules; i++)
            {
                grammar_rev[i] = grammar[i];

                for (uint j=grammar_rev[i].len; j>0; j--)
                    xrule[grammar_rev[i].len-j] = grammar_rev[i].sequence[j-1];
                xrule[grammar_rev[i].len] = '\0';

                strcpy(grammar_rev[i].sequence, xrule);
            }

            sort(grammar_rev.begin()+1, grammar_rev.end(), sorting);

            std::cout<<"Gramatica inversa ordenada\n";

            delete [] xrule;

            // *************************************************************
            // Obtaining the permutation from reverse to direct ordering
            // *************************************************************
            uint bitsperm = bits(rules+1);
            uint sizeperm = (int)ceil(double(bitsperm*(rules+1))/W);

            uint *perm = new uint[sizeperm];
            for (uint i=0; i<sizeperm; i++) perm[i] = 0;
            for (uint i=1; i<=rules; i++) set_field(perm, bitsperm, i, grammar_rev[i].id);

            PermutationBuilder *pb = new PermutationBuilderMRRR(PERM_EPS, new BitSequenceBuilderRG(20));
            permutation = pb->build(perm, rules+1);

            delete pb;
            std::cout<<"Permutacion entre gramaticas creadas\n";
            // *************************************************************
            // Updating the 'inverted' grammar to obtain the final grammar
            // used to build the binary relation.
            // *************************************************************
            for (uint i=1; i<=rules; i++)
                grammar_inv[grammar_rev[i].id] = i;
            for (uint i=1; i<=rules; i++)
                if (grammar[i].len != 1) grammar[i].left = grammar_inv[grammar[i].left];

            delete [] grammar_inv;
            for (uint i=1; i<=terminals+cdicc; i++) delete [] grammar[i].sequence;

            slp = new LabeledBinaryRelation(&grammar, cdicc, rules, rules, false);
            std::cout<<"slp creado\n";

            // *************************************************************
            // Building structures for the compressed sequence
            //  - Compressed sequence: symbols + start positions
            //  - prefix/Suffix binary relation for the compressed sequence
            // *************************************************************
            uint *lengthsSeq = new uint[clength];

            uint current = 0;
            for (uint i=0; i<clength; i++)
            {
                lengthsSeq[i] = lenrules->access(ctext[i]);
                current += lengthsSeq[i];
            }

            seqB = new DeltaCodes(lengthsSeq, clength, DELTA_SAMPLING);
            delete [] lengthsSeq;
            std::cout<<"SeqB Creada\n";
            // Building the text suffix array
            SuffixArray *sa = new SuffixArray();
            int * sa_ord = sa->sort(text, textlength);
            delete sa; delete [] text;
            std::cout<<"Suffix Array Creado\n";
            // Retrieving "valid" suffixes for start positions in the
            // compressed sequence
            int * suffixes = new int[clength];
            uint csuff = 1;

            for (uint i=0; i<=textlength; i++)
            {
                if (sa_ord[i] != 0)
                {
                    // The first position in the compressed sequence is not
                    // considered as suffix for the binary relation

                    // Binary search
                    uint l = 0, r = clength-1, c;
                    int val;
                    while (l <= r)
                    {
                        c = (l+r)/2;
                        val = seqB->select(c)+1;

                        if (val <  sa_ord[i]) l = c+1;
                        else
                        {
                            if (val > sa_ord[i]) r = c-1;
                            else break;
                        }
                    }

                    if (l <= r)
                    {
                        suffixes[csuff] = sa_ord[i];
                        csuff++;
                    }
                }
            }

            free(sa_ord);

            int * suffixes_inv = new int[length];
            for (uint i=1; i<csuff; i++)
            {
                suffixes_inv[suffixes[i]] = i+1;
            }
            delete [] suffixes;
            std::cout<<"Inverso de SA Creado\n";
            vector<Rule> seqSLP(csuff);

            // Inserts the final position with the first suffix: $
            seqSLP[0] = Rule(1, permutation->revpi(ctext[csuff-1]), csuff+1, 0);

            for (uint i=1; i<csuff; i++)
            {
                seqSLP[i] = Rule(suffixes_inv[seqB->select(i)+1], permutation->revpi(ctext[i-1]), i+1, 0);
            }

            delete [] suffixes_inv; free(ctext);

            seqR = new LabeledBinaryRelation();
            seqR->buildNoBitSeq(&seqSLP, csuff, csuff+1, rules, false);
            std::cout<<"Segunda relacion binaria creada\n";
            buildSamplingsOwn(sampling_length);
            std::cout<<"Sampling creado\n";


        }

        void buildSamplingsOwn(uint qsampling) {
            {
                // *** GRAMMAR SAMPLING
                // *******************
                uint DELTA_SAMP = 4;
                uint rules = slp->getNColumns();
                uchar *prefix = new uchar[qsampling + 1];
                uchar *current = new uchar[qsampling + 1];
                current[0] = '\0';

                uint csamp = 0;
                uint *pointers = new uint[rules + 1];
                uchar *samples = new uchar[rules * (qsampling + 1)];
                pointers[csamp] = 0;
                csamp++;
                uchar *ptr = samples;

                uint terminals = C->rank1(256);
                uint *posterms = new uint[terminals + 1];
                terminals = 0;

                for (uint i = 1; i < rules; i++) {
                    uint plen = lenrules->access(i);

                    if (plen > qsampling) {
                        extractCharsDirectNoSamplesOwn(i, plen, qsampling, prefix);
                        prefix[qsampling] = '\0';
                    } else {
                        extractCharsDirectNoSamplesOwn(i, plen, plen, prefix);
                        prefix[plen] = '\0';
                    }

                    if (strcmp((char *) current, (char *) prefix) != 0) {
                        strcpy((char *) ptr, (char *) prefix);
                        ptr += strlen((char *) prefix);
                        *ptr = '\0';
                        ptr++;

                        if (current[0] != prefix[0]) {
                            posterms[terminals] = ((csamp - 1) / BLOCK_SAMPLING);
                            terminals++;
                        }

                        pointers[csamp] = i;
                        csamp++;

                        strcpy((char *) current, (char *) prefix);
                    }
                }

                pointers[csamp] = rules + 1;
                pointers[csamp + 1] = rules + 2;

                posterms[terminals] = ((csamp - 2) / BLOCK_SAMPLING);
                terminals++;

                grammarSamp = new Sampling(samples, ptr - samples, pointers, csamp, BLOCK_SAMPLING, terminals, posterms,
                                           DELTA_SAMP);

                delete[] pointers;
                delete[] samples;
                delete[] prefix;
                delete[] current;
            }

            {
                // *** REVERSE SAMPLING
                // ********************
                uint DELTA_SAMP = 4;
                uint rules = slp->getNColumns();
                uchar *prefix = new uchar[qsampling + 1];
                uchar *suffix = new uchar[qsampling + 1];
                uchar *current = new uchar[qsampling + 1];
                current[0] = '\0';

                uint csamp = 0;
                uint *pointers = new uint[rules + 1];
                uchar *samples = new uchar[rules * (qsampling + 1)];
                pointers[csamp] = 0;
                csamp++;
                uchar *ptr = samples;

                uint terminals = C->rank1(256);
                uint *posterms = new uint[terminals + 1];
                terminals = 0;

                for (uint i = 1; i < rules; i++) {
                    uint crule = permutation->pi(i);
                    uint plen = lenrules->access(crule);

                    if (plen > qsampling) {
                        extractCharsReverseNoSamples(crule, plen, qsampling, suffix);
                        suffix[qsampling] = '\0';
                        for (int k = qsampling - 1; k >= 0; k--) { prefix[qsampling - k - 1] = suffix[k]; }
                        prefix[qsampling] = '\0';
                    } else {
                        extractCharsReverseNoSamples(crule, plen, plen, suffix);
                        suffix[plen] = '\0';
                        for (int k = plen - 1; k >= 0; k--) { prefix[plen - k - 1] = suffix[k]; }
                        prefix[plen] = '\0';
                    }

                    if (strcmp((char *) current, (char *) prefix) != 0) {
                        strcpy((char *) ptr, (char *) prefix);
                        ptr += strlen((char *) prefix);
                        *ptr = '\0';
                        ptr++;

                        if (current[0] != prefix[0]) {
                            posterms[terminals] = ((csamp - 1) / BLOCK_SAMPLING);
                            terminals++;
                        }

                        pointers[csamp] = i;
                        csamp++;

                        strcpy((char *) current, (char *) prefix);
                    }
                }

                pointers[csamp] = rules + 1;
                pointers[csamp + 1] = rules + 2;

                posterms[terminals] = ((csamp - 2) / BLOCK_SAMPLING);
                terminals++;

                reverseSamp = new Sampling(samples, ptr - samples, pointers, csamp, BLOCK_SAMPLING, terminals, posterms,
                                           DELTA_SAMP);

                delete[] pointers;
                delete[] samples;
                delete[] prefix;
                delete[] suffix;
                delete[] current;
            }

            {
                // *** SEQUENCE SAMPLING
                // ********************
                uint DELTA_SAMP = 4;
                uint lenseq = seqR->getNRows();
                uchar *prefix = new uchar[qsampling + 1];
                uchar *current = new uchar[qsampling + 1];
                current[0] = '\0';

                uint csamp = 0;
                uint *pointers = new uint[lenseq + 1];
                uchar *samples = new uchar[lenseq * (qsampling + 1)];
                pointers[csamp] = 0;
                csamp++;
                uchar *ptr = samples;

                uint terminals = C->rank1(256);
                uint *posterms = new uint[terminals + 1];
                terminals = 0;

                for (uint i = 2; i < lenseq; i++) {
                    uint pos = seqR->labelForRowNoBitSeq(i) - 1;
                    uint rule = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos + 2));
                    uint len = lenrules->access(rule);

                    if (len > qsampling) {
                        extractCharsDirectNoSamples(rule, len, qsampling, prefix);
                        prefix[qsampling] = '\0';
                    } else {
                        uint read = 0;

                        while (true) {
                            if (len < qsampling - read) {
                                extractCharsDirectNoSamples(rule, len, len, prefix + read);
                                read += len;
                            } else {
                                extractCharsDirectNoSamples(rule, len, qsampling - read, prefix + read);
                                read += qsampling - read;
                                break;
                            }

                            pos++;

                            if (pos == (lenseq - 1)) break;

                            rule = permutation->pi(seqR->retrieveColumnPerLabelNoBitSeq(pos + 2));
                            len = lenrules->access(rule);
                        }

                        prefix[read] = '\0';
                    }

                    if (strcmp((char *) current, (char *) prefix) != 0) {
                        strcpy((char *) ptr, (char *) prefix);
                        ptr += strlen((char *) prefix);
                        *ptr = '\0';
                        ptr++;

                        if (current[0] != prefix[0]) {
                            if (C->select1(terminals + 1) != (size_t) prefix[0]) {
                                size_t t;

                                do {
                                    posterms[terminals] = ((csamp - 1) / BLOCK_SAMPLING);
                                    terminals++;
                                    t = C->select1(terminals + 1);
                                } while (t != (size_t) prefix[0]);
                            }

                            posterms[terminals] = ((csamp - 1) / BLOCK_SAMPLING);
                            terminals++;
                        }

                        pointers[csamp] = i;
                        csamp++;

                        strcpy((char *) current, (char *) prefix);
                    }
                }

                pointers[csamp] = lenseq;
                pointers[csamp + 1] = lenseq + 1;

                posterms[terminals] = ((csamp - 2) / BLOCK_SAMPLING);
                terminals++;

                sequenceSamp = new Sampling(samples, ptr - samples, pointers, csamp, BLOCK_SAMPLING, terminals,
                                            posterms, DELTA_SAMP);

                delete[] pointers;
                delete[] samples;
                delete[] prefix;
                delete[] current;
            }
        }
        void extractCharsDirectNoSamplesOwn(uint rule, uint len, uint chars, uchar *sample)
        {
            RuleTrav rt(rule, len, PROOF);

            // Extracting the rule
            while (true)
            {
                if (rt.r[rt.level].len > 1)
                {
                    rt.r[rt.level].left = permutation->pi(slp->firstPointForLabel_X(rt.r[rt.level].id, &(rt.p[rt.level])));
                    rt.d[rt.level] = false;

                    rt.level++;
                    rt.r[rt.level].id = rt.r[rt.level-1].left;
                }
                else
                {
                    sample[rt.read] = (uchar)C->select1(rankY(rt.r[rt.level].id));
                    rt.read++;
                    if (rt.read == chars) break;

                    do rt.level--; while (rt.d[rt.level]);

                    rt.r[rt.level].right = slp->firstPointForLabel_Y(rt.r[rt.level].id, rt.p[rt.level]);
                    rt.d[rt.level] = true;

                    rt.level++;
                    rt.r[rt.level].id = rt.r[rt.level-1].right;
                }

                rt.r[rt.level].len = lenrules->access(rt.r[rt.level].id);
            }
        }
        int save();

        static int load(char *filename, RePairSLPIndex **rpi);

        static int load(const int & AA,char *filename, RePairSLPIndex **rpi)
        {
            *rpi = new RePairSLPIndex();
            (*rpi)->filename = string(filename);

            // Opening the input stream
            ifstream input(string((*rpi)->filename+string(".ind")).c_str());

            if (input.good())
            {
                (*rpi)->textlength = loadValue<uint>(input);
                (*rpi)->terminals = loadValue<uint>(input);

                // Loading all structures in the index
                (*rpi)->C = BitSequence::load(input);
                (*rpi)->Y = loadValue<uint>(input, (*rpi)->terminals);
                (*rpi)->lenrules = factorization::load(input);
                (*rpi)->permutation = Permutation::load(input);
                (*rpi)->slp = LabeledBinaryRelation::load(input);
                (*rpi)->seqB = DeltaCodes::load(input);
                (*rpi)->seqlength = ((*rpi)->seqB)->elements();
                (*rpi)->seqR = LabeledBinaryRelation::load(input);

                input.close();

                // Loading samplings
                input.open(string((*rpi)->filename+string(".samp")).c_str());

                if (input.good())
                {
                    (*rpi)->grammarSamp = Sampling::load(input);
                    (*rpi)->reverseSamp = Sampling::load(input);
                    (*rpi)->sequenceSamp = Sampling::load(input);
                    input.close();

                    return (*rpi)->grammarSamp->getMaxLength();
                }
                else
                {
                    delete *rpi; return 0;
                }
            }
            else
            {
                delete *rpi; return 0;
            }
        }

        unsigned int size();

        unsigned int count(uchar *pattern, uint length);

        vector<unsigned int> *locate(uchar *pattern, uint length, uint *numocc);

        unsigned char *extract(unsigned int start, unsigned int end);

        unsigned int length();

        ~RePairSLPIndex();

        size_t n_rules() { return seqlength; }

        uint n_terminals() { return terminals; }

    protected:
        string filename;

        uint textlength;
        uint seqlength;
        uint terminals;
        Mapper *mapper;

        BitSequence *C;            // Marks the symbols (in Sigma) used in the SLP
        uint *Y;            // Marks terminal rules
        factorization *lenrules;    // Stores length rules
        Permutation *permutation;    // Stores the permutation from reverse to direct ordering
        LabeledBinaryRelation *slp;    // Binary relation for the SLP

        DeltaCodes *seqB;        // Stores the length (in terminals) of each element in the compressed sequence
        LabeledBinaryRelation *seqR;    // Binary relation for prefixes/suffixes in the compressed sequence

        Sampling *grammarSamp;        // Sampling for the grammar.
        Sampling *reverseSamp;        // Sampling for the reverse grammar.
        Sampling *sequenceSamp;        // Sampling for the compressed sequence.


        bool
        static sorting(Rule i, Rule j) {
            if (strcmp(i.sequence, j.sequence) < 0) return true;
            else return false;
        }

        uint
        static lcp(uchar *str1, uchar *str2, uint len1, uint len2) {
            uint equals;
            uint len = len1;
            if (len2 < len1) len = len2;

            for (equals = 1; equals < len; equals++)
                if (str1[equals] != str2[equals]) break;

            return equals;
        }

        uint
        rankY(uint val) {
            // Binary search
            uint l = 0, r = terminals - 1, c;

            while (l <= r) {
                c = (l + r) / 2;
                if (Y[c] < val) l = c + 1;
                else {
                    if (Y[c] > val) r = c - 1;
                    else break;
                }
            }

            c++;

            return c;
        }

        /** Expands a rule for sortig the Re-Pair grammar.
            @rule: the rule to be expanded.
            @expanded: the expanded rule.
            @grammar: pointer to the Re-Pair grammar.
        */
        void expandRule(Rule r, char *expanded, vector<Rule> *grammar);

        /** Builds sampling over the index
            @qsampling: sampling length.
        */
        void buildSamplings(uint qsampling);

        /** Extracts a sample of chars length from the rule.
            @rule: the rule to be extracted.
            @len: length of the 'rule'.
            @chars: number of chars in the sample.
            @sample: sample.
        */
        void extractCharsDirectNoSamples(uint rule, uint len, uint chars, uchar *sample);

        /** Extracts a sample of qsampling length from the reverse rule.
            @rule: the rule to be extracted.
            @len: length of the 'rule'.
            @chars: number of chars in the sample.
            @sample: sample.
        */
        void extractCharsReverseNoSamples(uint rule, uint len, uint chars, uchar *sample);

        /** Extracts an snippet of chars length from the given position of the rule.
            @rule: the rule to be extracted.
            @len: length of the 'rule'.
            @position: initial position for extraction
            @chars: number of chars in the sample.
            @snippet: rule snippet.
        */
        void extractCharsDirect(uint rule, uint len, uint position, uint chars, uchar *snippet);

        /** Delimits the ranges of rules expanding (in G) all prefixes in the query.
            @q: structure containing all query information
        */
        void getPrefixesInSLP(FullQuery *q);

        /** Delimits the ranges of rules expanding (in G) all suffixes in the query.
            @q: structure containing all query information
        */
        void getSuffixesInSLP(FullQuery *q);

        /** Delimits the ranges of rules expanding (in S) all suffixes in the query.
            @q: structure containing all query information
        */
        void getSuffixesInSequence(FullQuery *q);

        /** Jumps some characters at the end of the rule in the query q.
            @rt: RuleTrav structure containing the query information required for jumping.
        */
        inline void jumpCharsReverse(RuleTrav *rt);

        /** Jumps some characters at the begin of the rule in the query q.
            @rt: RuleTrav structure containing the query information required for jumping.
        */
        inline void jumpCharsDirect(RuleTrav *rt);

        /** Performs a process of expansion and comparison with respect to the given prefix.
            @prefix: the pattern being compared.
            @q: structure containing all query information. The rule for comparison is
            provided by the RuleTrav structure internal to the query.
        */
        void expandAndCompareReverse(PSpattern *prefix, FullQuery *q);

        /** Performs a process of expansion (in the grammar G) and comparison with
            respect to the given suffix.
            @suffix: the pattern being compared.
            @q: structure containing all query information. The rule for comparison is
            provided by the RuleTrav structure internal to the query.
        */
        void expandAndCompareDirectInG(PSpattern *suffix, FullQuery *q);

        /** Performs a process of expansion (in the sequence S) and comparison with
            respect to the given suffix.
            @suffix: the pattern being compared.
            @q: structure containing all query information. The rule for comparison is
            provided by the RuleTrav structure internal to the query.
        */
        void expandAndCompareDirectInS(PSpattern *suffix, FullQuery *q);

        /** Expands the rule in the query q (through RuleTrav) and compares it
            with the pattern given in prefix.
            @preffix: the pattern being compared.
            @q: structure containing all query information. The rule for comparison is
            provided by the RuleTrav structure internal to the query.
        */
        inline int expandRuleAndCompareReverse(PSpattern *prefix, FullQuery *q);

        /** Expands the rule in the query q (through RuleTrav) and compares it
            with the pattern given in suffix.
            @suffix: the pattern being compared.
            @q: structure containing all query information. The rule for comparison is
            provided by the RuleTrav structure internal to the query.
        */
        inline int expandRuleAndCompareDirectInG(PSpattern *suffix, FullQuery *q);

        /** Expands the rule in the query q (through RuleTrav) and compares it
            with the pattern given in suffix.
            @suffix: the pattern being compared.
            @pos: position of the current rule in the compressed sequence.
            @jumped: number of char previously read (jumped) in the current rule.
            @q: structure containing all query information. The rule for comparison is
            provided by the RuleTrav structure internal to the query.
        */
        inline int expandRuleAndCompareDirectInS(PSpattern *suffix, uint pos, uint jumped, FullQuery *q);

        /** Obtains the range of rules: [left, right] delimiting the pattern given in
            prefix. This operation uses rule as pivot in both binary searches.
            @rule: the rule used as pivot.
            @prefix: the pattern being delimited.
            @q: structure containing all query information.
        */
        void obtainRangeReverse(uint rule, PSpattern *prefix, FullQuery *q);

        /** Obtains the range of rules: [left, right] delimiting in G the pattern
            given in suffix. This operation uses rule as pivot in both binary searches.
            @rule: the rule used as pivot.
            @suffix: the pattern being delimited.
            @q: structure containing all query information.
        */
        void obtainRangeDirectInG(uint rule, PSpattern *suffix, FullQuery *q);


        /** Obtains the range of rules: [left, right] delimiting in S the pattern
            given in suffix. This operation uses rule as pivot in both binary searches.
            @rule: the rule used as pivot.
            @suffix: the pattern being delimited.
            @q: structure containing all query information.
        */
        void obtainRangeDirectInS(uint rule, PSpattern *suffix, FullQuery *q);
    };
};
#endif  /* _REPAIRSLPINDEX_H */

