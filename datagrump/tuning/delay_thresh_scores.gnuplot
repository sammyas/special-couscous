#!/usr/bin/env gnuplot

reset
set terminal png

set key off
set xlabel "Window Size (packets)"
set ylabel "Score (Throughput * Delay)"
set title "Score for Different Window Sizes"
set datafile separator ","
plot "delay_thresh.csv" using 1:($3/$5*1000) with points
