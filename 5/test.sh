#!/bin/bash 

path="./../input" 
program="./a.out"
code="code.c"

gcc "$code" -o "$program"
for file in "$path"/in*.txt
do
    number=$(echo "$file" | grep -o '[0-9]\+')
    $program "$file" "out$number.txt"
    echo "Result for file $file: $(cat "out$number.txt")"
done
rm $program

