//
// Created by alpachec on 17-09-18.
//

#ifndef IMPROVED_GRAMMAR_INDEX_BUILD_HYB_LZ77_H
#define IMPROVED_GRAMMAR_INDEX_BUILD_HYB_LZ77_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <algorithm>
#include <vector>
#include <string>
#include <time.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <iostream>
#include <fstream>
#include <math.h>

#include <hsi_parser_lz77/compute_lz77.hpp>


#ifndef _W64 // 8 Bytes
#define _W64 64
#endif
#ifndef _WW64 // 16 Bytes
#define _WW64 128
#endif
#ifndef _BW64
#define _BW64 6
#endif


#ifndef uchar
#define uchar unsigned char
#endif

long double wclock() {
    timeval tim;
    gettimeofday(&tim, NULL);
    return tim.tv_sec + (tim.tv_usec / 1000000.0L);
}

FILE *file_open(std::string filename, std::string mode) {
    FILE *f = fopen(filename.c_str(), mode.c_str());
    if (f == NULL) {
        perror(filename.c_str());
        std::exit(EXIT_FAILURE);
    }
    return f;
}

template<typename value_type>
void read_from_file(value_type* dest, uint64_t length, FILE *f) {
    uint64_t fread_ret = fread(dest, sizeof(value_type), length, f);
    if (fread_ret != length) {
        fprintf(stderr, "\nError: fread failed.\n");
        std::exit(EXIT_FAILURE);
    }
}

template<typename value_type>
void read_from_file(value_type* dest, uint64_t length, std::string fname) {
    FILE *f = file_open(fname, "r");
    read_from_file<value_type>(dest, length, f);
    fclose(f);
}

uint64_t file_size(std::string filename) {
    FILE *f = file_open(filename, "r");
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    if (size < 0) {
        perror(filename.c_str());
        std::exit(EXIT_FAILURE);
    }
    fclose(f);
    return (uint64_t)size;
}

template<typename char_type, typename text_offset_type>
char_type *decode_lz77(
        uint64_t text_length,
        std::vector<std::pair<text_offset_type, text_offset_type> > &parsing) {
    uint64_t n_phrases = parsing.size();
    char_type *text = new char_type[text_length];
    uint64_t j = 0;
    for (uint64_t i = 0; i < n_phrases; ++i) {
        uint64_t pos = parsing[i].first;
        uint64_t len = parsing[i].second;
        if (len == 0) text[j++] = (char_type)pos;
        else {
            for (uint64_t t = 0; t < len; ++t)
                text[j + t] = text[pos + t];
            j += len;
        }
    }
    return text;
}

// set the number x as a bitstring sequence in *A. In the range of bits [ini, .. ini+len-1] of *A. Here x has len bits
void setNum64(ulong *A, ulong ini, uint len, ulong x) {
    ulong i=ini>>_BW64, j=ini-(i<<_BW64);

    if ((j+len)>_W64){
        ulong myMask = ~(~0ul >> j);
        A[i] = (A[i] & myMask) | (x >> (j+len-_W64));
        myMask = ~0ul >> (j+len-_W64);
        A[i+1] = (A[i+1] & myMask) | (x << (_WW64-j-len));
    }else{
        ulong myMask = (~0ul >> j) ^ (~0ul << (_W64-j-len)); // XOR: 1^1=0^0=0; 0^1=1^0=1
        A[i] = (A[i] & myMask) | (x << (_W64-j-len));
    }
}

template<typename char_type, typename text_offset_type>
void store_lz77(std::vector<std::pair<text_offset_type, text_offset_type> > &parsing, ulong nT, char *fileStore) {
    uint64_t n_phrases = parsing.size();
    uint64_t pos, len, MAX_len, MAX_pos, i;
    uint lg_n, lg_len;

    fprintf(stderr, "\nTo Store Dictionary:\n");
    MAX_len = MAX_pos = 0;
    for (i = 0; i < n_phrases; ++i) {
        pos = parsing[i].first;
        len = parsing[i].second;
        //fprintf(stderr, "(%lu,%lu) ", pos, len);
        if (pos > MAX_pos)
            MAX_pos = pos;
        if (len > MAX_len)
            MAX_len = len;
    }
    lg_n = 1 + (uint)(log(MAX_pos)/log(2));
    lg_len = 1 + (uint)(log(MAX_len)/log(2));

    fprintf(stderr, "\nlg_n = %d, MAX_len = %lu, lg_len = %d \n", lg_n, MAX_len, lg_len);

    ulong size_ARR_POS = n_phrases*lg_n/(8*sizeof(ulong));		// number of (ulong) cells for ARR_POS
    if ((n_phrases*lg_n)%(8*sizeof(ulong)))
        size_ARR_POS++;
    ulong *ARR_POS = new ulong[size_ARR_POS];

    ulong size_ARR_LEN = n_phrases*lg_len/(8*sizeof(ulong));
    if ((n_phrases*lg_len)%(8*sizeof(ulong)))
        size_ARR_LEN++;
    ulong *ARR_LEN = new ulong[size_ARR_LEN];

    fprintf(stderr, "\n\nStoring Dictionary of %lu phrases\n", n_phrases);

    for (i=0; i < n_phrases; ++i) {
        pos = parsing[i].first;
        len = parsing[i].second;
        setNum64(ARR_POS, (ulong)i*lg_n, lg_n, pos);
        setNum64(ARR_LEN, (ulong)i*lg_len, lg_len, len);
        //fprintf(stderr, "(%lu, %lu) ", pos, len);
    }

    fprintf(stderr, "\nSave data structure in file %s\n", fileStore);

    fprintf(stderr, "\n n_phrases = %lu\n", n_phrases);
    std::ofstream os(fileStore, std::ofstream::binary);
    os.write((const char*)&nT, sizeof(ulong));
    os.write((const char*)&n_phrases, sizeof(uint64_t));	// number of factors
    os.write((const char*)&lg_n, sizeof(uint));
    os.write((const char*)&lg_len, sizeof(uint));
    os.write((const char*)ARR_POS, size_ARR_POS*sizeof(ulong));
    os.write((const char*)ARR_LEN, size_ARR_LEN*sizeof(ulong));
    os.close();
}

// example:
// /home/ferrada/data/documents/alabarda/alabarda /home/ferrada/data/documents/alabarda/parser.lz77
// /home/ferrada/data/repetitive/cere/cere5MB /home/ferrada/data/repetitive/cere/5MB/parser.lz77
// /home/ferrada/data/documents/english/english_X/english_X /home/ferrada/data/documents/english/english_X/parser.lz77

void build_hyb_lz77(char * text_file, char* store_file) {


    typedef uint8_t char_type;
    typedef uint64_t text_offset_type;
    typedef std::pair<text_offset_type, text_offset_type> pair_type;

    fprintf(stderr, "sizeof(char_type) = %lu\n", sizeof(char_type));
    fprintf(stderr, "sizeof(text_offset_type) = %lu\n\n",
            sizeof(text_offset_type));

    std::string text_filename = text_file;
    uint64_t text_length = file_size(text_filename);

    fprintf(stderr, "Text filename = %s\n", text_filename.c_str());
    fprintf(stderr, "Text length = %lu\n\n", text_length);

    fprintf(stderr, "  Read text... ");
    char_type *text = new char_type[text_length];
    read_from_file(text, text_length, text_filename);
    fprintf(stderr, "DONE\n");

    {
        // Compute LZ77 parsing.
        std::vector<pair_type> parsing;
        fprintf(stderr, "  Compute LZ77... ");
        long double start = wclock();
        compute_lz77(text, text_length, parsing);
        long double elapsed = wclock() - start;
        fprintf(stderr, "%.2Lfs, ", elapsed);
        fprintf(stderr, "z = %lu, ", parsing.size());

        // Store LZ77 to file.
        char fileStore[400];
        strcpy(fileStore, store_file);
        store_lz77<char_type, text_offset_type>(parsing, text_length, fileStore);

        // Verify parsing.
        char_type *decoded_text =
                decode_lz77<char_type, text_offset_type>(text_length, parsing);
        if (!std::equal(text, text + text_length, decoded_text)) {
            fprintf(stderr, "FAILED!\n");
            std::exit(EXIT_FAILURE);
        } else fprintf(stderr, "OK\n");

        // Clean up.
        delete[] decoded_text;
    }

    // Clean up.
    delete[] text;
}


















#endif //IMPROVED_GRAMMAR_INDEX_BUILD_HYB_LZ77_H
