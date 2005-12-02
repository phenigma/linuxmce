#!/bin/sh

# tmp dir for my files
WORK="${TMPDIR-/tmp}/${0##*/}-$$"
mkdir "$WORK" || exit 1
trap 'rm -rf "$WORK"' EXIT

for file in `find linux -type d | grep -v CVS | grep -v .cvsignore` ; do
    mkdir -p "$WORK/${file}"
done
for file in `find linux -type f | grep -v CVS | grep -v .cvsignore` ; do
	tmpfile="$WORK/${file}.$$"
	perl -ne 's/[ \t]+$//; 
        s/^\ \ \ \ \ \ \ \ /\t/; 
        s/^\ \ \ \ \ \ \ \t/\t/; 
        s/^\ \ \ \ \ \ \t/\t/; 
        s/^\ \ \ \ \ \t/\t/; 
        s/^\ \ \ \t/\t/; 
        s/^\ \ \t/\t/; 
        s/^\ \t/\t/; 
        s/\t\ \ \ \ \ \ \ \ /\t\t/g; 
        s/\t\ \ \ \ \ \ \ \t/\t\t/g; 
        s/\t\ \ \ \ \ \ \t/\t\t/g; 
        s/\t\ \ \ \ \ \t/\t\t/g; 
        s/\t\ \ \ \t/\t\t/g; 
        s/\t\ \ \t/\t\t/g; 
        s/\t\ \t/\t\t/g;
	print' < "${file}" > "${tmpfile}"
	diff -u "${file}" "${tmpfile}" | sed 		\
		-e "s|^--- ${file}|--- ${file}.orig|"	\
		-e "s|^+++ ${tmpfile}|+++ ${file}|"
	rm -f "$tmpfile"
done
