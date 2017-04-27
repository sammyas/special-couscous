#!/usr/bin/env gnuplot

reset
set terminal png

set key off
set xlabel "Signal Delay (ms)"
set ylabel "Throughput (Mbits/s)"
set title "Throughput-Delay Plot for Different Algorithms"
set xrange [*:*] reverse
set datafile separator ","
plot "overall.csv" using 4:2:1 with labels offset 0,char 1 point
