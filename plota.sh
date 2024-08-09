#!/bin/bash

for file in graficos/*.gp; do
    filename=$(basename "$file" .dat)
    gnuplot graficos/"$filename"
done