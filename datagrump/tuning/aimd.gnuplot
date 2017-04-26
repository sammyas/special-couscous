#!/usr/bin/env gnuplot

reset
set terminal png size 1024,768

set key on
set xlabel "Signal Delay (ms)"
set ylabel "Throughput (Mbits/s)"
set title "Throughput-Delay Plot for Different AIMD Parameters"
set xrange [*:*] reverse
set datafile separator ","
plot "aimd-combined.csv" using 9:7:0 title "Parameter Index (see table)" with labels #offset 0,char 1 point
