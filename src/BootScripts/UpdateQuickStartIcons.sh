#!/bin/bash

PIXDIR="/home/quick_start_icons"

if [[ ! -d $PIXDIR ]]; then
        mkdir -p $PIXDIR
fi

icon=$(mysql -u root -D pluto_main -N -e "SELECT Icon FROM QuickStartTemplate")
template=$(mysql -u root -D pluto_main -N -e "SELECT PK_QuickStartTemplate FROM QuickStartTemplate")

ICON=( $icon )
TEMPLATE=( $template )
counter=${#ICON[@]}


index=0

while [[ $index -le $(($counter-1)) ]]; do

        wget --timeout=10 -O $PIXDIR/${TEMPLATE[$index]} ${ICON[$index]}
        convert $PIXDIR/${TEMPLATE[$index]} $PIXDIR/template_${TEMPLATE[$index]}.jpg
        rm $PIXDIR/${TEMPLATE[$index]}
        RET=$?
        if [[ "$RET" == "1" ]]; then
        echo "Internet connection problem or bad URL !"
        fi
        index=$[$index + 1]
done

