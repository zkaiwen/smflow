#!/bin/bash

FILES=../circuits/raw/IWLS/SEQ_/*.ndf

for f in $FILES
do
	echo "Converting EDF file $f to dot..."
	../circuits/raw/e2g -fp $f
done

echo "Moving Circuits to DOT Folder"
mv -v ../circuits/raw/IWLS/SEQ_/*.dot ../circuits/raw/DOT

perl dot2g.pl -d ../circuits/raw/DOT/

echo "Moving Circuits to Database Folder"
mv -v ../circuits/raw/DOT/*.g ../circuits/iwls/SEQ_

echo "Cleaning up DOT Files"
rm -v ../circuits/raw/DOT/*.dot

echo "[ -- COMPLETE -- ]"
