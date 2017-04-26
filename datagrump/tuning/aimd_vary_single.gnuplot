#!/usr/bin/env gnuplot

reset
set terminal png

set key off
set xlabel "Value of Constant (see label)"
set ylabel "Score (Throughput * Delay)"
set title "Score Variation When Varying Single AIMD Constants"
set logscale x
set datafile separator ","
plot "aimd_vary_single.csv" using 2:($4/$6*1000):1 with labels
