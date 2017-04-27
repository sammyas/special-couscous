#!/usr/bin/env gnuplot

reset
set terminal png

set key on
set xlabel "Signal Delay (ms)"
set ylabel "Throughput (Mbits/s)"
set title "Throughput-Delay Plot for Different Delay Thresholds"
set xrange [*:*] reverse
set datafile separator ","
plot "delay_thresh.csv" using 5:3:1 title "Delay Threshold" with labels offset 0,char 1 point
