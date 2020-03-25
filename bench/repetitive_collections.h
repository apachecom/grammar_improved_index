//
// Created by alpachec on 16-09-18.
//

#ifndef IMPROVED_GRAMMAR_INDEX_REPETITIVE_COLLECTIONS_H
#define IMPROVED_GRAMMAR_INDEX_REPETITIVE_COLLECTIONS_H

#include <iostream>
#include <map>
#include <cstring>

using namespace std;


std::map<std::string,uint> collections_code = {
        //{"../test/collection/repetitve/reals/einstein.de.txt"   ,0},
        //{"einstein.de.txt"   ,0},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/cere"              ,1},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/coreutils"         ,2},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/einstein.de.txt"   ,3},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/einstein.en.txt"   ,4},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/Escherichia_Coli"  ,5},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/influenza"         ,6},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/kernel"            ,7},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/para"              ,8},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/world_leaders"     ,9},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudo/dblp.xml.0001.1",10},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudo/dblp.xml.00001.1",11},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudo/dblp.xml.0001.2",12},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudo/dblp.xml.00001.2",13},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudo/dna.001.1",14},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudo/english.001.2",15},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudo/proteins.001.1",16}
};



std::map<uint, std::string> collections_code_inv = {
        //{"../test/collection/repetitve/reals/einstein.de.txt"   ,0},
        //{"einstein.de.txt"   ,0},
        {1 , "cere"             },
        {2 , "coreutils"        },
        {3 , "einstein.de.txt"  },
        {4 , "einstein.en.txt"  },
        {5 , "Escherichia_Coli" },
        {6 , "influenza"        },
        {7 , "kernel"           },
        {8 , "para"             },
        {9 , "world_leaders"    },
        {10, "dblp.xml.0001.1"  },
        {11, "dblp.xml.00001.1" },
        {12, "dblp.xml.0001.2"  },
        {13, "dblp.xml.00001.2" },
        {14, "dna.001.1"        },
        {15, "english.001.2"    },
        {16, "proteins.001.1"   }
};

std::map<uint, std::string> collections_code_inv_dir = {
        //{"../test/collection/repetitve/reals/einstein.de.txt"   ,0},
        //{"einstein.de.txt"   ,0},
        {1 , "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/cere"                  },
        {2 , "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/coreutils"             },
        {3 , "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/einstein.de.txt"       },
        {4 , "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/einstein.en.txt"       },
        {5 , "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/Escherichia_Coli"      },
        {6 , "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/influenza"             },
        {7 , "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/kernel"                },
        {8 , "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/para"                  },
        {9 , "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/world_leaders"         },
        {10, "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/dblp.xml.0001.1"  },
        {11, "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/dblp.xml.00001.1" },
        {12, "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/dblp.xml.0001.2"  },
        {13, "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/dblp.xml.00001.2" },
        {14, "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/dna.001.1"        },
        {15, "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/english.001.2"    },
        {16, "/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/proteins.001.1"   }
};



std::map<std::string,std::string> collections_name = {
        //{"../test/collection/repetitve/reals/einstein.de.txt"   ,0},
        //{"einstein.de.txt"   ,0},
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/cere"                   , "cere"                      },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/coreutils"              , "coreutils"                 },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/einstein.de.txt"        , "einstein.de.txt"           },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/einstein.en.txt"        , "einstein.en.txt"           },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/Escherichia_Coli"       , "Escherichia_Coli"          },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/influenza"              , "influenza"                 },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/kernel"                 , "kernel"                    },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/para"                   , "para"                      },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/reals/world_leaders"          , "world_leaders"             },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/dblp.xml.0001.1"   , "dblp.xml.0001.1"           },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/dblp.xml.00001.1"  , "dblp.xml.00001.1"          },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/dblp.xml.0001.2"   , "dblp.xml.0001.2"           },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/dblp.xml.00001.2"  , "dblp.xml.00001.2"          },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/dna.001.1"         , "dna.001.1"                 },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/english.001.2"     , "english.001.2"             },
        {"/media/alejandro/DATA1/phd/pasantia/repetitiveCollections/pseudoreal/proteins.001.1"    , "proteins.001.1"            }
};



#endif //IMPROVED_GRAMMAR_INDEX_REPETITIVE_COLLECTIONS_H
