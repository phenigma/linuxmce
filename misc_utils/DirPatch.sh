#!/bin/bash

replacementsL="/home/samba/repositories/pluto/replacements/main/binary-i386"
replacementsR="/home/mirrors/replacements/main/binary-i386"

# TODO: make recursive
ScriptL="
for i in *; do
	stat -c %s \"\$i\" | tr '\n' ' '
	md5sum \"\$i\"
done | awk '{print \$3,\$2,\$1}' | sort
"

ScriptR="${ScriptL//\\\\/\\\\}"
ScriptR="${ScriptR//\"/\\\"}"
ScriptR="${ScriptR//\$/\\\$}"

sections="replacements"

location=$(pwd)
for section in $sections; do
	eval dirL=\$${section}L
	eval dirR=\$${section}R
	(cd "$dirL"; eval "$ScriptL") >$section.local
	ssh uploads@plutohome.com "cd \"$dirR\"; eval \"$ScriptR\"" >$section.remote
	diff -u $section.{remote,local} | egrep '^[+-]' | egrep -v '^\+\+\+ |^--- ' | sort | awk '{print $1}' >$section.changes

	: >$section.patch.sh
	chmod +x $section.patch.sh
	
	echo '#!/bin/bash' >>$section.patch.sh
	echo "mkdir -p $dirR" >>$section.patch.sh
	echo "cd $dirR" >>$section.patch.sh
	files=$(cat $section.changes | grep '^-' | cut -c2- | tr '\n' ' ')
	for file in $files; do
		echo "rm -f $file" >>$section.patch.sh
	done

	files=$(cat $section.changes | grep '^\+' | cut -c2- | tr '\n' ' ')
	(cd $dirL; tar -czvf $location/$section.tar.gz $files)
	echo "tar -xzvf $section.tar.gz" >>$section.patch.sh
done
