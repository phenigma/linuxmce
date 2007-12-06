#!/bin/bash

grabber_picture="$1"
output_picture="$2"

create_theme ()
{
convert  \
           /usr/share/thumbnail-generator/themes/cd/base.png -geometry  +0+0  -composite \
           /tmp/thumb$$.png  -geometry  +19+5  -composite \
           /usr/share/thumbnail-generator/themes/cd/top.png  -geometry +0+0  -composite \
           "$output_picture"
}
picture_aspect=$(($(identify -format %w "$grabber_picture") - $(identify -format %h "$grabber_picture")))

if [ "$picture_aspect" = "0" ]; then
        convert "$grabber_picture"  -thumbnail 98x98 /tmp/thumb$$.png
elif [ "$picture_aspect" -gt "0" ]; then
        convert "$grabber_picture"  -thumbnail 300x98 /tmp/thumb$$.png
        convert /tmp/thumb$$.png -crop 98x98+$(( ($(identify -format %w /tmp/thumb$$.png) - 98) / 2))+0  +repage /tmp/thumb$$.png
else
        convert "$grabber_picture"  -thumbnail 98x500 /tmp/thumb$$.png
        convert /tmp/thumb$$.png -crop 98x98+0+$(( ($(identify -format %h /tmp/thumb$$.png) - 98) / 2))  +repage /tmp/thumb$$.png
fi
create_theme

rm  /tmp/thumb$$.???

