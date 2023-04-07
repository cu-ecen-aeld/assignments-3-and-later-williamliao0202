#!/bin/sh
# $1 : filesdir
# $2 : searchstr

if [ $# -ne 2 ]
then
        echo "format : $0 <filesdir> <searchstr>"
	exit 1
else
	if [ ! -d $1 ] 
	then
		echo "Error: $1 is not a directory."
		exit 1
	fi
	num_files=$(find "$1" -type f 2>/dev/null | wc -l)
	num_match_lines=$(grep -r "$2" "$1" | wc -l)
	echo "The number of files are $num_files and the number of matching lines are $num_match_lines"
fi
