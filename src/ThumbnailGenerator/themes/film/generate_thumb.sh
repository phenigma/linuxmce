#!/bin/bash

grabber_picture="$1"
output_picture="$2"

convert "$grabber_picture"  -thumbnail 100x100 /tmp/thumb$$.png

convert  \
	/tmp/thumb$$.png -matte -bordercolor none -border 10x0 -composite \
	/usr/share/thumbnail-generator/themes/film/left.png -gravity west -composite \
	/usr/share/thumbnail-generator/themes/film/right.png -gravity east -composite \
	-background  black  \( +clone -shadow 69x2+2+2 \) +swap \
	-background  none   -flatten \
	"$output_picture"

rm  /tmp/thumb$$.png

