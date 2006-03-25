#!/bin/bash

version=7676
changelog=$(cat <<'END'
nvidia-graphics-drivers (1.0.$version-1) unstable; urgency=low

  * built with $version

 -- Radu Cristescu <radu.c@plutohome.com>  Mon, 13 Feb 2006 19:56:00 +0200

END
)

for i in debian/* debian.binary/*; do
	if [[ "$i" == */changelog ]]; then
		if ! grep -q "$version" "$i"; then
			eval "echo \"$changelog\"" >"$i".new
			cat "$i" >>"$i".new
			mv "$i"{.new,}
		fi
		continue
	fi
	[[ -d "$i" ]] && continue
	sed -i 's/7174/'"$version"'/g; s/pkg0/pkg1/g; s/glxtokens/glxext/g' "$i"
done

sed -i 's/README\(.txt\)*/README.txt/g' debian/nvidia-glx.docs*
