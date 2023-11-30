#!/bin/bash

# Function to create a directory with specified number of files and size
create_directory() {
    local dir_name=$1
    local num_files=$2
    local file_size=$3

    mkdir -p $dir_name
    cd $dir_name

    for ((i=1; i<=$num_files; i++)); do
        dd if=/dev/zero of=file_$i bs=$file_size count=1
    done

    cd ..
}

# Create directories with files
create_directory "directory1" 50 1G
create_directory "directory2" 5000 10M

# Create directory with files and subdirectories
mkdir -p directory3
cd directory3

for ((i=1; i<=50; i++)); do
    dd if=/dev/zero of=file_$i bs=10M count=1
done

for ((i=1; i<=100; i++)); do
    mkdir -p subdirectory_$i
    cd subdirectory_$i
    for ((j=1; j<=100; j++)); do
        dd if=/dev/zero of=file_$j bs=10M count=1
    done
    cd ..
done

cd ..

# Measure time for each case using the time command
echo "Time taken for directory1:"
time du -sh directory1

echo "Time taken for directory2:"
time du -sh directory2

echo "Time taken for directory3:"
time du -sh directory3
