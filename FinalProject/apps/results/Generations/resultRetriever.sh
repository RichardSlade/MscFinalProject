#! /bin/bash

dir=$1
cd $(echo "$1")

for i in {1..29}
do
	file=$(echo "$i.txt")
	awk '{print $6}' $file > results$i.txt
done

echo > results.txt
paste -d , results*.txt > results.txt

for i in {1..29}
do
	rm $(echo "results$i.txt")
done
