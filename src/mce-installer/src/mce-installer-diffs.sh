#!/bin/bash

diffs=$(diff -u $1 $2)

echo "$diffs"


