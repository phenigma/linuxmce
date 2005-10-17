#!/bin/bash

replacementsL="/home/samba/repositories/pluto/replacements/main/binary-i386"
replacementsR="/home/mirrors/replacements/main/binary-i386"

# TODO: make recursive
ScriptL="
for i in *; do
	stat -c %s \"\$i\" | tr '\n' ' '
	md5sum \"\$i\"
done | awk '{print \$3,\$2,\$1}' | sort -k1
"

ScriptR="${ScriptL//\\\\/\\\\}"
ScriptR="${ScriptR//\"/\\\"}"
ScriptR="${ScriptR//\$/\\\$}"

sections="replacements"

location=$(pwd)
for section in $sections; do
	eval dirL=\$${section}L
	eval dirR=\$${section}R
	(cd "$dirL"; eval "$ScriptL") | sort -k1 >$section.local
	ssh uploads@plutohome.com "cd \"$dirR\"; eval \"$ScriptR\"" | sort -k1 >$section.remote
	diff -u $section.{remote,local} | egrep '^[+-]' | egrep -v '^\+\+\+ |^--- ' | sort -k1 | awk '{print $1}' >$section.changes

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
	for file in $files; do
		echo "# add $file" >>$section.patch.sh
	done
	(cd $dirL; tar -czvf $location/$section.tar.gz $files)
	echo "tar -xzvf /home/uploads/$section.tar.gz" >>$section.patch.sh
done
