#!/bin/bash

FILES=../circuits/raw/netlist/*.ndf

for f in $FILES
do
	echo "Converting EDF file $f to dot..."
	../circuits/raw/e2g -fp $f
done

mv -v ../circuits/raw/netlist/*.dot ../circuits/raw/DOT/

perl dot2g.pl -d ../circuits/raw/DOT/

mv -v ../circuits/raw/DOT/*.g ../circuits/netlist/
