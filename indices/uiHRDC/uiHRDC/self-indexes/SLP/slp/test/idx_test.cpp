//
// Created by alpachec on 16-09-18.
//


#include <fstream>
#include <chrono>
#include "../RePairSLPIndex.h"
#include "../Utils/offsets.h"

#include <cppUtils.h>

using namespace cds_static;
using namespace std::chrono;
using timer = std::chrono::high_resolution_clock;

void slp_build()
{
    //uchar *text = (uchar *) collection.c_str();
    char f [] = "output";
    ifstream in("einstein.de.txt", ifstream::in);

    in.seekg(0,ios_base::end);
    uint tsize = in.tellg();
    in.seekg(0,ios_base::beg);

    if (in.good())
    {
        // Reading the input text
        uchar *text = loadValue<uchar>(in, tsize);
        in.close();

        cout << endl;
        cout << " >> Building the SLP Index for 'prob_pattern_pt':" << endl;
        cout << "    # Input file size: " << tsize << " bytes" << endl;

        int qgram = 4;
        if (qgram < 0)
            qgram = 4;
        cout << "    # " << qgram << "-grams will be additionally indexed in the grammar" << endl;

        // New RePairSLPIndex
        RePairSLPIndex *indexer = new RePairSLPIndex();
        indexer->build(text, tsize, qgram, f);
        indexer->save();

        delete indexer;
    }
    else cerr << endl << "Please, check your input file" << endl << endl;
    //uchar *text = (uchar *) "abracadabra\0";
}



int main (int argc, char *argv[] ){
    slp_build();
}
