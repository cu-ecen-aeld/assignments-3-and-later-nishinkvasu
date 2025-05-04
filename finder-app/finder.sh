#!/bin/bash

# Accepts the following runtime arguments: the first argument is a path to a directory on the filesystem, referred to below as 
# filesdir; the second argument is a text string which will be searched within these files, referred to below as searchstr
# Exits with return value 1 error and print statements if any of the parameters above were not specified
# Exits with return value 1 error and print statements if filesdir does not represent a directory on the filesystem
# Prints a message "The number of files are X and the number of matching lines are Y" where X is the number of files in the directory and all subdirectories and Y is the number of matching lines found in respective files, where a matching line refers to a line which contains searchstr (and may also contain additional content).

# check if there are at least two arguments
if [ $# -lt 2 ]
then
    echo "Error: Script takes in two arguments"
    exit 1
fi

filesdir=$1
searchstr=$2

# check if path is valid
if [ ! -d $filesdir ]
then 
    echo "No such directory exists"
    exit 1
fi

pushd $filesdir 2>&1 >> /dev/null
noOfFiles=$(find . -type f | wc -l)
hits=$(grep -ar $searchstr . | wc -l)
popd 2>&1 >> /dev/null

# echo $(pwd)

echo "The number of files are "$noOfFiles" and the number of matching lines are $hits"






