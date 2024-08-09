#!/usr/bin/gnuplot -c

## set encoding iso_8859_15
set encoding utf

# set terminal qt persist

set grid

set style data point
set style function line
set style line 1 lc 3 pt 7 ps 0.3
set boxwidth 1
set xtics
# set xrange [0:]
set xlabel  "N (tamanho matriz)"


# ALTERNATIVA 2: Tabela com 3 colunas 
#
set key outside right # posição da legenda: outside {left | right}
unset logscale x
set logscale y 10
set datafile separator whitespace
set ylabel  "FLOPS_DP"
set title   "FLOPS_DP funções MatVet e MatMat"
set terminal qt 1 title "FLOPS_DP funções MatVet e MatMat"
set datafile separator ","
plot 'FLOPS_DP.dat' using 1:2 title "<Matriz x Vetor Sem Otimização FLOPS_DP>" with linespoints, \
     '' using 1:3 title "<Matriz x Vetor Sem Otimização FLOPS_AVX>" with linespoints, \
     '' using 1:4 title "<Matriz x Matriz Sem Otimização FLOPS_DP>" with linespoints, \
     '' using 1:5 title "<Matriz x Matriz Sem Otimização FLOPS_AVX>" with linespoints, \
     '' using 1:6 title "<Matriz x Vetor Com Otimização FLOPS_DP>" with linespoints, \
     '' using 1:7 title "<Matriz x Matriz Com Otimização FLOPS_AVX>" with linespoints

pause -1


# Gerando figura PNG
set terminal png
set output "FLOPS_DP.png"
plot 'FLOPS_DP.dat' using 1:2 title "<Matriz x Vetor Sem Otimização>" with linespoints, \
     '' using 1:3 title "<Matriz x Matriz Sem Otimização>" with linespoints, \
     '' using 1:4 title "<Matriz x Vetor Com Otimização>" with linespoints, \
     '' using 1:5 title "<Matriz x Matriz Com Otimização>" with linespoints
replot
unset output