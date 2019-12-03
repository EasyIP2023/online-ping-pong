#!/bin/bash

# I'm not the best bash programmer

OUTPUT=$((
(
ps aux | grep "online" >&1
) 1>log.txt
) 2>&1)

pat='[0-9]+'
IFS=' ' # space is set as delimiter
FILE=log.txt
num=2
while read LINE; do
  count=0
  read -ra ADDR <<< "$LINE" # str is read into an array as tokens separated by IFS
  for i in "${ADDR[@]}"; do # access each element of array
    let count++
    if (("$count" == "$num")); then >&2
      [[ $i =~ $pat ]]
      # echo "${BASH_REMATCH[0]}"
      kill -9 "$BASH_REMATCH"
      exit
    fi
  done
done < $FILE
