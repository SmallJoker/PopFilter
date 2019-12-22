#!/bin/bash

make

if [ $? -ne 0 ]; then
	exit
fi

./PopFilter "$@"

if [ $? -ne 0 ]; then
	exit
fi

gnuplot -p -e "plot \"plot_data.dat\" with linespoints"
