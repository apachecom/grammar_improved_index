# Improved Grammar Index

This project is the implementation of a grammar-based index proposed in [1]. Self-indexes are
Compact Data Structures that represent the text in a compressed way and allows us to count
the occurrences of a pattern (count), return the positions of the original text where these
occurrences appear (locate) and extract any substring of the text (display).
This index uses O(G lg n) bits and can find the occ occurrences of patterns 
P[1..m] in time O((m^2 + occ)lg G), where G is the size of the grammar.

[1] Claude, F., Navarro, G., & Pacheco, A. (2020). Grammar-Compressed Indexes with Logarithmic Search Time. https://arxiv.org/pdf/2004.01032.pdf

## Compilation 🚀
First download the project

```
git clone https://github.com/apachecom/grammar_improved_index.git
```

In order to compile the program, it is necessary to download, compile and install the Succinct Data Structure Library (SDSL).

```
git clone https://github.com/simongog/sdsl-lite
cd sdsl-lite
./install-sh 
```
You also must replace some original files in the library with the files in the folder sdsl-files.

In order to run benchmark you will need some external libraries as gflags (https://github.com/gflags/gflags.git) and libcds (https://github.com/fclaude/libcds.git)


Now, it is posible to compile the binaries
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE 
make 
```

In order to test the algorithm, you can use the collections located in the repository http://pizzachili.dcc.uchile.cl/repcorpus.html
```
wget  http://pizzachili.dcc.uchile.cl/repcorpus/real/einstein.en.txt.gz
gunzip einstein.en.txt.gz
```
You must process collections in order to remove forbidden characters.
```
./process_coll <input_file> <output_file>
```
## Build the index 

To build the index, you must run

```
./build_indices <collection_file> <index_out_file> 
```

Since we use google-benchmark, you can generate CSV files with the results
```
./build_indices <collection_file> <index_out_file> <mem_out_file> --benchmark_out=<results_file> --benchmark_out_format=csv
```

## Display operation

To extract a substring, you must execute:

```
./display_indices <index_file> <positions_file> min_len max_len gap --benchmark_out=<output_result_file>  --benchmark_out_format=csv
```

where:

- "index_file" is the path of the index file without the extension (Note that ./build_indices generates several versions of our index);

- "positions_file" is a file with the positions to extract separated by a line jump;

- "min" is the minimum length to extract;

- "max" is the maximun length to extract;

- "gap" is the gap betwen lenght;

example:

```
./display_indices f1 ptos 10 100 10 --benchmark_out=out.csv  --benchmark_out_format=csv
```
This will perform an experiment which we will measure the extraction procedure for lengths (10,20,30,...,100)



## Locate operation

To locate a pattern, you must execute:
```
./locate_indices <index_file> <pattern_file> min_len max_len gap --benchmark_out=<output_result_file>  --benchmark_out_format=csv
```

where:

- "pattern_file" is a file with the patterns to locate NOT separated by a line jump; The patterns must have length = max_len

The rest of the parameters are the same as for the extraction