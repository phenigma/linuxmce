#!/bin/sh

# tmp dir for my files
WORK="${TMPDIR-/tmp}/${0##*/}-$$"
mkdir "$WORK" || exit 1
trap 'rm -rf "$WORK"' EXIT

for file in *.[ch]; do
	tmpfile="$WORK/${file}.$$"
	perl -ne 's/[ \t]+$//; print' < "${file}" > "${tmpfile}"
	diff -u "${file}" "${tmpfile}" | sed 		\
		-e "s|^--- ${file}|--- ${file}.orig|"	\
		-e "s|^+++ ${tmpfile}|+++ ${file}|"
	rm -f "$tmpfile"
done
