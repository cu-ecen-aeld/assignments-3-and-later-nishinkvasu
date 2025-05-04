#!/bin/bash

# Accepts the following arguments: the first argument is a full path to a file (including filename) on the filesystem, referred to below as writefile; the second argument is a text string which will be written within this file, referred to below as writestr

# Exits with value 1 error and print statements if any of the arguments above were not specified

# Creates a new file with name and path writefile with content writestr, overwriting any existing file and creating the path if it doesn’t exist. Exits with value 1 and error print statement if the file could not be created.

if [ $# -lt 2 ]
then
    echo "Script requires two arguments"
    exit 1
fi

writefile=$1
writestr=$2

filename=$(basename $writefile) # extract filename
pathname=$(dirname $writefile) # extract path

# echo "filename is $filename"
# echo "filepath is $pathname"

if [ ! -d $pathname ]
then
    # echo "create folder structure"
    mkdir -p $pathname # what if pathname cannot be created
    if [ $? -ne 0 ]
    then
        echo "Error"
        return 1
    fi
fi

cd $pathname

if [ ! -e $filename ]
then
    # echo "create file"
    touch $filename #what if file cannot be created
    if [ $? -ne 0 ]
    then
        echo "Error"
        return 1
    fi
fi

#write to file
echo "$writestr" > $filename




