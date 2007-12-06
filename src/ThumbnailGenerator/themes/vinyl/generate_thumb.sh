#!/bin/bash

grabber_picture="$1"
output_picture="$2"

create_theme ()
{
convert  \
           /usr/share/thumbnail-generator/themes/vinyl/base.png -geometry  +0+0  -composite \
           /tmp/thumb$$.png  -geometry  +4+3  -composite \
           /usr/share/thumbnail-generator/themes/vinyl/top.png  -geometry +0+0  -composite \
           "$output_picture"
}
picture_aspect=$(($(identify -format %w "$grabber_picture") - $(identify -format %h "$grabber_picture")))

if [ "$picture_aspect" = "0" ]; then
        convert "$grabber_picture"  -thumbnail 86x86 /tmp/thumb$$.png
elif [ "$picture_aspect" -gt "0" ]; then
        convert "$grabber_picture"  -thumbnail 300x86 /tmp/thumb$$.png
        convert /tmp/thumb$$.png -crop 86x86+$(( ($(identify -format %w /tmp/thumb$$.png) - 86) / 2))+0  +repage /tmp/thumb$$.png
else
        convert "$grabber_picture"  -thumbnail 86x500 /tmp/thumb$$.png
        convert /tmp/thumb$$.png -crop 86x86+0+$(( ($(identify -format %h /tmp/thumb$$.png) - 86) / 2))  +repage /tmp/thumb$$.png
fi
create_theme

rm  /tmp/thumb$$.??? ;

