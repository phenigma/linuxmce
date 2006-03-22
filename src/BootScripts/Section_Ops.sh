#!/bin/bash


#  '
#  '## BEGIN : SectionName
#  '   Some line of data
#  '   And some other line of data
#  '## END : SectionName
#  '


## Creates and/or populates a section from a file
function PopulateSection {
        local File=$1
        local SectionName=$2
        local SectionData=$3

        ## Create the file if it doesn't exist
        if [[ ! -f $File ]]; then
                touch $File
        fi

        ## Check if our section exists in the file
        grep -q "^## BEGIN : $SectionName" $File
        if [[ "$?" != "0" ]]; then
                echo -e "\n## BEGIN : $SectionName\n\n## END : $SectionName\n" >> $File
        fi

        ## Replace the section
        rep="## BEGIN : $SectionName\n$SectionData\n## END : $SectionName"
        awk -vrep="$rep" "/^## BEGIN : $SectionName/{print rep}/^## BEGIN : $SectionName/,/## END : $SectionName/{next}1" < $File > $File.$$
        mv $File.$$ $File

}

## Clears and removes a section from a file
function ClearSection {
        local File=$1
        local SectionName=$2
        local SectionData=$3

        if [[ -f $File ]]; then
                PopulateSection "$File" "$SectionName" ""
        fi
}

## Checkes if a section exists in a file
function CheckSectionExists {
	local File=$1
	local SectionName=$2

	ret=false	
	if grep -q "^## BEGIN : $SectionName" ;then
		if grep -q "^## END : $SectionName" ;then
			ret=true
		fi
	fi

	echo $ret
}
