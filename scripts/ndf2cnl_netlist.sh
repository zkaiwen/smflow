#!/bin/bash

FILES=../circuits/raw/netlist/*.ndf

for f in $FILES
do
	echo "\n\nConverting EDF file $f to cnl..."
	../circuits/raw/ndf2cnl $f ../circuits/netlist/
done

