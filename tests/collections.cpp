//
// Created by inspironXV on 8/17/2018.
//


#include <string>

const std::string dirFolder [] = {
        "tests/collections/DNA/",
        "tests/collections/SOURCES/",
        "tests/collections/PITCHES/",
        "tests/collections/PROTEINS/",
        "tests/collections/ENGLISH/",
        "tests/collections/xml/"
};

const std::string dircollection [] = {

        "dna",
        ////"dna222",
        "dna.50MB",
        "dna.100MB",
        "dna.200MB",

        "sources",
        //"222sources.50MB",
        "sources.50MB",
        "sources.100MB",
        "sources.200MB",

        "pitches",
        ///"2222_pitches.50MB",
        "pitches.50MB",

        "proteins",
        "proteins.50MB",
        "proteins.100MB",
        "proteins.200MB",

        "english",
        "english.50MB",
        "english.100MB",
        "english.200MB",
        "english.1024MB",

        "dblp.xml",
        "dblp.xml.50MB",
        "dblp.xml.100MB",
        "dblp.xml.200MB",

};

enum dataDir{
    dir_DNA = 0,
    dir_sources,
    dir_pitches,
    dir_proteins,
    dir_english,
    dir_xml
};
enum dataCollection{
    DNA = 0,
    DNA50,
    DNA100,
    DNA200,

    sources,
    sources50,
    sources100,
    sources200,

    pitches,
    pitches50,

    proteins,
    proteins50,
    proteins100,
    proteins200,

    english,
    english50,
    english100,
    english200,
    english1024,

    xml,
    xml50,
    xml100,
    xml200

} ;
