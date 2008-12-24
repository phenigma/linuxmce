#!/bin/bash

PIXDIR="/home/quick_start_icons"

if [[ ! -d $PIXDIR ]]; then
        mkdir -p $PIXDIR
fi

icon=$(RunSQL "SELECT Icon FROM QuickStartTemplate")
template=$(RunSQL "SELECT PK_QuickStartTemplate FROM QuickStartTemplate")

ICON=( $icon )
TEMPLATE=( $template )
counter=${#ICON[@]}


index=0

while [[ $index -le $(($counter-1)) ]]; do

        wget --tries=1 --timeout=2 -O $PIXDIR/${TEMPLATE[$index]} ${ICON[$index]}
        convert $PIXDIR/${TEMPLATE[$index]} $PIXDIR/template_${TEMPLATE[$index]}.jpg
        rm $PIXDIR/${TEMPLATE[$index]}
        RET=$?
        if [[ "$RET" == "1" ]]; then
        echo "Internet connection problem or bad URL !"
        fi
        index=$[$index + 1]
done

