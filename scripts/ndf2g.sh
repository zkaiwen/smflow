#!/bin/bash

FILES=../circuits/raw/EDF/*.ndf

for f in $FILES
do
	echo "Converting EDF file $f to dot..."
	../circuits/raw/ndf2cnl $f ../circuits/testbench/
done

