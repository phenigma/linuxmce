#!/bin/bash

for i in 0 1; do
	for j in 0 1 2 3 4 5 6 7; do
		echo "^[[$i;3${j}m: [$i;3${j}mAlpha beta gamma delta .. whatever[0m"
	done
done
