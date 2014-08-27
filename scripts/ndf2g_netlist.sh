#!/bin/bash

FILES=../circuits/raw/netlist/*.ndf

for f in $FILES
do
	echo "Converting EDF file $f to dot..."
	../circuits/raw/ndf2cnl $f ../circuits/netlist/
done
