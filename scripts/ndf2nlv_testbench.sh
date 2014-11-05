#!/bin/bash

FILES=../circuits/raw/EDF/*.ndf

for f in $FILES
do
	../circuits/raw/ndf2nlv $f ../circuits/raw/NLV/
	echo "Conversion of EDF file $f to nlv complete!"
done

