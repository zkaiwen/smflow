#!/bin/bash

FILES=../circuits/raw/MUX/*.ndf

for f in $FILES
do
	echo "Converting EDF file $f to dot..."
	../circuits/raw/e2g -fp $f
done

mv -v ../circuits/raw/MUX/*.dot ../circuits/raw/tDOT/

perl dot2g.pl -d ../circuits/raw/tDOT/

mv -v ../circuits/raw/tDOT/*.g ../circuits/mux/
