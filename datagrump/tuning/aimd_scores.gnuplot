#!/usr/bin/env gnuplot

reset
set terminal png size 1024,768

set key off
set xlabel "Parameter Index (see table)"
set ylabel "Score (Throughput * Delay)"
set title "Score for Different AIMD Constants"
set datafile separator ","
plot "aimd-combined.csv" using 0:($7/$9*1000):0 with labels point
