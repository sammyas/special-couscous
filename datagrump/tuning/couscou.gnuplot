#!/usr/bin/env gnuplot

reset
set terminal png

set key on
set xlabel "Signal Delay (ms)"
set ylabel "Throughput (Mbits/s)"
set title "Throughput-Delay Plot for Different Couscou Constants"
set xrange [*:*] reverse
set datafile separator ","
plot "couscou-tuning3.csv" using 7:5:0 title "Parameter Index (see table)" with labels offset -1,char 0 point
