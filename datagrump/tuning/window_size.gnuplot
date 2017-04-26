#!/usr/bin/env gnuplot

reset
set terminal png

set key off
set xlabel "Signal Delay (ms)"
set ylabel "Throughput (Mbits/s)"
set title "Throughput-Delay Plot for Different Window Sizes"
set xrange [*:*] reverse
set datafile separator ","
plot "window_size_plot.csv" using 5:3:1 title "Window size" with labels offset 0,char 1 point
