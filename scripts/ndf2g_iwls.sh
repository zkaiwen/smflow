#!/bin/bash

FILES=../circuits/raw/IWLS/*.ndf

for f in $FILES
do
	echo "Converting EDF file $f to dot..."
	../circuits/raw/e2g -fp $f
done

mv -v ../circuits/raw/IWLS/*.dot ../circuits/raw/IWLS_dot/

perl dot2g.pl -d ../circuits/raw/IWLS_dot/

mv -v ../circuits/raw/IWLS/*.g ../circuits/iwls
