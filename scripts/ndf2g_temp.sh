#!/bin/bash

FILES=../circuits/raw/temp/*.ndf

for f in $FILES
do
	echo "\n\nConverting EDF file $f to cnl..."
	../circuits/raw/ndf2cnl $f ../circuits/temp/
done

