#!/usr/bin/env gnuplot

reset
set terminal png

set key on
set xlabel "Value of Alpha (see label for window scale factor)"
set ylabel "Score (Throughput * Delay)"
set title "Score for Different Couscou Alpha and Window Scale Factor"
set datafile separator ","
plot "couscou-combined-1-2.csv" using 1:($5/$7*1000):2 title "Window Scale Factor" with labels
