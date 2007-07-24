#!/bin/bash

#Coredump statistics

echo "*COREDUMP STATISTICS*:" >>/CoreDumps.log
echo "======================" >>/CoreDumps.log

coreCount="0"
for file in  /usr/pluto/coredump/* ;do
        if [[ ! -d $file ]] ;then
               coreCount=$(( $coreCount + 1 ))
        fi
done

if [[ "$coreCount" != "0" ]]; then
       echo "There are $coreCount coredumps: " >>/CoreDumps.log
       for coreFile in /usr/pluto/coredump/* ;do
           if [[ ! -d $coreFile ]] ;then
                   coreFile=$(basename $coreFile) >>/CoreDumps.log
                   echo -e " * $coreFile  \t$(du -sh /usr/pluto/coredump/$coreFile | cut -f1)" >>/CoreDumps.log

           fi
       done
       #echo -e "\nNOTE: The coredumps will be available on 10.0.0.150 in /var/coredumps directory"
else
       echo ' There are no coredumps on this computer.' >>/CoreDumps.log

fi
																						
