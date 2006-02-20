#!/bin/bash

replacementsL="/home/samba/repositories/pluto/replacements/main/binary-i386"
replacementsR="/home/mirrors/replacements/main/binary-i386"

Script()
{
	find | while read line; do
		if [[ -d "$line" ]]; then
			echo -n "0 0 $line "
			echo "D"
		elif [[ -f "$line" ]]; then
			stat -c %s "$line" | tr '\n' ' '
			md5sum "$line" | tr '\n' ' '
			echo "F"
		fi
	done | awk '{print $4,$3,$2,$1}' | sort -k1,2
}

ScriptL="$(declare -f Script); Script"

ScriptR="${ScriptL//\\\\/\\\\}"
ScriptR="${ScriptR//\"/\\\"}"
ScriptR="${ScriptR//\$/\\\$}"

sections="replacements"

location=$(pwd)
for section in $sections; do
	eval dirL=\$${section}L
	eval dirR=\$${section}R
	(cd "$dirL"; eval "$ScriptL") | sort -k1,2 >$section.local
	ssh uploads@plutohome.com "cd \"$dirR\"; eval \"$ScriptR\"" | sort -k1,2 >$section.remote
	diff -u $section.{remote,local} | egrep '^[+-]' | egrep -v '^\+\+\+ |^--- ' | sort -k1,2 | awk '{print $1,$2}' >$section.changes

	: >$section.patch.sh
	chmod +x $section.patch.sh
	
	echo '#!/bin/bash' >>$section.patch.sh
	echo "mkdir -p $dirR" >>$section.patch.sh
	echo "cd $dirR" >>$section.patch.sh

	# Remove files then directories
	# it usually goes like this: the directories to be removed are left empty by removing the files in them
	# if a directory is to be removed but it's not empty, something went wrong
	# rmdir is there to make sure we don't delete non-empty directories
	# but this situation should never happen with this algorithm, unless tampered with
	
	# Removed files
	entries=$(cat $section.changes | grep '^-F' | cut -f2 -d' ' | tr '\n' ' ')
	for FileName in $entries; do
		echo "rm -f $FileName" >>$section.patch.sh
	done
	
	# Removed directories
	entries=$(cat $section.changes | grep '^-D' | cut -f2 -d' ' | tr '\n' ' ')
	for FileName in $entries; do
		echo "rmdir $FileName" >>$section.patch.sh
	done

	# Added files/directories
	entries=$(cat $section.changes | grep '^\+' | cut -c2- | tr '\n ' ' _')
	files=
	for entry in $entries; do
		Type="${entry:0:1}"
		FileName="${entry:2}"
		case "$Type" in
			F) echo "# add file $FileName" >>$section.patch.sh; files="$files $FileName" ;;
			D) echo "mkdir -p $FileName" >>$section.patch.sh ;;
			*) echo ""
		esac
	done
	(cd $dirL; tar -czvf $location/$section.tar.gz $files)
	echo "tar -xzvf /home/uploads/$section.tar.gz" >>$section.patch.sh
done
