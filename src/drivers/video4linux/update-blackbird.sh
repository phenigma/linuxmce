#!/bin/bash

modlist="cx88_blackbird cx8802 cx8800 cx88xx"
revlist=""
for i in $modlist; do revlist="$i $revlist"; done

if [ "$1" = "" ]
then
	echo "(re)building modules..."
	make || exit 1

	echo "(re)installing modules..."
	sudo make install || exit 2
fi

echo -n "unloading modules... "
for i in $modlist
do
	sudo rmmod $i
	echo -n " $i "
done
echo "done."
sudo depmod -a

echo -n "(re)loading modules... "
for i in $revlist
do
	sudo modprobe $i
	echo -n " $i "
done
echo " done."
