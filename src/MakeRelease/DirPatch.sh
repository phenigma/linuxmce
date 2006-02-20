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

	# Removed files/directories
	entries=$(cat $section.changes | grep '^-' | cut -c2- | tr '\n ' ' _')
	for entry in $entries; do
		Type="${entry:0:1}"
		FileName="${entry:2}"
		case "$Type" in
			F) echo "rm -f $FileName" >>$section.patch.sh ;;
			D) echo "rmdir $FileName" >>$section.patch.sh ;;
			*) echo "# Unknown type $Type" ;;
		esac
	done

	# Added files/directories
	entries=$(cat $section.changes | grep '^\+' | cut -c2- | tr '\n ' ' _')
	files=
	for entry in $entries; do
		Type="${entry:0:1}"
		FileName="${entry:2}"
		case "$Type" in
			F) echo "# add file $FileName" >>$section.patch.sh; files="$files $FileName" ;;
			D) echo "# add directory $FileName" >>$section.patch.sh ;; # don't add directories to the list; tar knows paths and we only need changed files
			*) echo ""
		esac
	done
	(cd $dirL; tar -czvf $location/$section.tar.gz $files)
	echo "tar -xzvf /home/uploads/$section.tar.gz" >>$section.patch.sh
done
