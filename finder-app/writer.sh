#!/bin/sh
# $1 : writefile
# $2 : writestr
if [ $# -ne 2 ]
then
	echo "format : $0 <wrtiefile> <writestr>"
	exit 1
else
	if [ ! -d "$(dirname $1)" ]
	then
		mkdir -p "$(dirname $1)" || { echo "Error: could not create directory"; exit 1; }
	fi
	echo "$2" > "$1" || { echo "Error: The file could not be create."; exit 1; }
fi
