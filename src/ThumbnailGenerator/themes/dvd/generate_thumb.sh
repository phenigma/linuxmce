#!/bin/bash

grabber_picture="$1"
output_picture="$2"

theme_name="dvd-1"

if [[ "$theme_name" = "dvd-1" ]]; then
	create_theme ()
	{
	convert /tmp/thumb$$.png -crop 77x106+0+0  +repage /tmp/thumb$$.png
	convert  \
	           /usr/share/thumbnail-generator/themes/dvd/base-1.png -geometry  +0+0  -composite \
	           /tmp/thumb$$.png  -geometry  +10+17  -composite \
	           /usr/share/thumbnail-generator/themes/dvd/top.png  -geometry +0+0  -composite \
	           "$output_picture"
	}
elif [[ "$theme_name" = "dvd-2" ]]; then
	create_theme ()
	{
	convert /tmp/thumb$$.png -crop 77x106+0+0  +repage /tmp/thumb$$.png
	convert  \
	           /usr/share/thumbnail-generator/themes/dvd/base-2.png -geometry  +0+0  -composite \
	           /tmp/thumb$$.png  -geometry  +10+17  -composite \
	           /usr/share/thumbnail-generator/themes/dvd/top.png  -geometry +0+0  -composite \
	           "$output_picture"
	}
elif [[ "$theme_name" = "dvd-3" ]]; then
	create_theme ()
	{
	convert /tmp/thumb$$.png -crop 77x106+0+0  +repage /tmp/thumb$$.png
	convert  \
	           /usr/share/thumbnail-generator/themes/dvd/base-3.png -geometry  +0+0  -composite \
	           /tmp/thumb$$.png  -geometry  +10+17  -composite \
	           /usr/share/thumbnail-generator/themes/dvd/top.png  -geometry +0+0  -composite \
	           "$output_picture"
	}
else
	create_theme ()
	{
	convert /tmp/thumb$$.png -crop 77x106+0+0  +repage /tmp/thumb$$.png
	convert  \
	           /usr/share/thumbnail-generator/themes/dvd/base.png -geometry  +0+0  -composite \
	           /tmp/thumb$$.png  -geometry  +10+17  -composite \
	           /usr/share/thumbnail-generator/themes/dvd/top.png  -geometry +0+0  -composite \
	           "$output_picture"
	}
fi


picture_aspect=$(echo "scale=2; $(identify -format %w "$grabber_picture")/$(identify -format %h "$grabber_picture")*100" | bc | cut -d. -f 1)

if [ $picture_aspect -gt 65 ]; then
	convert "$grabber_picture"  -thumbnail 555x106 /tmp/thumb$$.png
else
	convert "$grabber_picture"  -thumbnail 72x555 /tmp/thumb$$.png
fi
create_theme
rm  /tmp/thumb$$.???

