#!/bin/bash

# *****************************************************************************************
# find_and_replace_in_files.sh
# This script does a recursive, case sensitive directory search and replace of files
# To make a case insensitive search replace, use the -i switch in the grep call
# uses a startdirectory parameter so that you can run it outside of specified directory - else this script will modify itself!
# *****************************************************************************************

# **************** Change Variables Here ************
startdirectory="."
searchterm="Int-Type"
replaceterm="Int-Type"
# **********************************************************

echo "******************************************"
echo "* Search and Replace in Files Version .1 *"
echo "******************************************"

        for file in $(grep -l -R $searchterm $startdirectory)
          do
           sed -e "s/$searchterm/$replaceterm/g" $file > /tmp/tempfile.tmp
           mv /tmp/tempfile.tmp $file
           echo "Modified: " $file
        done

echo " *** Yay! All Done! *** "
