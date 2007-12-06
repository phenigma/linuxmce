#!/bin/bash

grabber_picture="$1"
output_picture="$2"


create_theme ()
{
convert "$1" \
	-bordercolor white  -border 3 \
	\( /usr/share/thumbnail-generator/themes/photos/top.png -background  none -rotate $(( rotate * -1 )) \) -gravity center  -composite \
	-background  none   -rotate $rotate \
	-background  black  \( +clone -shadow 69x1+1+1 \) +swap \
	-background  none   -flatten \
	"$1"
}

#If there are 3 pictures:
if [[ -n "$grabber_picture3" ]]; then
	convert "$grabber_picture"  -thumbnail 56x56 /tmp/thumb$$.png
	convert "$grabber_picture2"  -thumbnail 56x56 /tmp/thumb2$$.png
	convert "$grabber_picture3"  -thumbnail 56x56 /tmp/thumb3$$.png
	picture_aspect=$(echo "scale=2; $(identify -format %w /tmp/thumb$$.png)/$(identify -format %h /tmp/thumb$$.png)*100" | bc | cut -d. -f 1)
	picture_aspect2=$(echo "scale=2; $(identify -format %w /tmp/thumb2$$.png)/$(identify -format %h /tmp/thumb2$$.png)*100" | bc | cut -d. -f 1)
	picture_aspect3=$(echo "scale=2; $(identify -format %w /tmp/thumb3$$.png)/$(identify -format %h /tmp/thumb3$$.png)*100" | bc | cut -d. -f 1)

	#If true then vertical arrange:
	if [ "$(( (picture_aspect + picture_aspect2 + picture_aspect3) / 3 ))" -gt 100 ]; then
		icon_size=90x128

		if [ "$picture_aspect" -gt 100 ]; then
			rotate=30
		else
			rotate=-60
		fi
		create_theme /tmp/thumb$$.png

		if [ "$picture_aspect2" -gt 100 ]; then
			rotate=0
		else
			rotate=-90
		fi
		create_theme /tmp/thumb2$$.png

		if [ "$picture_aspect3" -gt 100 ]; then
			rotate=-30
		else
			rotate=-120
		fi
		create_theme /tmp/thumb3$$.png

		convert  \
		           -size $icon_size null: -composite \
		           /tmp/thumb$$.png -composite \
		           /tmp/thumb2$$.png -gravity west -composite \
		           /tmp/thumb3$$.png -gravity southwest -composite \
		           "$output_picture"

	#horizontal arrange:
	else
		icon_size=128x90

		if [ "$picture_aspect" -gt 105 ]; then
			rotate=-120
		else
			rotate=-30
		fi
		create_theme /tmp/thumb$$.png

		if [ "$picture_aspect2" -gt 105 ]; then
			rotate=-90
		else
			rotate=0
		fi
		create_theme /tmp/thumb2$$.png

		if [ "$picture_aspect3" -gt 105 ]; then
			rotate=-60
		else
			rotate=30
		fi
		create_theme /tmp/thumb3$$.png

		convert  \
		           -size $icon_size null: -composite \
		           /tmp/thumb$$.png -composite \
		           /tmp/thumb2$$.png -gravity north -composite \
		           /tmp/thumb3$$.png -gravity northeast -composite \
		           "$output_picture"
	fi

	rm /tmp/thumb$$.???
	rm /tmp/thumb2$$.???
	rm /tmp/thumb3$$.???

#If there are 2 pictures:
elif [[ -n "$grabber_picture2" ]]; then
	convert "$grabber_picture"  -thumbnail 56x56 /tmp/thumb$$.png
	convert "$grabber_picture2"  -thumbnail 56x56 /tmp/thumb2$$.png
	picture_aspect=$(echo "scale=2; $(identify -format %w /tmp/thumb$$.png)/$(identify -format %h /tmp/thumb$$.png)*100" | bc | cut -d. -f 1)
	picture_aspect2=$(echo "scale=2; $(identify -format %w /tmp/thumb2$$.png)/$(identify -format %h /tmp/thumb2$$.png)*100" | bc | cut -d. -f 1)

	#If true then vertical arrange:
	if [ "$(( (picture_aspect + picture_aspect2) / 2 ))" -gt 100 ]; then
		icon_size=90x128

		if [ "$picture_aspect" -gt 100 ]; then
			rotate=30
		else
			rotate=-60
		fi
		create_theme /tmp/thumb$$.png

		if [ "$picture_aspect2" -gt 100 ]; then
			rotate=0
		else
			rotate=-90
		fi
		create_theme /tmp/thumb2$$.png

		convert  \
		           -size $icon_size null: -composite \
		           /tmp/thumb$$.png -composite \
		           /tmp/thumb2$$.png -gravity west -composite \
		           "$output_picture"

	#horizontal arrange:
	else
		icon_size=128x90

		if [ "$picture_aspect" -gt 105 ]; then
			rotate=-120
		else
			rotate=-30
		fi
		create_theme /tmp/thumb$$.png

		if [ "$picture_aspect2" -gt 105 ]; then
			rotate=-90
		else
			rotate=0
		fi
		create_theme /tmp/thumb2$$.png

		convert  \
		           -size $icon_size null: -composite \
		           /tmp/thumb$$.png -composite \
		           /tmp/thumb2$$.png -gravity north -composite \
		           "$output_picture"
	fi

	rm /tmp/thumb$$.???
	rm /tmp/thumb2$$.???

#If there is 1 picture:
else
	convert "$grabber_picture"  -thumbnail 56x56 /tmp/thumb$$.png
	picture_aspect=$(echo "scale=2; $(identify -format %w /tmp/thumb$$.png)/$(identify -format %h /tmp/thumb$$.png)*100" | bc | cut -d. -f 1)

	#If true then vertical arrange:
	if [ "$picture_aspect" -gt 100 ]; then
		icon_size=90x128

		if [ "$picture_aspect" -gt 100 ]; then
			rotate=30
		else
			rotate=-60
		fi
		create_theme /tmp/thumb$$.png


	#horizontal arrange:
	else
		icon_size=128x90

		if [ "$picture_aspect" -gt 105 ]; then
			rotate=-120
		else
			rotate=-30
		fi
		create_theme /tmp/thumb$$.png
	fi
	convert  \
	           -size $icon_size null: -composite \
	           /tmp/thumb$$.png -composite \
	           "$output_picture"

	rm /tmp/thumb$$.???

fi

