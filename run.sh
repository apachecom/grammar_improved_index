#!/bin/bash

make

#$1 collection name i.e. ~/influenza
#$2 prefix file output ~/influenza

#process collection to remove 0 and 1 simbols
./process_coll $1 "$2_N0" 1 

#create random pattern from collection 
# 1000 patterns of len 10 
./process_coll "$2_N0" "$2_N0" 0 10 1000

#build the index 
./build_indices "$2_N0" "$2_N0"

#run display benchmark
./display_indices "$2_N0" "$2_N0-10.pos" 10 10 1

#run locate benchmark
./locate_indices "$2_N0" "$2_N0-10.ptt"



