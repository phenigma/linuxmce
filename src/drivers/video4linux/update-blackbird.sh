#!/bin/bash

modlist="cx88_blackbird cx8802 cx8800 cx88xx"
revlist=""
for i in $modlist; do revlist="$i $revlist"; done

echo "(re)building modules..."
make || exit 1

echo "(re)installing modules..."
sudo make install || exit 2

echo -n "unloading modules... "
sudo rmmod $modlist || exit 3
echo "done."
sudo depmod -a

echo -n "(re)loading modules... "
for i in $revlist
do
	sudo modprobe $i || exit 4
	echo -n " $i "
done
echo " done."
