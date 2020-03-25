#!/bin/bash



cd cmake-build-release


sudo make bench_qgram_display_pattern_len

#./bench_qgram_display_pattern_len 1 /media/alejandro/DATA1/phd/examen_de_calificacion/paper
./bench_qgram_display_pattern_len 2 /media/alejandro/DATA1/phd/examen_de_calificacion/paper
./bench_qgram_display_pattern_len 4 /media/alejandro/DATA1/phd/examen_de_calificacion/paper
./bench_qgram_display_pattern_len 5 /media/alejandro/DATA1/phd/examen_de_calificacion/paper
./bench_qgram_display_pattern_len 6 /media/alejandro/DATA1/phd/examen_de_calificacion/paper
./bench_qgram_display_pattern_len 7 /media/alejandro/DATA1/phd/examen_de_calificacion/paper
#./bench_qgram_display_pattern_len 8 /media/alejandro/DATA1/phd/examen_de_calificacion/paper


